#pragma once
#include "memo/service/BaseProcess.hpp"
#include "model/TagSvc.grpc.pb.h"

#include <grpcpp/impl/codegen/completion_queue.h>
#include <grpcpp/impl/codegen/async_unary_call.h>

namespace memo {
namespace service {
    class TagSvc;
namespace process {
namespace tag {

class SearchProcess : public BaseProcess<service::TagSvc, model::TagSearchRq, model::TagSearchRs>
{
public:
    static Ptr Create(service::TagSvc& iSvc);

    SearchProcess(service::TagSvc& iSvc);
    ~SearchProcess();

    void init(grpc::ServerCompletionQueue& ioCompletionQueue) override;

    void execute() override;

    Ptr duplicate() const override;
};

} // namespace tag
} // namespace process
} // namespace service
} // namespace memo
