#pragma once
#include "memo/service/Service.hpp"
#include "memo/service/Process.hpp"

#include <grpcpp/impl/codegen/completion_queue.h>
#include <grpcpp/impl/codegen/async_unary_call_impl.h>

#include <unordered_map>

namespace memo {
    class Resources;
namespace service {

class BaseService : public memo::service::Service
{
public:
    BaseService(const std::shared_ptr<Resources>& ioResources, grpc::ServerCompletionQueue& ioCompletionQueue);

    virtual ~BaseService();

    BaseService(const BaseService&) = delete;
    BaseService& operator=(const BaseService&) = delete;

    virtual void registerProcesses() = 0;

    /// Service interface
    bool executeProcess(Process* process) override;

    void enable() override;

    void disable() override;

    int getId() const override;

protected:
    void registerProcess(Process::Ptr iProcess);

    std::shared_ptr<Resources> resources_;
    grpc::ServerCompletionQueue& completionQueue_;
    std::unordered_map<Process*, Process::Ptr> processes_;
};

} // namespace service
} // namespace memo
