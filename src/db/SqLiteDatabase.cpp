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


Sqlite3Wrapper::Sqlite3Wrapper(const std::string& dbFilePath)
    : dbFilePath_(dbFilePath)
{
}

Sqlite3Wrapper::~Sqlite3Wrapper()
{
    Sqlite3Wrapper::close();
}

bool Sqlite3Wrapper::open()
{
    int returnCode = SQLITE_OK;
    if (!handle_)
    {
        returnCode = sqlite3_open(dbFilePath_.c_str(), &handle_);
        if (returnCode != SQLITE_OK)
            close();
    }
    return (returnCode == SQLITE_OK && handle_);
}

bool Sqlite3Wrapper::close()
{
    if (sqlite3_close(handle_) == SQLITE_OK)
    {
        handle_ = nullptr;
        return true;
    }
    return false;
}

bool Sqlite3Wrapper::isOpen() const
{
    return handle_ != nullptr;
}

bool Sqlite3Wrapper::exec(const std::string& query, const SQLCallback& callback)
{
    callback_ = callback;
    auto returnCode = sqlite3_exec(handle_, query.c_str(), onRowReturned, &callback_, nullptr);
    return returnCode == SQLITE_OK || returnCode == SQLITE_ABORT;
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
