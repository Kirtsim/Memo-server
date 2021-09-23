#pragma once
#include "model/ModelDefs.hpp"
#include <string>
#include <vector>
#include <memory>

namespace memo { class Sqlite3Wrapper; }
namespace memo::test {

struct MemoValues { unsigned long id; std::string title; std::string description; unsigned long timestamp; };
struct TagValues { unsigned long id; std::string name; int color; unsigned long timestamp; };
struct TaggedValues { unsigned long memoId; unsigned long tagId; };

model::MemoPtr CreateMemo(const test::MemoValues& values);

model::TagPtr CreateTag(const test::TagValues& values);

test::MemoValues MemoToValues(const model::Memo& memo);

test::TagValues TagToValues(const model::Tag& tag);

std::vector<std::string> ToStringVector(const model::Memo& memo);

std::vector<std::string> ToStringVector(const model::Tag& tag);

std::vector<std::vector<std::string>> ExecCommand(Sqlite3Wrapper& sqlite3, const std::string& command);

} // namespace memo::test
