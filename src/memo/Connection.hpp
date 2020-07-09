#pragma once
#include "memo/Request.hpp"
#include "memo/Reply.hpp"
#include "memo/tools/RequestParser.hpp"

#include <boost/enable_shared_from_this.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/array.hpp>

namespace memo {
namespace manager {
    class ConnectionManager;
} // namespace manager
namespace tools {
    class RequestHandler;
} // namespace tools

class Connection : public boost::enable_shared_from_this<Connection>
{
public:
    using Ptr = boost::shared_ptr<Connection>;

    explicit Connection(boost::asio::io_service& ioIOService,
                        manager::ConnectionManager& ioConnectionManager,
                        tools::RequestHandler& ioRequestHandler);

    boost::asio::ip::tcp::socket& accessSocket();

    /// Start the first asynchronous operation for the connection.
    void start();

    /// Stop all asynchronous operations associated with the connection.
    void stop();

    Connection(const Connection&) = delete;
    Connection& operator=(const Connection&) = delete;
private:
    /// Handle completion of a read operation.
    void handleRead(const boost::system::error_code& iErrorCode,
                    size_t iTransferredBytesCount);

    /// Handle completion of a write operation.
    void handleWrite(const boost::system::error_code& iErrorCode,
                     size_t iTransferredBytesCount);

    boost::asio::ip::tcp::socket socket;
    boost::array<char, 8192>     dataBuffer;
    manager::ConnectionManager&  connectionManager;
    tools::RequestHandler&       requestHandler;

    Request request;
    Reply   reply;
    tools::RequestParser requestParser;
};

} // namespace memo
