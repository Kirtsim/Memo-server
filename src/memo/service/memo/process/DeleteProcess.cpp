#include "memo/service/memo/process/DeleteProcess.hpp"
#include "memo/service/memo/MemoSvc.hpp"
#include "logger/logger.hpp"

namespace memo::service::process::memo {

IProcess::Ptr DeleteProcess::Create(MemoSvc& iSvc)
{
    auto process = std::make_shared<DeleteProcess>(iSvc);
    return std::static_pointer_cast<IProcess>(process);
}

DeleteProcess::DeleteProcess(MemoSvc& iSvc) : BaseProcess(iSvc)
{}

DeleteProcess::~DeleteProcess() = default;

void DeleteProcess::init(grpc::ServerCompletionQueue& ioCompletionQueue)
{
    svc_.RequestDelete(&context_, &request_, &writer_, &ioCompletionQueue, &ioCompletionQueue, this);
    state_ = PROCESSING;
}

void DeleteProcess::execute()
{
    LOG_TRC("[DeleteProcess] Execution [start] >>>");
    auto status = svc_.Delete(&context_, &request_, &response_);
    writer_.Finish(response_, status, this);
    state_ = FINISHED;
    LOG_TRC("[DeleteProcess] Execution [end] <<<");
}

IProcess::Ptr DeleteProcess::duplicate() const
{
    return Create(svc_);
}

} // namespace memo::service::process::memo
