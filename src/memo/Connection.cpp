#include "memo/Connection.hpp"
#include "memo/tools/RequestHandler.hpp"
#include "memo/manager/ConnectionManager.hpp"

#include <iostream>
#include <boost/bind.hpp>

namespace memo{

Connection::Connection(boost::asio::io_service& ioIOService,
                       manager::ConnectionManager& ioConnectionManager, 
                       tools::RequestHandler& ioRequestHandler) :
    socket(ioIOService),
    connectionManager(ioConnectionManager),
    requestHandler(ioRequestHandler)
{}

boost::asio::ip::tcp::socket& Connection::accessSocket()
{
    return socket;
}

void Connection::start()
{
    socket.async_read_some(
            boost::asio::buffer(dataBuffer),
            boost::bind(&Connection::handleRead, shared_from_this(),
                        boost::asio::placeholders::error,
                        boost::asio::placeholders::bytes_transferred));
    std::cout << "[Connection] started." << std::endl;
}

void Connection::stop()
{
    socket.close();
    std::cout << "[Connection] stopped." << std::endl;
}

void Connection::handleRead(const boost::system::error_code& iErrorCode,
                            size_t iTransferredBytesCount)
{
    std::cout << "[Connection] Handling request..." << std::endl;
    if (iErrorCode)
    {
        if (iErrorCode != boost::asio::error::operation_aborted)
            connectionManager.stop(shared_from_this());
        return;
    }

    std::cout << "[Connection] parsing request..." << std::endl;
    boost::tribool aResult;

    boost::tie(aResult, boost::tuples::ignore) =
        requestParser.parse(request,
                            dataBuffer.data(),
                            dataBuffer.data() + iTransferredBytesCount);

    if (aResult)
    {
        std::cout << "[Connection] parsing Success." << std::endl;

        requestHandler.handleRequest(request, reply);
        const auto aHandler = boost::bind(&Connection::handleWrite,
                                          shared_from_this(),
                                          boost::asio::placeholders::error,
                                          boost::asio::placeholders::bytes_transferred);
        boost::asio::async_write(socket, reply.toBuffers(), aHandler);
    }
    else if (!aResult)
    {
        std::cout << "[Connection] parsing Failure;" << std::endl;
        reply = Reply::StockReply(Reply::Status::bad_request);
        const auto& aHandler = boost::bind(&Connection::handleWrite,
                                           shared_from_this(),
                                           boost::asio::placeholders::error,
                                           boost::asio::placeholders::bytes_transferred);
        boost::asio::async_write(socket, reply.toBuffers(), aHandler);
    }
    else
    {
        const auto& aHandler = boost::bind(&Connection::handleRead,
                                           shared_from_this(),
                                           boost::asio::placeholders::error,
                                           boost::asio::placeholders::bytes_transferred);
        socket.async_read_some(boost::asio::buffer(dataBuffer), aHandler);
    }
}

void Connection::handleWrite(const boost::system::error_code& iErrorCode,
                             const size_t iTransferredBytesCount)
{
    std::cout << "[Connection] Transferred bytes: " << iTransferredBytesCount << std::endl;
	if (!iErrorCode)
    {
        std::cout << "[Connection] Reply sent successfully." << std::endl;
        connectionManager.stop(shared_from_this());
    }
    else if (iErrorCode != boost::asio::error::operation_aborted)
    {
        std::cout << "[Connection] Failed to send reply." << std::endl;
        connectionManager.stop(shared_from_this());
    }
}

} // namespace memo
