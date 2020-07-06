#pragma once
#include "memo/Header.hpp"

#include <string>
#include <vector>

namespace memo {

class Request 
{
public:
    void setMethod(const std::string& iMethod)
    {
        method = iMethod;
    }

    const std::string& getMethod() const
    {
        return method;
    }

    void setUri(const std::string& iUri)
    {
        uri = iUri;
    }

    const std::string& getUri() const
    {
        return uri;
    }

    void setHttpVersionMajor(int iVersion)
    {
        httpVersionMajor = iVersion;
    }

    int getHttpVersionMajor() const
    {
        return httpVersionMajor;
    }

    void setHttpVersionMinor(int iVersion)
    {
        httpVersionMinor = iVersion;
    }

    int getHttpVersionMinor() const
    {
        return httpVersionMinor;
    }

    void addToHeaders(const Header& iHeader)
    {
        headers.emplace_back(iHeader);
    }

    std::vector<Header>& accessHeaders()
    {
        return headers;
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

