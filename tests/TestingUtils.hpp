#pragma once
#include <string>

namespace memo::test {

/// @brief Returns the absolute file path of a file defined by the relative path
///        passed to this function. The relative path is relative to the directory
///        where test files are stored.
/// @param relativeFilePath relative file path within the directory for test files.
/// @return the absolute file path to the test file in the test-files directory.
std::string TestFilePath(const std::string& relativeFilePath);

/// @brief Removes the file specified by the file path.
/// @param filePath path to the file to be removed.
/// @return true if file was removed, false otherwise
bool RemoveFile(const std::string& filePath);

} // namespace memo::test


