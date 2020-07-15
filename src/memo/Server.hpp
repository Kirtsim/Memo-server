#pragma once
#include "memo/manager/ConnectionManager.hpp"
#include "memo/tools/RequestHandler.hpp"
#include <memo/tools/Receptor.hpp>
#include "memo/Connection.hpp"
#include "memo/Request.hpp"
#include "memo/Reply.hpp"

#include <boost/asio/signal_set.hpp>

#include <map>

namespace memo {

class Server : private Connection::Callback,
               private tools::Receptor::Callback
{
    using SocketPtr_t = std::shared_ptr<boost::asio::ip::tcp::socket>;

public:
    explicit Server(const std::string& iAddress, 
                    const std::string& iPort,
                    const std::string& iDocRoot);

    // Opens the receptor (blocking call)
    void run();
private:
    // Handle a request to stop the server.
    void handleStop();

    // Connection::Callback methods
    void receiveData(const std::string& iData,
                     const std::string& iConnectionId) override;

    void onConnectionError(const boost::system::error_code& iErrorCode,
                           const std::string& iConnectionId) override;

    void onDataSent(const std::string& iConnectionId) override;

    // Receptor::Callback methods
    void acceptIncomingRequest(const SocketPtr_t& ioSocket) override;


    tools::Receptor receptor;

    // Used to register termination signals
    boost::asio::signal_set signals;

    manager::ConnectionManager connectionManager;
    tools::RequestHandler requestHandler;

    // Cached replies (need to be stored while being sent)
    std::map<std::string, Reply::Ptr> replies;

};

} // namespace memo
