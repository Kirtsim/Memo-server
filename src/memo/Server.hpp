#pragma once
#include "memo/manager/ConnectionManager.hpp"
#include "memo/tools/RequestHandler.hpp"
#include "memo/Connection.hpp"
#include "memo/Request.hpp"
#include "memo/Reply.hpp"

#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/signal_set.hpp>

#include <map>

namespace memo {

class Server : private Connection::Callback
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

    // Connection::Callback methods
    void receiveData(const std::string& iData,
                     const std::string& iConnectionId) override;

    void onConnectionError(const boost::system::error_code& iErrorCode,
                           const std::string& iConnectionId) override;

    void onDataSent(const std::string& iConnectionId) override;


    /// The io_service used to perform asynchronous operations.
    boost::asio::io_service ioService;

    /// The signal_set is used to register for process termination notifications.
    boost::asio::signal_set signals;

    /// Acceptor used to listen for incoming connections.
    boost::asio::ip::tcp::acceptor acceptor;

    /// The connection manager which owns all live connections.
    manager::ConnectionManager connectionManager;

    std::shared_ptr<boost::asio::ip::tcp::socket> nextReceiver;

    /// The handler for all incoming requests.
    tools::RequestHandler requestHandler;

    std::map<std::string, Reply::Ptr> replies;
};

} // namespace memo
