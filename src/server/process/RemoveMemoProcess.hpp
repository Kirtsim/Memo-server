#pragma once
#include "server/process/BaseProcess.hpp"
#include "MemoSvc.grpc.pb.h"

#include <grpcpp/impl/codegen/completion_queue.h>
#include <grpcpp/impl/codegen/async_unary_call.h>

namespace memo {
class MemoService;

class RemoveMemoProcess : public process::BaseProcess<MemoService, proto::RemoveMemoRq, proto::RemoveMemoRs>
{
public:
    static Ptr Create(MemoService& service);

    explicit RemoveMemoProcess(MemoService& service);
    ~RemoveMemoProcess() override;

    void init(grpc::ServerCompletionQueue& completionQueue) override;

    void execute() override;

    Ptr duplicate() const override;
};


} // namespace memo