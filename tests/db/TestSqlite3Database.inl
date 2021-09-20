#pragma once
#include "db/Sqlite3Database.hpp"
#include "db/Sqlite3Wrapper.hpp"
#include "db/Sqlite3TestHelperFunctions.hpp"
#include "TestingUtils.hpp"
#include "model/ModelDefs.hpp"
#include <gtest/gtest.h>

namespace memo {

namespace {
     model::MemoPtr CreateMemo(const test::MemoValues& values);

     std::vector<std::string> ToStringVector(const model::MemoPtr& memo);

     std::vector<std::vector<std::string>> ExecCommand(Sqlite3Wrapper& sqlite3, const std::string& command);
}

TEST(TestSqlite3Database, test_updateMemo_Update_primitive_fields)
{
    auto testFile = test::TestFilePath("temp__.db");
    auto db = Sqlite3Database(std::make_unique<Sqlite3Wrapper>(testFile));
    Sqlite3Wrapper sqlite3(testFile);
    sqlite3.open();

    ASSERT_TRUE(test::RecreateTables(sqlite3));
    ASSERT_TRUE(test::InsertMemoRow(sqlite3, {1, "Old title", "Old description", 11111}));

    auto memo = CreateMemo({1, "New title", "New Desc", 22222});
    EXPECT_TRUE(db.updateMemo(memo));

    const auto expectedValues = ToStringVector(memo);
    const auto selectedRows = ExecCommand(sqlite3, "SELECT * FROM Memo WHERE id=1;");
    ASSERT_EQ(1ul, selectedRows.size());
    EXPECT_EQ(expectedValues, selectedRows.front());
}

TEST(TestSqlite3Database, test_updateMemo_Update_attributes_and_add_and_remove_tags)
{
    auto testFile = test::TestFilePath("temp__.db");
    auto db = Sqlite3Database(std::make_unique<Sqlite3Wrapper>(testFile));
    Sqlite3Wrapper sqlite3(testFile);
    sqlite3.open();

    ASSERT_TRUE(test::RecreateTables(sqlite3));
    ASSERT_TRUE(test::InsertMemoRow(sqlite3, {1, "Old title", "Old description", 11111}));
    ASSERT_TRUE(test::InsertTagRow(sqlite3, {1, "Tag1", 1111, 101010}));
    ASSERT_TRUE(test::InsertTagRow(sqlite3, {2, "Tag2", 2222, 202020}));
    ASSERT_TRUE(test::InsertTagRow(sqlite3, {3, "Tag3", 3333, 303030}));
    ASSERT_TRUE(test::InsertTaggedRow(sqlite3, {1, 1}));
    ASSERT_TRUE(test::InsertTaggedRow(sqlite3, {1, 3}));

    auto memo = CreateMemo({1, "New title", "New Desc", 22222});
    memo->setTagIds({2});

    EXPECT_TRUE(db.updateMemo(memo));

    std::vector<std::string> expectedMemoValues = ToStringVector(memo);
    const auto selectedMemoRows = ExecCommand(sqlite3, "SELECT * FROM Memo WHERE id=1;");
    ASSERT_EQ(1ul, selectedMemoRows.size());
    EXPECT_EQ(expectedMemoValues, selectedMemoRows.front());

    const auto selectedTagIdRows = ExecCommand(sqlite3, "SELECT tagId FROM Tagged WHERE memoId=1;");
    std::vector<std::string> expectedTagIds {"2"};
    ASSERT_EQ(1ul, selectedTagIdRows.size());
    EXPECT_EQ(expectedTagIds, selectedTagIdRows.front());
}

namespace {

    model::MemoPtr CreateMemo(const test::MemoValues& values)
    {
        auto memo = std::make_shared<model::Memo>();
        memo->setId(static_cast<unsigned long>(values.id));
        memo->setTitle(values.title);
        memo->setDescription(values.description);
        memo->setTimestamp(static_cast<unsigned long>(values.timestamp));
        return memo;
    }

    std::vector<std::string> ToStringVector(const model::MemoPtr& memo)
    {
        return {
            std::to_string(memo->id()),
            memo->title(),
            memo->description(),
            std::to_string(memo->timestamp()),
        };
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
} // namespace
} // namespace memo
