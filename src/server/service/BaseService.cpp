#include "server/service/BaseService.hpp"
#include "server/Resources.hpp"
#include "logger/logger.hpp"

namespace memo {

BaseService::BaseService(const Resources::Ptr& resources, grpc::ServerCompletionQueue& completionQueue) :
    resources_(resources),
    completionQueue_(completionQueue)
{}

BaseService::~BaseService() = default;

bool BaseService::executeProcess(IProcess* process)
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
    auto pointerAddress = reinterpret_cast<size_t>(this);
    return pointerAddress;
}

void BaseService::registerProcess(const IProcess::Ptr& process)
{
    LOG_TRC("[BaseService] Registering new process");
    if (process)
    {
        process->init(completionQueue_);
        processes_.insert({ process.get(), process });
    }
    else
    {
        LOG_WRN("[BaseService] Failed to register: process is null.");
    }
}

} // namespace memo
