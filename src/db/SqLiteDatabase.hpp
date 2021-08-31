#pragma once
#include "db/IDatabase.hpp"
#include <memory>

class sqlite3;

namespace memo {

class SQLiteDatabase : public IDatabase
{
public:
    explicit SQLiteDatabase(const std::string& dbFilePath);

    ~SQLiteDatabase() override;

    bool open() override;

    bool close() override;

    bool exec(const std::string& query, const SQLCallback& callback) override;

private:
    std::string dbFilePath_;

    SQLCallback callback_;

    sqlite3* handle_;
};

} // namespace memo
