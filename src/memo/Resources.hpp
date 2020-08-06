#pragma once
#include <string>

namespace memo {
namespace manager {
    class ConnectionManager;
} // namespace manager

// Class that holds data/tools that should be globally accessible
class Resources
{
    using ConnectionManagerPtr_t = std::unique_ptr<manager::ConnectionManager>;
public:
    using Ptr = std::shared_ptr<Resources>;

    static Ptr Create(const std::string& iAddress,
                      const std::string& iPortNumber,
                      const std::string& iDocumentRoot,
                      ConnectionManagerPtr_t iConnectionManager);

    manager::ConnectionManager& getConnectionManager();

    const std::string& getAddress() const;
    const std::string& getPortNumber() const;
    const std::string& getDocumentRoot() const;

    Resources(const Resources&) = delete;
    Resources& operator=(const Resources&) = delete;

private:
    Resources(const std::string& iAddress,
              const std::string& iPortNumber,
              const std::string& iDocumentRoot,
              ConnectionManagerPtr_t iConnectionManager);

private:
    std::string address;
    std::string portNumber;
    std::string documentRoot;

    ConnectionManagerPtr_t connectionManager;
};

} // namespace memo
