#include "memo/service/BaseService.hpp"
#include "memo/Resources.hpp"
#include "logger/logger.hpp"

namespace memo {
namespace service {

BaseService::BaseService(const Resources::Ptr& ioResources, grpc::ServerCompletionQueue& ioCompletionQueue) :
    resources_(ioResources),
    completionQueue_(ioCompletionQueue)
{}

BaseService::~BaseService() = default;

bool BaseService::executeProcess(Process* process)
{
    auto it = processes_.find(process);
    if (it == end(processes_))
    {
        LOG_WRN("[BaseService] Process not found");
        return false;
    }

    if (process->isFinished())
    {
        LOG_TRC("[BaseService] Process finished");
        processes_.erase(it);
        return true;
    }

    registerProcess(process->duplicate());

    process->execute();
    return true;
}

void BaseService::enable()
{
    registerProcesses();
    LOG_TRC("[BaseService] Service enabled.");
}

void BaseService::disable()
{
    processes_.clear();
    LOG_TRC("[BaseService] Service disabled");
}

int BaseService::getId() const
{
    size_t pointerAddress = reinterpret_cast<size_t>(this);
    return pointerAddress;
}

void BaseService::registerProcess(Process::Ptr iProcess)
{
    LOG_TRC("[BaseService] Registering new process");
    iProcess->init(completionQueue_);
    processes_.insert({ iProcess.get(), iProcess });
}

} // namespace service
} // namespace memo
