#include "server/process/RemoveMemoProcess.hpp"
#include "server/service/MemoService.hpp"
#include "logger/Logger.hpp"

namespace memo {

IProcess::Ptr RemoveMemoProcess::Create(MemoService& service)
{
    return std::make_shared<RemoveMemoProcess>(service);
}

RemoveMemoProcess::RemoveMemoProcess(MemoService& service)
    : BaseProcess(service)
{
}

RemoveMemoProcess::~RemoveMemoProcess() = default;

void RemoveMemoProcess::init(grpc::ServerCompletionQueue& completionQueue)
{
    svc_.RequestRemoveMemo(&context_, &request_, &writer_, &completionQueue, &completionQueue, this);
    state_ = PROCESSING;
}

void RemoveMemoProcess::execute()
{
    LOG_INF("[RemoveMemoProcess] Execution [start] >>>")
    auto status = svc_.RemoveMemo(&context_, &request_, &response_);
    writer_.Finish(response_, status, this);
    state_ = FINISHED;
    LOG_INF("[RemoveMemoProcess] Execution [end] <<<")
}

IProcess::Ptr RemoveMemoProcess::duplicate() const
{
    return Create(svc_);
}

} // namespace memo
