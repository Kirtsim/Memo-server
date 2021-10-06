#include "server/Server.hpp"
#include "server/service/MemoService.hpp"
#include "server/service/TagService.hpp"
#include "server/process/IProcess.hpp"
#include "db/Sqlite3Wrapper.hpp"
#include "db/Sqlite3Database.hpp"

#include "logger/logger.hpp"

#include <grpcpp/server_builder.h>
#include <filesystem>

namespace memo {

namespace {
Resources::Ptr ConstructResources(const std::string& address,
                                  const std::string& portNumber)
{
    auto sqlite3Wrapper = std::make_unique<Sqlite3Wrapper>(std::filesystem::current_path().string() + "/test.db");
    auto db = std::make_unique<Sqlite3Database>(std::move(sqlite3Wrapper));
    return Resources::Create(address, portNumber, std::move(db));
}

} // namespace

Server::Server(const std::string& ipAddress, const std::string& port) :
    ipAddress_(ipAddress),
    port_(port),
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
        auto* process = static_cast<IProcess*>(tag);
        executeProcess(process);
    }

    LOG_TRC("[Server] Run complete.");
}

void Server::initialize(const std::string& serverAddress)
{
    grpc::ServerBuilder builder;

    builder.AddListeningPort(serverAddress, grpc::InsecureServerCredentials());

    completionQueue_ = builder.AddCompletionQueue();

    auto memoService = std::make_shared<memo::MemoService>(resources_, *completionQueue_);
    auto tagService = std::make_shared<memo::TagService>(resources_, *completionQueue_);
    services_.insert({ memoService->getId(), memoService });
    services_.insert({ tagService->getId(), tagService });
    builder.RegisterService(memoService.get());
    builder.RegisterService(tagService.get());

    server_ = builder.BuildAndStart();
    memoService->enable();
    tagService->enable();

    LOG_TRC("[Server] Server listening on " << serverAddress);
}

void Server::executeProcess(IProcess* process)
{
    auto it = services_.find(process->serviceId());
    if (it == end(services_))
    {
        delete process;
        return;
    }

    bool processFound = it->second->executeProcess(process);
    if (!processFound)
    {
        delete process;
    }
}

} // namespace memo
