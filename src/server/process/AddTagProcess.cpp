#include "server/process/AddTagProcess.hpp"
#include "server/service/TagService.hpp"
#include "logger/Logger.hpp"


namespace memo {

IProcess::Ptr AddTagProcess::Create(TagService& service)
{
    auto process = std::make_shared<AddTagProcess>(service);
    return std::static_pointer_cast<IProcess>(process);
}

AddTagProcess::AddTagProcess(TagService& service) : BaseProcess(service)
{
}

AddTagProcess::~AddTagProcess() = default;

void AddTagProcess::init(grpc::ServerCompletionQueue& completionQueue)
{
    svc_.RequestAddTag(&context_, &request_, &writer_, &completionQueue, &completionQueue, this);
    state_ = PROCESSING;
}

void AddTagProcess::execute()
{
    LOG_TRC("[AddTagProcess] Execution [start] >>>");
    auto status = svc_.AddTag(&context_, &request_, &response_);
    writer_.Finish(response_, status, this);
    state_ = FINISHED;
    LOG_TRC("[AddTagProcess] Execution [end] <<<");
}

IProcess::Ptr AddTagProcess::duplicate() const
{
    return Create(svc_);
}
} // namespace memo