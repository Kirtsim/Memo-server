#pragma once
#include "server/service/IService.hpp"
#include "server/Resources.hpp"

#include <grpcpp/impl/codegen/completion_queue.h>
#include <grpcpp/server.h>

#include <unordered_map>

namespace memo {
namespace service {
    class IProcess;
}

using CompletionQueuePtr_t = std::unique_ptr<grpc::ServerCompletionQueue>;

class Server
{
public:
    Server(const std::string& iIpAddress, const std::string& iPort);
    ~Server();

    void run();
private:
    void initialize(const std::string& iServerAddress);
    void executeProcess(service::IProcess* ioProcess);

    std::string ipAddress_;
    std::string port_;
    memo::Resources::Ptr resources_;

    std::unique_ptr<grpc::Server> server_;
    CompletionQueuePtr_t completionQueue_;
    std::unordered_map<int, service::IService::Ptr> services_;
};

} // namespace memo
