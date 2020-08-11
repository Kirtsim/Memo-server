#pragma once
#include "memo/service/memo/process/BaseProcess.hpp"
#include "memo/service/memo/MemoSvc.hpp"
#include "model/MemoSvc.grpc.pb.h"

#include <grpcpp/impl/codegen/completion_queue.h>
#include <grpcpp/impl/codegen/async_unary_call_impl.h>

namespace memo {
namespace service {
namespace process {
namespace memo {

class SearchProcess : public BaseProcess<service::MemoSvc, model::MemoSearchRq, model::MemoSearchRs>
{
public:
    static Ptr Create(MemoSvc& iSvc);

    SearchProcess(MemoSvc& iSvc);
    ~SearchProcess();

    void init(grpc::ServerCompletionQueue& ioCompletionQueue) override;

    void execute() override;

    Ptr duplicate() const override;
};

} // namespace memo
} // namespace process
} // namespace service
} // namespace memo
