#include "memo/Server.hpp"
#include "memo/tools/RequestParser.hpp"
#include <iostream>

#include <boost/bind.hpp>
#include <boost/asio/placeholders.hpp>

namespace memo {

Server::Server(const std::string& iAddress, 
               const std::string& iPort,
               const std::string& iDocRoot) :
    ioService(),
    signals(ioService),
    acceptor(ioService),
    connectionManager(),
    nextReceiver(),
    requestHandler(iDocRoot)
{
  // Register to handle the signals that indicate when the server should exit.
  // It is safe to register for the same signal multiple times in a program,
  // provided all registration for the specified signal is made through Asio.
    std::cout << "Starting Server on address: " << iAddress << ":" << iPort << std::endl;
    signals.add(SIGINT);
    signals.add(SIGTERM);
#if defined(SIGQUIT)
    signals.add(SIGQUIT);
#endif // defined(SIGQUIT)
    signals.async_wait(boost::bind(&Server::handleStop, this));

    // Open the acceptor with the option to reuse the address (i.e. SO_REUSEADDR).
    boost::asio::ip::tcp::resolver aResolver(ioService);
    boost::asio::ip::tcp::resolver::query aQuery(iAddress, iPort);
    boost::asio::ip::tcp::endpoint aEndpoint = *aResolver.resolve(aQuery);
    acceptor.open(aEndpoint.protocol());
    acceptor.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
    acceptor.bind(aEndpoint);
    acceptor.listen();

    startAccept();
}

void Server::run()
{
    // The io_service::run() call will block until all asynchronous operations
    // have finished. While the server is running, there is always at least one
    // asynchronous operation outstanding: the asynchronous accept call waiting
    // for new incoming connections.
    ioService.run();
}

void Server::startAccept()
{
    nextReceiver.reset(new boost::asio::ip::tcp::socket(ioService));
    acceptor.async_accept(*nextReceiver,
                          boost::bind(&Server::handleAccept, this,
                                      boost::asio::placeholders::error));
}

void Server::handleAccept(const boost::system::error_code& iErrorCode)
{
    // Check whether the server was stopped by a signal before this completion
    // handler had a chance to run.
    if (!acceptor.is_open())
        return;

    if (!iErrorCode)
        connectionManager.openConnection(nextReceiver, *this);
    startAccept();
}

void Server::handleStop()
{
    // The server is stopped by cancelling all outstanding asynchronous
    // operations. Once all operations have finished the io_service::run() call
    // will exit.
    acceptor.close();
    connectionManager.closeAll();
    std::cout << "Server shutdown" << std::endl;
}

void Server::receiveData(const std::string& iData,
                         const std::string& iConnectionId)
{
    std::cout << "[Server] Received data:\n" << iData << std::endl;
    tools::RequestParser aParser;
    Request aRequest;
    boost::tribool aResult;
    boost::tie(aResult, boost::tuples::ignore) =
        aParser.parse(aRequest, iData.data(), iData.data() + iData.size());

    Reply::Ptr aReply = replies.insert({iConnectionId, std::make_shared<Reply>()}).first->second;
    auto& aConnection = connectionManager.getConnectionById(iConnectionId);

    if (!aResult || aResult == boost::indeterminate)
    {
        std::cout << "[Server] Failed to parse incoming request." << std::endl;
        *aReply = Reply::StockReply(Reply::Status::bad_request);
        aConnection.sendData(aReply->toBuffers());
        return;
    }
    requestHandler.handleRequest(aRequest, *aReply);
    aConnection.sendData(aReply->toBuffers());
}

void Server::onConnectionError(const boost::system::error_code& iErrorCode,
                               const std::string& iConnectionId)
{
    std::cout << "Connection error code: " << iErrorCode << std::endl;
    connectionManager.closeConnection(iConnectionId);
}

void Server::onDataSent(const std::string& iConnectionId)
{
    connectionManager.closeConnection(iConnectionId);
    auto aIt = replies.find(iConnectionId);
    if (aIt != std::end(replies))
            replies.erase(aIt);
}


} // namespace memo
