#include "Utils.hpp"

namespace memo::test {

std::string TestFilePath(const std::string& relativeFilePath)
{
    return std::string(TESTS_DIR) + "/test_files/" + relativeFilePath;
}

} // namespace memo::test