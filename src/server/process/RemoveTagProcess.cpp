#include "RemoveTagProcess.hpp"
#include "server/service/TagService.hpp"
#include "logger/Logger.hpp"

namespace memo {

IProcess::Ptr RemoveTagProcess::Create(TagService& service)
{
    return std::make_shared<RemoveTagProcess>(service);
}

RemoveTagProcess::RemoveTagProcess(TagService& service)
    : BaseProcess(service)
{
}

RemoveTagProcess::~RemoveTagProcess() = default;

void RemoveTagProcess::init(grpc::ServerCompletionQueue& completionQueue)
{
    svc_.RequestRemoveTag(&context_, &request_, &writer_, &completionQueue, &completionQueue, this);
    state_ = PROCESSING;
}

void RemoveTagProcess::execute()
{
    LOG_INF("[RemoveTagProcess] Execution [start] >>>")
    auto status = svc_.RemoveTag(&context_, &request_, &response_);
    writer_.Finish(response_, status, this);
    state_ = FINISHED;
    LOG_INF("[RemoveTagProcess] Execution [end] <<<")
}

IProcess::Ptr RemoveTagProcess::duplicate() const
{
    return Create(svc_);
}

} // namespace memo

