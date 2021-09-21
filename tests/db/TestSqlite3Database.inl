#pragma once
#include "db/Sqlite3Database.hpp"
#include "db/Sqlite3Wrapper.hpp"
#include "db/Sqlite3TestHelperFunctions.hpp"
#include "db/Tools.hpp"
#include "TestingUtils.hpp"
#include "model/ModelDefs.hpp"
#include <gtest/gtest.h>

namespace memo {

namespace {
     model::MemoPtr CreateMemo(const test::MemoValues& values);

     model::TagPtr CreateTag(const test::TagValues& values);

     std::vector<std::string> ToStringVector(const model::MemoPtr& memo);

     std::vector<std::string> ToStringVector(const model::TagPtr& tag);

     std::vector<std::vector<std::string>> ExecCommand(Sqlite3Wrapper& sqlite3, const std::string& command);

     std::string TestFilePath();
}

TEST(TestSqlite3Database, test_updateMemo_Update_primitive_fields)
{
    auto db = Sqlite3Database(std::make_unique<Sqlite3Wrapper>(TestFilePath()));
    Sqlite3Wrapper sqlite3(TestFilePath());
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
    auto db = Sqlite3Database(std::make_unique<Sqlite3Wrapper>(TestFilePath()));
    Sqlite3Wrapper sqlite3(TestFilePath());
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

TEST(TestSqlite3Database, test_updateMemo_Memo_with_given_title_already_exists_Fail)
{
    auto db = Sqlite3Database(std::make_unique<Sqlite3Wrapper>(TestFilePath()));
    Sqlite3Wrapper sqlite3(TestFilePath());
    sqlite3.open();

    test::MemoValues firstMemoValues {1, "Existing title", "Description1", 11111};
    test::MemoValues memoToUpdateValues {2, "Old title", "Old description", 22222};
    ASSERT_TRUE(test::RecreateTables(sqlite3));
    ASSERT_TRUE(test::InsertMemoRow(sqlite3, firstMemoValues));
    ASSERT_TRUE(test::InsertMemoRow(sqlite3, memoToUpdateValues));

    auto memo = CreateMemo({2, "Existing title", "New Desc", 33333});
    EXPECT_FALSE(db.updateMemo(memo));

    const auto expectedFirstRow = ToStringVector(CreateMemo(firstMemoValues));
    const auto expectedSecondRow = ToStringVector(CreateMemo(memoToUpdateValues));
    const auto selectedMemoRows = ExecCommand(sqlite3, "SELECT * FROM Memo ORDER BY id;");
    ASSERT_EQ(2ul, selectedMemoRows.size());
    EXPECT_EQ(expectedFirstRow, selectedMemoRows.front());
    EXPECT_EQ(expectedSecondRow, selectedMemoRows.back());
}

TEST(TestSqlite3Database, test_UpdateTag)
{
    auto db = Sqlite3Database(std::make_unique<Sqlite3Wrapper>(TestFilePath()));
    Sqlite3Wrapper sqlite3(TestFilePath());
    sqlite3.open();

    auto newTag = CreateTag({1, "NetTagName", 222, 22222});
    ASSERT_TRUE(test::RecreateTables(sqlite3));
    ASSERT_TRUE(test::InsertTagRow(sqlite3, {1, "OldTagName", 111, 11111}));

    EXPECT_TRUE(db.updateTag(newTag));

    auto expectedValues = ToStringVector(newTag);
    auto selectedRows = ExecCommand(sqlite3, "SELECT * FROM Tag WHERE id=1;");
    ASSERT_EQ(1ul, selectedRows.size());
    EXPECT_EQ(selectedRows.front(), expectedValues);
}

TEST(TestSqlite3Database, test_UpdateTag_Tag_with_the_same_name_exists_Fail)
{
    auto db = Sqlite3Database(std::make_unique<Sqlite3Wrapper>(TestFilePath()));
    Sqlite3Wrapper sqlite3(TestFilePath());
    sqlite3.open();

    auto newTag = CreateTag({2, "ExistingTagName", 333, 33333});
    test::TagValues firstTagValues {1, "ExistingTagName", 111, 11111};
    test::TagValues oldValues {2, "TagNameToUpdate", 222, 22222};
    ASSERT_TRUE(test::RecreateTables(sqlite3));
    ASSERT_TRUE(test::InsertTagRow(sqlite3, firstTagValues));
    ASSERT_TRUE(test::InsertTagRow(sqlite3, oldValues));

    EXPECT_FALSE(db.updateTag(newTag));

    auto expectedFirstRow = ToStringVector(CreateTag(firstTagValues));
    auto expectedSecondRow = ToStringVector(CreateTag(oldValues));
    auto selectedRows = ExecCommand(sqlite3, "SELECT * FROM Tag ORDER BY id;");
    ASSERT_EQ(2ul, selectedRows.size());
    EXPECT_EQ(selectedRows.front(), expectedFirstRow);
    EXPECT_EQ(selectedRows.back(), expectedSecondRow);
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

    model::TagPtr CreateTag(const test::TagValues& values)
    {
        auto tag = std::make_shared<model::Tag>();
        tag->setId(static_cast<unsigned long>(values.id));
        tag->setName(values.name);
        tag->setColor(tools::IntToColor(values.color));
        tag->setTimestamp(static_cast<unsigned long>(values.timestamp));
        return tag;
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

    std::vector<std::string> ToStringVector(const model::TagPtr& tag)
    {
        return {
            std::to_string(tag->id()),
            tag->name(),
            std::to_string(tools::ColorToInt(tag->color())),
            std::to_string(tag->timestamp()),
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

    std::string TestFilePath()
    {
        return test::TestFilePath("temp__.db");
    }
} // namespace
} // namespace memo
