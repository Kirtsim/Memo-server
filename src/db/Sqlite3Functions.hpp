#pragma once
#include <string>
#include <vector>

namespace memo {

class ISqlite3Wrapper;
struct TagSearchFilter;

namespace model { class Memo; }
namespace model { class Tag; }


bool UpdateMemoTable(const model::Memo& memo, ISqlite3Wrapper& sqlite3);

bool UpdateTagTable(const model::Tag& tag, ISqlite3Wrapper& sqlite3);

bool SelectMemoTagIds(unsigned long memoId, std::vector<unsigned long>& tagIds, ISqlite3Wrapper& sqlite3);

bool InsertMemoTagIds(unsigned long memoId, const std::vector<unsigned long>& tagIds, ISqlite3Wrapper& sqlite3);

bool DeleteMemoTagIds(unsigned long memoId, const std::vector<unsigned long>& tagIds, ISqlite3Wrapper& sqlite3);

std::string BuildTagQuery(const TagSearchFilter& filter);

} // namespace memo
