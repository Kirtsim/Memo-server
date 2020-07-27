#include "memo/Connection.hpp"
#include "logger/logger.hpp"

#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio/write.hpp>
#include <boost/asio/placeholders.hpp>

namespace memo {

Connection::Connection(const SocketPtr& ioSocket,
                       Callback& ioCallback) :
    socket(ioSocket),
    callback(ioCallback)
{}

Connection::~Connection()
{
    close();
}

void Connection::setId(const std::string& iConnectionId)
{
    id = iConnectionId;
}

void Connection::open()
{
    LOG_TRC("[Connection] Opening...");
    socket->async_read_some(
            boost::asio::buffer(dataBuffer),
            boost::bind(&Connection::handleRead, this,
                        boost::asio::placeholders::error,
                        boost::asio::placeholders::bytes_transferred));
    LOG_TRC("[Connection] started.");
}

void Connection::close()
{
    if (socket->is_open())
    {
        socket->close();
        LOG_TRC("[Connection] stopped.");
    }
}

namespace {
bool DataReadComplete(const std::string& iData)
{
    return iData.empty() || iData.rfind("\r\n\r\n") != std::string::npos;
}
}

void Connection::handleRead(const boost::system::error_code& iErrorCode,
                            size_t iTransferredBytesCount)
{
    LOG_TRC("[Connection] Handling request...");

    auto aData = std::string(dataBuffer.data(), iTransferredBytesCount);
    dataStream << aData;

    if (DataReadComplete(aData))
    {
        LOG_TRC("[Connection] Reading request data complete.");
        callback.receiveData(dataStream.str(), id);
        return;
    }

    if (!iErrorCode)
    {
        const auto& aHandler = boost::bind(&Connection::handleRead, this,
                                           boost::asio::placeholders::error,
                                           boost::asio::placeholders::bytes_transferred);

        socket->async_read_some(boost::asio::buffer(dataBuffer), aHandler);
        return;
    }

    callback.onConnectionError(iErrorCode, id);
}

void Connection::sendData(const std::vector<boost::asio::const_buffer>& iDataBuffers)
{
    const auto aHandler = boost::bind(&Connection::handleWrite, this,
                                      boost::asio::placeholders::error,
                                      boost::asio::placeholders::bytes_transferred);
    boost::asio::async_write(*socket, iDataBuffers, aHandler);
}

void Connection::handleWrite(const boost::system::error_code& iErrorCode,
                             const size_t iTransferredBytesCount)
{
    LOG_INF("[Connection] Transferred bytes: " << iTransferredBytesCount);
    if (iErrorCode)
    {
        LOG_WRN("[Connection] Failed to send reply.");
        callback.onConnectionError(iErrorCode, id);
    }
    LOG_TRC("[Connection] Reply sent successfully.");
    callback.onDataSent(id);
}

} // namespace memo
