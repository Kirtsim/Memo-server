#pragma once
#include "memo/manager/ConnectionManager.hpp"
#include "memo/tools/RequestHandler.hpp"
#include "memo/Connection.hpp"

namespace memo {

class Server
{
public:
    /// Construct the server to listen on the specified TCP address and port, and
    /// serve up files from the given directory.
    explicit Server(const std::string& iAddress, 
                    const std::string& iPort,
                    const std::string& iDocRoot);

    /// Run the server's io_service loop.
    void run();
private:
    /// Initiate an asynchronous accept operation.
    void startAccept();

    /// Handle completion of an asynchronous accept operation.
    void handleAccept(const boost::system::error_code& e);

    /// Handle a request to stop the server.
    void handleStop();

    /// The io_service used to perform asynchronous operations.
    boost::asio::io_service ioService;

    /// The signal_set is used to register for process termination notifications.
    boost::asio::signal_set signals;

    /// Acceptor used to listen for incoming connections.
    boost::asio::ip::tcp::acceptor acceptor;

    /// The connection manager which owns all live connections.
    manager::ConnectionManager connectionManager;

    /// The next connection to be accepted.
    Connection::Ptr newConnection;

    /// The handler for all incoming requests.
    tools::RequestHandler requestHandler;
};

} // namespace memo
