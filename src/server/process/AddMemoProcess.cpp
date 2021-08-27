#include "server/process/AddMemoProcess.hpp"
#include "server/service/MemoService.hpp"
#include "logger/logger.hpp"

namespace memo {

IProcess::Ptr AddMemoProcess::Create(MemoService& service)
{
    auto process = std::make_shared<AddMemoProcess>(service);
    return process;
}

AddMemoProcess::AddMemoProcess(MemoService& service) : BaseProcess(service)
{
}

AddMemoProcess::~AddMemoProcess() = default;

void AddMemoProcess::init(grpc::ServerCompletionQueue& completionQueue)
{
    svc_.RequestAddMemo(&context_, &request_, &writer_, &completionQueue, &completionQueue, this);
    state_ = PROCESSING;
}

void AddMemoProcess::execute()
{
    LOG_TRC("[AddMemoProcess] Execution [start] >>>");
    auto status = svc_.AddMemo(&context_, &request_, &response_);
    writer_.Finish(response_, status, this);
    state_ = FINISHED;
    LOG_TRC("[AddMemoProcess] Execution [end] <<<");
}

IProcess::Ptr AddMemoProcess::duplicate() const
{
    return Create(svc_);
}

} // namespace memo