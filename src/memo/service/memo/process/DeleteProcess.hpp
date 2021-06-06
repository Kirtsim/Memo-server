#pragma once
#include "memo/service/BaseProcess.hpp"
#include "MemoSvc.grpc.pb.h"

#include <grpcpp/impl/codegen/completion_queue.h>
#include <grpcpp/impl/codegen/async_unary_call.h>

namespace memo::service {
    class MemoSvc;
namespace process::memo {

class DeleteProcess : public BaseProcess<service::MemoSvc, proto::Id, proto::OperationStatus>
{
public:
    static Ptr Create(MemoSvc& iSvc);

    explicit DeleteProcess(MemoSvc& iSvc);
    ~DeleteProcess() override;

    void init(grpc::ServerCompletionQueue& ioCompletionQueue) override;

    void execute() override;

    Ptr duplicate() const override;
};

} // namespace process:::memo
} // namespace memo::service
