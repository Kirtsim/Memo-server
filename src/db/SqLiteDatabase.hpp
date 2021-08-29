#pragma once
#include "db/IDatabase.hpp"

class sqlite3;

namespace memo {

class SQLiteDatabase : public IDatabase
{
public:
    ~SQLiteDatabase() override;

    bool open() override;

    bool close() override;

    bool exec(const std::string& query, const SQLCallback& callback) override;

private:
    SQLCallback callback_;

    sqlite3* handle_;
};

} // namespace memo
