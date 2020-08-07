#pragma once
#include "memo/service/Process.hpp"
#include "model/MemoSvc.grpc.pb.h"
#include "memo/service/memo/MemoSvc.hpp"

#include <grpcpp/impl/codegen/completion_queue.h>
#include <grpcpp/impl/codegen/async_unary_call_impl.h>

namespace memo {
namespace service {
    class MemoSvc;
namespace process {


template<class RequestType, class ResponseType>
class BaseProcess : public Process
{
    using ResponseWriter_t = grpc::ServerAsyncResponseWriter<ResponseType>;

public:
    BaseProcess(MemoSvc& iSvc);
    ~BaseProcess();

    bool isFinished() const override;

    int serviceId() const override;

protected:
    enum State { PROCESSING, FINISHED };

    grpc::ServerContext context_;
    MemoSvc& svc_;
    ResponseWriter_t writer_;
    RequestType request_;
    ResponseType response_;
    State state_;
};

template<class ResponseType, class RequestType>
BaseProcess<ResponseType, RequestType>::BaseProcess(MemoSvc& iSvc) :
    svc_(iSvc), writer_(&context_)
{}

template<class ResponseType, class RequestType>
BaseProcess<ResponseType, RequestType>::~BaseProcess() = default;

template<class ResponseType, class RequestType>
bool BaseProcess<ResponseType, RequestType>::isFinished() const
{
    return state_ == FINISHED;
}

template<class ResponseType, class RequestType>
int BaseProcess<ResponseType, RequestType>::serviceId() const
{
    return svc_.getId();
}

} // namespace process
} // namespace service
} // namespace memo
