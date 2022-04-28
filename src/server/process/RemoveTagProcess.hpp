#pragma once
#include "server/process/BaseProcess.hpp"
#include "TagSvc.grpc.pb.h"

#include <grpcpp/impl/codegen/completion_queue.h>
#include <grpcpp/impl/codegen/async_unary_call.h>

namespace memo {
class TagService;

class RemoveTagProcess : public process::BaseProcess<TagService, proto::RemoveTagRq, proto::RemoveTagRs>
{
public:
    static Ptr Create(TagService& service);

    explicit RemoveTagProcess(TagService& service);
    ~RemoveTagProcess() override;

    void init(grpc::ServerCompletionQueue& completionQueue) override;

    void execute() override;

    Ptr duplicate() const override;
};


} // namespace memo#pragma once
