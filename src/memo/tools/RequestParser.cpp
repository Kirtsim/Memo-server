#include "memo/tools/RequestParser.hpp"
#include "memo/Request.hpp"
#include "logger/logger.hpp"

#include <iostream>

namespace memo {
namespace tools {

RequestParser::RequestParser() :
    state(State::kMethod_start)
{
}

void RequestParser::reset()
{
    state  = State::kMethod_start;
    method = "";
    uri    = "";
    httpVersionMajor = 0;
    httpVersionMinor = 0;
    headers.clear();
}

boost::tribool RequestParser::consume(char iInputChar)
{
    switch (state)
    {
    case State::kMethod_start:
        if (!IsChar(iInputChar) || IsCtrlChar(iInputChar) || IsTspecial(iInputChar))
            return false;

        state = State::kMethod;
        method.push_back(iInputChar);
        return boost::indeterminate;

    case State::kMethod:
        if (iInputChar == ' ')
        {
            state = State::kUri;
            return boost::indeterminate;
        }

        if (!IsChar(iInputChar) || IsCtrlChar(iInputChar) || IsTspecial(iInputChar))
            return false;
        method.push_back(iInputChar);
        return boost::indeterminate;

    case State::kUri:
        if (IsCtrlChar(iInputChar))
            return false;
        if (iInputChar == ' ')
            state = State::kHttp_version_h;
        else
            uri.push_back(iInputChar);
        return boost::indeterminate;

    case State::kHttp_version_h:
        if (iInputChar == 'H')
        {
            state = State::kHttp_version_t_1;
            return boost::indeterminate;
        }
        return false;

    case State::kHttp_version_t_1:
        if (iInputChar == 'T')
        {
            state = State::kHttp_version_t_2;
            return boost::indeterminate;
        }
        return false;

    case State::kHttp_version_t_2:
        if (iInputChar == 'T')
        {
            state = State::kHttp_version_p;
            return boost::indeterminate;
        }
        return false;

    case State::kHttp_version_p:
        if (iInputChar == 'P')
        {
            state = State::kHttp_version_slash;
            return boost::indeterminate;
        }
          return false;

    case State::kHttp_version_slash:
        if (iInputChar == '/')
        {
            httpVersionMajor = 0;
            httpVersionMinor = 0;
            state = State::kHttp_version_major_start;
            return boost::indeterminate;
        }
        return false;

    case State::kHttp_version_major_start:
        if (IsDigit(iInputChar))
        {
            httpVersionMajor = httpVersionMajor * 10 + iInputChar - '0';
            state = State::kHttp_version_major;
            return boost::indeterminate;
        }
        return false;

    case State::kHttp_version_major:
        if (iInputChar == '.')
        {
            state = State::kHttp_version_minor_start;
            return boost::indeterminate;
        }
        if (IsDigit(iInputChar))
        {
            httpVersionMajor = httpVersionMajor * 10 + iInputChar - '0';
            return boost::indeterminate;
        }
        return false;

    case State::kHttp_version_minor_start:
        if (IsDigit(iInputChar))
        {
            httpVersionMinor = httpVersionMinor * 10 + iInputChar - '0';
            state = State::kHttp_version_minor;
            return boost::indeterminate;
        }
        return false;

    case State::kHttp_version_minor:
        if (iInputChar == '\r')
        {
            state = State::kExpecting_newline_1;
            return boost::indeterminate;
        }
        if (IsDigit(iInputChar))
        {
            httpVersionMinor = httpVersionMinor * 10 + iInputChar - '0';
            return boost::indeterminate;
        }
        return false;

    case State::kExpecting_newline_1:
        if (iInputChar == '\n')
        {
            state = State::kHeader_line_start;
            return boost::indeterminate;
        }
        return false;

    case State::kHeader_line_start:
        if (iInputChar == '\r')
        {
            state = State::kExpecting_newline_3;
            return boost::indeterminate;
        }
        if (!headers.empty() && (iInputChar == ' ' || iInputChar == '\t'))
        {
            state = State::kHeader_lws;
            return boost::indeterminate;
        }
        if (!IsChar(iInputChar) || IsCtrlChar(iInputChar) || IsTspecial(iInputChar))
        {
            return false;
        }
        else
        {
            headers.emplace_back();
            headers.back().name.push_back(iInputChar);
            state = State::kHeader_name;
            return boost::indeterminate;
        }

    case State::kHeader_lws:
        if (iInputChar == '\r')
        {
            state = State::kExpecting_newline_2;
            return boost::indeterminate;
        }
        if (iInputChar == ' ' || iInputChar == '\t')
        {
            return boost::indeterminate;
        }

        if (IsCtrlChar(iInputChar))
            return false;
        state = State::kHeader_value;
        headers.back().value.push_back(iInputChar);
        return boost::indeterminate;

    case State::kHeader_name:
        if (iInputChar == ':')
        {
            state = State::kSpace_before_header_value;
            return boost::indeterminate;
        }

        if (!IsChar(iInputChar) || IsCtrlChar(iInputChar) || IsTspecial(iInputChar))
            return false;
        headers.back().name.push_back(iInputChar);
        return boost::indeterminate;

    case State::kSpace_before_header_value:
        if (iInputChar == ' ')
        {
            state = State::kHeader_value;
            return boost::indeterminate;
        }
        return false;

    case State::kHeader_value:
        if (iInputChar == '\r')
        {
            state = State::kExpecting_newline_2;
            return boost::indeterminate;
        }

        if (IsCtrlChar(iInputChar))
            return false;
        headers.back().value.push_back(iInputChar);
        return boost::indeterminate;

    case State::kExpecting_newline_2:
        if (iInputChar == '\n')
        {
            state = State::kHeader_line_start;
            return boost::indeterminate;
        }
        return false;

    case State::kExpecting_newline_3:
        return (iInputChar == '\n');

    default:
        return false;
    }
}

void RequestParser::updateRequest(Request& oRequest)
{
    LOG_INF("[RequestParser] Received request:");
    LOG_INF("[RequestParser] -----------------");
    LOG_INF("[RequestParser]  method:       " << method);
    LOG_INF("[RequestParser]  uri:          " << uri);
    LOG_INF("[RequestParser]  http version: " << httpVersionMajor << "." << httpVersionMinor);
    LOG_INF("[RequestParser]  headers: ");

    oRequest.setMethod(method);
    oRequest.setUri(uri);
    oRequest.setHttpVersionMajor(httpVersionMajor);
    oRequest.setHttpVersionMinor(httpVersionMinor);
    for (const auto& aHeader : headers)
    {
        LOG_INF("[RequestParser]     +-  " << aHeader.name << ": " << aHeader.value);
        oRequest.addToHeaders( { aHeader.name, aHeader.value } );
    }
    LOG_INF("[RequestParser] +++++++++++++++++");
}

bool RequestParser::IsChar(int c)
{
    return c >= 0 && c <= 127;
}

bool RequestParser::IsCtrlChar(int c)
{
    return (c >= 0 && c <= 31) || (c == 127);
}

bool RequestParser::IsTspecial(int c)
{
    switch (c)
    {
    case '(': case ')': case '<': case '>':  case '@':
    case ',': case ';': case ':': case '\\': case '"':
    case '/': case '[': case ']': case '?':  case '=':
    case '{': case '}': case ' ': case '\t':
        return true;
    default:
        return false;
    }
}

bool RequestParser::IsDigit(int c)
{
    return c >= '0' && c <= '9';
}


} // namespace memo
} // namespace tools


