#include "memo/Transaction.hpp"
#include "memo/Resources.hpp"
#include "memo/Request.hpp"
#include "memo/Reply.hpp"
#include "memo/tools/RequestParser.hpp"
#include "memo/manager/ConnectionManager.hpp"

#include <iostream>

namespace memo {

Transaction::Transaction(const ResourcesPtr_t& iResources,
                         Callback& iCallback) :
    resources(iResources),
    connectionManager(resources->getConnectionManager()),
    callback(iCallback),
    requestHandler(resources->getDocumentRoot())
{}

// Required to be explicitly declared/defined. Otherwise forward declaring
// memo::Reply won't work when using std::unique_ptr. o_O
Transaction::~Transaction() = default;

std::string Transaction::open(const SocketPtr_t& iSocket)
{
    if (socket)
        throw std::runtime_error("Transaction has already initiated a connection.");
    socket = iSocket;
    connectionId = connectionManager.openConnection(iSocket, *this);
    return connectionId;
}

void Transaction::cancel(bool iSilent)
{
    using namespace boost::system;
    if (!socket)
        return;

    connectionManager.closeConnection(connectionId);
    if (!iSilent)
        callback.onTransactionError(errc::make_error_code(errc::connection_aborted), connectionId);
}


// ###############################################################
// # Connection::Callback methods
// ###############################################################

void Transaction::receiveData(const std::string& iData,
                              const std::string& iConnectionId)
{
    std::cout << "[Transaction] Received data:\n" << iData << std::endl;
    tools::RequestParser aParser;
    Request aRequest;
    boost::tribool aResult;
    boost::tie(aResult, boost::tuples::ignore) =
        aParser.parse(aRequest, iData.data(), iData.data() + iData.size());

    auto& aConnection = connectionManager.getConnectionById(iConnectionId);

    reply = std::make_unique<Reply>();
    if (!aResult || aResult == boost::indeterminate)
    {
        std::cout << "[Transaction] Failed to parse incoming request." << std::endl;
        reply = std::make_unique<Reply>(Reply::StockReply(Reply::Status::bad_request));
        aConnection.sendData(reply->toBuffers());
        return;
    }
    requestHandler.handleRequest(aRequest, *reply);
    aConnection.sendData(reply->toBuffers());
}

void Transaction::onConnectionError(const boost::system::error_code& iErrorCode,
                               const std::string& iConnectionId)
{
    std::cout << "[Transaction] Connection error code: " << iErrorCode << std::endl;
    connectionManager.closeConnection(iConnectionId);
    callback.onTransactionError(iErrorCode, iConnectionId);
}

void Transaction::onDataSent(const std::string& iConnectionId)
{
    connectionManager.closeConnection(iConnectionId);
    callback.onTransactionComplete(iConnectionId);
}

} // namespace memo
