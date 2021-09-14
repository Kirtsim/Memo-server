#pragma once
#include <vector>

namespace memo {

class ISqlite3Wrapper;

namespace model { class Memo; }
namespace model { class Tag; }


bool UpdateMemoTable(const model::Memo& memo, ISqlite3Wrapper& sqlite3);

bool UpdateTagTable(const model::Tag& tag, ISqlite3Wrapper& sqlite3);

bool SelectMemoTagIds(const model::Memo& memo, std::vector<unsigned long>& tagIds, ISqlite3Wrapper& sqlite3);

bool InsertMemoTagIds(const model::Memo& memo, const std::vector<unsigned long>& tagIds, ISqlite3Wrapper& sqlite3);

bool DeleteMemoTagIds(const model::Memo& memo, const std::vector<unsigned long>& tagIds, ISqlite3Wrapper& sqlite3);

} // namespace memo
