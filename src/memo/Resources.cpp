#include "memo/Resources.hpp"

namespace memo {
Resources::Ptr Resources::Create(const std::string& iAddress,
                                 const std::string& iPortNumber)
{
    return Ptr(new Resources(iAddress, iPortNumber));
}

Resources::Resources(const std::string& iAddress,
                     const std::string& iPortNumber) :
    address(iAddress),
    portNumber(iPortNumber)
{}

const std::string& Resources::getAddress() const
{
    return address;
}

const std::string& Resources::getPortNumber() const
{
    return portNumber;
}

} // namespace memo
