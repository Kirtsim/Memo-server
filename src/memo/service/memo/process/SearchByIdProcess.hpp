#pragma once

#include "memo/service/memo/process/BaseProcess.hpp"
#include "model/MemoSvc.grpc.pb.h"

#include <grpcpp/impl/codegen/completion_queue.h>
#include <grpcpp/impl/codegen/async_unary_call_impl.h>

namespace memo {
namespace service {
namespace process {


class SearchByIdProcess : public BaseProcess<model::IdList, model::MemoSearchRs>
{
public:
    static Ptr Create(MemoSvc& iSvc);

    SearchByIdProcess(MemoSvc& iSvc);
    ~SearchByIdProcess();

    void init(grpc::ServerCompletionQueue& ioCompletionQueue) override;

    void execute() override;

    Ptr duplicate() const override;
};

} // namespace process
} // namespace service
} // namespace memo
