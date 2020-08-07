#pragma once
#include "memo/service/Service.hpp"
#include "memo/service/Process.hpp"
#include "model/MemoSvc.grpc.pb.h"

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

using CompletionQueuePtr_t = std::unique_ptr<grpc::ServerCompletionQueue>;

class MemoSvc : public model::MemoSvc::AsyncService,
                public memo::service::Service
{
public:
    MemoSvc(const std::shared_ptr<Resources>& ioResources, CompletionQueuePtr_t ioCompletionQueue);

    ~MemoSvc();

    MemoSvc(const MemoSvc&) = delete;
    MemoSvc& operator=(const MemoSvc&) = delete;

    grpc::Status Search(grpc::ServerContext* context,
                        const model::MemoSearchRq* request,
                        model::MemoSearchRs* response) override;

    grpc::Status SearchById(grpc::ServerContext* context,
                            const model::IdList* request,
                            model::MemoSearchRs* response) override;

    grpc::Status Create(grpc::ServerContext* context,
                        const model::Memo* request,
                        model::Id* response) override;

    grpc::Status Update(grpc::ServerContext* context,
                        const model::Memo* request,
                        model::OperationStatus* response) override;

    grpc::Status Delete(grpc::ServerContext* context,
                        const model::Id* request,
                        model::OperationStatus* response) override;

    /// Service interface
    void enable() override;

    void disable() override;

    int getId() const override;

private:
    void registerProcess(Process::Ptr iProcess);

    std::shared_ptr<Resources> resources_;
    CompletionQueuePtr_t completionQueue_;
    std::unordered_map<Process*, Process::Ptr> processes_;
};

} // namespace service
} // namespace memo
