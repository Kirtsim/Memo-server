#include "memo/Server.hpp"
#include "memo/service/memo/MemoSvc.hpp"
#include "memo/service/tag/TagSvc.hpp"
#include "memo/service/Process.hpp"

#include "logger/logger.hpp"

#include <grpcpp/server_builder.h>

#include <iostream>
#include <exception>

namespace memo {

namespace {
Resources::Ptr ConstructResources(const std::string& iAddress,
                                  const std::string& iPortNumber)
{
    return Resources::Create(iAddress, iPortNumber);
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
    completionQueue_->Shutdown();

    void* ignoredTag;
    bool ignoredOk;
    while (completionQueue_->Next(&ignoredTag, &ignoredOk)) {}
}

void Server::run()
{
    LOG_TRC("[Server] Launching server ...");
    std::string serverAddress = ipAddress_ + ":" + port_;
    initialize(serverAddress);

    void* tag;
    bool  isOk;
    while (completionQueue_->Next(&tag, &isOk))
    {
        if (!tag) continue;
        service::Process* process = static_cast<service::Process*>(tag);
        executeProcess(process);
    }

    LOG_TRC("[Server] Run complete.");
}

void Server::initialize(const std::string iServerAddress)
{
    grpc::ServerBuilder builder;

    builder.AddListeningPort(iServerAddress, grpc::InsecureServerCredentials());

    completionQueue_ = builder.AddCompletionQueue();

    auto memoService = std::make_shared<memo::service::MemoSvc>(resources_, *completionQueue_);
    auto tagService = std::make_shared<memo::service::TagSvc>(resources_, *completionQueue_);
    services_.insert({ memoService->getId(), std::static_pointer_cast<service::Service>(memoService) });
    services_.insert({ tagService->getId(), std::static_pointer_cast<service::Service>(tagService) });
    builder.RegisterService(memoService.get());
    builder.RegisterService(tagService.get());

    server_ = builder.BuildAndStart();
    memoService->enable();
    tagService->enable();

    LOG_TRC("[Server] Server listening on " << iServerAddress);
}

void Server::executeProcess(service::Process* ioProcess)
{
    auto it = services_.find(ioProcess->serviceId());
    if (it == end(services_))
    {
        delete ioProcess;
        return;
    }

    bool processFound = it->second->executeProcess(ioProcess);
    if (!processFound)
    {
        delete ioProcess;
    }
}

} // namespace memo
