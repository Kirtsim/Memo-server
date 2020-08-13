#pragma once
#include <string>

namespace memo {

// Class that holds data/tools that should be globally accessible
class Resources
{
public:
    using Ptr = std::shared_ptr<Resources>;

    static Ptr Create(const std::string& iAddress,
                      const std::string& iPortNumber);

    const std::string& getAddress() const;
    const std::string& getPortNumber() const;

    Resources(const Resources&) = delete;
    Resources& operator=(const Resources&) = delete;

private:
    Resources(const std::string& iAddress,
              const std::string& iPortNumber);

private:
    std::string address;
    std::string portNumber;
};

} // namespace memo
