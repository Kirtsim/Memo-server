#include "memo/tools/Tools.hpp"

#include <map>

namespace memo {
namespace tools {

namespace {
const std::string kDefaultMime = "text/plain";
const std::map<std::string, std::string> kExtension2MimeMap {
    { "gif",  "image/gif" },
    { "htm",  "text/html" },
    { "html", "text/html" },
    { "jpg",  "image/jpeg" },
    { "png",  "image/png" },
};
} // namespace

    
std::string Tools::Extension2Mime(const std::string& iExtension)
{
    auto aIt = kExtension2MimeMap.find(iExtension);
    if (aIt != std::end(kExtension2MimeMap))
        return aIt->second;
    return kDefaultMime;
}

} // namespace memo
} // namespace tools

