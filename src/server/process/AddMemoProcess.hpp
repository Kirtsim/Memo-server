#pragma once
#include "server/process/BaseProcess.hpp"
#include "MemoSvc.grpc.pb.h"

#include <grpcpp/impl/codegen/completion_queue.h>
#include <grpcpp/impl/codegen/async_unary_call.h>

namespace memo {
    class MemoService;

class AddMemoProcess : public process::BaseProcess<MemoService, proto::AddMemoRq, proto::AddMemoRs>
{
public:
    static Ptr Create(MemoService& service);

    explicit AddMemoProcess(MemoService& service);
    ~AddMemoProcess() override;

    void init(grpc::ServerCompletionQueue& completionQueue) override;

    void execute() override;

    Ptr duplicate() const override;
};

} // namespace memo
