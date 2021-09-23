#pragma once
#include "ModelTestHelperFunctions.hpp"
#include <string>

namespace memo { class Sqlite3Wrapper; }

namespace memo::test {

bool RecreateTables(Sqlite3Wrapper& sqlite3);

bool InsertMemoRow(Sqlite3Wrapper& sqlite3, const MemoValues& values);


bool InsertTagRow(Sqlite3Wrapper& sqlite3, const TagValues& values);


bool InsertTaggedRow(Sqlite3Wrapper& sqlite3, const TaggedValues& values);

std::vector<std::vector<std::string>> ExecCommand(Sqlite3Wrapper& sqlite3, const std::string& command);

} // namespace memo::test


