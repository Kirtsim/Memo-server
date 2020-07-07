#include "memo/Server.hpp"
#include <boost/bind.hpp>

namespace memo {

Server::Server(const std::string& iAddress, 
               const std::string& iPort,
               const std::string& iDocRoot) :
    ioService(),
    signals(ioService),
    acceptor(ioService),
    connectionManager(),
    newConnection(),
    requestHandler(iDocRoot)
{
  // Register to handle the signals that indicate when the server should exit.
  // It is safe to register for the same signal multiple times in a program,
  // provided all registration for the specified signal is made through Asio.
    signals.add(SIGINT);
    signals.add(SIGTERM);
#if defined(SIGQUIT)
    signals.add(SIGQUIT);
#endif // defined(SIGQUIT)
    signals.async_wait(boost::bind(&Server::handleStop, this));

    // Open the acceptor with the option to reuse the address (i.e. SO_REUSEADDR).
    boost::asio::ip::tcp::resolver aResolver(ioService);
    boost::asio::ip::tcp::resolver::query aQuery(iAddress, iPort);
    boost::asio::ip::tcp::endpoint aEndpoint = *aResolver.resolve(aQuery);
    acceptor.open(aEndpoint.protocol());
    acceptor.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
    acceptor.bind(aEndpoint);
    acceptor.listen();

    startAccept();
}

void Server::run()
{
    // The io_service::run() call will block until all asynchronous operations
    // have finished. While the server is running, there is always at least one
    // asynchronous operation outstanding: the asynchronous accept call waiting
    // for new incoming connections.
    ioService.run();
}

void Server::startAccept()
{
    newConnection.reset(new Connection(ioService,
                                       connectionManager, 
                                       requestHandler));
    acceptor.async_accept(newConnection->accessSocket(),
                          boost::bind(&Server::handleAccept, this,
                                      boost::asio::placeholders::error));
}

void Server::handleAccept(const boost::system::error_code& iErrorCode)
{
    // Check whether the server was stopped by a signal before this completion
    // handler had a chance to run.
    if (!acceptor.is_open())
        return;

    if (!iErrorCode)
        connectionManager.start(newConnection);
    startAccept();
}

void Server::handleStop()
{
    // The server is stopped by cancelling all outstanding asynchronous
    // operations. Once all operations have finished the io_service::run() call
    // will exit.
    acceptor.close();
    connectionManager.stop_all();
}

} // namespace memo
