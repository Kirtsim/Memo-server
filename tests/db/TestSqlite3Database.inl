#pragma once
#include "db/Sqlite3Database.hpp"
#include "db/Sqlite3Wrapper.hpp"
#include "db/Sqlite3TestHelperFunctions.hpp"
#include "db/Tools.hpp"
#include "ModelTestHelperFunctions.hpp"
#include "TestingUtils.hpp"
#include "model/ModelDefs.hpp"
#include <gtest/gtest.h>

namespace memo {

class TestSqlite3Database : public testing::Test
{
    static const std::string dbFileName;
protected:
    TestSqlite3Database();

    void SetUp() override;

    void TearDown() override;

protected:
    Sqlite3Wrapper sqlite3;
    Sqlite3Database db;
};

const std::string TestSqlite3Database::dbFileName = "temp__.db";

TestSqlite3Database::TestSqlite3Database()
    : testing::Test()
    , sqlite3(test::TestFilePath(dbFileName))
    , db(std::make_unique<Sqlite3Wrapper>(test::TestFilePath(dbFileName)))
{
}

void TestSqlite3Database::SetUp()
{
    ASSERT_TRUE(sqlite3.open());
    ASSERT_TRUE(test::RecreateTables(sqlite3));
}

void TestSqlite3Database::TearDown()
{
    test::RemoveFile(test::TestFilePath(dbFileName));
}


TEST_F(TestSqlite3Database, test_listMemos_without_filter_and_tag_ids)
{
    auto memo1 = test::CreateMemo({1, "T1", "D1", 1111});
    auto memo2 = test::CreateMemo({2, "T2", "D2", 2222});
    auto memo3 = test::CreateMemo({3, "T3", "D3", 3333});

    ASSERT_TRUE(test::InsertMemoRow(sqlite3, test::MemoToValues(*memo1)));
    ASSERT_TRUE(test::InsertMemoRow(sqlite3, test::MemoToValues(*memo2)));
    ASSERT_TRUE(test::InsertMemoRow(sqlite3, test::MemoToValues(*memo3)));

    const auto selectedMemos = db.listMemos({});
    const auto selectedRows = test::ToStringVectors(selectedMemos);
    const auto expectedRows = test::ToStringVectors({memo1, memo2, memo3});
    EXPECT_EQ(expectedRows, selectedRows);
}

TEST_F(TestSqlite3Database, test_listMemos_Check_memos_contain_expected_tag_ids)
{
    auto memo1 = test::CreateMemo({1, "T1", "D1", 1111});
    auto memo2 = test::CreateMemo({2, "T2", "D2", 2222});
    auto memo3 = test::CreateMemo({3, "T3", "D3", 3333});
    auto memo4 = test::CreateMemo({4, "T4", "D4", 4444});
    std::vector<model::MemoPtr> memos { memo1, memo2, memo3, memo4 };
    memo1->setTagIds({ 1, 2, 3 });
    memo2->setTagIds({ 2 });
    memo3->setTagIds({ 1, 3 });

    for (const auto& memo : memos)
        ASSERT_TRUE(test::InsertMemoRow(sqlite3, test::MemoToValues(*memo))) << "Id: " << memo->id();

    ASSERT_TRUE(test::InsertTagRow(sqlite3, {1, "N1", 11, 1111 }));
    ASSERT_TRUE(test::InsertTagRow(sqlite3, {2, "N2", 11, 1111 }));
    ASSERT_TRUE(test::InsertTagRow(sqlite3, {3, "N3", 11, 1111 }));
    ASSERT_TRUE(test::InsertTaggedRow(sqlite3, {memo1->id(), 1}));
    ASSERT_TRUE(test::InsertTaggedRow(sqlite3, {memo1->id(), 2}));
    ASSERT_TRUE(test::InsertTaggedRow(sqlite3, {memo1->id(), 3}));
    ASSERT_TRUE(test::InsertTaggedRow(sqlite3, {memo2->id(), 2}));
    ASSERT_TRUE(test::InsertTaggedRow(sqlite3, {memo3->id(), 1}));
    ASSERT_TRUE(test::InsertTaggedRow(sqlite3, {memo3->id(), 3}));

    const auto selectedMemos = db.listMemos({});
    const auto selectedRows = test::ToStringVectors(selectedMemos);
    const auto expectedRows = test::ToStringVectors(memos);
    EXPECT_EQ(expectedRows, selectedRows);
    ASSERT_EQ(4ul, selectedMemos.size());
    for (auto i = 0ul; i < selectedMemos.size(); ++i)
    {
        ASSERT_NE(nullptr, selectedMemos[i]) << " i = " << i;
        EXPECT_EQ(memos[i]->id(), selectedMemos[i]->id()) << " i = " << i;
        EXPECT_EQ(memos[i]->tagIds(), selectedMemos[i]->tagIds()) << " i = " << i;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TEST_F(TestSqlite3Database, test_listTags_without_filter)
{
    auto tag1 = test::CreateTag({1, "T1", 111, 11111});
    auto tag2 = test::CreateTag({2, "T2", 222, 22222});
    auto tag3 = test::CreateTag({3, "T3", 333, 33333});

    ASSERT_TRUE(test::InsertTagRow(sqlite3, test::TagToValues(*tag1)));
    ASSERT_TRUE(test::InsertTagRow(sqlite3, test::TagToValues(*tag2)));
    ASSERT_TRUE(test::InsertTagRow(sqlite3, test::TagToValues(*tag3)));

    auto selectedTags = db.listTags({});
    const auto expectedRows = test::ToStringVectors({ tag1, tag2, tag3 });
    const auto actualRows = test::ToStringVectors(selectedTags);
    EXPECT_EQ(expectedRows, actualRows);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TEST_F(TestSqlite3Database, test_updateMemo_Update_primitive_fields)
{
    ASSERT_TRUE(test::InsertMemoRow(sqlite3, {1, "Old title", "Old description", 11111}));

    auto memo = test::CreateMemo({1, "New title", "New Desc", 22222});
    EXPECT_TRUE(db.updateMemo(memo));

    const auto expectedValues = test::ToStringVector(*memo);
    const auto selectedRows = test::ExecCommand(sqlite3, "SELECT * FROM Memo WHERE id=1;");
    ASSERT_EQ(1ul, selectedRows.size());
    EXPECT_EQ(expectedValues, selectedRows.front());
}

TEST_F(TestSqlite3Database, test_updateMemo_Update_attributes_and_add_and_remove_tags)
{
    ASSERT_TRUE(test::InsertMemoRow(sqlite3, {1, "Old title", "Old description", 11111}));
    ASSERT_TRUE(test::InsertTagRow(sqlite3, {1, "Tag1", 1111, 101010}));
    ASSERT_TRUE(test::InsertTagRow(sqlite3, {2, "Tag2", 2222, 202020}));
    ASSERT_TRUE(test::InsertTagRow(sqlite3, {3, "Tag3", 3333, 303030}));
    ASSERT_TRUE(test::InsertTaggedRow(sqlite3, {1, 1}));
    ASSERT_TRUE(test::InsertTaggedRow(sqlite3, {1, 3}));

    auto memo = test::CreateMemo({1, "New title", "New Desc", 22222});
    memo->setTagIds({2});

    EXPECT_TRUE(db.updateMemo(memo));

    std::vector<std::string> expectedMemoValues = test::ToStringVector(*memo);
    const auto selectedMemoRows = test::ExecCommand(sqlite3, "SELECT * FROM Memo WHERE id=1;");
    ASSERT_EQ(1ul, selectedMemoRows.size());
    EXPECT_EQ(expectedMemoValues, selectedMemoRows.front());

    const auto selectedTagIdRows = test::ExecCommand(sqlite3, "SELECT tagId FROM Tagged WHERE memoId=1;");
    std::vector<std::string> expectedTagIds {"2"};
    ASSERT_EQ(1ul, selectedTagIdRows.size());
    EXPECT_EQ(expectedTagIds, selectedTagIdRows.front());
}

TEST_F(TestSqlite3Database, test_updateMemo_Memo_with_given_title_already_exists_Fail)
{
    auto firstMemo = test::CreateMemo({1, "Existing title", "Description1", 11111});
    auto memoToUpdate = test::CreateMemo({2, "Old title", "Old description", 22222});
    ASSERT_TRUE(test::InsertMemoRow(sqlite3, test::MemoToValues(*firstMemo)));
    ASSERT_TRUE(test::InsertMemoRow(sqlite3, test::MemoToValues(*memoToUpdate)));

    auto memo = test::CreateMemo({2, "Existing title", "New Desc", 33333});
    EXPECT_FALSE(db.updateMemo(memo));

    const auto expectedFirstRow = test::ToStringVector(*firstMemo);
    const auto expectedSecondRow = test::ToStringVector(*memoToUpdate);
    const auto selectedMemoRows = test::ExecCommand(sqlite3, "SELECT * FROM Memo ORDER BY id;");
    ASSERT_EQ(2ul, selectedMemoRows.size());
    EXPECT_EQ(expectedFirstRow, selectedMemoRows.front());
    EXPECT_EQ(expectedSecondRow, selectedMemoRows.back());
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TEST_F(TestSqlite3Database, test_UpdateTag)
{
    auto newTag = test::CreateTag({1, "NetTagName", 222, 22222});
    ASSERT_TRUE(test::InsertTagRow(sqlite3, {1, "OldTagName", 111, 11111}));

    EXPECT_TRUE(db.updateTag(newTag));

    auto expectedValues = test::ToStringVector(*newTag);
    auto selectedRows = test::ExecCommand(sqlite3, "SELECT * FROM Tag WHERE id=1;");
    ASSERT_EQ(1ul, selectedRows.size());
    EXPECT_EQ(selectedRows.front(), expectedValues);
}

TEST_F(TestSqlite3Database, test_UpdateTag_Tag_with_the_same_name_exists_Fail)
{
    auto firstTag = test::CreateTag({1, "ExistingTagName", 111, 11111});
    auto oldTag = test::CreateTag({2, "TagNameToUpdate", 222, 22222});
    ASSERT_TRUE(test::InsertTagRow(sqlite3, test::TagToValues(*firstTag)));
    ASSERT_TRUE(test::InsertTagRow(sqlite3, test::TagToValues(*oldTag)));

    auto newTag = test::CreateTag({2, "ExistingTagName", 333, 33333});
    EXPECT_FALSE(db.updateTag(newTag));

    auto expectedFirstRow = test::ToStringVector(*firstTag);
    auto expectedSecondRow = test::ToStringVector(*oldTag);
    auto selectedRows = test::ExecCommand(sqlite3, "SELECT * FROM Tag ORDER BY id;");
    ASSERT_EQ(2ul, selectedRows.size());
    EXPECT_EQ(selectedRows.front(), expectedFirstRow);
    EXPECT_EQ(selectedRows.back(), expectedSecondRow);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TEST_F(TestSqlite3Database, test_InsertMemo_does_not_insert_memo_with_existing_name_and_returns_false)
{
    auto existingMemo = test::CreateMemo({1, "Title1", "Desc1", 11111});
    ASSERT_TRUE(test::InsertMemoRow(sqlite3, test::MemoToValues(*existingMemo)));

    auto newMemo = test::CreateMemo({0, "Title1", "Desc 2", 22222});
    const bool success = db.insertMemo(newMemo);
    EXPECT_FALSE(success);

    const auto expectedRows = test::ToStringVectors({existingMemo});
    const auto actualRows = test::ExecCommand(sqlite3, "SELECT * FROM Memo;");
    EXPECT_EQ(expectedRows, actualRows);
}

TEST_F(TestSqlite3Database, test_InsertMemo_does_not_proceed_to_inserting_into_Tagged_table_if_memo_title_exists)
{
    ASSERT_TRUE(test::InsertTagRow(sqlite3, {1, "T1", 111, 1111}));
    ASSERT_TRUE(test::InsertTagRow(sqlite3, {2, "T2", 222, 2222}));

    auto existingMemo = test::CreateMemo({1, "Title1", "Desc1", 11111});
    ASSERT_TRUE(test::InsertMemoRow(sqlite3, test::MemoToValues(*existingMemo)));

    auto newMemo = test::CreateMemo({0, "Title1", "Desc 2", 22222});
    const bool success = db.insertMemo(newMemo);
    EXPECT_FALSE(success);

    const auto expectedMemoRows = test::ToStringVectors({existingMemo});
    const auto actualMemoRows = test::ExecCommand(sqlite3, "SELECT * FROM Memo;");
    const TableRows expectedTaggedRows = {};
    const auto actualTaggedRows = test::ExecCommand(sqlite3, "SELECT * FROM Tagged ORDER BY tagId;");
    EXPECT_EQ(expectedMemoRows, actualMemoRows);
    EXPECT_EQ(expectedTaggedRows, actualTaggedRows);
}

TEST_F(TestSqlite3Database, test_InsertMemo_also_inserts_into_the_Tagged_table)
{
    ASSERT_TRUE(test::InsertTagRow(sqlite3, {1, "T1", 111, 1111}));
    ASSERT_TRUE(test::InsertTagRow(sqlite3, {2, "T2", 222, 2222}));
    ASSERT_TRUE(test::InsertTagRow(sqlite3, {3, "T3", 333, 3333}));

    auto memo = test::CreateMemo({1, "Title1", "Desc 2", 22222});
    memo->setTagIds({1, 3});
    const bool success = db.insertMemo(memo);
    EXPECT_TRUE(success);

    const auto expectedMemoRows = test::ToStringVectors({memo});
    const auto actualMemoRows = test::ExecCommand(sqlite3, "SELECT * FROM Memo;");
    const TableRows expectedTaggedRows = {{"1", "1"}, {"1", "3"}};
    const auto actualTaggedRows = test::ExecCommand(sqlite3, "SELECT * FROM Tagged ORDER BY tagId;");
    EXPECT_EQ(expectedMemoRows, actualMemoRows);
    EXPECT_EQ(expectedTaggedRows, actualTaggedRows);
}

TEST_F(TestSqlite3Database, test_InsertMemo_inserts_nothing_if_tag_does_not_exist)
{
    ASSERT_TRUE(test::InsertTagRow(sqlite3, {1, "T1", 111, 1111}));
    ASSERT_TRUE(test::InsertTagRow(sqlite3, {2, "T2", 222, 2222}));

    auto memo = test::CreateMemo({1, "Title1", "Desc 2", 22222});
    memo->setTagIds({1, 3});
    const bool success = db.insertMemo(memo);
    EXPECT_FALSE(success);

    const TableRows expectedMemoRows = {};
    const auto actualMemoRows = test::ExecCommand(sqlite3, "SELECT * FROM Memo;");
    const TableRows expectedTaggedRows = {};
    const auto actualTaggedRows = test::ExecCommand(sqlite3, "SELECT * FROM Tagged ORDER BY tagId;");
    EXPECT_EQ(expectedMemoRows, actualMemoRows);
    EXPECT_EQ(expectedTaggedRows, actualTaggedRows);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TEST_F(TestSqlite3Database, test_InsertTag_autoincrements_the_tag_id)
{
    auto existingTag = test::CreateTag({1, "Tag1", 111, 1111});
    ASSERT_TRUE(test::InsertTagRow(sqlite3, test::TagToValues(*existingTag)));

    auto tag = test::CreateTag({0, "Tag2", 22, 2222});
    EXPECT_TRUE(db.insertTag(tag));

    tag->setId(2);
    const auto expectedRows = test::ToStringVectors({existingTag, tag});
    const auto actualRows = test::ExecCommand(sqlite3, "SELECT * FROM Tag ORDER BY id;");
    EXPECT_EQ(expectedRows, actualRows);
}

TEST_F(TestSqlite3Database, test_InsertTag_does_not_insert_if_tag_name_already_exists)
{
    auto existingTag = test::CreateTag({1, "TestTag", 111, 1111});
    ASSERT_TRUE(test::InsertTagRow(sqlite3, test::TagToValues(*existingTag)));

    auto tag = test::CreateTag({2, "TestTag", 22, 2222});
    EXPECT_FALSE(db.insertTag(tag));

    const auto expectedRows = test::ToStringVectors({existingTag});
    const auto actualRows = test::ExecCommand(sqlite3, "SELECT * FROM Tag;");
    EXPECT_EQ(expectedRows, actualRows);
}
} // namespace memo
