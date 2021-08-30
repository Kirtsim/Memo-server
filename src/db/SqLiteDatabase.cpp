#include "db/SqLiteDatabase.hpp"
#include <sqlite3.h>

namespace memo {

SQLiteDatabase::SQLiteDatabase(const std::string& dbFilePath)
    : dbFilePath_(dbFilePath)
{
}

SQLiteDatabase::~SQLiteDatabase()
{
    sqlite3_close(handle_);
}

bool SQLiteDatabase::open()
{
    int returnCode = SQLITE_OK;
    if (!handle_)
    {
        returnCode = sqlite3_open(dbFilePath_.c_str(), &handle_);
    }
    return (returnCode == SQLITE_OK && handle_);
}

bool SQLiteDatabase::close()
{
    return sqlite3_close(handle_) == SQLITE_OK;
}

bool SQLiteDatabase::exec(const std::string& query, const SQLCallback& callback)
{
    return false;
}
} // namespace memo
