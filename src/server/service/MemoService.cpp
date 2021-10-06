#include "server/service/MemoService.hpp"
#include "server/process/ListMemosProcess.hpp"
#include "server/process/AddMemoProcess.hpp"
#include "server/Resources.hpp"
#include "model/Memo.hpp"

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
    LOG_TRC("[MemoService] ++ List memos ++");
    auto initMemo = [](proto::Memo& memo, const std::string& title)
    {
        memo.set_id(101);
        memo.set_title(title);
        memo.set_description("Lorem ipsum: short description.");
        memo.set_timestamp(100L);
        memo.add_tag_ids(0);
    };
    const bool noFilter = request->filter().title_starts_with().empty();
    if (noFilter)
    {
        initMemo(*response->add_memos(), "Memo 1");
        initMemo(*response->add_memos(), "My trip to America");
        initMemo(*response->add_memos(), "Very memorable situation");
        initMemo(*response->add_memos(), "My first job");
        initMemo(*response->add_memos(), "Living in Switzerland");
        initMemo(*response->add_memos(), "Spending time in one of the UK's hospitals");
        initMemo(*response->add_memos(), "Welcome to Krispy Kreme");
        initMemo(*response->add_memos(), "Bojnicka zoo");
        initMemo(*response->add_memos(), "Being a lifeguard in America");
        initMemo(*response->add_memos(), "Graduation from Huddersfield university");
        initMemo(*response->add_memos(), "Our cat");
    }
    else
    {
        initMemo(*response->add_memos(), request->filter().title_starts_with());
    }
    response->set_request_uuid(request->uuid());
    LOG_TRC("[MemoService] -- List memos -- done");
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
