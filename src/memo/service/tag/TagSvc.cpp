#include "memo/service/tag/TagSvc.hpp"
#include "memo/service/tag/process/SearchProcess.hpp"
#include "memo/service/tag/process/CreateProcess.hpp"
#include "memo/service/tag/process/UpdateProcess.hpp"
#include "memo/service/tag/process/DeleteProcess.hpp"
#include "memo/Resources.hpp"
#include "logger/logger.hpp"

namespace memo {
namespace service {

TagSvc::TagSvc(const Resources::Ptr& ioResources, grpc::ServerCompletionQueue& ioCompletionQueue) :
    resources_(ioResources),
    completionQueue_(ioCompletionQueue)
{
    LOG_TRC("[TagSvc] TagSvc created");
}

TagSvc::~TagSvc() = default;

namespace {
void InitTag(model::Tag& ioTag, const std::string& iName)
{
    ioTag.set_name(iName);
    ioTag.set_color("#FFFFFF");
    ioTag.set_timestamp(100L);
}

} // namespace

grpc::Status TagSvc::Search(grpc::ServerContext* iContext,
                             const model::TagSearchRq* iRequest,
                             model::TagSearchRs* ioResponse)
{
    LOG_TRC("[TagSvc] Search");
    InitTag(*ioResponse->add_tags(), iRequest->nameoptions().startswith());
    return grpc::Status::OK;
}


grpc::Status TagSvc::Create(grpc::ServerContext* ioContext,
                             const model::Tag* iRequest,
                             model::OperationStatus* ioResponse)
{
    LOG_TRC("[TagSvc] Create");
    ioResponse->set_status(model::OperationStatus::SUCCESS);
    return grpc::Status::OK;
}

grpc::Status TagSvc::Update(grpc::ServerContext* ioContext,
                             const model::Tag* iRequest,
                             model::OperationStatus* ioResponse)
{
    LOG_TRC("[TagSvc] Update");
    ioResponse->set_status(model::OperationStatus::SUCCESS);
    return grpc::Status::OK;
}

grpc::Status TagSvc::Delete(grpc::ServerContext* ioContext,
                             const model::TagName* iRequest,
                             model::OperationStatus* ioResponse)
{
    LOG_TRC("[TagSvc] Delete");
    ioResponse->set_status(model::OperationStatus::SUCCESS);
    return grpc::Status::OK;
}

// TODO: Extract this to a "base" Service class
bool TagSvc::executeProcess(Process* process)
{
    auto it = processes_.find(process);
    if (it == end(processes_))
    {
        LOG_WRN("[TagSvc] Process not found");
        return false;
    }

    if (process->isFinished())
    {
        LOG_TRC("[TagSvc] Process finished");
        processes_.erase(process);
        return true;
    }

    registerProcess(process->duplicate());
    process->execute();
    return true;
}

void TagSvc::enable()
{
    registerProcess(process::tag::SearchProcess::Create(*this));
    registerProcess(process::tag::CreateProcess::Create(*this));
    registerProcess(process::tag::UpdateProcess::Create(*this));
    registerProcess(process::tag::DeleteProcess::Create(*this));

    LOG_TRC("[TagSvc] Service enabled.");
    LOG_INF("[TagSvc] Number of active processes: " <<  processes_.size());
}

void TagSvc::disable()
{
    LOG_TRC("[TagSvc] Disabling service ...");
    processes_.clear();
    LOG_TRC("[TagSvc] Service disabled");
}

int TagSvc::getId() const
{
    size_t pointerAddress = reinterpret_cast<size_t>(this);
    return pointerAddress;
}

void TagSvc::registerProcess(Process::Ptr iProcess)
{
    LOG_TRC("[TagSvc] Registering new process");
    iProcess->init(completionQueue_);
    processes_.insert({ iProcess.get(), iProcess });
}

} // namespace service
} // namespace memo
