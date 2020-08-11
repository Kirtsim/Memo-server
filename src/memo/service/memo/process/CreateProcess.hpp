#pragma once

#include "memo/service/memo/process/BaseProcess.hpp"
#include "model/MemoSvc.grpc.pb.h"

#include <grpcpp/impl/codegen/completion_queue.h>
#include <grpcpp/impl/codegen/async_unary_call_impl.h>

namespace memo {
namespace service {
namespace process {

class CreateProcess : public BaseProcess<service::MemoSvc, model::Memo, model::Id>
{
public:
    static Ptr Create(MemoSvc& iSvc);

    CreateProcess(MemoSvc& iSvc);
    ~CreateProcess();

    void init(grpc::ServerCompletionQueue& ioCompletionQueue) override;

    void execute() override;

    Ptr duplicate() const override;
};

} // namespace process
} // namespace service
} // namespace memo
