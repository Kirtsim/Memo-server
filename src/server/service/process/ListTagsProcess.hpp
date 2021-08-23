#pragma once
#include "server/service/BaseProcess.hpp"
#include "TagSvc.grpc.pb.h"

#include <grpcpp/impl/codegen/completion_queue.h>
#include <grpcpp/impl/codegen/async_unary_call.h>


namespace memo {
    class TagService;

class ListTagsProcess : public process::BaseProcess<TagService, proto::ListTagsRq, proto::ListTagsRs>
{
public:
    static Ptr Create(TagService& service);

    explicit ListTagsProcess(TagService& service);
    ~ListTagsProcess() override;

    void init(grpc::ServerCompletionQueue& ioCompletionQueue) override;

    void execute() override;

    Ptr duplicate() const override;
};

} // namespace memo
