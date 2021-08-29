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

    virtual bool open() = 0;

    virtual bool close() = 0;

    virtual bool exec(const std::string& query, const SQLCallback& callback) = 0;
};

} // namespace memo
