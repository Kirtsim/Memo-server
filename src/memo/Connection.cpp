#include "memo/Connection.hpp"

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
    std::cout << "[Connection] Opening..." << std::endl;
    socket->async_read_some(
            boost::asio::buffer(dataBuffer),
            boost::bind(&Connection::handleRead, this,
                        boost::asio::placeholders::error,
                        boost::asio::placeholders::bytes_transferred));
    std::cout << "[Connection] started." << std::endl;
}

void Connection::close()
{
    if (socket->is_open())
    {
        socket->close();
        std::cout << "[Connection] stopped." << std::endl;
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
    std::cout << "[Connection] Handling request..." << std::endl;

    auto aData = std::string(dataBuffer.data(), iTransferredBytesCount);
    dataStream << aData;

    if (DataReadComplete(aData))
    {
        std::cout << "[Connection] Reading request data complete." << std::endl;
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
    std::cout << "[Connection] Transferred bytes: " << iTransferredBytesCount << std::endl;
    if (iErrorCode)
    {
        std::cout << "[Connection] Failed to send reply." << std::endl;
        callback.onConnectionError(iErrorCode, id);
    }
    std::cout << "[Connection] Reply sent successfully." << std::endl;
    callback.onDataSent(id);
}

} // namespace memo
