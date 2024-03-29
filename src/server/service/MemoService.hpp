#pragma once
#include "server/service/BaseService.hpp"
#include "MemoSvc.grpc.pb.h"
#include <memory>

namespace memo {
class MemoService : public proto::MemoService::AsyncService,
                    public BaseService
{
public:
    MemoService(const std::shared_ptr<Resources>& resources, grpc::ServerCompletionQueue& completionQueue);

    ~MemoService() override;
    MemoService& operator=(const MemoService&) = delete;

    grpc::Status ListMemos(grpc::ServerContext* context,
                        const proto::ListMemosRq* request,
                        proto::ListMemosRs* response) override;

    grpc::Status AddMemo(grpc::ServerContext* context,
                        const proto::AddMemoRq* request,
                        proto::AddMemoRs* response) override;

    grpc::Status UpdateMemo(grpc::ServerContext* context,
                        const proto::UpdateMemoRq* request,
                        proto::UpdateMemoRs* response) override;

    grpc::Status RemoveMemo(grpc::ServerContext* context,
                        const proto::RemoveMemoRq* request,
                        proto::RemoveMemoRs* response) override;

    void registerProcesses() override;
};

} // namespace memo
