#pragma once
#include "memo/service/BaseProcess.hpp"
#include "model/MemoSvc.grpc.pb.h"

#include <grpcpp/impl/codegen/completion_queue.h>
#include <grpcpp/impl/codegen/async_unary_call.h>

namespace memo::service {
    class MemoSvc;
namespace process::memo {

class CreateProcess : public BaseProcess<service::MemoSvc, model::Memo, model::MemoCreateRs>
{
public:
    static Ptr Create(MemoSvc& iSvc);

    explicit CreateProcess(MemoSvc& iSvc);
    ~CreateProcess() override;

    void init(grpc::ServerCompletionQueue& ioCompletionQueue) override;

    void execute() override;

    Ptr duplicate() const override;
};

} // namespace process::memo
} // namespace memo::service
