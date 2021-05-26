#pragma once
#include "memo/service/BaseService.hpp"
#include "model/MemoSvc.grpc.pb.h"

#include <unordered_map>

namespace memo { class Resources; }

namespace memo::service {

class SearchProcess;
class SearchByIdProcess;
class CreateProcess;
class DeleteProcess;
class UpdateProcess;

class MemoSvc : public model::MemoSvc::AsyncService,
                public memo::service::BaseService
{
public:
    MemoSvc(const std::shared_ptr<Resources>& ioResources, grpc::ServerCompletionQueue& ioCompletionQueue);

    ~MemoSvc() override;

    MemoSvc(const MemoSvc&) = delete;
    MemoSvc& operator=(const MemoSvc&) = delete;

    grpc::Status Search(grpc::ServerContext* context,
                        const model::MemoSearchRq* request,
                        model::MemoSearchRs* response) override;

    grpc::Status SearchById(grpc::ServerContext* context,
                            const model::IdList* request,
                            model::MemoSearchRs* response) override;

    grpc::Status Create(grpc::ServerContext* context,
                        const model::Memo* request,
                        model::MemoCreateRs* response) override;

    grpc::Status Update(grpc::ServerContext* context,
                        const model::Memo* request,
                        model::OperationStatus* response) override;

    grpc::Status Delete(grpc::ServerContext* context,
                        const model::Id* request,
                        model::OperationStatus* response) override;

    /// BaseService methods
    void registerProcesses() override;
};

} // namespace memo::service
