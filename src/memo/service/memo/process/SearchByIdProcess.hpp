#pragma once
#include "memo/service/BaseProcess.hpp"
#include "MemoSvc.grpc.pb.h"

#include <grpcpp/impl/codegen/completion_queue.h>
#include <grpcpp/impl/codegen/async_unary_call.h>

namespace memo::service {
    class MemoSvc;
namespace process::memo {


class SearchByIdProcess : public BaseProcess<service::MemoSvc, proto::IdList, proto::MemoSearchRs>
{
public:
    static Ptr Create(MemoSvc& iSvc);

    explicit SearchByIdProcess(MemoSvc& iSvc);
    ~SearchByIdProcess() override;

    void init(grpc::ServerCompletionQueue& ioCompletionQueue) override;

    void execute() override;

    Ptr duplicate() const override;
};

} // namespace process::memo
} // namespace memo::service
