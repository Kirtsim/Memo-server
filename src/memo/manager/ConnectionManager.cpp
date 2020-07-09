#include "memo/manager/ConnectionManager.hpp"

#include <iostream>
#include <boost/bind.hpp>

namespace memo {
namespace manager {

void ConnectionManager::start(const Connection::Ptr& iConnection)
{
    std::cout << "[Manager] Starting new connection..." << std::endl;
    connections.insert(iConnection);
    iConnection->start();
}

void ConnectionManager::stop(const Connection::Ptr& iConnection)
{
    std::cout << "[Manager] Stopping connection..." << std::endl;
    connections.erase(iConnection);
    iConnection->stop();
}

void ConnectionManager::stop_all()
{
    std::cout << "[Manager] Stopping all connections..." << std::endl;
    std::for_each (connections.begin(), connections.end(),
                   boost::bind(&Connection::stop, _1));
    connections.clear();
}

} // namespace memo
} // namespace manager

