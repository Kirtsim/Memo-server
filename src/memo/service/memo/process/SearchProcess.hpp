#pragma once
#include "memo/service/BaseProcess.hpp"
#include "model/MemoSvc.grpc.pb.h"

#include <grpcpp/impl/codegen/completion_queue.h>
#include <grpcpp/impl/codegen/async_unary_call.h>

namespace memo {
namespace service {
    class MemoSvc;
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
