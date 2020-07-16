#pragma once
#include <boost/asio/ip/tcp.hpp>
#include <boost/array.hpp>

#include <sstream>
#include <memory>

namespace memo {

class Connection
{
public:
    using Ptr = std::shared_ptr<Connection>;
    using SocketPtr = std::shared_ptr<boost::asio::ip::tcp::socket>;

    class Callback
    {
    public:
        virtual ~Callback() = default;
        virtual void receiveData(const std::string& iData,
                                 const std::string& iConnectionId) = 0;

        virtual void onConnectionError(const boost::system::error_code& iErrorCode,
                                       const std::string& iConnectionId) = 0;

        virtual void onDataSent(const std::string& iConnectionId) = 0;
    };

    Connection(const SocketPtr& ioSocket,
               Callback& ioDataReceiver);
    ~Connection();

    void setId(const std::string& iConnectionId);

    /// Start the first asynchronous operation for the connection.
    void open();

    /// Stop all asynchronous operations associated with the connection.
    void close();

    void sendData(const std::vector<boost::asio::const_buffer>& iDataBuffers);

    Connection(const Connection&) = delete;
    Connection& operator=(const Connection&) = delete;
private:
    /// Handle completion of a read operation.
    void handleRead(const boost::system::error_code& iErrorCode,
                    size_t iTransferredBytesCount);

    /// Handle completion of a write operation.
    void handleWrite(const boost::system::error_code& iErrorCode,
                     size_t iTransferredBytesCount);

    SocketPtr socket;
    boost::array<char, 8192> dataBuffer;
    std::stringstream        dataStream;

    Callback& callback;
    std::string id;
};

} // namespace memo
