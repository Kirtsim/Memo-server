#include "memo/tools/RequestHandler.hpp"
#include "memo/tools/Tools.hpp"
#include "memo/Request.hpp"
#include "memo/Reply.hpp"
#include "logger/logger.hpp"

#include <iostream>
#include <fstream>

namespace memo {
namespace tools {

RequestHandler::RequestHandler(const std::string& iDocRoot) :
    docRoot(iDocRoot)
{}

void RequestHandler::handleRequest(const Request& iRequest, Reply& ioReply)
{
    std::string aRequestPath;
    if (!DecodeUrl(iRequest.getUri(), aRequestPath))
    {
        LOG_WRN("[RequestHandler] could not decode uri.");
        ioReply = Reply::StockReply(Reply::Status::bad_request);
        return;
    }
    LOG_INF("[RequestHandler] Decoded uri: " << aRequestPath);

    // Request path must be absolute and must not contain "..".
    if (not IsAbsolute(aRequestPath))
    {
        LOG_WRN("[RequestHandler] Uri is not an absolute path.");
        ioReply = Reply::StockReply(Reply::Status::bad_request);
        return;
    }

    // If path ends in slash (i.e. is a directory) then add "index.html".
    if (aRequestPath[aRequestPath.size() - 1] == '/')
    {
        aRequestPath += "index.html";
        LOG_INF("[RequestHandler] Setting uri to : " << aRequestPath);
    }

    std::string aFileExtension = FileExtensionOf(aRequestPath);
    std::string aFullFilePath  = docRoot + aRequestPath;
    std::string aContent;
    bool aReadSuccess = ReadFileContent(aFullFilePath, aContent);
    if (!aReadSuccess)
    {
        LOG_WRN("[RequestHandler] Could not read file content.");
        ioReply = Reply::StockReply(Reply::Status::not_found);
        return;
    }

    // Fill out the reply to be sent to the client.
    ioReply.setStatus(Reply::Status::ok);
    ioReply.setContent(aContent);
    ioReply.accessHeaders().resize(2);
    ioReply.accessHeaders()[0].setName("Content-Length");
    ioReply.accessHeaders()[0].setValue(std::to_string(aContent.size()));
    ioReply.accessHeaders()[1].setName("Content-Type");
    ioReply.accessHeaders()[1].setValue(Tools::Extension2Mime(aFileExtension));
}

bool RequestHandler::DecodeUrl(const std::string& iUrlStr, std::string& oOutput)
{
    oOutput.clear();
    oOutput.reserve(iUrlStr.size());
    for (std::size_t i = 0; i < iUrlStr.size(); ++i)
    {
        if (iUrlStr[i] == '%')
        {
            if (i + 3 > iUrlStr.size())
                return false;
            int value = 0;
            std::istringstream aInputStream(iUrlStr.substr(i + 1, 2));
            if (aInputStream >> std::hex >> value)
            {
                oOutput += static_cast<char>(value);
                i += 2;
            }
            else
            {
              return false;
            }
        }
        else if (iUrlStr[i] == '+')
        {
            oOutput += ' ';
        }
        else
        {
            oOutput += iUrlStr[i];
        }
    }
    return true;
}

bool RequestHandler::IsAbsolute(const std::string& iPath)
{
    return !iPath.empty()  &&
           iPath[0] == '/' &&
           iPath.find("..") == std::string::npos;
}

std::string RequestHandler::FileExtensionOf(const std::string& iPath)
{
    std::size_t aLastSlashPos = iPath.find_last_of("/");
    std::size_t aLastDotPos   = iPath.find_last_of(".");
    if (aLastSlashPos != std::string::npos && aLastDotPos > aLastSlashPos)
    {
        return iPath.substr(aLastDotPos + 1);
    }
    return "";
}

bool RequestHandler::ReadFileContent(const std::string& iFilePath, std::string& oContent)
{
    // Open the file to send back.
    std::ifstream aInputStream(iFilePath.c_str(), std::ios::in | std::ios::binary);
    if (!aInputStream)
    {
        return false;
    }

    char aBuffer[512];
    while (aInputStream.read(aBuffer, sizeof(aBuffer)).gcount() > 0)
        oContent.append(aBuffer, static_cast<size_t>(aInputStream.gcount()));
    return true;
}

} // namespace tools
} // namespace memo
