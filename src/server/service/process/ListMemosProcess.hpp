#pragma once
#include "server/service/BaseProcess.hpp"
#include "MemoSvc.grpc.pb.h"

#include <grpcpp/impl/codegen/completion_queue.h>
#include <grpcpp/impl/codegen/async_unary_call.h>

namespace memo {
    class MemoService;

class ListMemosProcess : public service::process::BaseProcess<MemoService, proto::ListMemosRq, proto::ListMemosRs>
{
public:
    static Ptr Create(MemoService& service);

    explicit ListMemosProcess(MemoService& service);
    ~ListMemosProcess() override;

    void init(grpc::ServerCompletionQueue& ioCompletionQueue) override;

    void execute() override;

    Ptr duplicate() const override;
};

} // namespace memo
