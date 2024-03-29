#include "db/Sqlite3Functions.hpp"
#include "db/ISqlite3Wrapper.hpp"
#include "db/Sqlite3Schema.hpp"
#include "db/IDatabase.hpp"
#include "db/Tools.hpp"
#include "model/Memo.hpp"
#include "model/Tag.hpp"
#include "logger/Logger.hpp"
#include <set>
#include <sstream>

namespace memo {

std::vector<std::vector<std::string>> SelectRows(const std::string& query, ISqlite3Wrapper& sqlite3)
{
    std::vector<std::vector<std::string>> selection;
    auto callback = [&selection](const std::vector<std::string>& values, const std::vector<std::string>&)
    {
        selection.emplace_back(values);
        return false;
    };
    const bool success = sqlite3.exec(query, callback);
    if (!success)
        LOG_DBG("Failed to execute query:\n" << query)
    return selection;
}

unsigned long SelectMemoId(const std::string& title, ISqlite3Wrapper& sqlite3)
{
    const std::string query = "SELECT " + MemoTable::att::kId + " FROM " + MemoTable::kName + " WHERE " +
                              MemoTable::att::kTitle + " LIKE '" + tools::EscapeSingleQuotes(title) + "';";
    const auto selection = SelectRows(query, sqlite3);
    if (selection.empty() || selection.front().empty())
        return -1ul;
    try
    {
        return std::stoul(selection.front().front());
    }
    catch (const std::invalid_argument&)
    {
        LOG_DBG("(SelectMemoId) Conversion to unsigned long failed for value " << selection.front().front())
    }
    return -1ul;
}

bool UpdateMemoTable(const model::Memo& memo, ISqlite3Wrapper& sqlite3)
{
    namespace att = MemoTable::att;
    const auto title = tools::EscapeSingleQuotes(memo.title());
    const auto description = tools::EscapeSingleQuotes(memo.description());
    std::stringstream memoCmd;
    memoCmd << "UPDATE " << MemoTable::kName
    << " SET " << att::kTitle << "='" << title << "', "
               << att::kDescription << "='" << description << "', "
               << att::kTimestamp   << "=" << memo.timestamp()
    << " WHERE " << att::kId << "=" << memo.id() << ";";
    return sqlite3.exec(memoCmd.str(), nullptr);
}

bool UpdateTagTable(const model::Tag& tag, ISqlite3Wrapper& sqlite3)
{
    namespace att = TagTable::att;

    std::stringstream updateCmd;
    updateCmd << "UPDATE " << TagTable::kName
              << " SET " << att::kName << "='" << tools::EscapeSingleQuotes(tag.name()) << "', "
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

bool InsertMemo(const model::Memo& memo, ISqlite3Wrapper& sqlite3)
{
    namespace att = MemoTable::att;

    const std::string title = tools::EscapeSingleQuotes(memo.title());
    const std::string description = tools::EscapeSingleQuotes(memo.description());
    std::stringstream insertCmd;
    insertCmd << "INSERT INTO " << MemoTable::kName << " (" << att::kTitle << ", " << att::kDescription << ", "
              << att::kTimestamp << ") "
              << "VALUES ('" << title << "', '" << description << "', " << memo.timestamp() << ");";

    return sqlite3.exec(insertCmd.str(), nullptr);
}

bool InsertTag(const model::Tag& tag, ISqlite3Wrapper& sqlite3)
{
    namespace att = TagTable::att;

    std::stringstream insertCmd;
    const auto tagName = tools::EscapeSingleQuotes(tag.name());
    const int colorValue = tools::ColorToInt(tag.color());
    insertCmd << "INSERT INTO " << TagTable::kName << " (" << att::kName << ", " << att::kTimestamp << ", "
              << att::kColor << ") "
              << "VALUES ('" << tagName << "', " << tag.timestamp() << ", " << colorValue << ");";
    return sqlite3.exec(insertCmd.str(), nullptr);
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

namespace {
    std::string ConstructContainsKeysCondition(const std::string& attribute, const std::vector<std::string>& keywords);

    template<typename ItemType>
    std::string AttributeInCondition(const std::string& attribute, const std::vector<ItemType>& items);
}

std::string BuildMemoQuery(const MemoSearchFilter filter)
{
    namespace att = MemoTable::att;
    std::stringstream query;
    std::vector<std::string> conditions;

    if (!filter.ids.empty())
        conditions.emplace_back(AttributeInCondition(att::kId, filter.ids));

    const bool isExactTitleMatch = !filter.exactTitleMatch.empty();
    if (isExactTitleMatch)
        conditions.emplace_back(att::kTitle + " LIKE '" + tools::EscapeSingleQuotes(filter.exactTitleMatch) + "'");

    if (!isExactTitleMatch && !filter.titlePrefix.empty())
        conditions.emplace_back(att::kTitle + " LIKE '" + tools::EscapeSingleQuotes(filter.titlePrefix) + "%'");

    if (!isExactTitleMatch && !filter.titleKeywords.empty())
    {
        const auto condition = ConstructContainsKeysCondition(att::kTitle, filter.titleKeywords);
        if (!condition.empty())
            conditions.emplace_back(condition);
    }

    if (!filter.memoKeywords.empty())
    {
        const auto condition = ConstructContainsKeysCondition(att::kDescription, filter.memoKeywords);
        if (!condition.empty())
            conditions.emplace_back(condition);
    }

    if (filter.filterFromDate)
        conditions.emplace_back(att::kTimestamp + " >= " + std::to_string(filter.filterFromDate.value()));
    if (filter.filterUntilDate)
        conditions.emplace_back(att::kTimestamp + " <= " + std::to_string(filter.filterUntilDate.value()));

    query << "SELECT " << att::kId << ", " << att::kTitle << ", " << att::kDescription << ", " << att::kTimestamp
          << " FROM " << MemoTable::kName;

    const bool hasTagIds = !filter.tagIds.empty();
    if (hasTagIds)
    {
        conditions.emplace_back(AttributeInCondition(TaggedTable::att::kTagId, filter.tagIds));

        query << " INNER JOIN " << TaggedTable::kName << " ON "
              << MemoTable::kName << "." << att::kId << " = "  << TaggedTable::kName << "." << TaggedTable::att::kMemoId;
    }

    if (!conditions.empty())
    {
        query << " WHERE " << conditions.front();
        for (auto i = 1ul; i < conditions.size(); ++i)
            query << " AND " << conditions[i];
    }
    query << " GROUP BY " << att::kId << " LIMIT " << filter.maxCount << ";";
    return query.str();
}

std::string BuildTagQuery(const TagSearchFilter& filter)
{
    namespace att = TagTable::att;
    std::stringstream query;
    std::vector<std::string> conditions;
    const bool exactMatch = !filter.exactNameMatch.empty();

    if (!filter.ids.empty())
        conditions.emplace_back(AttributeInCondition(att::kId, filter.ids));

    if (exactMatch)
        conditions.emplace_back(att::kName + " LIKE '" + tools::EscapeSingleQuotes(filter.exactNameMatch) + "'");

    if (!exactMatch && !filter.namePrefix.empty())
    {
        std::string condition = att::kName + " LIKE '" + tools::EscapeSingleQuotes(filter.namePrefix) + "%'";
        conditions.emplace_back(condition);
    }

    if (!exactMatch && !filter.nameContains.empty())
    {
        std::string condition = att::kName + " LIKE '%" + tools::EscapeSingleQuotes(filter.nameContains) + "%'";
        conditions.emplace_back(condition);
    }

    if (!filter.colors.empty())
        conditions.emplace_back(AttributeInCondition(att::kColor, filter.colors));

    if (filter.filterFromDate)
        conditions.emplace_back(att::kTimestamp + " >= " + std::to_string(filter.filterFromDate.value()));
    if (filter.filterUntilDate)
        conditions.emplace_back(att::kTimestamp + " <= " + std::to_string(filter.filterUntilDate.value()));

    if (!filter.memoIds.empty())
        conditions.emplace_back(AttributeInCondition(TaggedTable::att::kMemoId, filter.memoIds));

    query << "SELECT " << att::kId << ", " << att::kName << ", " << att::kColor << ", " << att::kTimestamp
          << " FROM " << TagTable::kName;

    const bool requiresJoin = !filter.memoIds.empty();
    if (requiresJoin)
    {
        query << " INNER JOIN " << TaggedTable::kName << " ON "
              << TagTable::kName << "." << att::kId << " = "  << TaggedTable::kName << "." << TaggedTable::att::kTagId;
    }

    if (!conditions.empty())
        query << " WHERE " << conditions.front();
    for (auto i = 1ul; i < conditions.size(); ++i)
        query << " AND " << conditions[i];

    query << " GROUP BY " << att::kId << " LIMIT " << filter.maxCount << ";";
    return query.str();
}


namespace {

    template<typename ItemType>
    std::string AttributeInCondition(const std::string& attribute, const std::vector<ItemType>& items)
    {
        std::stringstream stream;
        stream << attribute << " IN (";
        if (!items.empty())
            stream << items.front();

        for (auto i = 1ul; i < items.size(); ++i)
            stream << "," << items[i];
        stream << ")";
        return stream.str();
    }

    std::string ConstructContainsKeysCondition(const std::string& attribute, const std::vector<std::string>& keywords)
    {
        std::set<std::string> uniqueKeywords(keywords.begin(), keywords.end());
        uniqueKeywords.erase("");
        if (uniqueKeywords.empty() || attribute.empty())
            return "";

        std::stringstream condition;
        condition << "(" << attribute << " LIKE '%" << tools::EscapeSingleQuotes(*uniqueKeywords.begin()) << "%'";
        uniqueKeywords.erase(uniqueKeywords.begin());

        for (const auto& keyword : uniqueKeywords)
            condition << " OR " << attribute << " LIKE '%" << tools::EscapeSingleQuotes(keyword) << "%'";

        condition << ")";
        return condition.str();
    }
}
} // namespace memo
