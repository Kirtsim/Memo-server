#include "AddMemoProcess.hpp"
#include "memo/service/MemoService.hpp"
#include "logger/logger.hpp"

namespace memo {

service::IProcess::Ptr AddMemoProcess::Create(MemoService& service)
{
    auto process = std::make_shared<AddMemoProcess>(service);
    return process;
}

AddMemoProcess::AddMemoProcess(MemoService& service) : BaseProcess(service)
{
}

AddMemoProcess::~AddMemoProcess() = default;

void AddMemoProcess::init(grpc::ServerCompletionQueue& ioCompletionQueue)
{
    svc_.RequestAddMemo(&context_, &request_, &writer_, &ioCompletionQueue, &ioCompletionQueue, this);
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

service::IProcess::Ptr AddMemoProcess::duplicate() const
{
    return memo::service::IProcess::Ptr();
}
} // namespace memo