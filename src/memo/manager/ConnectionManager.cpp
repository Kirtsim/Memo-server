#include "memo/manager/ConnectionManager.hpp"
#include <boost/bind.hpp>

namespace memo {
namespace manager {

void ConnectionManager::start(const Connection::Ptr& iConnection)
{
    connections.insert(iConnection);
    iConnection->start();
}

void ConnectionManager::stop(const Connection::Ptr& iConnection)
{
    connections.erase(iConnection);
    iConnection->stop();
}

void ConnectionManager::stop_all()
{
    std::for_each (connections.begin(), connections.end(),
                   boost::bind(&Connection::stop, _1));
    connections.clear();
}

} // namespace memo
} // namespace manager

