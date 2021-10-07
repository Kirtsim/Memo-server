#include "server/service/BaseService.hpp"
#include "server/Resources.hpp"
#include "logger/Logger.hpp"

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
        LOG_INF("[BaseService] Process finished");
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
    LOG_INF("[BaseService] Service enabled.");
}

void BaseService::disable()
{
    processes_.clear();
    LOG_INF("[BaseService] Service disabled");
}

int BaseService::getId() const
{
    auto pointerAddress = reinterpret_cast<size_t>(this);
    return pointerAddress;
}

size_t BaseService::processCount() const
{
    return processes_.size();
}

void BaseService::registerProcess(const IProcess::Ptr& process)
{
    LOG_INF("[BaseService] Registering new process")
    if (process)
    {
        process->init(completionQueue_);
        processes_.insert({ process.get(), process });
    }
    else
    {
        LOG_WRN("[BaseService] Failed to register: process is null.")
    }
}

Resources& BaseService::resources()
{
    return *resources_;
}

grpc::ServerCompletionQueue& BaseService::completionQueue()
{
    return completionQueue_;
}

} // namespace memo
