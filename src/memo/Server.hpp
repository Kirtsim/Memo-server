#pragma once
#include "memo/manager/ConnectionManager.hpp"
#include "memo/tools/Receptor.hpp"
#include "memo/Transaction.hpp"

#include <boost/asio/signal_set.hpp>

#include <map>

namespace memo {

class Server : private tools::Receptor::Callback,
               public Transaction::Callback
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

    void removeTransaction(const std::string& iTxnId);

    // ---------------------------------------------------------------
    // Receptor::Callback methods
    // ---------------------------------------------------------------
    void acceptIncomingRequest(const SocketPtr_t& ioSocket) override;

    // ---------------------------------------------------------------
    // Transaction::Callback methods
    // ---------------------------------------------------------------
    void onTransactionComplete(const std::string& iTxnId) override;
    void onTransactionError(const boost::system::error_code& iErrorCode,
                            const std::string& iTxnId) override;


    // ---------------------------------------------------------------
    // Attributes
    // ---------------------------------------------------------------

    const std::string documentRoot;

    tools::Receptor receptor;

    // Used to register termination signals
    boost::asio::signal_set signals;

    manager::ConnectionManager connectionManager;

    // Cached replies (need to be stored while being sent)
    std::map<std::string, Transaction::Ptr> transactions;

};

} // namespace memo
