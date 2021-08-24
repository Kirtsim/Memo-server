#include "server/service/TagService.hpp"
#include "server/service/process/ListTagsProcess.hpp"
#include "server/service/process/AddTagProcess.hpp"

#include "logger/logger.hpp"

namespace memo {

TagService::TagService(const std::shared_ptr<Resources>& resources,
                       grpc::ServerCompletionQueue& completionQueue)
        : BaseService(resources, completionQueue)
{
    LOG_TRC("[TagService] TagService created");
}

TagService::~TagService() = default;

grpc::Status TagService::ListTags(grpc::ServerContext*,
                                  const proto::ListTagsRq* request,
                                  proto::ListTagsRs* response)
{
    LOG_TRC("[TagService] ++ List tags ++");
    auto initTag = [](proto::Tag& tag, const std::string& name)
    {
        tag.set_id(101);
        tag.set_name(name);
        tag.set_timestamp(100L);
    };
    const bool noFilter = request->filter().name_starts_with().empty();
    if (noFilter)
    {
        initTag(*response->add_tags(), "school");
        initTag(*response->add_tags(), "simple");
        initTag(*response->add_tags(), "outdoor");
        initTag(*response->add_tags(), "cake");
        initTag(*response->add_tags(), "fun");
        initTag(*response->add_tags(), "work");
        initTag(*response->add_tags(), "programming");
        initTag(*response->add_tags(), "computer");
        initTag(*response->add_tags(), "family");
        initTag(*response->add_tags(), "flowers");
        initTag(*response->add_tags(), "program");
    }
    else
    {
        initTag(*response->add_tags(), request->filter().name_starts_with());
    }
    response->set_request_uuid(request->uuid());
    LOG_TRC("[TagService] -- List tags -- done");
    return grpc::Status::OK;
}

grpc::Status TagService::AddTag(grpc::ServerContext*,
                                const proto::AddTagRq* request,
                                proto::AddTagRs* response)
{
    LOG_TRC("[TagService] Adding new Tag");
    const auto& tag = request->tag();
    LOG_TRC("ID: " << tag.id());
    LOG_TRC("Name: " << tag.name());
    LOG_TRC("Created at: " << tag.timestamp());
    LOG_TRC("[TagService] Adding new Tag - SUCCESS");
    // TODO: insert into a db.
    auto addedTag = response->mutable_tag();
    *addedTag = tag;
    addedTag->set_id(111222333); // TODO: this number will be assigned when added to a db.
    response->set_request_uuid(request->uuid());
    response->mutable_operation_status()->set_code(200);
    response->mutable_operation_status()->set_type(proto::OperationStatus_Type_SUCCESS);
    return grpc::Status::OK;
}

grpc::Status TagService::UpdateTag(grpc::ServerContext* context,
                                   const proto::UpdateTagRq* request,
                                   proto::UpdateTagRs* response)
{
    return WithAsyncMethod_UpdateTag::UpdateTag(context, request, response);
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

    LOG_INF("[MemoService] Registered " <<  processes_.size() << " processes.");
}

} // namespace memo
