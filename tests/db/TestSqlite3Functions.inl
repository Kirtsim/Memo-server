#pragma once
#include "db/Sqlite3Functions.hpp"
#include "db/Sqlite3Wrapper.hpp"
#include "db/Sqlite3Schema.hpp"
#include "db/Tools.hpp"
#include "model/Memo.hpp"
#include "model/Tag.hpp"
#include "Utils.hpp"
#include <gtest/gtest.h>
#include <sstream>

namespace memo {

namespace {
struct MemoValues { int id; std::string title; std::string description; long timestamp; };
struct TagValues { int id; std::string name; int color; long timestamp; };
struct TaggedValues { int memoId; int tagId; };

bool CreateTables(Sqlite3Wrapper& sqlite3);

bool InsertMemoRow(Sqlite3Wrapper& sqlite3, const MemoValues& values);

bool InsertTagRow(Sqlite3Wrapper& sqlite3, const TagValues& values);

bool InsertTaggedRow(Sqlite3Wrapper& sqlite3, const TaggedValues& values);

}

TEST(TestSqlite3Wrapper, test_UpdateMemoTable)
{
    model::Memo memo;
    memo.setId(1);
    memo.setTitle("New title");
    memo.setDescription("New desc");
    memo.setTimestamp(12345);

    const auto tempDbFile = test::TestFilePath("temp__.db");
    Sqlite3Wrapper sqlite3(tempDbFile);
    sqlite3.open();
    ASSERT_TRUE(CreateTables(sqlite3));
    ASSERT_TRUE(InsertMemoRow(sqlite3, {1, "Old title", "Old desc", 11111}));

    EXPECT_TRUE(UpdateMemoTable(memo, sqlite3));
    std::vector<std::vector<std::string>> returnedValues;
    auto callback = [&](const std::vector<std::string>& values, const std::vector<std::string>&)
    {
        returnedValues.emplace_back(values);
        return false;
    };

    sqlite3.exec("SELECT * FROM Memo WHERE id=1;", callback);
    ASSERT_EQ(returnedValues.size(), 1ul);
    ASSERT_EQ(returnedValues[0].size(), 4ul);
    EXPECT_EQ(returnedValues[0][1], memo.title());
    EXPECT_EQ(returnedValues[0][2], memo.description());
    EXPECT_EQ(returnedValues[0][3], std::to_string(memo.timestamp()));
}

TEST(TestSqlite3Wrapper, test_UpdateTagTable)
{
    model::Tag tag;
    tag.setId(1);
    tag.setName("New name");
    tag.setColor({10, 10, 10});
    tag.setTimestamp(22222);

    const auto tempDbFile = test::TestFilePath("temp__.db");
    Sqlite3Wrapper sqlite3(tempDbFile);
    sqlite3.open();
    ASSERT_TRUE(CreateTables(sqlite3));
    ASSERT_TRUE(InsertTagRow(sqlite3, {1, "Old name", 12345, 11111}));

    EXPECT_TRUE(UpdateTagTable(tag, sqlite3));
    std::vector<std::vector<std::string>> returnedValues;
    auto callback = [&](const std::vector<std::string>& values, const std::vector<std::string>&)
    {
        returnedValues.emplace_back(values);
        return false;
    };

    sqlite3.exec("SELECT * FROM Tag WHERE id=1;", callback);
    ASSERT_EQ(returnedValues.size(), 1ul);
    ASSERT_EQ(returnedValues[0].size(), 4ul);
    EXPECT_EQ(returnedValues[0][1], tag.name());
    EXPECT_EQ(returnedValues[0][2], std::to_string(tools::ColorToInt(tag.color())));
    EXPECT_EQ(returnedValues[0][3], std::to_string(tag.timestamp()));
}


TEST(TestSqlite3Wrapper, test_SelectMemoTagIds)
{
    model::Memo memo;
    memo.setId(1);
    memo.setTitle("New title");
    memo.setDescription("New desc");
    memo.setTimestamp(12345);

    const auto tempDbFile = test::TestFilePath("temp__.db");
    Sqlite3Wrapper sqlite3(tempDbFile);
    sqlite3.open();

    ASSERT_TRUE(CreateTables(sqlite3));
    ASSERT_TRUE(InsertTagRow(sqlite3, {1, "Tag 1", 11111, 1111111}));
    ASSERT_TRUE(InsertTagRow(sqlite3, {2, "Tag 2", 22222, 2222222}));
    ASSERT_TRUE(InsertTagRow(sqlite3, {3, "Tag 3", 33333, 3333333}));
    ASSERT_TRUE(InsertMemoRow(sqlite3, {(int)memo.id(), memo.title(), memo.description(), (int)memo.timestamp()}));
    ASSERT_TRUE(InsertTaggedRow(sqlite3, {1, 1}));
    ASSERT_TRUE(InsertTaggedRow(sqlite3, {1, 3}));

    std::vector<unsigned long> expectedTagIds { 1, 3 };
    std::vector<unsigned long> selectedTagIds;
    EXPECT_TRUE(SelectMemoTagIds(memo, selectedTagIds, sqlite3));
    EXPECT_EQ(selectedTagIds, expectedTagIds);
}

TEST(TestSqlite3Wrapper, test_InsertMemoTagIds)
{
    model::Memo memo;
    memo.setId(1);
    memo.setTitle("New title");
    memo.setDescription("New desc");
    memo.setTimestamp(12345);

    const auto tempDbFile = test::TestFilePath("temp__.db");
    Sqlite3Wrapper sqlite3(tempDbFile);
    sqlite3.open();

    ASSERT_TRUE(CreateTables(sqlite3));
    ASSERT_TRUE(InsertTagRow(sqlite3, {1, "Tag 1", 11111, 1111111}));
    ASSERT_TRUE(InsertTagRow(sqlite3, {2, "Tag 2", 22222, 2222222}));
    ASSERT_TRUE(InsertTagRow(sqlite3, {3, "Tag 3", 33333, 3333333}));
    ASSERT_TRUE(InsertMemoRow(sqlite3, {(int)memo.id(), memo.title(), memo.description(), (int)memo.timestamp()}));

    const std::vector<unsigned long> expectedTagIds { 1, 3 };
    EXPECT_TRUE(InsertMemoTagIds(memo, expectedTagIds, sqlite3));

    std::vector<unsigned long> returnedTagIds;
    auto callback = [&](const std::vector<std::string>& values, const std::vector<std::string>&)
    {
        if (!values.empty())
            returnedTagIds.emplace_back(std::stoul(values.front()));
        return false;
    };
    sqlite3.exec("SELECT tagId FROM Tagged WHERE memoId=1 ORDER BY tagId;", callback);
    EXPECT_EQ(returnedTagIds, expectedTagIds);
}


TEST(TestSqlite3Wrapper, test_DeleteMemoTagIds)
{
    model::Memo memo;
    memo.setId(1);
    memo.setTitle("New title");
    memo.setDescription("New desc");
    memo.setTimestamp(12345);

    const auto tempDbFile = test::TestFilePath("temp__.db");
    Sqlite3Wrapper sqlite3(tempDbFile);
    sqlite3.open();

    ASSERT_TRUE(CreateTables(sqlite3));
    ASSERT_TRUE(InsertTagRow(sqlite3, {1, "Tag 1", 11111, 1111111}));
    ASSERT_TRUE(InsertTagRow(sqlite3, {2, "Tag 2", 22222, 2222222}));
    ASSERT_TRUE(InsertTagRow(sqlite3, {3, "Tag 3", 33333, 3333333}));
    ASSERT_TRUE(InsertMemoRow(sqlite3, {(int)memo.id(), memo.title(), memo.description(), (int)memo.timestamp()}));
    ASSERT_TRUE(InsertTaggedRow(sqlite3, {1, 1}));
    ASSERT_TRUE(InsertTaggedRow(sqlite3, {1, 2}));
    ASSERT_TRUE(InsertTaggedRow(sqlite3, {1, 3}));

    const std::vector<unsigned long> expectedTagIds { 2, };
    EXPECT_TRUE(DeleteMemoTagIds(memo, {1, 3}, sqlite3));

    std::vector<unsigned long> returnedTagIds;
    auto callback = [&](const std::vector<std::string>& values, const std::vector<std::string>&)
    {
        if (!values.empty())
            returnedTagIds.emplace_back(std::stoul(values.front()));
        return false;
    };
    sqlite3.exec("SELECT tagId FROM Tagged WHERE memoId=1 ORDER BY tagId;", callback);
    EXPECT_EQ(returnedTagIds, expectedTagIds);
}

namespace {

bool CreateTables(Sqlite3Wrapper& sqlite3)
{
    std::stringstream recreateTablesCmd;
    recreateTablesCmd << "DROP TABLE IF EXISTS " << TaggedTable::kName << ";\n"
                      << "DROP TABLE IF EXISTS " << MemoTable::kName << ";\n"
                      << "DROP TABLE IF EXISTS " << TagTable::kName << ";\n"
                      << "CREATE TABLE IF NOT EXISTS " << MemoTable::kName << " (\n"
                      << MemoTable::att::kId << " INTEGER PRIMARY KEY,\n"
                      << MemoTable::att::kTitle << " TEXT NOT NULL,\n"
                      << MemoTable::att::kDescription << " TEXT,\n"
                      << MemoTable::att::kTimestamp << " INTEGER NOT NULL);\n"

                      << "CREATE TABLE IF NOT EXISTS " << TagTable::kName << "(\n"
                      << TagTable::att::kId << " INTEGER PRIMARY KEY,\n"
                      << TagTable::att::kName << " TEXT NOT NULL,\n"
                      << TagTable::att::kColor << " INTEGER,\n"
                      << TagTable::att::kTimestamp << " INTEGER NOT NULL);\n"

                      << "CREATE TABLE IF NOT EXISTS " << TaggedTable::kName << "(\n"
                      << TaggedTable::att::kMemoId << " INTEGER NOT NULL,\n"
                      << TaggedTable::att::kTagId  << " INTEGER NOT NULL,\n"
                      << "FOREIGN KEY (" << TaggedTable::att::kMemoId << ") REFERENCES "
                                         << MemoTable::kName << " (" << MemoTable::att::kId << "),\n"
                      << "FOREIGN KEY (" << TaggedTable::att::kTagId << ") REFERENCES "
                                         << TagTable::kName << " (" << TagTable::att::kId << "));";

    return sqlite3.exec(recreateTablesCmd.str(), nullptr);
}

bool InsertMemoRow(Sqlite3Wrapper& sqlite3, const MemoValues& values)
{
    std::stringstream stream;
    stream << "INSERT INTO Memo (id, title, description, timestamp) "
           << "VALUES (" << values.id << ", '" << values.title << "', '"
                         << values.description << "', " << values.timestamp << ");";
    return sqlite3.exec(stream.str(), nullptr);
}

bool InsertTagRow(Sqlite3Wrapper& sqlite3, const TagValues& values)
{
    std::stringstream stream;
    stream << "INSERT INTO Tag (id, name, color, timestamp) "
           << "VALUES (" << values.id << ", '" << values.name << "', "
           << values.color << ", " << values.timestamp << ");";
    return sqlite3.exec(stream.str(), nullptr);
}

bool InsertTaggedRow(Sqlite3Wrapper& sqlite3, const TaggedValues& values)
{
    std::stringstream stream;
    stream << "INSERT INTO Tagged (memoId, tagId) "
           << "VALUES (" << values.memoId << ", " << values.tagId << ");";
    return sqlite3.exec(stream.str(), nullptr);
}

}

} // namespace memo
