#pragma once
#include "server/service/IService.hpp"
#include "server/Resources.hpp"

#include <grpcpp/impl/codegen/completion_queue.h>
#include <grpcpp/server.h>

#include <unordered_map>

namespace memo {

class IProcess;

using CompletionQueueUPtr = std::unique_ptr<grpc::ServerCompletionQueue>;

class Server
{
public:
    Server(const std::string& ipAddress, const std::string& port);
    ~Server();

    void run();
private:
    void initialize(const std::string& serverAddress);
    void executeProcess(IProcess* process);

    std::string ipAddress_;
    std::string port_;
    memo::Resources::Ptr resources_;

    std::unique_ptr<grpc::Server> server_;
    CompletionQueueUPtr completionQueue_;
    std::unordered_map<int, IService::Ptr> services_;
};

} // namespace memo
