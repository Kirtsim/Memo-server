#include "db/Sqlite3Functions.hpp"
#include "db/ISqlite3Wrapper.hpp"
#include "db/Sqlite3Schema.hpp"
#include "db/IDatabase.hpp"
#include "db/Tools.hpp"
#include "model/Memo.hpp"
#include "model/Tag.hpp"
#include "logger/logger.hpp"
#include <sstream>

namespace memo {

std::vector<model::TagPtr> SelectTags(const std::string& query, ISqlite3Wrapper& sqlite3)
{
    std::vector<model::TagPtr> selection;
    auto callback = [&selection](const std::vector<std::string>& values, const std::vector<std::string>&)
            {
        if (values.size() != 4ul)
            return true;

        // TODO: Here a mapping should be applied and made sure the correct attributes are being returned.
        try
        {
            auto tag = std::make_shared<model::Tag>();
            tag->setId(std::stoul(values[0]));
            tag->setName(values[1]);
            int colorValue = std::stoi(values[2]);
            tag->setColor(tools::IntToColor(colorValue));
            tag->setTimestamp(std::stoul(values[3]));
            selection.emplace_back(tag);
            return false;
        }
        catch(const std::invalid_argument& ex)
        {
            LOG_WRN("Failed create Tag from SELECT query.");
            return true;
        }
    };
    const bool success = sqlite3.exec(query, callback);
    if (!success)
        LOG_DBG("Failed to execute query:\n" << query);
    return selection;
}

bool UpdateMemoTable(const model::Memo& memo, ISqlite3Wrapper& sqlite3)
{
    namespace att = MemoTable::att;
    std::stringstream memoCmd;
    memoCmd << "UPDATE " << MemoTable::kName
    << " SET " << att::kTitle << "='" << memo.title() << "', "
               << att::kDescription << "='" << memo.description() << "', "
               << att::kTimestamp   << "=" << memo.timestamp()
    << " WHERE " << att::kId << "=" << memo.id() << ";";
    return sqlite3.exec(memoCmd.str(), nullptr);
}

bool UpdateTagTable(const model::Tag& tag, ISqlite3Wrapper& sqlite3)
{
    namespace att = TagTable::att;

    std::stringstream updateCmd;
    updateCmd << "UPDATE " << TagTable::kName
              << " SET " << att::kName << "='" << tag.name() << "', "
                         << att::kColor << "=" << tools::ColorToInt(tag.color()) << ", "
                         << att::kTimestamp << "=" << tag.timestamp()
              << " WHERE " << att::kId << "=" << tag.id() << ";";

    return sqlite3.exec(updateCmd.str(), nullptr);

}

bool SelectMemoTagIds(const unsigned long memoId, std::vector<unsigned long>& tagIds, ISqlite3Wrapper& sqlite3)
{
    namespace att = TaggedTable::att;
    std::stringstream selectCmd;
    selectCmd << "SELECT " << att::kTagId << " FROM " << TaggedTable::kName
              << " WHERE " << att::kMemoId << "=" << memoId << ";";
    auto selectTagIdsCallback = [&tagIds](const std::vector<std::string>& id, const std::vector<std::string>&)
    {
        tagIds.emplace_back(std::stoul(id.front()));
        return false;
    };
    return sqlite3.exec(selectCmd.str(), selectTagIdsCallback);
}

bool InsertMemoTagIds(const unsigned long memoId, const std::vector<unsigned long>& tagIds, ISqlite3Wrapper& sqlite3)
{
    namespace att = TaggedTable::att;
    if (tagIds.empty())
        return true;

    std::stringstream insertCmd;
    insertCmd << "INSERT INTO " << TaggedTable::kName << "(" << att::kMemoId << ", " << att::kTagId << ") "
              << "VALUES (" << memoId << "," << tagIds.front() << ")";
    for (auto i = 1ul; i < tagIds.size(); ++i)
        insertCmd << ", (" << memoId << "," << tagIds[i] << ")";
    insertCmd << ";";
    return sqlite3.exec(insertCmd.str(), nullptr);
}

bool DeleteMemoTagIds(const unsigned long memoId, const std::vector<unsigned long>& tagIds, ISqlite3Wrapper& sqlite3)
{
    namespace att = TaggedTable::att;
    if (tagIds.empty())
        return true;

    std::stringstream deleteCmd;
    deleteCmd << "DELETE FROM " << TaggedTable::kName
              << " WHERE " << att::kMemoId << "=" << memoId << " AND " << att::kTagId << " IN (" << tagIds.front();
    for (auto i = 1ul; i < tagIds.size(); ++i)
        deleteCmd << "," << tagIds[i];
    deleteCmd << ");";

    return sqlite3.exec(deleteCmd.str(), nullptr);
}

std::string BuildTagQuery(const TagSearchFilter& filter)
{
    namespace att = TagTable::att;
    std::stringstream query;
    std::vector<std::string> conditions;
    if (!filter.namePrefix.empty())
    {
        std::string condition = att::kName + " LIKE '" + filter.namePrefix + "%'";
        conditions.emplace_back(condition);
    }

    if (!filter.nameContains.empty())
    {
        std::string condition = att::kName + " LIKE '%" + filter.nameContains + "%'";
        conditions.emplace_back(condition);
    }

    if (!filter.colors.empty())
    {
        std::stringstream colorIn;
        colorIn << att::kColor << " IN (" << filter.colors.front();
        for (auto i = 1ul; i < filter.colors.size(); ++i)
            colorIn << "," << filter.colors[i];
        colorIn << ")";
        conditions.emplace_back(colorIn.str());
    }

    if (filter.filterFromDate)
        conditions.emplace_back(att::kTimestamp + " >= " + std::to_string(filter.filterFromDate.value()));
    if (filter.filterUntilDate)
        conditions.emplace_back(att::kTimestamp + " <= " + std::to_string(filter.filterUntilDate.value()));

    if (!filter.memoIds.empty())
    {
        std::stringstream memoIdIn;
        memoIdIn << TaggedTable::att::kMemoId << " IN (" << filter.memoIds.front();
        for (auto i = 1ul; i < filter.memoIds.size(); ++i)
            memoIdIn << "," << filter.memoIds[i];
        memoIdIn << ")";
        conditions.emplace_back(memoIdIn.str());
    }
    const bool requiresJoin = !filter.memoIds.empty();
    if (requiresJoin)
    {
        query << "SELECT " << att::kId << ", " << att::kName << ", " << att::kColor << ", " << att::kTimestamp
              << " FROM " << TagTable::kName
              << " INNER JOIN " << TaggedTable::kName << " ON "
              << TagTable::kName << "." << att::kId << " = "  << TaggedTable::kName << "." << TaggedTable::att::kTagId;
    }
    else
        query << "SELECT * FROM " << TagTable::kName;

    if (!conditions.empty())
        query << " WHERE " << conditions.front();
    for (auto i = 1ul; i < conditions.size(); ++i)
        query << " AND " << conditions[i];

    query << " GROUP BY " << att::kId << ";";
    return query.str();
}

} // namespace memo
