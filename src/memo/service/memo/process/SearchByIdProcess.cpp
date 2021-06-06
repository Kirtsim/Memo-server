#include "memo/service/memo/process/SearchByIdProcess.hpp"
#include "memo/service/memo/MemoSvc.hpp"
#include "logger/logger.hpp"

namespace memo::service::process::memo {

IProcess::Ptr SearchByIdProcess::Create(MemoSvc& iSvc)
{
    auto process = std::make_shared<SearchByIdProcess>(iSvc);
    return std::static_pointer_cast<IProcess>(process);
}

SearchByIdProcess::SearchByIdProcess(MemoSvc& iSvc) : BaseProcess(iSvc)
{}

SearchByIdProcess::~SearchByIdProcess() = default;

void SearchByIdProcess::init(grpc::ServerCompletionQueue& ioCompletionQueue)
{
    svc_.RequestSearchById(&context_, &request_, &writer_, &ioCompletionQueue, &ioCompletionQueue, this);
    state_ = PROCESSING;
}

void SearchByIdProcess::execute()
{
    LOG_TRC("[SearchByIdProcess] Execution [start] >>>");
    auto status = svc_.SearchById(&context_, &request_, &response_);
    writer_.Finish(response_, status, this);
    state_ = FINISHED;
    LOG_TRC("[SearchByIdProcess] Execution [end] <<<");
}

IProcess::Ptr SearchByIdProcess::duplicate() const
{
    return Create(svc_);
}

} // namespace memo::service::process::memo
