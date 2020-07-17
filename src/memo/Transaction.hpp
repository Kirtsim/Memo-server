#pragma once
#include "memo/tools/RequestHandler.hpp"
#include "memo/Connection.hpp"

#include <boost/asio/ip/tcp.hpp>

namespace memo {
    class Reply;
    class Resources;

namespace manager {
    class ConnectionManager;
} // namespace manager

class Transaction : private Connection::Callback
{
    using SocketPtr_t = std::shared_ptr<boost::asio::ip::tcp::socket>;
    using ResourcesPtr_t = std::shared_ptr<Resources>;

public:
    using Ptr = std::shared_ptr<Transaction>;

    class Callback
    {
    public:
        virtual ~Callback() = default;
        virtual void onTransactionComplete(const std::string& iTxnId) = 0;
        virtual void onTransactionError(const boost::system::error_code& iErrorCode,
                                        const std::string& iTxnId) = 0;
    };

    Transaction(const ResourcesPtr_t& iResources, Callback& iCallback);
    ~Transaction();


    // Creates and opens a connection and returns the connection's id.
    // Throws std::runtime_error if opened second time.
    std::string open(const SocketPtr_t& iSocket);

    // Interrupts and closes the connection. Triggers onTransactionError
    // if no flag is passed (iSilent=true)
    void cancel(bool iSilent=false);


    // ---------------------------------------------------------------
    // Connection::Callback methods
    // ---------------------------------------------------------------
    void receiveData(const std::string& iData,
                     const std::string& iConnectionId) override;

    void onConnectionError(const boost::system::error_code& iErrorCode,
                           const std::string& iConnectionId) override;

    void onDataSent(const std::string& iConnectionId) override;


    Transaction(const Transaction&) = delete;
    Transaction& operator=(const Transaction&) = delete;

private:
    ResourcesPtr_t resources;
    manager::ConnectionManager& connectionManager;
    SocketPtr_t socket;
    std::string connectionId;
    std::unique_ptr<Reply> reply;
    Callback& callback;
    tools::RequestHandler requestHandler;
};

} // namespace memo
