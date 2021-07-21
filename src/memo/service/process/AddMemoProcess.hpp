#pragma once
#include "memo/service/BaseProcess.hpp"
#include "MemoSvc.grpc.pb.h"

#include <grpcpp/impl/codegen/completion_queue.h>
#include <grpcpp/impl/codegen/async_unary_call.h>

namespace memo {
    class MemoService;

class AddMemoProcess : public service::process::BaseProcess<MemoService, proto::AddMemoRq, proto::AddMemoRs>
{
public:
    static Ptr Create(MemoService& service);

    explicit AddMemoProcess(MemoService& service);
    ~AddMemoProcess() override;

    void init(grpc::ServerCompletionQueue& ioCompletionQueue) override;

    void execute() override;

    Ptr duplicate() const override;
};

} // namespace memo
