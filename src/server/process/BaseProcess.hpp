#pragma once
#include "server/process/IProcess.hpp"

#include <grpcpp/impl/codegen/completion_queue.h>
#include <grpcpp/impl/codegen/server_context.h>
#include <grpcpp/impl/codegen/async_unary_call.h>

namespace memo::process {


template<class ServiceType, class RequestType, class ResponseType>
class BaseProcess : public IProcess
{
    using ResponseWriter_t = grpc::ServerAsyncResponseWriter<ResponseType>;

public:
    explicit BaseProcess(ServiceType& service);
    ~BaseProcess() override;

    bool isFinished() const override;

    int serviceId() const override;

protected:
    enum State { PROCESSING, FINISHED };

    grpc::ServerContext context_;
    ServiceType& svc_;
    ResponseWriter_t writer_;
    RequestType request_;
    ResponseType response_;
    State state_;
};

template<class ServiceType, class RequestType, class ResponseType>
        BaseProcess<ServiceType, RequestType, ResponseType>::BaseProcess(ServiceType& service) :
        svc_(service), writer_(&context_)
{
}

template<class ServiceType, class RequestType, class ResponseType>
BaseProcess<ServiceType, RequestType, ResponseType>::~BaseProcess() = default;

template<class ServiceType, class RequestType, class ResponseType>
bool BaseProcess<ServiceType, RequestType, ResponseType>::isFinished() const
{
    return state_ == FINISHED;
}

template<class ServiceType, class RequestType, class ResponseType>
int BaseProcess<ServiceType, RequestType, ResponseType>::serviceId() const
{
    return svc_.getId();
}

} // namespace memo::process
