#include "memo/service/MemoService.hpp"
#include "memo/service/process/ListMemosProcess.hpp"
#include "memo/service/process/AddMemoProcess.hpp"

#include "logger/logger.hpp"

namespace memo {

MemoService::MemoService(const std::shared_ptr<Resources>& ioResources,
                         grpc::ServerCompletionQueue& ioCompletionQueue)
        : BaseService(ioResources, ioCompletionQueue)
{
    LOG_TRC("[MemoService] MemoService created");
}

MemoService::~MemoService() = default;

grpc::Status MemoService::ListMemos(grpc::ServerContext* context, const proto::ListMemosRq* request,
                                    proto::ListMemosRs* response)
{
    return WithAsyncMethod_ListMemos::ListMemos(context, request, response);
}

grpc::Status MemoService::AddMemo(grpc::ServerContext* context, const proto::AddMemoRq* request,
                                  proto::AddMemoRs* response)
{
    return WithAsyncMethod_AddMemo::AddMemo(context, request, response);
}

grpc::Status MemoService::UpdateMemo(grpc::ServerContext* context, const proto::UpdateMemoRq* request,
                                     proto::UpdateMemoRs* response)
{
    return WithAsyncMethod_UpdateMemo::UpdateMemo(context, request, response);
}

grpc::Status MemoService::RemoveMemo(grpc::ServerContext* context, const proto::RemoveMemoRq* request,
                                     proto::RemoveMemoRs* response)
{
    return WithAsyncMethod_RemoveMemo::RemoveMemo(context, request, response);
}

void MemoService::registerProcesses()
{
    registerProcess(ListMemosProcess::Create(*this));
    registerProcess(AddMemoProcess::Create(*this));

    LOG_INF("[MemoService] Registered " <<  processes_.size() << " processes.");
}

} // namespace memo
