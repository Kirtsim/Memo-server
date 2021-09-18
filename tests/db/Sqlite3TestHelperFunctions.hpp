#pragma once
#include <string>

namespace memo { class Sqlite3Wrapper; }

namespace memo::test {

struct MemoValues { int id; std::string title; std::string description; long timestamp; };
struct TagValues { int id; std::string name; int color; long timestamp; };
struct TaggedValues { int memoId; int tagId; };

bool RecreateTables(Sqlite3Wrapper& sqlite3);

bool InsertMemoRow(Sqlite3Wrapper& sqlite3, const MemoValues& values);


bool InsertTagRow(Sqlite3Wrapper& sqlite3, const TagValues& values);


bool InsertTaggedRow(Sqlite3Wrapper& sqlite3, const TaggedValues& values);


} // namespace memo::test


