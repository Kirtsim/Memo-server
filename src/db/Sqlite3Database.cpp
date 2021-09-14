#include "db/Sqlite3Database.hpp"
#include "db/ISqlite3Wrapper.hpp"
#include "db/Sqlite3Functions.hpp"
#include "db/Sqlite3Schema.hpp"
#include "model/Memo.hpp"
#include "model/Tag.hpp"
#include <sstream>
#include <set>

#include <logger/logger.hpp>

namespace memo {

namespace {

const std::string kLogTag = "[Sqlite3Database] ";

} // namespace

Sqlite3Database::Sqlite3Database(std::unique_ptr<ISqlite3Wrapper> wrapper)
    : sqlite3_(std::move(wrapper))
{
}

std::vector<model::MemoPtr> Sqlite3Database::listMemos(const MemoSearchFilter& filter)
{
    return {};
}

std::vector<model::TagPtr> Sqlite3Database::listTags(const TagSearchFilter& filter)
{
    return {};
}

bool Sqlite3Database::updateMemo(const model::MemoPtr& memo)
{
    LOG_INF(kLogTag << "Updating memo ...")
    if (!memo)
    {
        LOG_WRN(kLogTag << "Tag is NULL. Aborting.")
        return false;
    }

    beginTransaction();
    std::vector<unsigned long> queriedTagIds;
    if (!SelectMemoTagIds(*memo, queriedTagIds, *sqlite3_))
    {
        LOG_WRN(kLogTag << "Update memo FAILURE.")
        rollback();
        return false;
    }

    std::set<unsigned long> oldTagIds(queriedTagIds.begin(), queriedTagIds.end());
    auto checkIdNotPresent = [&oldTagIds](const unsigned long tagId) { return oldTagIds.erase(tagId) == 0; };

    std::vector<unsigned long> tagIdsToAdd;
    std::copy_if(memo->tagIds().begin(), memo->tagIds().end(), std::back_inserter(tagIdsToAdd), checkIdNotPresent);

    std::vector<unsigned long> tagIdsToDelete(oldTagIds.begin(), oldTagIds.end());
    bool failed = !DeleteMemoTagIds(*memo, tagIdsToDelete, *sqlite3_)
               || !InsertMemoTagIds(*memo, tagIdsToAdd, *sqlite3_)
               || !UpdateMemoTable(*memo, *sqlite3_);

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

bool Sqlite3Database::updateTag(const model::TagPtr& tag)
{
    LOG_INF(kLogTag << "Updating tag ...")
    if (!tag)
    {
        LOG_WRN(kLogTag << "Tag is NULL. Aborting.")
        return false;
    }

    beginTransaction();
    if (!UpdateTagTable(*tag, *sqlite3_))
    {
        LOG_WRN(kLogTag << "Update tag FAILURE.")
        rollback();
        return false;
    }
    commitTransaction();
    LOG_INF(kLogTag << "Update tag SUCCESS.")
    return true;
}

bool Sqlite3Database::insertMemo(const model::MemoPtr& memo)
{
    return false;
}

bool Sqlite3Database::insertTag(const model::TagPtr& tag)
{
    return false;
}

bool Sqlite3Database::deleteMemo(const model::MemoPtr& memo)
{
    return false;
}

bool Sqlite3Database::deleteTag(const model::TagPtr& tag)
{
    return false;
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