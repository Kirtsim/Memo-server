#include "memo/service/memo/process/CreateProcess.hpp"
#include "memo/service/memo/MemoSvc.hpp"
#include "logger/logger.hpp"

namespace memo {
namespace service {
namespace process {
namespace memo {

Process::Ptr CreateProcess::Create(MemoSvc& iSvc)
{
    auto process = std::make_shared<CreateProcess>(iSvc);
    return std::static_pointer_cast<Process>(process);
}

CreateProcess::CreateProcess(MemoSvc& iSvc) : BaseProcess(iSvc)
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

Process::Ptr CreateProcess::duplicate() const
{
    return Create(svc_);
}

} // namespace memo
} // namespace process
} // namespace service
} // namespace memo
