#pragma once
#include "memo/service/BaseService.hpp"
#include "model/TagSvc.grpc.pb.h"

#include <unordered_map>

namespace memo {
    class Resources;
namespace service {

class SearchProcess;
class SearchByIdProcess;
class CreateProcess;
class DeleteProcess;
class UpdateProcess;

class TagSvc : public model::TagSvc::AsyncService,
               public memo::service::BaseService
{
public:
    TagSvc(const std::shared_ptr<Resources>& ioResources, grpc::ServerCompletionQueue& ioCompletionQueue);

    ~TagSvc();

    TagSvc(const TagSvc&) = delete;
    TagSvc& operator=(const TagSvc&) = delete;

    grpc::Status Search(grpc::ServerContext* context,
                        const model::TagSearchRq* request,
                        model::TagSearchRs* response) override;

    grpc::Status Create(grpc::ServerContext* context,
                        const model::Tag* request,
                        model::OperationStatus* response) override;

    grpc::Status Update(grpc::ServerContext* context,
                        const model::Tag* request,
                        model::OperationStatus* response) override;

    grpc::Status Delete(grpc::ServerContext* context,
                        const model::TagName* request,
                        model::OperationStatus* response) override;

    /// BaseService methods
    void registerProcesses() override;
};

} // namespace service
} // namespace memo
