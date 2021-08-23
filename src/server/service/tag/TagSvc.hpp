#pragma once
#include "server/service/BaseService.hpp"
#include "TagSvc.grpc.pb.h"

#include <unordered_map>

namespace memo {
    class Resources;

class SearchProcess;
class SearchByIdProcess;
class CreateProcess;
class DeleteProcess;
class UpdateProcess;

class TagSvc : public proto::TagSvc::AsyncService,
               public memo::BaseService
{
public:
    TagSvc(const std::shared_ptr<Resources>& ioResources, grpc::ServerCompletionQueue& ioCompletionQueue);

    ~TagSvc() override;

    TagSvc(const TagSvc&) = delete;
    TagSvc& operator=(const TagSvc&) = delete;

    grpc::Status Search(grpc::ServerContext* context,
                        const proto::TagSearchRq* request,
                        proto::TagSearchRs* response) override;

    grpc::Status Create(grpc::ServerContext* context,
                        const proto::Tag* request,
                        proto::OperationStatus* response) override;

    grpc::Status Update(grpc::ServerContext* context,
                        const proto::Tag* request,
                        proto::OperationStatus* response) override;

    grpc::Status Delete(grpc::ServerContext* context,
                        const proto::TagName* request,
                        proto::OperationStatus* response) override;

    /// BaseService methods
    void registerProcesses() override;
};

} // namespace memo
