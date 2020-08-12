#include "memo/service/tag/process/UpdateProcess.hpp"
#include "memo/service/tag/TagSvc.hpp"
#include "logger/logger.hpp"

namespace memo {
namespace service {
namespace process {
namespace tag {

Process::Ptr UpdateProcess::Create(service::TagSvc& iSvc)
{
    auto process = std::make_shared<UpdateProcess>(iSvc);
    return std::static_pointer_cast<Process>(process);
}

UpdateProcess::UpdateProcess(service::TagSvc& iSvc) : BaseProcess(iSvc)
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

Process::Ptr UpdateProcess::duplicate() const
{
    return Create(svc_);
}

} // namespace tag
} // namespace process
} // namespace service
} // namespace memo
