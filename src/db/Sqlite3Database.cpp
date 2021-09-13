#include "db/Sqlite3Database.hpp"
#include "db/ISqlite3Wrapper.hpp"
#include "model/Memo.hpp"
#include "model/Tag.hpp"


namespace memo {

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
    return false;
}

bool Sqlite3Database::updateTag(const model::TagPtr& tag)
{
    return false;
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
    return false;
}

bool Sqlite3Database::commitTransaction()
{
    return false;
}

bool Sqlite3Database::rollback()
{
    return false;
}
} // namespace memo