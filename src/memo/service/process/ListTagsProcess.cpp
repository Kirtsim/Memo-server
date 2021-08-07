#include "memo/service/process/ListTagsProcess.hpp"
#include "memo/service/TagService.hpp"
#include "logger/logger.hpp"


namespace memo {

service::IProcess::Ptr ListTagsProcess::Create(TagService& service)
{
    auto process = std::make_shared<ListTagsProcess>(service);
    return std::static_pointer_cast<IProcess>(process);
}

ListTagsProcess::ListTagsProcess(TagService& service) : BaseProcess(service)
{
}

ListTagsProcess::~ListTagsProcess() = default;

void ListTagsProcess::init(grpc::ServerCompletionQueue& ioCompletionQueue)
{
    svc_.RequestListTags(&context_, &request_, &writer_, &ioCompletionQueue, &ioCompletionQueue, this);
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

service::IProcess::Ptr ListTagsProcess::duplicate() const
{
    return Create(svc_);
}
} // namespace memo
