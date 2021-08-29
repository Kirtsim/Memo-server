#include "db/SqLiteDatabase.hpp"
#include <sqlite3.h>

namespace memo {

SQLiteDatabase::~SQLiteDatabase()
{
    sqlite3_close(handle_);
}

bool SQLiteDatabase::open()
{
    return false;
}

bool SQLiteDatabase::close()
{
    return false;
}

bool SQLiteDatabase::exec(const std::string& query, const SQLCallback& callback)
{
    return false;
}
} // namespace memo