#include "memo/Connection.hpp"
#include "memo/tools/RequestHandler.hpp"
#include "memo/manager/ConnectionManager.hpp"

#include <boost/bind.hpp>

namespace memo{

Connection::Connection(boost::asio::io_service& ioIOService,
                       manager::ConnectionManager& ioConnectionManager, 
                       tools::RequestHandler& ioRequestHandler) :
    socket(ioIOService),
    connectionManager(ioConnectionManager),
    requestHandler(ioRequestHandler)
{
}

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
}

void Connection::stop()
{
	socket.close();
}

void Connection::handleRead(const boost::system::error_code& iErrorCode,
                            size_t iTransformedBytesCount)
{
    if (iErrorCode)
    {
        if (iErrorCode != boost::asio::error::operation_aborted)
            connectionManager.stop(shared_from_this());
        return;
    }

    boost::tribool aResult;
    boost::tie(aResult, boost::tuples::ignore) = 
        requestParser.parse(request, 
                            dataBuffer.data(), 
                            dataBuffer.data() + iTransformedBytesCount);

    if (aResult)
    {
        requestHandler.handleRequest(request, reply);
        const auto& aHandler = boost::bind(&Connection::handleWrite, 
                                           shared_from_this(),
                                           boost::asio::placeholders::error);
        boost::asio::async_write(socket, reply.toBuffers(), aHandler);
    }
    else if (!aResult)
    {
        reply = Reply::StockReply(Reply::Status::bad_request);
        const auto& aHandler = boost::bind(&Connection::handleWrite, 
                                           shared_from_this(),
                                           boost::asio::placeholders::error);
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

void Connection::handleWrite(const boost::system::error_code& iErrorCode)
{
    if (iErrorCode)
    {
        if (iErrorCode != boost::asio::error::operation_aborted)
            connectionManager.stop(shared_from_this());
        return;
    }
    // Initiate graceful connection closure.
    boost::system::error_code ignored_ec;
    socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ignored_ec);
}

} // namespace memo
