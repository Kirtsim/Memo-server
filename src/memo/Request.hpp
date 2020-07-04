#pragma once
#include "memo/Header.hpp"

#include <string>
#include <vector>

namespace memo {

class Request 
{
public:
    const std::string& getMethod() const
    {
        return method;
    }

    const std::string& getUri() const
    {
        return uri;
    }

    int getHttpVersionMajor() const
    {
        return httpVersionMajor;
    }

    int getHttpVersionMinor() const
    {
        return httpVersionMinor;
    }

    const std::vector<Header>& getHeaders() const
    {
        return headers;
    }

private:
    std::string method;
    std::string uri;
    int httpVersionMajor;
    int httpVersionMinor;
    std::vector<Header> headers;
};

} // namespace memo

