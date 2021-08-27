#pragma once
#include "server/service/IService.hpp"
#include "server/process/IProcess.hpp"

#include <grpcpp/impl/codegen/completion_queue.h>
#include <grpcpp/impl/codegen/async_unary_call.h>

#include <unordered_map>
#include <memory>

namespace memo {

class Resources;

class BaseService : public memo::IService
{
public:
    BaseService(const std::shared_ptr<Resources>& resources, grpc::ServerCompletionQueue& completionQueue);

    ~BaseService() override;

    BaseService(const BaseService&) = delete;
    BaseService& operator=(const BaseService&) = delete;

    virtual void registerProcesses() = 0;

    /// IService interface
    bool executeProcess(IProcess* process) override;

    void enable() override;

    void disable() override;

    int getId() const override;

protected:
    void registerProcess(const IProcess::Ptr& process);

    std::shared_ptr<Resources> resources_;
    grpc::ServerCompletionQueue& completionQueue_;
    std::unordered_map<IProcess*, IProcess::Ptr> processes_;
};

} // namespace memo
