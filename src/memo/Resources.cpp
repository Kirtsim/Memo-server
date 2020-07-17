#include "memo/Resources.hpp"
#include "memo/manager/ConnectionManager.hpp"

namespace memo {
Resources::Ptr Resources::Create(const std::string& iAddress,
                                 const std::string& iPortNumber,
                                 const std::string& iDocumentRoot,
                                 ConnectionManagerPtr_t iConnectionManager)
{
    if (!iConnectionManager)
        throw std::invalid_argument("Failed to create Resources class. "
                                    "Reason: ConnectionManager is NULL"); 

    return Ptr(new Resources(iAddress, iPortNumber, iDocumentRoot, 
                             std::move(iConnectionManager)));
}

Resources::Resources(const std::string& iAddress,
                     const std::string& iPortNumber,
                     const std::string& iDocumentRoot,
                     ConnectionManagerPtr_t iConnectionManager) :
    address(iAddress),
    portNumber(iPortNumber),
    documentRoot(iDocumentRoot),
    connectionManager(std::move(iConnectionManager))
{}

manager::ConnectionManager& Resources::getConnectionManager()
{
    return *connectionManager;
}

const std::string& Resources::getAddress() const
{
    return address;
}

const std::string& Resources::getPortNumber() const
{
    return portNumber;
}

const std::string& Resources::getDocumentRoot() const
{
    return documentRoot;
}

} // namespace memo 
