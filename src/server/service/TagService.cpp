#include "server/service/TagService.hpp"
#include "server/process/ListTagsProcess.hpp"
#include "server/process/AddTagProcess.hpp"
#include "server/process/UpdateTagProcess.hpp"
#include "server/Resources.hpp"
#include "db/IDatabase.hpp"
#include "db/Tools.hpp"
#include "model/Tag.hpp"

#include "logger/Logger.hpp"

namespace memo {

TagService::TagService(const std::shared_ptr<Resources>& resources,
                       grpc::ServerCompletionQueue& completionQueue)
        : BaseService(resources, completionQueue)
{
    LOG_INF("[TagService] TagService created")
}

TagService::~TagService() = default;

grpc::Status TagService::ListTags(grpc::ServerContext*,
                                  const proto::ListTagsRq* request,
                                  proto::ListTagsRs* response)
{
    LOG_INF("[TagService] Listing tags ...")
    TagSearchFilter filter;
    filter.namePrefix = request->filter().name_starts_with();
    filter.nameContains = request->filter().contains();
    filter.colors.assign(request->filter().colours().begin(), request->filter().colours().end());
    if (request->filter().has_creation_time())
    {
        if (request->filter().creation_time().start() != -1ul)
            filter.filterFromDate = request->filter().creation_time().start();
        if (request->filter().creation_time().end() != -1ul)
            filter.filterUntilDate = request->filter().creation_time().end();
    }

    auto& db = resources().database();
    const auto selection = db.listTags(filter);
    LOG_DBG("[TagService] Found " << selection.size() << " Tag results.")

    for (const auto& tagModel : selection)
    {
        if (!tagModel)
            continue;
        auto tag = response->add_tags();
        tag->set_id(tagModel->id());
        tag->set_name(tagModel->name());
        tag->set_color(tools::ColorToInt(tagModel->color()));
        tag->set_timestamp(tagModel->timestamp());
    }

    response->set_request_uuid(request->uuid());
    LOG_INF("[TagService] Returning " << response->tags_size() << " Tags.")
    return grpc::Status::OK;
}

grpc::Status TagService::AddTag(grpc::ServerContext*,
                                const proto::AddTagRq* request,
                                proto::AddTagRs* response)
{
    LOG_INF("[TagService] Adding new Tag '" << request->tag().name() << "' ...")
    const auto& protoTag = request->tag();
    model::Tag tag;
    tag.setName(protoTag.name());
    tag.setColor(tools::IntToColor(protoTag.color()));
    tag.setTimestamp(protoTag.timestamp());

    model::TagPtr newTag;
    auto& db = resources().database();
    if (db.insertTag(tag))
    {
        auto filter = TagSearchFilter();
        filter.exactNameMatch = tag.name();
        const auto selection = db.listTags(filter);
        newTag = selection.empty() ? nullptr : selection.front();
    }

    if (newTag)
    {
        auto addedTag = response->mutable_tag();
        *addedTag = protoTag;
        addedTag->set_id(newTag->id());
        response->mutable_operation_status()->set_code(200);
        response->mutable_operation_status()->set_type(proto::OperationStatus_Type_SUCCESS);
        LOG_DBG("Newly inserted tag id: " << newTag->id())
    }
    else
    {
        response->mutable_operation_status()->set_code(-1);
        response->mutable_operation_status()->set_type(proto::OperationStatus_Type_FAILURE);
    }

    response->set_request_uuid(request->uuid());
    return grpc::Status::OK;
}

grpc::Status TagService::UpdateTag(grpc::ServerContext*,
                                   const proto::UpdateTagRq* request,
                                   proto::UpdateTagRs* response)
{
    LOG_INF("[TagService] Updating tag ...")
    LOG_DBG("Tag id: " << request->tag().id())
    auto& db = resources().database();
    model::Tag tag;
    tag.setId(request->tag().id());
    tag.setName(request->tag().name());
    tag.setColor(tools::IntToColor(request->tag().color()));
    tag.setTimestamp(request->tag().timestamp());

    response->set_request_uuid(request->uuid());
    if (db.updateTag(tag))
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

grpc::Status TagService::RemoveTag(grpc::ServerContext* context,
                                   const proto::RemoveTagRq* request,
                                   proto::RemoveTagRs* response)
{
    return WithAsyncMethod_RemoveTag::RemoveTag(context, request, response);
}

void TagService::registerProcesses()
{
    registerProcess(ListTagsProcess::Create(*this));
    registerProcess(AddTagProcess::Create(*this));
    registerProcess(UpdateTagProcess::Create(*this));

    LOG_INF("[TagService] Registered " << processCount() << " processes.")
}

} // namespace memo
