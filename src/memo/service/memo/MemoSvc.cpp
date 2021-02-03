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
    BaseService(ioResources, ioCompletionQueue)
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
    if (false == iRequest->titleoptions().startswith().empty())
    {
        InitMemo(*ioResponse->add_memos(), iRequest->titleoptions().startswith());
    }
    else
    {
        InitMemo(*ioResponse->add_memos(), "Memo 1");
        InitMemo(*ioResponse->add_memos(), "My trip to America");
        InitMemo(*ioResponse->add_memos(), "Very memorable situation");
        InitMemo(*ioResponse->add_memos(), "My first job");
        InitMemo(*ioResponse->add_memos(), "Living in Switzerland");
        InitMemo(*ioResponse->add_memos(), "Spending time in one of the UK's hospitals");
        InitMemo(*ioResponse->add_memos(), "Welcome to Krispy Kreme");
        InitMemo(*ioResponse->add_memos(), "Bojnicka zoo");
        InitMemo(*ioResponse->add_memos(), "Being a lifeguard in America");
        InitMemo(*ioResponse->add_memos(), "Graduation from Huddersfield university");
        InitMemo(*ioResponse->add_memos(), "Our cat");
    }
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

void MemoSvc::registerProcesses()
{
    registerProcess(process::memo::SearchProcess::Create(*this));
    registerProcess(process::memo::SearchByIdProcess::Create(*this));
    registerProcess(process::memo::CreateProcess::Create(*this));
    registerProcess(process::memo::UpdateProcess::Create(*this));
    registerProcess(process::memo::DeleteProcess::Create(*this));

    LOG_INF("[MemoSvc] Registered " <<  processes_.size() << " processes.");
}

} // namespace service
} // namespace memo
