#pragma once
#include <string>

namespace memo {

class Reply;
class Request;

namespace tools {


/// The common handler for all incoming requests.
class RequestHandler
{
public:
    RequestHandler(const RequestHandler&) = delete;
    RequestHandler& operator=(const RequestHandler&) = delete;

    explicit RequestHandler(const std::string& iDocRoot);

    void handleRequest(const Request& iRequest, Reply& ioReply);

private:
    std::string docRoot;

    bool ReadFileContent(const std::string& iFilePath, std::string& oContent);

    static bool IsAbsolute(const std::string& iPath);
    static std::string FileExtensionOf(const std::string& iPath);
    static bool DecodeUrl(const std::string& iUrlStr, std::string& oOutput);
};
} // namespace tools
} // namespace memo
