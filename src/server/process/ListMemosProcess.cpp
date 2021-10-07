#include "server/process/ListMemosProcess.hpp"
#include "server/service/MemoService.hpp"
#include "logger/Logger.hpp"

namespace memo {

IProcess::Ptr ListMemosProcess::Create(MemoService& service)
{
    auto process = std::make_shared<ListMemosProcess>(service);
    return std::static_pointer_cast<IProcess>(process);
}

ListMemosProcess::ListMemosProcess(MemoService& service) : BaseProcess(service)
{
}

ListMemosProcess::~ListMemosProcess() = default;

void ListMemosProcess::init(grpc::ServerCompletionQueue& completionQueue)
{
    svc_.RequestListMemos(&context_, &request_, &writer_, &completionQueue, &completionQueue, this);
    state_ = PROCESSING;
}

void ListMemosProcess::execute()
{
    LOG_TRC("[ListMemosProcess] Execution [start] >>>");
    auto status = svc_.ListMemos(&context_, &request_, &response_);
    writer_.Finish(response_, status, this);
    state_ = FINISHED;
    LOG_TRC("[ListMemosProcess] Execution [end] <<<");
}

IProcess::Ptr ListMemosProcess::duplicate() const
{
    return Create(svc_);
}

} // namespace memo
