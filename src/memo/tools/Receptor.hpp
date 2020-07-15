#pragma once
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/io_service.hpp>

namespace memo {
namespace tools {

class Receptor 
{
    using SocketPtr_t = std::shared_ptr<boost::asio::ip::tcp::socket>;
public:
    using Ptr = std::shared_ptr<Receptor>;

    class Callback
    {
    public:
        virtual void acceptIncomingRequest(const SocketPtr_t& ioSocket) = 0;
    };

    struct Address
    {
        std::string address;
        std::string port;
    };

    Receptor(const Address& iAddress, Callback& iCallback);
    ~Receptor();

    bool isOpen();

    // A blocking operation running until all asynchronous operations
    // have finished. Can be stopped by calling close(). While running,
    // there is always at least one asynchronous accept call waiting
    // for new incoming connections.
    void open();

    void close();

    boost::asio::io_service& accessIoService();

private:
    void listen();
    void notify(const boost::system::error_code& iErrorCode);

    boost::asio::io_service ioService;
    boost::asio::ip::tcp::acceptor acceptor;
    Callback&   callback;
    SocketPtr_t socket;
};

} // namespace tools
} // namespace memo
