#include "server/Resources.hpp"

namespace memo {

Resources::Ptr Resources::Create(const std::string& address,
                                 const std::string& portNumber)
{
    return Ptr(new Resources(address, portNumber));
}

Resources::Resources(const std::string& address,
                     const std::string& portNumber) :
    serverAddress_(address),
    portNumber_(portNumber)
{}

const std::string& Resources::serverAddress() const
{
    return serverAddress_;
}

const std::string& Resources::portNumber() const
{
    return portNumber_;
}

} // namespace memo
