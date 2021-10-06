#include "server/Resources.hpp"

namespace memo {

Resources::Ptr Resources::Create(const std::string& address,
                                 const std::string& portNumber,
                                 std::unique_ptr<IDatabase> database)
{
    assert(database);
    return Ptr(new Resources(address, portNumber, std::move(database)));
}

Resources::Resources(const std::string& address,
                     const std::string& portNumber,
                     std::unique_ptr<IDatabase> database)
    : serverAddress_(address)
    , portNumber_(portNumber)
    , database_(std::move(database))
{}

const std::string& Resources::serverAddress() const
{
    return serverAddress_;
}

const std::string& Resources::portNumber() const
{
    return portNumber_;
}

IDatabase& Resources::database()
{
    return *database_;
}

} // namespace memo
