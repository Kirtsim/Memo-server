#pragma once
#include <memory>
#include <string>

namespace memo {

// Class that holds data/tools that should be globally accessible
class Resources
{
public:
    using Ptr = std::shared_ptr<Resources>;

    static Ptr Create(const std::string& iAddress,
                      const std::string& iPortNumber);

    const std::string& serverAddress() const;

    const std::string& portNumber() const;

    Resources(const Resources&) = delete;
    Resources& operator=(const Resources&) = delete;

private:
    Resources(const std::string& address,
              const std::string& portNumber);

private:
    std::string serverAddress_;
    std::string portNumber_;
};

} // namespace memo
