#pragma once
#include "server/service/BaseProcess.hpp"
#include "TagSvc.grpc.pb.h"

#include <grpcpp/impl/codegen/completion_queue.h>
#include <grpcpp/impl/codegen/async_unary_call.h>

namespace memo::service {
    class TagSvc;
namespace process::tag {

class DeleteProcess : public BaseProcess<service::TagSvc, proto::TagName, proto::OperationStatus>
{
public:
    static Ptr Create(service::TagSvc& iSvc);

    explicit DeleteProcess(service::TagSvc& iSvc);
    ~DeleteProcess() override;

    void init(grpc::ServerCompletionQueue& ioCompletionQueue) override;

    void execute() override;

    Ptr duplicate() const override;
};

} // namespace process::tag
} // namespace memo::service
