#include "server/service/MemoService.hpp"
#include "server/process/ListMemosProcess.hpp"
#include "server/process/AddMemoProcess.hpp"
#include "server/process/RemoveMemoProcess.hpp"
#include "server/process/UpdateMemoProcess.hpp"
#include "server/Resources.hpp"
#include "model/Memo.hpp"
#include "db/IDatabase.hpp"
#include "db/Tools.hpp"

#include "logger/Logger.hpp"

namespace memo {

MemoService::MemoService(const std::shared_ptr<Resources>& resources,
                         grpc::ServerCompletionQueue& completionQueue)
        : BaseService(resources, completionQueue)
{
    LOG_INF("[MemoService] MemoService created")
}

MemoService::~MemoService() = default;

grpc::Status MemoService::ListMemos(grpc::ServerContext*, const proto::ListMemosRq* request,
                                    proto::ListMemosRs* response)
{
    LOG_INF("[MemoService] Listing memos ...")
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
    std::set<unsigned long> allTagIds;
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
        allTagIds.insert(memo->tagIds().begin(), memo->tagIds().end());
    }
    TagSearchFilter tagFilter;
    tagFilter.ids.assign(allTagIds.begin(), allTagIds.end());
    const auto tags = db.listTags(tagFilter);
    for (const auto& tag : tags)
    {
        if (!tag)
            continue;

        proto::Tag protoTag;
        protoTag.set_id(tag->id());
        protoTag.set_name(tag->name());
        protoTag.set_color(tools::ColorToInt(tag->color()));
        protoTag.set_timestamp(tag->timestamp());
        (*response->mutable_tags())[tag->id()] = protoTag;
    }

    response->set_request_uuid(request->uuid());
    LOG_INF("[MemoService] Found " << selection.size() << " Memo results.")
    return grpc::Status::OK;
}

grpc::Status MemoService::AddMemo(grpc::ServerContext*, const proto::AddMemoRq* request,
                                  proto::AddMemoRs* response)
{
    LOG_INF("[MemoService] Adding Memo titled '" << request->memo().title() << "'.")
    const auto& protoMemo = request->memo();
    model::Memo memo;
    memo.setTitle(protoMemo.title());
    memo.setDescription(protoMemo.description());
    memo.setTimestamp(protoMemo.timestamp());
    std::vector<unsigned long> tagIds(protoMemo.tag_ids().begin(), protoMemo.tag_ids().end());
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
        LOG_DBG("Newly inserted memo id: " << newMemo->id())
    }
    else
    {
        response->mutable_operation_status()->set_code(-1);
        response->mutable_operation_status()->set_type(proto::OperationStatus_Type_FAILURE);
        LOG_WRN("Failed to insert new Memo.")
    }
    return grpc::Status::OK;
}

grpc::Status MemoService::UpdateMemo(grpc::ServerContext*, const proto::UpdateMemoRq* request,
                                     proto::UpdateMemoRs* response)
{
    LOG_INF("[MemoService] Updating memo ...")
    LOG_DBG("Memo id: " << request->memo().id())
    auto& db = resources().database();
    model::Memo memo;
    memo.setId(request->memo().id());
    memo.setTitle(request->memo().title());
    memo.setDescription(request->memo().description());
    memo.setTimestamp(request->memo().timestamp());

    response->set_request_uuid(request->request_uuid());
    if (db.updateMemo(memo))
    {
        response->mutable_operation_status()->set_code(200);
        response->mutable_operation_status()->set_type(proto::OperationStatus_Type_SUCCESS);
        LOG_DBG("Update success.")
    }
    else
    {
        response->mutable_operation_status()->set_code(-1);
        response->mutable_operation_status()->set_type(proto::OperationStatus_Type_FAILURE);
        LOG_DBG("Update failure.")
    }

    return grpc::Status::OK;
}

grpc::Status MemoService::RemoveMemo(grpc::ServerContext*, const proto::RemoveMemoRq* request,
                                     proto::RemoveMemoRs* response)
{
    LOG_INF("[MemoService] Removing " << request->ids().size() << " memos ...")
    auto& db = resources().database();

    MemoSearchFilter filter;

    filter.ids.assign(request->ids().begin(), request->ids().end());
    auto toRemove = db.listMemos(filter);
    for (const auto& memo : toRemove)
    {
        if (!memo)
            continue;

        if (db.deleteMemo(*memo))
        {
            auto removedMemo = response->add_removed_memos();
            removedMemo->set_id(memo->id());
            removedMemo->set_title(memo->title());
            removedMemo->set_description(memo->description());
            removedMemo->set_timestamp(memo->timestamp());
        }
    }
    response->set_request_uuid(request->request_uuid());
    response->mutable_operation_status()->set_code(200);
    response->mutable_operation_status()->set_type(proto::OperationStatus_Type_SUCCESS);
    LOG_INF("[MemoService] Deleted " << response->removed_memos().size() << " memos.")
    return grpc::Status::OK;
}

void MemoService::registerProcesses()
{
    registerProcess(ListMemosProcess::Create(*this));
    registerProcess(AddMemoProcess::Create(*this));
    registerProcess(RemoveMemoProcess::Create(*this));
    registerProcess(UpdateMemoProcess::Create(*this));

    LOG_INF("[MemoService] Registered " << processCount() << " processes.")
}

} // namespace memo
