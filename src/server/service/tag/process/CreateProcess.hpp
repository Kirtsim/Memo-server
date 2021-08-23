#pragma once
#include "server/service/BaseProcess.hpp"
#include "TagSvc.grpc.pb.h"

#include <grpcpp/impl/codegen/completion_queue.h>
#include <grpcpp/impl/codegen/async_unary_call.h>

namespace memo {
    class TagSvc;
namespace process::tag {

class CreateProcess : public BaseProcess<TagSvc, proto::Tag, proto::OperationStatus>
{
public:
    static Ptr Create(TagSvc& iSvc);

    explicit CreateProcess(TagSvc& iSvc);
    ~CreateProcess() override;

    void init(grpc::ServerCompletionQueue& ioCompletionQueue) override;

    void execute() override;

    Ptr duplicate() const override;
};

} // namespace process::tag
} // namespace memo
