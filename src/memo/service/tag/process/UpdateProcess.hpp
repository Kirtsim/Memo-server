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

class UpdateProcess : public BaseProcess<service::TagSvc, model::Tag, model::OperationStatus>
{
public:
    static Ptr Create(service::TagSvc& iSvc);

    UpdateProcess(service::TagSvc& iSvc);
    ~UpdateProcess();

    void init(grpc::ServerCompletionQueue& ioCompletionQueue) override;

    void execute() override;

    Ptr duplicate() const override;
};

} // namespace tag
} // namespace process
} // namespace service
} // namespace memo
