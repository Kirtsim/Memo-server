#pragma once
#include "server/process/BaseProcess.hpp"
#include "MemoSvc.grpc.pb.h"

#include <grpcpp/impl/codegen/completion_queue.h>
#include <grpcpp/impl/codegen/async_unary_call.h>

namespace memo {
    class MemoService;

class ListMemosProcess : public process::BaseProcess<MemoService, proto::ListMemosRq, proto::ListMemosRs>
{
public:
    static Ptr Create(MemoService& service);

    explicit ListMemosProcess(MemoService& service);
    ~ListMemosProcess() override;

    void init(grpc::ServerCompletionQueue& completionQueue) override;

    void execute() override;

    Ptr duplicate() const override;
};

} // namespace memo
