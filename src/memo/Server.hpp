#pragma once
#include "memo/service/memo/MemoSvc.hpp"
#include "memo/Resources.hpp"

#include <grpcpp/server.h>

#include <map>

namespace memo {

class Server
{
public:
    Server(const std::string& iIpAddress, const std::string& iPort);
    ~Server();

    void run();
private:
    std::string ipAddress_;
    std::string port_;
    memo::Resources::Ptr resources_;

    std::unique_ptr<service::MemoSvc> memoService_;
    std::unique_ptr<grpc::Server> server_;
};

} // namespace memo
