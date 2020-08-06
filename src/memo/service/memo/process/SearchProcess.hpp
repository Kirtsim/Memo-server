#pragma once
#include "memo/service/Process.hpp"
#include "model/MemoSvc.grpc.pb.h"

#include <grpcpp/impl/codegen/completion_queue.h>
#include <grpcpp/impl/codegen/async_unary_call_impl.h>

namespace memo {
namespace service {
    class MemoSvc;
namespace process {


class SearchProcess : public Process
{
    using ResponseWriter_t = grpc::ServerAsyncResponseWriter<model::MemoSearchRs>;

    enum State { kProcess, kFinish };
public:
    static Ptr Create(MemoSvc& iSvc);

    SearchProcess(MemoSvc& iSvc);
    ~SearchProcess();

    virtual void init(grpc::ServerCompletionQueue& ioCompletionQueue) override;

    void execute() override;

    bool isFinished() const override;

    int serviceId() const override;

    Ptr duplicate() const override;

private:
    grpc::ServerContext context_;
    MemoSvc& svc_;
    ResponseWriter_t writer_;
    model::MemoSearchRq request_;
    model::MemoSearchRs response_;
    State state_ = kProcess;
};

} // namespace process
} // namespace service
} // namespace memo
