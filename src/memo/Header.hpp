#pragma once
#include <string>

namespace memo {

class Header
{
public:
    void setName(const std::string& iName)
    {
        name = iName;
    }

    const std::string& getName() const
    {
        return name;
    }

    void setValue(const std::string& iValue)
    {
        value = iValue;
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

