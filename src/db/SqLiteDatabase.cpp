#include "db/SqLiteDatabase.hpp"
#include <sqlite3.h>

namespace memo {

namespace {
    /// @brief This is a callback function passed to sqlite3_exec(...). It transforms C-like data
    ///        filtered by the query into C++ equivalent and passes it onto a callback std::function.
    ///        This function is called for every row that the query returns.
    /// @param voidCallback parameter containing the callback function. Needs to be type-casted.
    /// @param columnCount number of columns to be expected in the resulting row.
    /// @param values row values
    /// @param columns column names
    /// @return 0 if the query should continue evaluating or non-0 if it should stop.
    int onRowReturned(void* voidCallback, int columnCount, char** values, char** columns);
} // namespace


SQLiteDatabase::SQLiteDatabase(const std::string& dbFilePath)
    : dbFilePath_(dbFilePath)
{
}

SQLiteDatabase::~SQLiteDatabase()
{
    SQLiteDatabase::close();
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
    if (sqlite3_close(handle_) == SQLITE_OK)
    {
        sqlite3_free(handle_);
        handle_ = nullptr;
        return true;
    }
    return false;
}

bool SQLiteDatabase::isOpen() const
{
    return handle_ != nullptr;
}

bool SQLiteDatabase::exec(const std::string& query, const SQLCallback& callback)
{
    callback_ = callback;
    sqlite3_exec(handle_, query.c_str(), onRowReturned, &callback_, nullptr);
    return false;
}

namespace {

    int onRowReturned(void* voidCallback, int columnCount, char** values, char** columns)
    {
        int returnValue = 0;
        auto callback = voidCallback ? static_cast<SQLCallback*>(voidCallback) : nullptr;
        if (callback)
        {
            std::vector<std::string> rowValues(static_cast<size_t>(columnCount));
            std::vector<std::string> columnNames(static_cast<size_t>(columnCount));
            for (size_t colIdx = 0; colIdx < static_cast<size_t>(columnCount); ++colIdx)
            {
                rowValues[colIdx] = std::string(values[colIdx]);
                columnNames[colIdx] = std::string(columns[colIdx]);
            }
            bool continueQuery = callback->operator()(rowValues, columnNames);
            returnValue = static_cast<int>(continueQuery);
        }
        return returnValue;
    }

} // namespace

} // namespace memo
