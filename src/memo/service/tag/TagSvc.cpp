#include "memo/service/tag/TagSvc.hpp"
#include "memo/service/tag/process/SearchProcess.hpp"
#include "memo/service/tag/process/CreateProcess.hpp"
#include "memo/service/tag/process/UpdateProcess.hpp"
#include "memo/service/tag/process/DeleteProcess.hpp"
#include "memo/Resources.hpp"
#include "logger/logger.hpp"

namespace memo::service {

TagSvc::TagSvc(const Resources::Ptr& ioResources, grpc::ServerCompletionQueue& ioCompletionQueue) :
    BaseService(ioResources, ioCompletionQueue)
{
    LOG_TRC("[TagSvc] TagSvc created");
}

TagSvc::~TagSvc() = default;

namespace {
void InitTag(proto::Tag& ioTag, const std::string& iName)
{
    ioTag.set_name(iName);
    ioTag.set_color("#FFFFFF");
    ioTag.set_timestamp(100L);
}

} // namespace

grpc::Status TagSvc::Search(grpc::ServerContext* iContext,
                             const proto::TagSearchRq* iRequest,
                             proto::TagSearchRs* ioResponse)
{
    LOG_TRC("[TagSvc] Search");
    InitTag(*ioResponse->add_tags(), iRequest->name_options().starts_with());
    return grpc::Status::OK;
}


grpc::Status TagSvc::Create(grpc::ServerContext* ioContext,
                             const proto::Tag* iRequest,
                             proto::OperationStatus* ioResponse)
{
    LOG_TRC("[TagSvc] Create");
    ioResponse->set_status(proto::OperationStatus::SUCCESS);
    return grpc::Status::OK;
}

grpc::Status TagSvc::Update(grpc::ServerContext* ioContext,
                             const proto::Tag* iRequest,
                             proto::OperationStatus* ioResponse)
{
    LOG_TRC("[TagSvc] Update");
    ioResponse->set_status(proto::OperationStatus::SUCCESS);
    return grpc::Status::OK;
}

grpc::Status TagSvc::Delete(grpc::ServerContext* ioContext,
                             const proto::TagName* iRequest,
                             proto::OperationStatus* ioResponse)
{
    LOG_TRC("[TagSvc] Delete");
    ioResponse->set_status(proto::OperationStatus::SUCCESS);
    return grpc::Status::OK;
}

void TagSvc::registerProcesses()
{
    registerProcess(process::tag::SearchProcess::Create(*this));
    registerProcess(process::tag::CreateProcess::Create(*this));
    registerProcess(process::tag::UpdateProcess::Create(*this));
    registerProcess(process::tag::DeleteProcess::Create(*this));

    LOG_INF("[TagSvc] Registered " <<  processes_.size() << " processes.");
}

} // namespace memo::service
