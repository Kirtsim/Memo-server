#include "AddTagProcess.hpp"
#include "server/service/TagService.hpp"
#include "logger/logger.hpp"


namespace memo {

service::IProcess::Ptr AddTagProcess::Create(TagService& service)
{
    auto process = std::make_shared<AddTagProcess>(service);
    return std::static_pointer_cast<IProcess>(process);
}

AddTagProcess::AddTagProcess(TagService& service) : BaseProcess(service)
{
}

AddTagProcess::~AddTagProcess() = default;

void AddTagProcess::init(grpc::ServerCompletionQueue& ioCompletionQueue)
{
    svc_.RequestAddTag(&context_, &request_, &writer_, &ioCompletionQueue, &ioCompletionQueue, this);
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

service::IProcess::Ptr AddTagProcess::duplicate() const
{
    return Create(svc_);
}
} // namespace memo