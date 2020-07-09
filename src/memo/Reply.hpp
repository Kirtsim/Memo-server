#pragma once
#include "memo/Header.hpp"
#include <boost/asio.hpp>

#include <vector>
#include <string>

namespace memo {

class Reply
{
public:
    enum Status {
        ok = 200,
        created = 201,
        accepted = 202,
        no_content = 204,
        multiple_choices = 300,
        moved_permanently = 301,
        moved_temporarily = 302,
        not_modified = 304,
        bad_request = 400,
        unauthorized = 401,
        forbidden = 403,
        not_found = 404,
        internal_server_error = 500,
        not_implemented = 501,
        bad_gateway = 502,
        service_unavailable = 503
    };

    std::vector<boost::asio::const_buffer> toBuffers();

    void setStatus(Status iStatus)
    {
        status = iStatus;
    }

    std::vector<Header>& accessHeaders()
    {
        return headers;
    }

    void addToHeaders(const Header& iHeader)
    {
        headers.push_back(iHeader);
    }

    void setContent(const std::string& iContent)
    {
        content = iContent;
    }

    const std::string& getContent() const
    {
        return content;
    }

    static Reply StockReply(Status iStatus);
    
private:
    Status status;
    std::vector<Header> headers;       
    std::string content;
};

}
