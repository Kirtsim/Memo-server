#include "server/process/UpdateTagProcess.hpp"
#include "server/service/TagService.hpp"
#include "logger/Logger.hpp"


namespace memo {

IProcess::Ptr UpdateTagProcess::Create(TagService& service)
{
    return std::make_shared<UpdateTagProcess>(service);
}

UpdateTagProcess::UpdateTagProcess(TagService& service)
    :  BaseProcess(service)
{
}

UpdateTagProcess::~UpdateTagProcess() = default;

void UpdateTagProcess::init(grpc::ServerCompletionQueue& completionQueue)
{
    svc_.RequestUpdateTag(&context_, &request_, &writer_, &completionQueue, &completionQueue, this);
    state_ = PROCESSING;
}

void UpdateTagProcess::execute()
{
    LOG_INF("[UpdateTagProcess] Execution [start] >>>")
    auto status = svc_.UpdateTag(&context_, &request_, &response_);
    writer_.Finish(response_, status, this);
    state_ = FINISHED;
    LOG_INF("[UpdateTagProcess] Execution [end] <<<")

}

IProcess::Ptr UpdateTagProcess::duplicate() const
{
    return Create(svc_);
}

} // namespace memo
