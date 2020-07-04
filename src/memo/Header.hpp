#pragma once
#include <string>

namespace memo {

class Header
{
public:
    const std::string& getName() const
    {
        return name;
    }

    const std::string& getValue() const
    {
        return value;
    }

private:
    std::string name;
    std::string value;
};

} // namespace memo

