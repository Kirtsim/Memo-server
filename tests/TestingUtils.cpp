#include "TestingUtils.hpp"

namespace memo::test {

std::string TestFilePath(const std::string& relativeFilePath)
{
    return std::string(TESTS_DIR) + "/test_files/" + relativeFilePath;
}

bool RemoveFile(const std::string& filePath)
{
    return std::remove(filePath.c_str()) == 0;
}

} // namespace memo::test