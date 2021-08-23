#pragma once
#include "server/service/BaseService.hpp"
#include "MemoSvc.grpc.pb.h"

#include <unordered_map>

namespace memo { class Resources; }

namespace memo {

class SearchProcess;
class SearchByIdProcess;
class CreateProcess;
class DeleteProcess;
class UpdateProcess;

class MemoSvc : public proto::MemoSvc::AsyncService,
                public memo::BaseService
{
public:
    MemoSvc(const std::shared_ptr<Resources>& ioResources, grpc::ServerCompletionQueue& ioCompletionQueue);

    ~MemoSvc() override;

    MemoSvc(const MemoSvc&) = delete;
    MemoSvc& operator=(const MemoSvc&) = delete;

    grpc::Status Search(grpc::ServerContext* context,
                        const proto::MemoSearchRq* request,
                        proto::MemoSearchRs* response) override;

    grpc::Status SearchById(grpc::ServerContext* context,
                            const proto::IdList* request,
                            proto::MemoSearchRs* response) override;

    grpc::Status Create(grpc::ServerContext* context,
                        const proto::Memo* request,
                        proto::MemoCreateRs* response) override;

    grpc::Status Update(grpc::ServerContext* context,
                        const proto::Memo* request,
                        proto::OperationStatus* response) override;

    grpc::Status Delete(grpc::ServerContext* context,
                        const proto::Id* request,
                        proto::OperationStatus* response) override;

    /// BaseService methods
    void registerProcesses() override;
};

} // namespace memo
