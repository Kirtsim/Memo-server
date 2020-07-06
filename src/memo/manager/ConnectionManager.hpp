#pragma once
#include "memo/Connection.hpp"

#include <set>

namespace memo {
namespace manager {

class ConnectionManager
{
public:
	void start(const Connection::Ptr& iConnection);

  	/// Stop the specified connection.
  	void stop(const Connection::Ptr& iConnection);

  	/// Stop all connections.
  	void stop_all();

    ConnectionManager(const ConnectionManager&) = delete;
    ConnectionManager& operator=(const ConnectionManager&) = delete;

private:
    std::set<Connection::Ptr> connections;
};
} // namespace memo
} // namespace manager

