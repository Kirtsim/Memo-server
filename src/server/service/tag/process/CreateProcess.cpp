#include "server/service/tag/process/CreateProcess.hpp"
#include "server/service/tag/TagSvc.hpp"
#include "logger/logger.hpp"

namespace memo::process::tag {

IProcess::Ptr CreateProcess::Create(TagSvc& iSvc)
{
    auto process = std::make_shared<CreateProcess>(iSvc);
    return std::static_pointer_cast<IProcess>(process);
}

CreateProcess::CreateProcess(TagSvc& iSvc) : BaseProcess(iSvc)
{}

CreateProcess::~CreateProcess() = default;

void CreateProcess::init(grpc::ServerCompletionQueue& ioCompletionQueue)
{
    svc_.RequestCreate(&context_, &request_, &writer_, &ioCompletionQueue, &ioCompletionQueue, this);
    state_ = PROCESSING;
}

void CreateProcess::execute()
{
    LOG_TRC("[CreateProcess] Execution [start] >>>");
    auto status = svc_.Create(&context_, &request_, &response_);
    writer_.Finish(response_, status, this);
    state_ = FINISHED;
    LOG_TRC("[CreateProcess] Execution [end] <<<");
}

IProcess::Ptr CreateProcess::duplicate() const
{
    return Create(svc_);
}

} // namespace memo::process::tag
