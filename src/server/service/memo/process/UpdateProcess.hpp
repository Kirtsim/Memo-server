#pragma once
#include "server/service/BaseProcess.hpp"
#include "MemoSvc.grpc.pb.h"

#include <grpcpp/impl/codegen/completion_queue.h>
#include <grpcpp/impl/codegen/async_unary_call.h>

namespace memo {
    class MemoSvc;
namespace process::memo {

class UpdateProcess : public BaseProcess<MemoSvc, proto::Memo, proto::OperationStatus>
{
public:
    static Ptr Create(MemoSvc& iSvc);

    explicit UpdateProcess(MemoSvc& iSvc);
    ~UpdateProcess() override;

    void init(grpc::ServerCompletionQueue& ioCompletionQueue) override;

    void execute() override;

    Ptr duplicate() const override;
};

} // namespace process::memo
} // namespace memo
