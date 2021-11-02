#include "db/Sqlite3Database.hpp"
#include "db/ISqlite3Wrapper.hpp"
#include "db/Sqlite3Functions.hpp"
#include "db/Sqlite3Schema.hpp"
#include "model/Memo.hpp"
#include "model/Tag.hpp"
#include "db/Tools.hpp"
#include <set>

#include <logger/Logger.hpp>

namespace memo {

namespace {

const std::string kLogTag = "[Sqlite3Database] ";

} // namespace

Sqlite3Database::Sqlite3Database(std::unique_ptr<ISqlite3Wrapper> wrapper)
    : sqlite3_(std::move(wrapper))
{
    if (sqlite3_->open())
    {
        sqlite3_->exec(MemoTable::cmd::Create(), nullptr);
        sqlite3_->exec(TagTable::cmd::Create(), nullptr);
        sqlite3_->exec(TaggedTable::cmd::Create(), nullptr);
        if (!sqlite3_->exec("PRAGMA foreign_keys = ON;", nullptr))
            LOG_WRN("Failed to enable enforcement of Foreign key constraints.")
    }
    else
        LOG_ERR(kLogTag << "Failed to open sqlite3 database.")
}

std::vector<model::MemoPtr> Sqlite3Database::listMemos(const MemoSearchFilter& filter)
{
    LOG_DBG("Selecting memos ...")
    const auto query = BuildMemoQuery(filter);
    const auto selectedRows = SelectRows(query, *sqlite3_);
    const auto expectedMemoValueCount = 4ul;

    std::vector<model::MemoPtr> memos;
    std::set<unsigned long> memoIds;
    LOG_DBG("Converting " << selectedRows.size() << " memo rows to Memos ...")
    for (const auto& row : selectedRows)
    {
        if (row.size() != expectedMemoValueCount)
        {
            LOG_WRN("SELECT query returned an invalid number of values per row: " << row.size() <<
                    ". Expected number of values: " << expectedMemoValueCount)
            break;
        }
        try
        {
            auto memo = std::make_shared<model::Memo>();
            memo->setId(std::stoul(row[0]));
            memo->setTitle(row[1]);
            memo->setDescription(row[2]);
            memo->setTimestamp(std::stoul(row[3]));

            const bool alreadyAdded = !memoIds.insert(memo->id()).second;
            if (alreadyAdded)
                continue;

            std::vector<unsigned long> tagIds;
            SelectMemoTagIds(memo->id(), tagIds, *sqlite3_);
            memo->setTagIds(tagIds);
            memos.emplace_back(memo);
        }
        catch (const std::invalid_argument&)
        {
            LOG_WRN("Error encountered while converting memo rows to model::Memos returned by a SELECT query.")
            break;
        }
    }
    LOG_DBG("Total number of selected Memos: " << memos.size())
    return memos;
}

std::vector<model::TagPtr> Sqlite3Database::listTags(const TagSearchFilter& filter)
{
    LOG_DBG("Listing Tags ...")
    const auto query = BuildTagQuery(filter);
    const auto selectedRows = SelectRows(query, *sqlite3_);
    const auto expectedValueCount = 4ul;

    LOG_DBG("Converting " << selectedRows.size() << " tag rows to Tags ...")
    std::vector<model::TagPtr> selectedTags;
    for (const auto& row : selectedRows) {
        if (row.size() != expectedValueCount)
        {
            LOG_WRN("SELECT query returned an invalid number of values per row: " << row.size() <<
                    ". Expected number of values: " << expectedValueCount)
            break;
        }
        try
        {
            auto tag = std::make_shared<model::Tag>();
            tag->setId(std::stoul(row[0]));
            tag->setName(row[1]);
            tag->setColor(tools::IntToColor(std::stoi(row[2])));
            tag->setTimestamp(std::stoul(row[3]));
            selectedTags.emplace_back(tag);
        }
        catch (const std::invalid_argument&)
        {
            LOG_WRN("Error encountered while converting tag rows to model::Tags returned by a SELECT query.")
            break;
        }
    }
    LOG_DBG("Total number of selected Tags: " << selectedTags.size())
    return selectedTags;
}

bool Sqlite3Database::updateMemo(const model::Memo& memo)
{
    LOG_INF(kLogTag << "Updating memo ...")

    beginTransaction();
    std::vector<unsigned long> queriedTagIds;
    if (!SelectMemoTagIds(memo.id(), queriedTagIds, *sqlite3_))
    {
        LOG_WRN(kLogTag << "Update memo FAILURE.")
        rollback();
        return false;
    }

    std::set<unsigned long> oldTagIds(queriedTagIds.begin(), queriedTagIds.end());
    auto checkIdNotPresent = [&oldTagIds](const unsigned long tagId) { return oldTagIds.erase(tagId) == 0; };

    std::vector<unsigned long> tagIdsToAdd;
    std::copy_if(memo.tagIds().begin(), memo.tagIds().end(), std::back_inserter(tagIdsToAdd), checkIdNotPresent);

    std::vector<unsigned long> tagIdsToDelete(oldTagIds.begin(), oldTagIds.end());
    bool failed = !DeleteMemoTagIds(memo.id(), tagIdsToDelete, *sqlite3_)
               || !InsertMemoTagIds(memo.id(), tagIdsToAdd, *sqlite3_)
               || !UpdateMemoTable(memo, *sqlite3_);

    if (failed)
    {
        LOG_WRN(kLogTag << "Update memo FAILURE.")
        rollback();
        return false;
    }

    commitTransaction();
    LOG_INF(kLogTag << "Memo update SUCCESS.")
    return true;
}

bool Sqlite3Database::updateTag(const model::Tag& tag)
{
    LOG_INF(kLogTag << "Updating tag ...")
    beginTransaction();
    if (!UpdateTagTable(tag, *sqlite3_))
    {
        LOG_WRN(kLogTag << "Update tag FAILURE.")
        rollback();
        return false;
    }
    commitTransaction();
    LOG_INF(kLogTag << "Update tag SUCCESS.")
    return true;
}

bool Sqlite3Database::insertMemo(const model::Memo& memo)
{
    LOG_DBG("Inserting Memo with title '" << memo.title() << "' ...")
    beginTransaction();
    auto insertSuccess = InsertMemo(memo, *sqlite3_);
    auto newId = SelectMemoId(memo.title(), *sqlite3_);
    if (!insertSuccess || newId == -1ul)
    {
        if (newId != -1ul)
            LOG_INF("Failed to insert Memo. Given title is already in use by another Memo.")
        else
            LOG_WRN("Failed to insert Memo.")
        rollback();
        return false;
    }

    if (!InsertMemoTagIds(newId, memo.tagIds(), *sqlite3_))
    {
        LOG_WRN("Failed to insert Memo. Tag ids could not be assigned.")
        rollback();
        return false;
    }
    commitTransaction();
    LOG_DBG("Memo inserted.")
    return true;
}

bool Sqlite3Database::insertTag(const model::Tag& tag)
{
    LOG_DBG("Inserting Tag with name '" << tag.name() << "' ...")

    if (InsertTag(tag, *sqlite3_))
    {
        LOG_DBG("Tag inserted.")
        return true;
    }
    LOG_WRN("Failed to insert Tag.")
    return false;
}

bool Sqlite3Database::deleteMemo(const model::Memo& memo)
{
    LOG_DBG("Deleting Memo ...")

    LOG_DBG("Memo id: " << memo.id())
    const auto stringMemoId = std::to_string(memo.id());
    const std::string deleteTaggedQuery = "DELETE FROM " + TaggedTable::kName +
                                        " WHERE " + TaggedTable::att::kMemoId + " = " + stringMemoId + ";";
    const std::string deleteMemoQuery = "DELETE FROM " + MemoTable::kName +
                                        " WHERE " + MemoTable::att::kId + " = " + stringMemoId + ";";
    beginTransaction();
    if (!sqlite3_->exec(deleteTaggedQuery, nullptr))
    {
        LOG_DBG("Failed to delete associated Tagged rows.")
        rollback();
        return false;
    }
    if (!sqlite3_->exec(deleteMemoQuery, nullptr))
    {
        rollback();
        return false;
    }
    commitTransaction();
    return true;
}

bool Sqlite3Database::deleteTag(const model::Tag& tag)
{
    LOG_DBG("Deleting Tag ...")
    LOG_DBG("Tag with id " << tag.id())
    const auto stringId = std::to_string(tag.id());
    const std::string deleteTagged = "DELETE FROM " + TaggedTable::kName +
                                     " WHERE " + TaggedTable::att::kTagId + " = " + stringId + ";";
    const std::string deleteTag = "DELETE FROM " + TagTable::kName +
                                  " WHERE " + TagTable::att::kId + " = " + stringId + ";";
    beginTransaction();
    if (!sqlite3_->exec(deleteTagged, nullptr))
    {
        LOG_DBG("Failed to delete associated Tagged rows.")
        rollback();
        return false;
    }
    if (!sqlite3_->exec(deleteTag, nullptr))
    {
        rollback();
        return false;
    }
    commitTransaction();
    return true;
}

bool Sqlite3Database::beginTransaction()
{
    return sqlite3_->exec("BEGIN TRANSACTION", nullptr);
}

bool Sqlite3Database::commitTransaction()
{
    return sqlite3_->exec("COMMIT", nullptr);
}

bool Sqlite3Database::rollback()
{
    return sqlite3_->exec("ROLLBACK", nullptr);
}

} // namespace memo