#pragma once
#include "db/IDatabase.hpp"
#include "db/Sqlite3Functions.hpp"
#include "db/Sqlite3Wrapper.hpp"
#include "db/Sqlite3Schema.hpp"
#include "db/Tools.hpp"
#include "db/Sqlite3TestHelperFunctions.hpp"
#include "ModelTestHelperFunctions.hpp"
#include "model/Memo.hpp"
#include "model/Tag.hpp"
#include "TestingUtils.hpp"
#include <gtest/gtest.h>
#include <sstream>

namespace memo {

TEST(TestSqlite3Functions, test_SelectTags)
{
    const auto tempDbFile = test::TestFilePath("temp__.db");
    Sqlite3Wrapper sqlite3(tempDbFile);
    ASSERT_TRUE(sqlite3.open());
    ASSERT_TRUE(test::RecreateTables(sqlite3));
    auto tag1 = test::CreateTag({1, "T1", 11, 1001});
    auto tag2 = test::CreateTag({2, "T2", 22, 2002});
    auto tag3 = test::CreateTag({3, "T3", 33, 3003});


    ASSERT_TRUE(test::InsertTagRow(sqlite3, test::TagToValues(*tag1)));
    ASSERT_TRUE(test::InsertTagRow(sqlite3, test::TagToValues(*tag2)));
    ASSERT_TRUE(test::InsertTagRow(sqlite3, test::TagToValues(*tag3)));

    const std::vector<model::TagPtr> expectedTags { tag1, tag2, tag3 };
    const auto actualTags = SelectTags("SELECT * FROM Tag;", sqlite3);
    ASSERT_EQ(expectedTags.size(), actualTags.size());
    for (auto i = 0ul; i < actualTags.size(); ++i)
    {
        auto actualTag = actualTags[i];
        EXPECT_NE(nullptr, actualTag) << "Index " << i;
        if (actualTag)
            EXPECT_EQ(*expectedTags[i], *actualTag) << "Index " << i;
    }
}

TEST(TestSqlite3Functions, test_SelectTags_Returns_empty_list_if_not_all_tag_attributes_are_queried)
{
    const auto tempDbFile = test::TestFilePath("temp__.db");
    Sqlite3Wrapper sqlite3(tempDbFile);
    ASSERT_TRUE(sqlite3.open());
    ASSERT_TRUE(test::RecreateTables(sqlite3));

    ASSERT_TRUE(test::InsertTagRow(sqlite3, {1, "T1", 11, 1001}));
    ASSERT_TRUE(test::InsertTagRow(sqlite3, {2, "T2", 22, 2002}));
    ASSERT_TRUE(test::InsertTagRow(sqlite3, {3, "T3", 33, 3003}));

    const std::vector<model::TagPtr> expectedTags = {};
    const auto actualTags = SelectTags("SELECT id, name, color FROM Tag;", sqlite3); // Missing timestamp
    EXPECT_EQ(0ul, actualTags.size());
    for (const auto& tag : actualTags)
    {
        if (tag) std::cout << "Tag with id " << tag->id();
    }
}

TEST(TestSqlite3Functions, DISABLED_test_SelectTags_handles_attributes_being_returned_in_different_order)
{
    const auto tempDbFile = test::TestFilePath("temp__.db");
    Sqlite3Wrapper sqlite3(tempDbFile);
    ASSERT_TRUE(sqlite3.open());
    ASSERT_TRUE(test::RecreateTables(sqlite3));

    auto tag1 = test::CreateTag({1, "T1", 11, 1001});
    ASSERT_TRUE(test::InsertTagRow(sqlite3, test::TagToValues(*tag1)));

    const std::vector<model::TagPtr> expectedTags = { tag1 };
    const auto actualTags = SelectTags("SELECT id, timestamp, name, color FROM Tag;", sqlite3);
    ASSERT_EQ(expectedTags.size(), actualTags.size());
    for (auto i = 0ul; i < actualTags.size(); ++i)
    {
        auto actualTag = actualTags[i];
        EXPECT_NE(nullptr, actualTag) << "Index " << i;
        if (actualTag)
            EXPECT_EQ(*expectedTags[i], *actualTag) << "Index " << i;
    }
}

TEST(TestSqlite3Functions, test_UpdateMemoTable)
{
    model::Memo memo;
    memo.setId(1);
    memo.setTitle("New title");
    memo.setDescription("New desc");
    memo.setTimestamp(12345);

    const auto tempDbFile = test::TestFilePath("temp__.db");
    Sqlite3Wrapper sqlite3(tempDbFile);
    sqlite3.open();
    ASSERT_TRUE(test::RecreateTables(sqlite3));
    ASSERT_TRUE(test::InsertMemoRow(sqlite3, {1, "Old title", "Old desc", 11111}));

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

TEST(TestSqlite3Functions, test_UpdateTagTable)
{
    model::Tag tag;
    tag.setId(1);
    tag.setName("New name");
    tag.setColor({10, 10, 10});
    tag.setTimestamp(22222);

    const auto tempDbFile = test::TestFilePath("temp__.db");
    Sqlite3Wrapper sqlite3(tempDbFile);
    sqlite3.open();
    ASSERT_TRUE(test::RecreateTables(sqlite3));
    ASSERT_TRUE(test::InsertTagRow(sqlite3, {1, "Old name", 12345, 11111}));

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

TEST(TestSqlite3Functions, test_SelectMemoTagIds)
{
    model::Memo memo;
    memo.setId(1);
    memo.setTitle("New title");
    memo.setDescription("New desc");
    memo.setTimestamp(12345);

    const auto tempDbFile = test::TestFilePath("temp__.db");
    Sqlite3Wrapper sqlite3(tempDbFile);
    sqlite3.open();

    ASSERT_TRUE(test::RecreateTables(sqlite3));
    ASSERT_TRUE(test::InsertTagRow(sqlite3, {1, "Tag 1", 11111, 1111111}));
    ASSERT_TRUE(test::InsertTagRow(sqlite3, {2, "Tag 2", 22222, 2222222}));
    ASSERT_TRUE(test::InsertTagRow(sqlite3, {3, "Tag 3", 33333, 3333333}));
    ASSERT_TRUE(test::InsertMemoRow(sqlite3, {memo.id(), memo.title(), memo.description(), memo.timestamp()}));
    ASSERT_TRUE(test::InsertTaggedRow(sqlite3, {1, 1}));
    ASSERT_TRUE(test::InsertTaggedRow(sqlite3, {1, 3}));

    std::vector<unsigned long> expectedTagIds { 1, 3 };
    std::vector<unsigned long> selectedTagIds;
    EXPECT_TRUE(SelectMemoTagIds(memo.id(), selectedTagIds, sqlite3));
    EXPECT_EQ(selectedTagIds, expectedTagIds);
}

TEST(TestSqlite3Functions, test_SelectMemoTagIds_No_tags_found_Return_success)
{
    const auto tempDbFile = test::TestFilePath("temp__.db");
    Sqlite3Wrapper sqlite3(tempDbFile);
    sqlite3.open();

    ASSERT_TRUE(test::RecreateTables(sqlite3));
    ASSERT_TRUE(test::InsertTagRow(sqlite3, {1, "Tag 1", 11111, 1111111}));
    ASSERT_TRUE(test::InsertMemoRow(sqlite3, {1, "Title", "Desc", 131313}));

    std::vector<unsigned long> selectedTagIds;
    EXPECT_TRUE(SelectMemoTagIds(1, selectedTagIds, sqlite3));
    EXPECT_EQ(std::vector<unsigned long>{}, selectedTagIds);
}

TEST(TestSqlite3Functions, test_InsertMemoTagIds)
{
    model::Memo memo;
    memo.setId(1);
    memo.setTitle("New title");
    memo.setDescription("New desc");
    memo.setTimestamp(12345);

    const auto tempDbFile = test::TestFilePath("temp__.db");
    Sqlite3Wrapper sqlite3(tempDbFile);
    sqlite3.open();

    ASSERT_TRUE(test::RecreateTables(sqlite3));
    ASSERT_TRUE(test::InsertTagRow(sqlite3, {1, "Tag 1", 11111, 1111111}));
    ASSERT_TRUE(test::InsertTagRow(sqlite3, {2, "Tag 2", 22222, 2222222}));
    ASSERT_TRUE(test::InsertTagRow(sqlite3, {3, "Tag 3", 33333, 3333333}));
    ASSERT_TRUE(test::InsertMemoRow(sqlite3, {memo.id(), memo.title(), memo.description(), memo.timestamp()}));

    const std::vector<unsigned long> expectedTagIds { 1, 3 };
    EXPECT_TRUE(InsertMemoTagIds(memo.id(), expectedTagIds, sqlite3));

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


TEST(TestSqlite3Functions, test_DeleteMemoTagIds)
{
    model::Memo memo;
    memo.setId(1);
    memo.setTitle("New title");
    memo.setDescription("New desc");
    memo.setTimestamp(12345);

    const auto tempDbFile = test::TestFilePath("temp__.db");
    Sqlite3Wrapper sqlite3(tempDbFile);
    sqlite3.open();

    ASSERT_TRUE(test::RecreateTables(sqlite3));
    ASSERT_TRUE(test::InsertTagRow(sqlite3, {1, "Tag 1", 11111, 1111111}));
    ASSERT_TRUE(test::InsertTagRow(sqlite3, {2, "Tag 2", 22222, 2222222}));
    ASSERT_TRUE(test::InsertTagRow(sqlite3, {3, "Tag 3", 33333, 3333333}));
    ASSERT_TRUE(test::InsertMemoRow(sqlite3, {memo.id(), memo.title(), memo.description(), memo.timestamp()}));
    ASSERT_TRUE(test::InsertTaggedRow(sqlite3, {1, 1}));
    ASSERT_TRUE(test::InsertTaggedRow(sqlite3, {1, 2}));
    ASSERT_TRUE(test::InsertTaggedRow(sqlite3, {1, 3}));

    const std::vector<unsigned long> expectedTagIds { 2, };
    EXPECT_TRUE(DeleteMemoTagIds(memo.id(), {1, 3}, sqlite3));

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

TEST(TestSqlite3Functions, test_BuildTagQuery)
{
    TagSearchFilter searchFilter;
    searchFilter.namePrefix = "name_prefix";
    searchFilter.nameContains = "name_contains";
    searchFilter.colors = {111, 222, 333};
    searchFilter.memoIds = {1, 2, 3};
    searchFilter.filterFromDate = 100001;
    searchFilter.filterUntilDate = 200002;

    std::string expectedQuery = "SELECT id, name, color, timestamp FROM Tag "
                                "INNER JOIN Tagged ON Tag.id = Tagged.tagId "
                                "WHERE name LIKE 'name_prefix%' AND name LIKE '%name_contains%' "
                                "AND color IN (111,222,333) AND timestamp >= 100001 AND timestamp <= 200002 "
                                "AND memoId IN (1,2,3) GROUP BY id;";
    const auto actualQuery = BuildTagQuery(searchFilter);
    EXPECT_EQ(expectedQuery, actualQuery);
}
} // namespace memo
