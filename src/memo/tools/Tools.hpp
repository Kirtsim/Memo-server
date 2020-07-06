#pragma once
#include <string>

namespace memo {
namespace tools {

class Tools
{
public:
    Tools() = delete;
    Tools& operator=(const Tools&) = delete;
    
    static std::string Extension2Mime(const std::string& iExtension);
};

} // namespace memo
} // namespace tools


