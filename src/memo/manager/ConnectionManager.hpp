#pragma once
#include "memo/Connection.hpp"

#include <map>

namespace memo {
namespace manager {

class ConnectionManager
{
public:
    ConnectionManager() = default;

    Connection& getConnectionById(const std::string& iId);

    std::string openConnection(const Connection::SocketPtr& ioSocket,
                               Connection::Callback& iCallback);

    void closeConnection(const std::string& iConnectionId);

    void closeAll();

    ConnectionManager(const ConnectionManager&) = delete;
    ConnectionManager& operator=(const ConnectionManager&) = delete;

private:
    std::map<std::string, Connection::Ptr> connections;
};
} // namespace memo
} // namespace manager

