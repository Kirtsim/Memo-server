#include "memo/Server.hpp"
#include "memo/tools/RequestParser.hpp"

#include <iostream>
#include <exception>

#include <boost/bind.hpp>
#include <boost/asio/placeholders.hpp>

namespace memo {

Server::Server(const std::string& iAddress, 
               const std::string& iPort,
               const std::string& iDocRoot) :
    receptor({ iAddress, iPort }, *this),
    signals(receptor.accessIoService()),
    requestHandler(iDocRoot)
{
    std::cout << "[Server] Starting on address: " << iAddress << ":" << iPort << std::endl;

    signals.add(SIGINT);
    signals.add(SIGTERM);
#if defined(SIGQUIT)
    signals.add(SIGQUIT);
#endif // defined(SIGQUIT)
    signals.async_wait(boost::bind(&Server::handleStop, this));
}

void Server::run()
{
    receptor.open();
}

void Server::handleStop()
{
    connectionManager.closeAll();
    receptor.close();
    std::cout << "[Server] Shutdown" << std::endl;
}

void Server::acceptIncomingRequest(const SocketPtr_t& ioSocket)
{
    if (!receptor.isOpen())
        return;

    connectionManager.openConnection(ioSocket, *this);
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
    std::cout << "[Server] Connection error code: " << iErrorCode << std::endl;
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
