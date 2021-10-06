#include "server/service/MemoService.hpp"
#include "server/process/ListMemosProcess.hpp"
#include "server/process/AddMemoProcess.hpp"
#include "server/Resources.hpp"
#include "model/Memo.hpp"
#include "db/IDatabase.hpp"

#include "logger/logger.hpp"

namespace memo {

MemoService::MemoService(const std::shared_ptr<Resources>& resources,
                         grpc::ServerCompletionQueue& completionQueue)
        : BaseService(resources, completionQueue)
{
    LOG_TRC("[MemoService] MemoService created");
}

MemoService::~MemoService() = default;

grpc::Status MemoService::ListMemos(grpc::ServerContext*, const proto::ListMemosRq* request,
                                    proto::ListMemosRs* response)
{
    LOG_TRC("[MemoService] Listing memos ...")
    MemoSearchFilter filter;
    filter.titlePrefix = request->filter().title_starts_with();
    filter.titleKeywords.assign(request->filter().title_contains().begin(), request->filter().title_contains().end());
    filter.memoKeywords.assign(request->filter().contains().begin(), request->filter().contains().end());
    filter.tagIds.assign(request->filter().tagged_by().begin(), request->filter().tagged_by().end());
    if (request->filter().creation_time().IsInitialized())
    {
        if (request->filter().creation_time().start() != -1ul)
            filter.filterFromDate = request->filter().creation_time().start();
        if (request->filter().creation_time().end() != -1ul)
            filter.filterUntilDate = request->filter().creation_time().end();
    }

    auto& db = resources().database();
    auto selection = db.listMemos(filter);
    for (const auto& memo : selection)
    {
        if (!memo)
            continue;
        auto protoMemo = response->add_memos();
        protoMemo->set_id(memo->id());
        protoMemo->set_title(memo->title());
        protoMemo->set_description(memo->description());
        protoMemo->mutable_tag_ids()->Add(memo->tagIds().begin(), memo->tagIds().end());
        protoMemo->set_timestamp(memo->timestamp());
    }
    response->set_request_uuid(request->uuid());
    LOG_TRC("[MemoService] Found " << selection.size() << " Memo results.")
    return grpc::Status::OK;
}

grpc::Status MemoService::AddMemo(grpc::ServerContext*, const proto::AddMemoRq* request,
                                  proto::AddMemoRs* response)
{
    LOG_TRC("[MemoService] Adding Memo titled '" << request->memo().title() << "'.");
    const auto& protoMemo = request->memo();
    model::Memo memo;
    memo.setTitle(protoMemo.title());
    memo.setDescription(protoMemo.description());
    memo.setTimestamp(protoMemo.timestamp());
    std::vector<unsigned long> tagIds(protoMemo.tag_ids().begin(), protoMemo.tag_ids().begin());
    memo.setTagIds(tagIds);

    model::MemoPtr newMemo;
    auto& db = resources().database();
    if (db.insertMemo(memo))
    {
        auto filter = MemoSearchFilter();
        filter.exactTitleMatch = memo.title();
        auto selection = db.listMemos(filter);
        newMemo = selection.empty() ? nullptr : selection.front();
    }

    if (newMemo)
    {
        auto addedMemo = response->mutable_added_memo();
        *addedMemo = protoMemo;
        addedMemo->set_id(newMemo->id());
        response->set_request_uuid(request->uuid());
        response->mutable_operation_status()->set_code(200);
        response->mutable_operation_status()->set_type(proto::OperationStatus_Type_SUCCESS);
        LOG_TRC("Memo inserted.")
        LOG_DBG("New memo id: '" << newMemo->id() << "'.")
    }
    else
    {
        response->mutable_operation_status()->set_code(-1);
        response->mutable_operation_status()->set_type(proto::OperationStatus_Type_FAILURE);
        LOG_WRN("Failed to insert new Memo.")
    }
    return grpc::Status::OK;
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

    LOG_INF("[MemoService] Registered " << processCount() << " processes.");
}

} // namespace memo
