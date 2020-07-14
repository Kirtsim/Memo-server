#include "memo/tools/Receptor.hpp"

#include <boost/asio/placeholders.hpp>

#include <iostream>
#include <exception>
#include <functional>

namespace memo {
namespace tools {

Receptor::Ptr Receptor::Create(IoServicePtr_t iIoService, const Address& iAddress, Callback& iCallback)
{
    if (!iIoService)
        throw std::invalid_argument("IOService is not defined.");
    return Ptr(new Receptor(std::move(iIoService), iAddress, iCallback));
}

Receptor::Receptor(IoServicePtr_t iIoService, const Address& iAddress, Callback& iCallback) :
    callback(iCallback),
    socket(),
    ioService(std::move(iIoService)),
    acceptor(*ioService)
{
    using namespace boost::asio::ip;
    tcp::resolver        aResolver(*ioService);
    tcp::resolver::query aQuery(iAddress.address, iAddress.port);
    endpoint = std::make_unique<tcp::endpoint>(*aResolver.resolve(aQuery));

    acceptor.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
    acceptor.bind(*endpoint);
}

Receptor::~Receptor()
{
    close();
}

void Receptor::listen()
{
    openAcceptor();
    socket = std::make_shared<boost::asio::ip::tcp::socket>(*ioService);

    auto aFunction = std::bind(&Receptor::notify, this, std::placeholders::_1);
    acceptor.async_accept(*socket, aFunction);
}

void Receptor::openAcceptor()
{
    if (!acceptor.is_open())
    {
        acceptor.open(endpoint->protocol());
        acceptor.listen();
    }
}

void Receptor::notify(const boost::system::error_code& iErrorCode)
{
    if (!iErrorCode)
    {
        std::cout << "[Receptor] Received error:\n" << iErrorCode << std::endl;
    }
    callback.acceptIncomingRequest(socket);        
    listen();
}

void Receptor::close()
{
    if (acceptor.is_open())
    {
        acceptor.close();
    }
    socket->cancel();
    socket->close();
}


} // namespace tools
} // namespace memo
