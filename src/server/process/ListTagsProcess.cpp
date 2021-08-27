#include "server/process/ListTagsProcess.hpp"
#include "server/service/TagService.hpp"
#include "logger/logger.hpp"


namespace memo {

IProcess::Ptr ListTagsProcess::Create(TagService& service)
{
    auto process = std::make_shared<ListTagsProcess>(service);
    return std::static_pointer_cast<IProcess>(process);
}

ListTagsProcess::ListTagsProcess(TagService& service) : BaseProcess(service)
{
}

ListTagsProcess::~ListTagsProcess() = default;

void ListTagsProcess::init(grpc::ServerCompletionQueue& completionQueue)
{
    svc_.RequestListTags(&context_, &request_, &writer_, &completionQueue, &completionQueue, this);
    state_ = PROCESSING;
}

void ListTagsProcess::execute()
{
    LOG_TRC("[ListTagsProcess] Execution [start] >>>");
    auto status = svc_.ListTags(&context_, &request_, &response_);
    writer_.Finish(response_, status, this);
    state_ = FINISHED;
    LOG_TRC("[ListTagsProcess] Execution [end] <<<");
}

IProcess::Ptr ListTagsProcess::duplicate() const
{
    return Create(svc_);
}
} // namespace memo
