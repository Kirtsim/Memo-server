#include "memo/service/memo/process/SearchProcess.hpp"
#include "memo/service/memo/MemoSvc.hpp"
#include "logger/logger.hpp"

namespace memo {
namespace service {
namespace process {

Process::Ptr SearchProcess::Create(MemoSvc& iSvc)
{
    auto process = std::make_shared<SearchProcess>(iSvc);
    return std::static_pointer_cast<Process>(process);
}

SearchProcess::SearchProcess(MemoSvc& iSvc) : BaseProcess(iSvc)
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

Process::Ptr SearchProcess::duplicate() const
{
    return Create(svc_);
}

} // namespace process
} // namespace service
} // namespace memo
