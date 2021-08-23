#include "server/service/memo/process/UpdateProcess.hpp"
#include "server/service/memo/MemoSvc.hpp"
#include "logger/logger.hpp"

namespace memo::process::memo {

IProcess::Ptr UpdateProcess::Create(MemoSvc& iSvc)
{
    auto process = std::make_shared<UpdateProcess>(iSvc);
    return std::static_pointer_cast<IProcess>(process);
}

UpdateProcess::UpdateProcess(MemoSvc& iSvc) : BaseProcess(iSvc)
{}

UpdateProcess::~UpdateProcess() = default;

void UpdateProcess::init(grpc::ServerCompletionQueue& ioCompletionQueue)
{
    svc_.RequestUpdate(&context_, &request_, &writer_, &ioCompletionQueue, &ioCompletionQueue, this);
    state_ = PROCESSING;
}

void UpdateProcess::execute()
{
    LOG_TRC("[UpdateProcess] Execution [start] >>>");
    auto status = svc_.Update(&context_, &request_, &response_);
    writer_.Finish(response_, status, this);
    state_ = FINISHED;
    LOG_TRC("[UpdateProcess] Execution [end] <<<");
}

IProcess::Ptr UpdateProcess::duplicate() const
{
    return Create(svc_);
}

} // namespace memo::process::memo
