#pragma once
#include "server/process/BaseProcess.hpp"
#include "TagSvc.grpc.pb.h"

#include <grpcpp/impl/codegen/completion_queue.h>
#include <grpcpp/impl/codegen/async_unary_call.h>


namespace memo {
    class TagService;

class UpdateTagProcess  : public process::BaseProcess<TagService, proto::UpdateTagRq, proto::UpdateTagRs>
{
public:
    static Ptr Create(TagService& service);

    explicit UpdateTagProcess(TagService& service);
    ~UpdateTagProcess() override;

    void init(grpc::ServerCompletionQueue& completionQueue) override;

    void execute() override;

    Ptr duplicate() const override;
};

} // namespace memo
