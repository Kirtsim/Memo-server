#include "db/Sqlite3Schema.hpp"
#include <sstream>

namespace {
std::string DropTableCmd(const std::string& tableName);
} // namespace

namespace memo::MemoTable::cmd {

std::string Drop()
{
    return ::DropTableCmd(MemoTable::kName);
}

std::string Create()
{
    std::stringstream command;
    command << "CREATE TABLE IF NOT EXISTS " << MemoTable::kName << " (\n"
            << MemoTable::att::kId << " INTEGER PRIMARY KEY,\n"
            << MemoTable::att::kTitle << " TEXT NOT NULL UNIQUE,\n"
            << MemoTable::att::kDescription << " TEXT,\n"
            << MemoTable::att::kTimestamp << " INTEGER NOT NULL);\n";
    return command.str();
}

} // memo::MemoTable::cmd

namespace memo::TagTable::cmd {

std::string Drop()
{
    return ::DropTableCmd(TagTable::kName);
}

std::string Create()
{
    std::stringstream command;
    command << "CREATE TABLE IF NOT EXISTS " << TagTable::kName << "(\n"
            << TagTable::att::kId << " INTEGER PRIMARY KEY,\n"
            << TagTable::att::kName << " TEXT NOT NULL UNIQUE,\n"
            << TagTable::att::kColor << " INTEGER,\n"
            << TagTable::att::kTimestamp << " INTEGER NOT NULL);\n";
    return command.str();
}

} // memo::MemoTable::cmd

namespace memo::TaggedTable::cmd {

std::string Drop()
{
    return ::DropTableCmd(TaggedTable::kName);
}

std::string Create()
{
    std::stringstream command;
    command << "CREATE TABLE IF NOT EXISTS " << TaggedTable::kName << "(\n"
            << TaggedTable::att::kMemoId << " INTEGER NOT NULL,\n"
            << TaggedTable::att::kTagId  << " INTEGER NOT NULL,\n"
            << "FOREIGN KEY (" << TaggedTable::att::kMemoId << ") REFERENCES "
            << MemoTable::kName << " (" << MemoTable::att::kId << "),\n"
            << "FOREIGN KEY (" << TaggedTable::att::kTagId << ") REFERENCES "
            << TagTable::kName << " (" << TagTable::att::kId << "));";
    return command.str();
}

} // memo::MemoTable::cmd

namespace {
std::string DropTableCmd(const std::string& tableName)
{
    return "DROP TABLE IF EXISTS " + tableName + ";";
}
} // namespace
