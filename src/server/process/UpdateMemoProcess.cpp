#include "server/process/UpdateMemoProcess.hpp"
#include "server/service/MemoService.hpp"
#include "logger/Logger.hpp"

namespace memo {

IProcess::Ptr UpdateMemoProcess::Create(MemoService& service)
{
    return std::make_shared<UpdateMemoProcess>(service);
}

UpdateMemoProcess::UpdateMemoProcess(MemoService& service)
    : BaseProcess(service)
{
}

UpdateMemoProcess::~UpdateMemoProcess() = default;

void UpdateMemoProcess::init(grpc::ServerCompletionQueue& completionQueue)
{
    svc_.RequestUpdateMemo(&context_, &request_, &writer_, &completionQueue, &completionQueue, this);
    state_ = PROCESSING;
}

void UpdateMemoProcess::execute()
{
    LOG_INF("[UpdateMemoProcess] Execution [start] >>>")
    auto status = svc_.UpdateMemo(&context_, &request_, &response_);
    writer_.Finish(response_, status, this);
    state_ = FINISHED;
    LOG_INF("[UpdateMemoProcess] Execution [end] <<<")
}

IProcess::Ptr UpdateMemoProcess::duplicate() const
{
    return Create(svc_);
}

} // namespace memo
