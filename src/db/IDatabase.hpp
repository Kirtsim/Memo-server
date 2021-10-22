#pragma once
#include "model/ModelDefs.hpp"
#include <string>
#include <optional>
#include <vector>

namespace memo {

struct MemoSearchFilter
{
    std::vector<unsigned long> ids;
    std::string exactTitleMatch;
    std::string titlePrefix;
    std::vector<std::string> titleKeywords;
    std::vector<std::string> memoKeywords;
    std::vector<unsigned long> tagIds;
    std::optional<unsigned long> filterFromDate;
    std::optional<unsigned long> filterUntilDate;
    size_t maxCount = 100;
};

struct TagSearchFilter
{
    std::vector<unsigned long> ids;
    std::string exactNameMatch;
    std::string namePrefix;
    std::string nameContains;
    std::vector<int> colors;
    std::vector<unsigned long> memoIds;
    std::optional<unsigned long> filterFromDate;
    std::optional<unsigned long> filterUntilDate;
    size_t maxCount = 100;
};

/// @brief Database for the Memo and Tag database. The database can have different
///        implementations, SQL, noSQL.
class IDatabase
{
public:
    virtual ~IDatabase() = default;

    virtual std::vector<model::MemoPtr> listMemos(const MemoSearchFilter& filter) = 0;

    virtual std::vector<model::TagPtr> listTags(const TagSearchFilter& filter) = 0;

    virtual bool updateMemo(const model::Memo& memo) = 0;

    virtual bool updateTag(const model::Tag& tag) = 0;

    virtual bool insertMemo(const model::Memo& memo) = 0;

    virtual bool insertTag(const model::Tag& tag) = 0;

    virtual bool deleteMemo(const model::Memo& memo) = 0;

    virtual bool deleteTag(const model::Tag& tag) = 0;
};

} // namespace memo
