#include "memo/Server.hpp"
#include "memo/manager/ConnectionManager.hpp"

#include "logger/logger.hpp"

#include <grpcpp/server_builder.h>

#include <iostream>
#include <exception>

namespace memo {

namespace {
Resources::Ptr ConstructResources(const std::string& iAddress,
                                  const std::string& iPortNumber)
{
    auto aConnectionManager = std::make_unique<manager::ConnectionManager>();
    return Resources::Create(iAddress, iPortNumber, "", std::move(aConnectionManager));
}

} // namespace

Server::Server(const std::string& iIpAddress, const std::string& iPort) :
    ipAddress_(iIpAddress),
    port_(iPort),
    resources_(ConstructResources(ipAddress_, port_))
{}

Server::~Server()
{
    LOG_TRC("[Server] Shutting down server ...");
    server_->Shutdown();
    memoService_->disable();
}

void Server::run()
{
    LOG_TRC("[Server] Launching server ...");
    std::string serverAddress = ipAddress_ + ":" + port_;

    grpc::ServerBuilder builder;

    builder.AddListeningPort(serverAddress, grpc::InsecureServerCredentials());

    auto completionQueue = builder.AddCompletionQueue();

    memoService_ = std::make_unique<memo::service::MemoSvc>(resources_, std::move(completionQueue));
    builder.RegisterService(memoService_.get());

    server_ = builder.BuildAndStart();

    LOG_TRC("[Server] Server listening on " << ipAddress_ << ":" << port_);
    memoService_->enable();
    LOG_TRC("[Server] Run complete.");
}
} // namespace memo
