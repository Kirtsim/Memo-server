#include "db/Sqlite3Schema.hpp"
#include "db/Sqlite3TestHelperFunctions.hpp"
#include "db/Sqlite3Wrapper.hpp"
#include <sstream>

namespace memo::test {

namespace {
std::string CreateMemoTableCommand();
std::string CreateTagTableCommand();
std::string CreateTaggedTableCommand();
} // namespace

bool RecreateTables(Sqlite3Wrapper& sqlite3)
{
    std::stringstream command;
    command << "DROP TABLE IF EXISTS " << TaggedTable::kName << ";\n"
            << "DROP TABLE IF EXISTS " << MemoTable::kName << ";\n"
            << "DROP TABLE IF EXISTS " << TagTable::kName << ";\n"
            << CreateMemoTableCommand()
            << CreateTagTableCommand()
            << CreateTaggedTableCommand();

    return sqlite3.exec(command.str(), nullptr);
}

bool InsertMemoRow(memo::Sqlite3Wrapper& sqlite3, const MemoValues& values)
{
    std::stringstream stream;
    stream << "INSERT INTO Memo (id, title, description, timestamp) "
           << "VALUES (" << values.id << ", '" << values.title << "', '"
           << values.description << "', " << values.timestamp << ");";
    return sqlite3.exec(stream.str(), nullptr);
}

bool InsertTagRow(memo::Sqlite3Wrapper& sqlite3, const TagValues& values)
{
    std::stringstream stream;
    stream << "INSERT INTO Tag (id, name, color, timestamp) "
           << "VALUES (" << values.id << ", '" << values.name << "', "
           << values.color << ", " << values.timestamp << ");";
    return sqlite3.exec(stream.str(), nullptr);
}

bool InsertTaggedRow(memo::Sqlite3Wrapper& sqlite3, const TaggedValues& values)
{
    std::stringstream stream;
    stream << "INSERT INTO Tagged (memoId, tagId) "
           << "VALUES (" << values.memoId << ", " << values.tagId << ");";
    return sqlite3.exec(stream.str(), nullptr);
}

std::vector<std::vector<std::string>> ExecCommand(Sqlite3Wrapper& sqlite3, const std::string& command)
{
    std::vector<std::vector<std::string>> returnedValues;
    auto callback = [&](const std::vector<std::string>& values, const std::vector<std::string>&)
    {
        returnedValues.emplace_back(values);
        return false;
    };
    sqlite3.exec(command, callback);
    return returnedValues;
}

namespace {
std::string CreateMemoTableCommand()
{
    std::stringstream command;
    command << "CREATE TABLE IF NOT EXISTS " << MemoTable::kName << " (\n"
            << MemoTable::att::kId << " INTEGER PRIMARY KEY,\n"
            << MemoTable::att::kTitle << " TEXT NOT NULL UNIQUE,\n"
            << MemoTable::att::kDescription << " TEXT,\n"
            << MemoTable::att::kTimestamp << " INTEGER NOT NULL);\n";
    return command.str();
}

std::string CreateTagTableCommand()
{
    std::stringstream command;
    command << "CREATE TABLE IF NOT EXISTS " << TagTable::kName << "(\n"
            << TagTable::att::kId << " INTEGER PRIMARY KEY,\n"
            << TagTable::att::kName << " TEXT NOT NULL UNIQUE,\n"
            << TagTable::att::kColor << " INTEGER,\n"
            << TagTable::att::kTimestamp << " INTEGER NOT NULL);\n";
    return command.str();
}

std::string CreateTaggedTableCommand()
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
} // namespace
} // namespace memo::test