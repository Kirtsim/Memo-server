#include "memo/service/memo/MemoSvc.hpp"
#include "memo/service/memo/process/SearchProcess.hpp"
#include "memo/service/memo/process/SearchByIdProcess.hpp"
#include "memo/service/memo/process/CreateProcess.hpp"
#include "memo/service/memo/process/UpdateProcess.hpp"
#include "memo/service/memo/process/DeleteProcess.hpp"
#include "memo/Resources.hpp"
#include "logger/logger.hpp"

namespace memo {
namespace service {

MemoSvc::MemoSvc(const Resources::Ptr& ioResources, grpc::ServerCompletionQueue& ioCompletionQueue) :
    resources_(ioResources),
    completionQueue_(ioCompletionQueue)
{
    LOG_TRC("[MemoSvc] MemoSvc created");
}

MemoSvc::~MemoSvc() = default;

namespace {
void InitMemo(model::Memo& ioMemo, const std::string& iTitle)
{
    ioMemo.set_id("fkasdlf-askdfjowe-sdfjkasldfj");
    ioMemo.set_title(iTitle);
    ioMemo.set_description("Lorem ipsum: short description.");
    ioMemo.set_timestamp(100L);
    ioMemo.add_tagnames("TestTag1");
    ioMemo.add_tagnames("#Testing");
}

} // namespace

grpc::Status MemoSvc::Search(grpc::ServerContext* iContext,
                             const model::MemoSearchRq* iRequest,
                             model::MemoSearchRs* ioResponse)
{

    LOG_TRC("[MemoSvc] Search");
    InitMemo(*ioResponse->add_memos(), iRequest->titleoptions().startswith());
    return grpc::Status::OK;
}

grpc::Status MemoSvc::SearchById(grpc::ServerContext* iContext,
                                 const model::IdList* iRequest,
                                 model::MemoSearchRs* ioResponse)
{
    LOG_TRC("[MemoSvc] SearchById");
    if (!iRequest->ids().empty())
        InitMemo(*ioResponse->add_memos(), "Memo with id: " + iRequest->ids(0).value());
    else
        InitMemo(*ioResponse->add_memos(), "You did not speicify any id!!");
    return grpc::Status::OK;
}

grpc::Status MemoSvc::Create(grpc::ServerContext* ioContext,
                             const model::Memo* iRequest,
                             model::Id* ioResponse)
{
    LOG_TRC("[MemoSvc] Create");
    ioResponse->set_value("aslfdkjasldfkjasldfjasdf-id");
    return grpc::Status::OK;
}

grpc::Status MemoSvc::Update(grpc::ServerContext* ioContext,
                             const model::Memo* iRequest,
                             model::OperationStatus* ioResponse)
{
    LOG_TRC("[MemoSvc] Update");
    ioResponse->set_status(model::OperationStatus::SUCCESS);
    return grpc::Status::OK;
}

grpc::Status MemoSvc::Delete(grpc::ServerContext* ioContext,
                             const model::Id* iRequest,
                             model::OperationStatus* ioResponse)
{
    LOG_TRC("[MemoSvc] Delete");
    ioResponse->set_status(model::OperationStatus::SUCCESS);
    return grpc::Status::OK;
}


bool MemoSvc::executeProcess(Process* process)
{
    auto it = processes_.find(process);
    if (it == end(processes_))
    {
        LOG_WRN("[MemoSvc] Process not found");
        return false;
    }

    if (process->isFinished())
    {
        LOG_TRC("[MemoSvc] Process finished");
        processes_.erase(it);
        return true;
    }

    registerProcess(process->duplicate());

    process->execute();
    return true;
}

void MemoSvc::enable()
{
    registerProcess(process::memo::SearchProcess::Create(*this));
    registerProcess(process::memo::SearchByIdProcess::Create(*this));
    registerProcess(process::memo::CreateProcess::Create(*this));
    registerProcess(process::memo::UpdateProcess::Create(*this));
    registerProcess(process::memo::DeleteProcess::Create(*this));

    LOG_TRC("[MemoSvc] Service enabled.");
    LOG_INF("[MemoSvc] Number of active processes: " <<  processes_.size());
}

void MemoSvc::disable()
{
    LOG_TRC("[MemoSvc] Disabling service ...");
    processes_.clear();
    LOG_TRC("[MemoSvc] Service disabled");
}

int MemoSvc::getId() const
{
    size_t pointerAddress = reinterpret_cast<size_t>(this);
    return pointerAddress;
}

void MemoSvc::registerProcess(Process::Ptr iProcess)
{
    LOG_TRC("[MemoSvc] Registering new process");
    iProcess->init(completionQueue_);
    processes_.insert({ iProcess.get(), iProcess });
}

} // namespace service
} // namespace memo
