#pragma once
#include <string>
#include <vector>
#include <functional>


namespace memo {

// Callback function that will be called for each queried row.
// param [0] - queried row values
// param [1] - queried row column names
// return - True if the query evaluation should continue, false if it should be aborted.
using SQLCallback = std::function<bool(const std::vector<std::string>&, const std::vector<std::string>&)>;

class IDatabase
{
public:
    virtual ~IDatabase() = default;

    /// @brief Opens the database for writing, brings it to a state where its data can be queried and
    ///        modified.
    /// @return true if the Database was open, false otherwise.
    virtual bool open() = 0;

    /// @brief Closes the database, deallocates resources. If successful, the database cannot be
    ///        modified or queried.
    /// @return true if the database was closed successfully, false otherwise.
    virtual bool close() = 0;

    /// @brief Executes a give query and calls the callback function once a result is available.
    ///        In case of a search query, the callback function is called for each row in the
    ///        table returned by the query.
    ///        In case of a non-search query, such as CREATE, UPDATE or DELETE, the callback
    ///        function is only called once.
    /// @param query string containing query to be executed.
    /// @param callback callback function that will be called for each result returned.
    /// @return true if execution was successful, false otherwise.
    virtual bool exec(const std::string& query, const SQLCallback& callback) = 0;
};

} // namespace memo
