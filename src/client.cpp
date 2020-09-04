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
#include "model/TagSvc.grpc.pb.h"

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
using memo::model::Tag;
using memo::model::TagSvc;
using memo::model::TagName;
using memo::model::TagSearchRq;
using memo::model::TagSearchRs;

class Call
{
public:
    virtual ~Call() = default;
    virtual void exec() = 0;
    virtual void registerIn(CompletionQueue* icompletionQueue) = 0;
    virtual void startCall() = 0;

    virtual Status getStatus() const = 0;
};

template<class Stub, class Reply>
class BaseCall : public Call
{
public:
    BaseCall(Stub& ioStub) :
        stub_(ioStub) {}
    virtual ~BaseCall() = default;
    void startCall() override
    {
        reader->StartCall();
        reader->Finish(&reply, &status_, (void*)this);
    }

    Status getStatus() const override
    {
        return status_;
    }
protected:
    Stub& stub_;
    Reply reply;
    ClientContext context;
    Status status_;
    std::unique_ptr<ClientAsyncResponseReader<Reply>> reader;
};

class MemoSearchCall : public BaseCall<MemoSvc::Stub, MemoSearchRs>
{
public:
    MemoSearchCall(MemoSvc::Stub& ioStub, MemoSearchRq iRequest) :
        BaseCall(ioStub), request_(iRequest) {}
    ~MemoSearchCall() = default;

    void registerIn(CompletionQueue* iCompletionQueue) override
    {
        reader = stub_.PrepareAsyncSearch(&context, request_, iCompletionQueue);
    }

    void exec() override
    {
        std::cout << "[MemoSearch] Received response." << std::endl;
        std::cout << " - contains " << reply.memos().size() << " memos." << std::endl;
        for (const auto& memo : reply.memos())
            std::cout << "Memo titled: \"" << memo.title() << "\"" << std::endl;
    }
private:
    MemoSearchRq request_;
};

class MemoSearchByIdCall : public BaseCall<MemoSvc::Stub, MemoSearchRs>
{
public:
    MemoSearchByIdCall(MemoSvc::Stub& ioStub, IdList iRequest) :
        BaseCall(ioStub), request_(iRequest) {}
    ~MemoSearchByIdCall() = default;

    void registerIn(CompletionQueue* iCompletionQueue) override
    {
        reader = stub_.PrepareAsyncSearchById(&context, request_, iCompletionQueue);
    }

    void exec() override
    {
        std::cout << "[MemoSearchById] Received response." << std::endl;
        std::cout << " - contains " << reply.memos().size() << " memos." << std::endl;
        for (const auto& memo : reply.memos())
            std::cout << "Memo titled: \"" << memo.title() << "\"" << std::endl;
    }
private:
    IdList request_;
};

class MemoCreateCall : public BaseCall<MemoSvc::Stub, Id>
{
public:
    MemoCreateCall(MemoSvc::Stub& ioStub, Memo iRequest):
        BaseCall(ioStub), request_(iRequest) {}
    ~MemoCreateCall() = default;

    void registerIn(CompletionQueue* iCompletionQueue) override
    {
        reader = stub_.PrepareAsyncCreate(&context, request_, iCompletionQueue);
    }

    void exec() override
    {
        std::cout << "[MemoCreate] Received response containing id: ";
        std::cout << reply.value() << std::endl;
    }
private:
    Memo request_;
};

class MemoUpdateCall : public BaseCall<MemoSvc::Stub, OperationStatus>
{
public:
    MemoUpdateCall(MemoSvc::Stub& ioStub, Memo iRequest) :
        BaseCall(ioStub), request_(iRequest) {}
    ~MemoUpdateCall() = default;

    void registerIn(CompletionQueue* iCompletionQueue) override
    {
        reader = stub_.PrepareAsyncUpdate(&context, request_, iCompletionQueue);
    }

    void exec() override
    {
        std::cout << "[MemoUpdate] Received response with status: ";
        std::cout << reply.status() << std::endl;
    }
private:
    Memo request_;
};

class MemoDeleteCall : public BaseCall<MemoSvc::Stub, OperationStatus>
{
public:
    MemoDeleteCall(MemoSvc::Stub& ioStub, Id iRequest) :
        BaseCall(ioStub), request_(iRequest) {}
    ~MemoDeleteCall() = default;

    void registerIn(CompletionQueue* iCompletionQueue) override
    {
        reader = stub_.PrepareAsyncDelete(&context, request_, iCompletionQueue);
    }

    void exec() override
    {
        std::cout << "[MemoDelete] Received response with status: ";
        std::cout << reply.status() << std::endl;
    }
private:
    Id request_;
};

class TagSearchCall : public BaseCall<TagSvc::Stub, TagSearchRs>
{
public:
    TagSearchCall(TagSvc::Stub& ioStub, TagSearchRq iRequest) :
        BaseCall(ioStub), request_(iRequest) {}
    ~TagSearchCall() = default;

    void registerIn(CompletionQueue* iCompletionQueue) override
    {
        reader = stub_.PrepareAsyncSearch(&context, request_, iCompletionQueue);
    }

    void exec() override
    {
        std::cout << "[TagSearch] Received response." << std::endl;
        std::cout << " - contains " << reply.tags().size() << " tags." << std::endl;
        for (const auto& memo : reply.tags())
            std::cout << "Tag named: \"" << memo.name() << "\"" << std::endl;
    }
private:
    TagSearchRq request_;
};

class TagCreateCall : public BaseCall<TagSvc::Stub, OperationStatus>
{
public:
    TagCreateCall(TagSvc::Stub& ioStub, Tag iRequest):
        BaseCall(ioStub), request_(iRequest) {}
    ~TagCreateCall() = default;

    void registerIn(CompletionQueue* iCompletionQueue) override
    {
        reader = stub_.PrepareAsyncCreate(&context, request_, iCompletionQueue);
    }

    void exec() override
    {
        std::cout << "[TagCreate] Received response with status: ";
        std::cout << reply.status() << std::endl;
    }
private:
    Tag request_;
};

class TagUpdateCall : public BaseCall<TagSvc::Stub, OperationStatus>
{
public:
    TagUpdateCall(TagSvc::Stub& ioStub, Tag iRequest) :
        BaseCall(ioStub), request_(iRequest) {}
    ~TagUpdateCall() = default;

    void registerIn(CompletionQueue* iCompletionQueue) override
    {
        reader = stub_.PrepareAsyncUpdate(&context, request_, iCompletionQueue);
    }

    void exec() override
    {
        std::cout << "[TagUpdate] Received response with status: ";
        std::cout << reply.status() << std::endl;
    }
private:
    Tag request_;
};

class TagDeleteCall : public BaseCall<TagSvc::Stub, OperationStatus>
{
public:
    TagDeleteCall(TagSvc::Stub& ioStub, TagName iRequest) :
        BaseCall(ioStub), request_(iRequest) {}
    ~TagDeleteCall() = default;

    void registerIn(CompletionQueue* iCompletionQueue) override
    {
        reader = stub_.PrepareAsyncDelete(&context, request_, iCompletionQueue);
    }

    void exec() override
    {
        std::cout << "[TagDelete] Received response with status: ";
        std::cout << reply.status() << std::endl;
    }
private:
    TagName request_;
};


class Client {
  public:
    explicit Client(const std::string& iAddress) :
        memoStub_(MemoSvc::NewStub(grpc::CreateChannel(
                                   iAddress,
                                   grpc::InsecureChannelCredentials()))),
        tagStub_(TagSvc::NewStub(grpc::CreateChannel(
                                   iAddress,
                                   grpc::InsecureChannelCredentials())))
    {}

    // Assembles the client's payload and sends it to the server.
    void memoSearch(const std::string& iTitle) {
        // Data we are sending to the server.
        MemoSearchRq request;
        request.mutable_titleoptions()->set_startswith(iTitle);

        MemoSearchCall* call = new MemoSearchCall(*memoStub_, request);
        call->registerIn(&cq_);
        call->startCall();
    }

    void memoSearchById(const std::string& iId) {
        // Data we are sending to the server.
        IdList request;
        request.mutable_ids()->Add()->set_value(iId);

        MemoSearchByIdCall* call = new MemoSearchByIdCall(*memoStub_, request);
        call->registerIn(&cq_);
        call->startCall();
    }

    void createMemo(const std::string& iTitle)
    {
        Memo request;
        request.set_title(iTitle);

        MemoCreateCall* call = new MemoCreateCall(*memoStub_, request);
        call->registerIn(&cq_);
        call->startCall();
    }

    void updateMemo(const std::string& iTitle)
    {
        Memo request;
        request.set_title(iTitle);

        MemoUpdateCall* call = new MemoUpdateCall(*memoStub_, request);
        call->registerIn(&cq_);
        call->startCall();
    }

    void deleteMemo(const std::string& iId)
    {
        Id request;
        request.set_value(iId);

        MemoDeleteCall* call = new MemoDeleteCall(*memoStub_, request);
        call->registerIn(&cq_);
        call->startCall();
    }

    void tagSearch(const std::string& iTitle) {
        // Data we are sending to the server.
        TagSearchRq request;
        request.mutable_nameoptions()->set_startswith(iTitle);

        TagSearchCall* call = new TagSearchCall(*tagStub_, request);
        call->registerIn(&cq_);
        call->startCall();
    }

    void createTag(const std::string& iName)
    {
        Tag request;
        request.set_name(iName);

        TagCreateCall* call = new TagCreateCall(*tagStub_, request);
        call->registerIn(&cq_);
        call->startCall();
    }

    void updateTag(const std::string& iName)
    {
        Tag request;
        request.set_name(iName);

        TagUpdateCall* call = new TagUpdateCall(*tagStub_, request);
        call->registerIn(&cq_);
        call->startCall();
    }

    void deleteTag(const std::string& iName)
    {
        TagName request;
        request.set_value(iName);

        TagDeleteCall* call = new TagDeleteCall(*tagStub_, request);
        call->registerIn(&cq_);
        call->startCall();
    }

    void stop()
    {
        cq_.Shutdown();
        void* ignoredTag;
        bool ignoredOk;
        while (cq_.Next(&ignoredTag, &ignoredOk))
        {
            Call* call = static_cast<Call*>(ignoredTag);
            delete call;
        }
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

    std::unique_ptr<MemoSvc::Stub> memoStub_;
    std::unique_ptr<TagSvc::Stub> tagStub_;

    CompletionQueue cq_;
};

int main(int argc, char* argv[]) {

  	try
  	{
  	    if (argc != 3)
  	    {
  	      std::cerr << "Usage: http_server <address> <port>\n";
  	      std::cerr << "Ex   : http_server 127.0.0.1 8000\n";
  	      return 1;
        }
  	}
  	catch (std::exception& e)
  	{
        std::cout << "exception:\n" << e.what() << std::endl;
  	}

    const std::string ipAddress = argv[1];
    const std::string port = argv[2];
    Client client(ipAddress+":"+port);

    // Spawn reader thread that loops indefinitely
    std::thread thread_ = std::thread(&Client::AsyncCompleteRpc, &client);

    client.memoSearch("Test title1");
    client.tagSearch("Test tagName");
    client.memoSearchById("Test-id-102934048320");
    client.createMemo("Test-memo-create");
    client.createTag("Test-tag-create");
    client.updateMemo("Test-memo-update");
    client.updateTag("Test-tag-update");
    client.deleteMemo("Test-memo-id-delete-102930143290");
    client.deleteTag("Test-tag-name-delete-");

    std::cout << "Press <Enter> to stop the client." << std::endl << std::endl;
    std::cin.ignore();

    client.stop();
    thread_.join();
    return 0;
}
