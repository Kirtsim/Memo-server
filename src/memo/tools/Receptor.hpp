#pragma once
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/io_service.hpp>

namespace memo {
namespace tools {

class Receptor 
{
    using SocketPtr_t    = std::shared_ptr<boost::asio::ip::tcp::socket>;
    using IoServicePtr_t = std::unique_ptr<boost::asio::io_service>;
    using EndpointPtr_t  = std::unique_ptr<boost::asio::ip::tcp::endpoint>;
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

    static Ptr Create(IoServicePtr_t iIoService, const Address& iAddress, Callback& iCallback);

    ~Receptor();

    void listen();
    void close();

private:
    Receptor(IoServicePtr_t iIoService, const Address& iAddress, Callback& iCallback);

    void notify(const boost::system::error_code& iErrorCode);
    void openAcceptor();

    Callback&      callback;
    SocketPtr_t    socket;
    IoServicePtr_t ioService;
    EndpointPtr_t  endpoint;
    boost::asio::ip::tcp::acceptor acceptor;
};

} // namespace tools
} // namespace memo
