#pragma once
#include "db/ISqlite3Wrapper.hpp"
#include <memory>

class sqlite3;

namespace memo {

/// @brief Implementation of the SQLite database.
class Sqlite3Wrapper : public ISqlite3Wrapper
{
public:
    explicit Sqlite3Wrapper(const std::string& dbFilePath);

    ~Sqlite3Wrapper() override;

    bool open() override;

    bool close() override;

    bool isOpen() const override;

    bool exec(const std::string& query, const SQLCallback& callback) override;

private:
    std::string dbFilePath_;

    SQLCallback callback_;

    sqlite3* handle_ = nullptr;
};

} // namespace memo
