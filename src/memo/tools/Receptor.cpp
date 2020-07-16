#include "memo/tools/Receptor.hpp"

#include <iostream>
#include <functional>

namespace memo {
namespace tools {

Receptor::Receptor(const Address& iAddress, Callback& iCallback) :
    ioService(),
    acceptor(ioService),
    callback(iCallback),
    socket()
{
    using namespace boost::asio::ip;

    tcp::resolver        aResolver(ioService);
    tcp::resolver::query aQuery(iAddress.address, iAddress.port);
    tcp::endpoint aEndpoint = *aResolver.resolve(aQuery);

    acceptor.open(aEndpoint.protocol());
    acceptor.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
    acceptor.bind(aEndpoint);
    acceptor.listen();
    listen();
}

Receptor::~Receptor()
{
    close();
}

bool Receptor::isOpen()
{
    return !ioService.stopped() && acceptor.is_open();
}

void Receptor::open()
{
    ioService.run();
}

void Receptor::close()
{
    if (acceptor.is_open())
    {
        acceptor.close();
    }
    ioService.stop();
}

boost::asio::io_service& Receptor::accessIoService()
{
    return ioService;
}

void Receptor::listen()
{
    socket = std::make_shared<boost::asio::ip::tcp::socket>(ioService);

    auto aFunction = std::bind(&Receptor::notify, this, std::placeholders::_1);
    acceptor.async_accept(*socket, aFunction);
}


void Receptor::notify(const boost::system::error_code& iErrorCode)
{
    std::cout << "[Receptor] Incoming connection request..." << std::endl;
    if (iErrorCode)
    {
        std::cout << "[Receptor] Received error:\n" << iErrorCode << std::endl;
        return;
    }
    callback.acceptIncomingRequest(socket);
    listen();
}

} // namespace tools
} // namespace memo
