#pragma once
#include "server/service/BaseService.hpp"
#include "TagSvc.grpc.pb.h"
#include <memory>

namespace memo {

class TagService : public proto::TagService::AsyncService,
                   public BaseService
{
public:
    TagService(const std::shared_ptr<Resources>& resources, grpc::ServerCompletionQueue& completionQueue);

    ~TagService() override;
    TagService& operator=(const TagService&) = delete;

    grpc::Status ListTags(grpc::ServerContext* context,
                          const proto::ListTagsRq* request,
                          proto::ListTagsRs* response) override;

    grpc::Status AddTag(grpc::ServerContext* context,
                        const proto::AddTagRq* request,
                        proto::AddTagRs* response) override;

    grpc::Status UpdateTag(grpc::ServerContext* context,
                           const proto::UpdateTagRq* request,
                           proto::UpdateTagRs* response) override;

    grpc::Status RemoveTag(grpc::ServerContext* context,
                           const proto::RemoveTagRq* request,
                           proto::RemoveTagRs* response) override;

    void registerProcesses() override;

};


} // namespace memo
