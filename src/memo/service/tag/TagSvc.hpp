#pragma once
#include "memo/service/Service.hpp"
#include "memo/service/Process.hpp"
#include "model/TagSvc.grpc.pb.h"

#include <grpcpp/impl/codegen/completion_queue.h>
#include <grpcpp/impl/codegen/async_unary_call_impl.h>

#include <unordered_map>

namespace memo {
    class Resources;
namespace service {

class SearchProcess;
class SearchByIdProcess;
class CreateProcess;
class DeleteProcess;
class UpdateProcess;

class TagSvc : public model::TagSvc::AsyncService,
               public memo::service::Service
{
public:
    TagSvc(const std::shared_ptr<Resources>& ioResources, grpc::ServerCompletionQueue& ioCompletionQueue);

    ~TagSvc();

    TagSvc(const TagSvc&) = delete;
    TagSvc& operator=(const TagSvc&) = delete;

    grpc::Status Search(grpc::ServerContext* context,
                        const model::TagSearchRq* request,
                        model::TagSearchRs* response) override;

    grpc::Status Create(grpc::ServerContext* context,
                        const model::Tag* request,
                        model::OperationStatus* response) override;

    grpc::Status Update(grpc::ServerContext* context,
                        const model::Tag* request,
                        model::OperationStatus* response) override;

    grpc::Status Delete(grpc::ServerContext* context,
                        const model::TagName* request,
                        model::OperationStatus* response) override;

    /// Service interface
    bool executeProcess(Process* process) override;

    void enable() override;

    void disable() override;

    int getId() const override;

private:
    void registerProcess(Process::Ptr iProcess);

    std::shared_ptr<Resources> resources_;
    grpc::ServerCompletionQueue& completionQueue_;
    std::unordered_map<Process*, Process::Ptr> processes_;
};

} // namespace service
} // namespace memo
