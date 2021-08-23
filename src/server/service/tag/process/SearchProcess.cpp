#include "server/service/tag/process/SearchProcess.hpp"
#include "server/service/tag/TagSvc.hpp"
#include "logger/logger.hpp"

namespace memo::service::process::tag {

IProcess::Ptr SearchProcess::Create(service::TagSvc& iSvc)
{
    auto process = std::make_shared<SearchProcess>(iSvc);
    return std::static_pointer_cast<IProcess>(process);
}

SearchProcess::SearchProcess(service::TagSvc& iSvc) : BaseProcess(iSvc)
{}

SearchProcess::~SearchProcess() = default;

void SearchProcess::init(grpc::ServerCompletionQueue& ioCompletionQueue)
{
    svc_.RequestSearch(&context_, &request_, &writer_, &ioCompletionQueue, &ioCompletionQueue, this);
    state_ = PROCESSING;
}

void SearchProcess::execute()
{
    LOG_TRC("[SearchProcess] Execution [start] >>>");
    auto status = svc_.Search(&context_, &request_, &response_);
    writer_.Finish(response_, status, this);
    state_ = FINISHED;
    LOG_TRC("[SearchProcess] Execution [end] <<<");
}

IProcess::Ptr SearchProcess::duplicate() const
{
    return Create(svc_);
}

} // namespace memo::service::process::tag
