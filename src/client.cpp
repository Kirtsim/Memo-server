/*
 *
 * Copyright 2015 gRPC authors.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */
#include "model/MemoSvc.grpc.pb.h"

#include <iostream>
#include <memory>
#include <string>

#include <grpcpp/grpcpp.h>
#include <grpc/support/log.h>
#include <thread>


using grpc::Channel;
using grpc::ClientAsyncResponseReader;
using grpc::ClientContext;
using grpc::CompletionQueue;
using grpc::Status;
using memo::model::MemoSearchRq;
using memo::model::MemoSearchRs;
using memo::model::IdList;
using memo::model::Memo;
using memo::model::Id;
using memo::model::OperationStatus;
using memo::model::MemoSvc;

class Call
{
public:
    virtual ~Call() = default;
    virtual void exec() = 0;
    virtual void registerIn(MemoSvc::Stub& iStub, CompletionQueue* icompletionQueue) = 0;
    virtual void startCall() = 0;
    virtual void finish() = 0;

    virtual Status getStatus() const = 0;
};

template<class Reply>
class BaseCall : public Call
{
public:
    virtual ~BaseCall() = default;
    void startCall() override
    {
        reader->StartCall();
    }

    void finish() override
    {
        reader->Finish(&reply, &status_, (void*)this);
    }

    Status getStatus() const override
    {
        return status_;
    }
protected:
    Reply reply;
    ClientContext context;
    Status status_;
    std::unique_ptr<ClientAsyncResponseReader<Reply>> reader;
};

class MemoSearchCall : public BaseCall<MemoSearchRs>
{
public:
    MemoSearchCall(MemoSearchRq iRequest) : BaseCall(), request_(iRequest) {}
    ~MemoSearchCall() = default;

    void registerIn(MemoSvc::Stub& iStub, CompletionQueue* iCompletionQueue) override
    {
        reader = iStub.PrepareAsyncSearch(&context, request_, iCompletionQueue);
    }

    void exec() override
    {
        std::cout << "[svc.search()] Received response." << std::endl;
        std::cout << " - contains " << reply.memos().size() << " memos." << std::endl;
        for (const auto& memo : reply.memos())
            std::cout << "Memo titled: \"" << memo.title() << "\"" << std::endl;
    }
private:
    MemoSearchRq request_;
};

class MemoSearchByIdCall : public BaseCall<MemoSearchRs>
{
public:
    MemoSearchByIdCall(IdList iRequest) : BaseCall(), request_(iRequest) {}
    ~MemoSearchByIdCall() = default;

    void registerIn(MemoSvc::Stub& iStub, CompletionQueue* iCompletionQueue) override
    {
        reader = iStub.PrepareAsyncSearchById(&context, request_, iCompletionQueue);
    }

    void exec() override
    {
        std::cout << "[svc.search()] Received response." << std::endl;
        std::cout << " - contains " << reply.memos().size() << " memos." << std::endl;
        for (const auto& memo : reply.memos())
            std::cout << "Memo titled: \"" << memo.title() << "\"" << std::endl;
    }
private:
    IdList request_;
};

class MemoCreateCall : public BaseCall<Id>
{
public:
    MemoCreateCall(Memo iRequest): BaseCall(), request_(iRequest) {}
    ~MemoCreateCall() = default;

    void registerIn(MemoSvc::Stub& iStub, CompletionQueue* iCompletionQueue) override
    {
        reader = iStub.PrepareAsyncCreate(&context, request_, iCompletionQueue);
    }

    void exec() override
    {
        std::cout << "[svc.create()] Received response containing id: ";
        std::cout << reply.value() << std::endl;
    }
private:
    Memo request_;
};

class MemoUpdateCall : public BaseCall<OperationStatus>
{
public:
    MemoUpdateCall(Memo iRequest) : BaseCall(), request_(iRequest) {}
    ~MemoUpdateCall() = default;

    void registerIn(MemoSvc::Stub& iStub, CompletionQueue* iCompletionQueue) override
    {
        reader = iStub.PrepareAsyncUpdate(&context, request_, iCompletionQueue);
    }

    void exec() override
    {
        std::cout << "[svc.update()] Received response with status: ";
        std::cout << reply.status() << std::endl;
    }
private:
    Memo request_;
};

class MemoDeleteCall : public BaseCall<OperationStatus>
{
public:
    MemoDeleteCall(Id iRequest) : BaseCall(), request_(iRequest) {}
    ~MemoDeleteCall() = default;

    void registerIn(MemoSvc::Stub& iStub, CompletionQueue* iCompletionQueue) override
    {
        reader = iStub.PrepareAsyncDelete(&context, request_, iCompletionQueue);
    }

    void exec() override
    {
        std::cout << "[svc.update()] Received response with status: ";
        std::cout << reply.status() << std::endl;
    }
private:
    Id request_;
};


class MemoClient {
  public:
    explicit MemoClient(std::shared_ptr<Channel> channel)
            : stub_(MemoSvc::NewStub(channel)) {}

    // Assembles the client's payload and sends it to the server.
    void search(const std::string& iTitle) {
        // Data we are sending to the server.
        MemoSearchRq request;
        request.mutable_titleoptions()->set_startswith(iTitle);

        MemoSearchCall* call = new MemoSearchCall(request);
        call->registerIn(*stub_, &cq_);
        call->startCall();
        call->finish();
    }

    void searchById(const std::string& iId) {
        // Data we are sending to the server.
        IdList request;
        request.mutable_ids()->Add()->set_value(iId);

        MemoSearchByIdCall* call = new MemoSearchByIdCall(request);
        call->registerIn(*stub_, &cq_);
        call->startCall();
        call->finish();
    }

    void create(const std::string& iTitle)
    {
        Memo request;
        request.set_title(iTitle);

        MemoCreateCall* call = new MemoCreateCall(request);
        call->registerIn(*stub_, &cq_);
        call->startCall();
        call->finish();
    }

    void update(const std::string& iTitle)
    {
        Memo request;
        request.set_title(iTitle);

        MemoUpdateCall* call = new MemoUpdateCall(request);
        call->registerIn(*stub_, &cq_);
        call->startCall();
        call->finish();
    }

    void deleteMemo(const std::string& iId)
    {
        Id request;
        request.set_value(iId);

        MemoDeleteCall* call = new MemoDeleteCall(request);
        call->registerIn(*stub_, &cq_);
        call->startCall();
        call->finish();
    }

    // Loop while listening for completed responses.
    // Prints out the response from the server.
    void AsyncCompleteRpc() {
        void* got_tag;
        bool ok = false;

        // Block until the next result is available in the completion queue "cq".
        while (cq_.Next(&got_tag, &ok)) {
            // The tag in this example is the memory location of the call object
            Call* call = static_cast<Call*>(got_tag);

            // Verify that the request was completed successfully. Note that "ok"
            // corresponds solely to the request for updates introduced by Finish().
            GPR_ASSERT(ok);

            if (call->getStatus().ok())
            {
                call->exec();
            }
            else
                std::cout << "RPC failed" << std::endl;

            // Once we're complete, deallocate the call object.
            delete call;
        }
    }

  private:

    std::unique_ptr<MemoSvc::Stub> stub_;

    CompletionQueue cq_;
};

int main(int argc, char* argv[]) {

  	try
  	{
  	    if (argc != 3)
  	    {
  	      std::cerr << "Usage: http_server <address> <port> <doc_root>\n";
  	      std::cerr << "  For IPv4, try:\n";
  	      std::cerr << "    receiver 0.0.0.0 80 .\n";
  	      std::cerr << "  For IPv6, try:\n";
  	      std::cerr << "    receiver 0::0 80 .\n";
  	      return 1;
        }
  	}
  	catch (std::exception& e)
  	{
        std::cout << "exception:\n" << e.what() << std::endl;
  	}

    const std::string ipAddress = argv[1];
    const std::string port = argv[2];
    MemoClient client(grpc::CreateChannel(
            (ipAddress+":"+port), grpc::InsecureChannelCredentials()));

    // Spawn reader thread that loops indefinitely
    std::thread thread_ = std::thread(&MemoClient::AsyncCompleteRpc, &client);

    client.search("Test title1");
    client.searchById("Test-id-102934048320");
    client.create("Test-title-create");
    client.update("Test-title-update");
    client.deleteMemo("Test-id-delete-102930143290");

    std::cout << "Press control-c to quit" << std::endl << std::endl;
    thread_.join();  //blocks forever

    return 0;
}
