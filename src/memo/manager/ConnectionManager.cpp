#include "memo/manager/ConnectionManager.hpp"
#include "logger/logger.hpp"

#include <boost/bind.hpp>

#include <iostream>
#include <exception>

namespace memo {
namespace manager {

Connection& ConnectionManager::getConnectionById(const std::string& iId)
{
    auto aIt = connections.find(iId);
    if (aIt != std::end(connections))
        return *aIt->second;
    throw std::invalid_argument("Connection not found");
}

std::string ConnectionManager::openConnection(const Connection::SocketPtr& ioSocket,
                                              Connection::Callback& iCallback)
{
    auto aConnection = std::make_shared<Connection>(ioSocket, iCallback);
    size_t aPointerAdress = reinterpret_cast<size_t>(aConnection.get());
    std::string aStrId = std::to_string(aPointerAdress);
    connections.insert({ aStrId, aConnection });

    LOG_TRC("[Manager] Opening connection [" << aStrId << "]");
    aConnection->setId(aStrId);
    aConnection->open();
    return aStrId;
}

void ConnectionManager::closeConnection(const std::string& iConnectionId)
{
    LOG_TRC("[Manager] Closing connection [" << iConnectionId << "]...");
    auto aIt = connections.find(iConnectionId);
    if (aIt != std::end(connections))
    {
        aIt->second->close();
        connections.erase(aIt);
    }
}

void ConnectionManager::closeAll()
{
    LOG_TRC("[Manager] Closing all connections...");
    for (auto aId2Connection : connections)
        aId2Connection.second->close();
    connections.clear();
}

} // namespace memo
} // namespace manager

