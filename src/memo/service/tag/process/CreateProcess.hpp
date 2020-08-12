#pragma once
#include "memo/service/BaseProcess.hpp"
#include "model/TagSvc.grpc.pb.h"

#include <grpcpp/impl/codegen/completion_queue.h>
#include <grpcpp/impl/codegen/async_unary_call_impl.h>

namespace memo {
namespace service {
    class TagSvc;
namespace process {
namespace tag {

class CreateProcess : public BaseProcess<service::TagSvc, model::Tag, model::OperationStatus>
{
public:
    static Ptr Create(service::TagSvc& iSvc);

    CreateProcess(service::TagSvc& iSvc);
    ~CreateProcess();

    void init(grpc::ServerCompletionQueue& ioCompletionQueue) override;

    void execute() override;

    Ptr duplicate() const override;
};

} // namespace tag
} // namespace process
} // namespace service
} // namespace memo
