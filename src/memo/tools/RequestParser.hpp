#pragma once

#include <vector>
#include <string>
#include <boost/logic/tribool.hpp>
#include <boost/tuple/tuple.hpp>

namespace memo {

    class Request;

namespace tools {

class RequestParser
{
public:

    RequestParser();

    void reset();

    /// Parse some data. The tribool return value is true when a complete request
    /// has been parsed, false if the data is invalid, indeterminate when more
    /// data is required. The InputIterator return value indicates how much of the
    /// input has been consumed.
    template <typename InputIterator>
    boost::tuple<boost::tribool, InputIterator> parse(Request& ioRequest,
                                                      InputIterator ioBeginIt, 
                                                      InputIterator ioEndIt)
    {
        while (ioBeginIt != ioEndIt)
        {
            boost::tribool aResult = consume(*ioBeginIt++);
            if (aResult || !aResult)
            {
                updateRequest(ioRequest);
                return boost::make_tuple(aResult, ioBeginIt);
            }
        }
        boost::tribool aResult = boost::indeterminate;
        return boost::make_tuple(aResult, ioBeginIt);
    }

private:
    /// Handle the next character of input.
    boost::tribool consume(char iInput);

    void updateRequest(Request& oRequest);

    static bool IsChar(int iByte);

    /// Check if a byte is an HTTP control character.
    static bool IsCtrlChar(int iByte);

    /// Check if a byte is defined as an HTTP tspecial character.
    static bool IsTspecial(int iByte);

    /// Check if a byte is a digit.
    static bool IsDigit(int iByte);

    /// The current state of the parser.
    enum State
    {
        kMethod_start,
        kMethod,
        kUri,
        kHttp_version_h,
        kHttp_version_t_1,
        kHttp_version_t_2,
        kHttp_version_p,
        kHttp_version_slash,
        kHttp_version_major_start,
        kHttp_version_major,
        kHttp_version_minor_start,
        kHttp_version_minor,
        kExpecting_newline_1,
        kHeader_line_start,
        kHeader_lws,
        kHeader_name,
        kSpace_before_header_value,
        kHeader_value,
        kExpecting_newline_2,
        kExpecting_newline_3
    };

    struct Header 
    {
        std::string name;
        std::string value;
    };

    State state;
    std::string method;
    std::string uri;
    int httpVersionMajor = 0;
    int httpVersionMinor = 0;
    std::vector<Header> headers;
};

} // namespace memo
} // namespace tools


