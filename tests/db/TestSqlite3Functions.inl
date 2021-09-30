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

TEST(TestSqlite3Functions, test_SelectRows_Tags_only)
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

    const test::Sqlite3Rows expectedTags { test::ToStringVector(*tag1), test::ToStringVector(*tag2),
                                           test::ToStringVector(*tag3) };
    const auto actualTags = SelectRows("SELECT * FROM Tag;", sqlite3);
    EXPECT_EQ(expectedTags, actualTags);
}

TEST(TestSqlite3Functions, test_SelectRows_Select_tag_rows_with_only_a_subset_of_attributes)
{
    const auto tempDbFile = test::TestFilePath("temp__.db");
    Sqlite3Wrapper sqlite3(tempDbFile);
    ASSERT_TRUE(sqlite3.open());
    ASSERT_TRUE(test::RecreateTables(sqlite3));

    ASSERT_TRUE(test::InsertTagRow(sqlite3, {1, "T1", 11, 1001}));
    ASSERT_TRUE(test::InsertTagRow(sqlite3, {2, "T2", 22, 2002}));
    ASSERT_TRUE(test::InsertTagRow(sqlite3, {3, "T3", 33, 3003}));

    const test::Sqlite3Rows expectedRows = { {"1", "T1"}, {"2", "T2"}, {"3", "T3"} };
    const auto actualRows = SelectRows("SELECT id, name FROM Tag;", sqlite3);
    EXPECT_EQ(expectedRows, actualRows);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TEST(TestSqlite3Functions, test_BuildTagQuery_Use_all_options_in_the_filter)
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

TEST(TestSqlite3Functions, test_BuildTagQuery_returns_a_simple_select_query_if_filter_is_empty)
{
    const std::string expectedQuery = "SELECT * FROM Tag GROUP BY id;";
    const auto actualQuery = BuildTagQuery({});
    EXPECT_EQ(expectedQuery, actualQuery);
}

TEST(TestSqlite3Functions, test_BuildTagQuery_Filter_by_name_prefix_only)
{
    TagSearchFilter searchFilter;
    searchFilter.namePrefix = "name_prefix";
    const auto actualQuery = BuildTagQuery(searchFilter);

    const std::string expectedQuery = "SELECT * FROM Tag WHERE name LIKE 'name_prefix%' GROUP BY id;";
    EXPECT_EQ(expectedQuery, actualQuery);
}

TEST(TestSqlite3Functions, test_BuildTagQuery_Filter_by_name_keyword_only)
{
    TagSearchFilter searchFilter;
    searchFilter.nameContains = "name_contains";
    const auto actualQuery = BuildTagQuery(searchFilter);

    const std::string expectedQuery = "SELECT * FROM Tag WHERE name LIKE '%name_contains%' GROUP BY id;";
    EXPECT_EQ(expectedQuery, actualQuery);
}

TEST(TestSqlite3Functions, test_BuildTagQuery_Filter_by_colors_only)
{
    TagSearchFilter searchFilter;
    searchFilter.colors = {111, 222, 333};
    const auto actualQuery = BuildTagQuery(searchFilter);

    const std::string expectedQuery = "SELECT * FROM Tag WHERE color IN (111,222,333) GROUP BY id;";
    EXPECT_EQ(expectedQuery, actualQuery);
}

TEST(TestSqlite3Functions, test_BuildTagQuery_creates_a_JOIN_query_if_the_filter_contains_memo_ids)
{
    TagSearchFilter searchFilter;
    searchFilter.memoIds = {1, 2, 3};
    const auto actualQuery = BuildTagQuery(searchFilter);

    const std::string expectedQuery = "SELECT id, name, color, timestamp FROM Tag "
                                      "INNER JOIN Tagged ON Tag.id = Tagged.tagId "
                                      "WHERE memoId IN (1,2,3) GROUP BY id;";
    EXPECT_EQ(expectedQuery, actualQuery);
}

TEST(TestSqlite3Functions, test_BuildTagQuery_Filter_only_contains_start_date)
{
    TagSearchFilter searchFilter;
    searchFilter.filterFromDate = 100001;
    const auto actualQuery = BuildTagQuery(searchFilter);

    const std::string expectedQuery = "SELECT * FROM Tag WHERE timestamp >= 100001 GROUP BY id;";
    EXPECT_EQ(expectedQuery, actualQuery);
}

TEST(TestSqlite3Functions, test_BuildTagQuery_Filter_only_contains_end_date)
{
    TagSearchFilter searchFilter;
    searchFilter.filterUntilDate = 200002;
    const auto actualQuery = BuildTagQuery(searchFilter);

    const std::string expectedQuery = "SELECT * FROM Tag WHERE timestamp <= 200002 GROUP BY id;";
    EXPECT_EQ(expectedQuery, actualQuery);
}

TEST(TestSqlite3Functions, test_BuildTagQuery_Filter_contains_only_one_color)
{
    TagSearchFilter searchFilter;
    searchFilter.colors = {111};
    const auto actualQuery = BuildTagQuery(searchFilter);

    const std::string expectedQuery = "SELECT * FROM Tag WHERE color IN (111) GROUP BY id;";
    EXPECT_EQ(expectedQuery, actualQuery);
}

TEST(TestSqlite3Functions, test_BuildTagQuery_Filter_contains_only_one_memo_id)
{
    TagSearchFilter searchFilter;
    searchFilter.memoIds = {1};
    const auto actualQuery = BuildTagQuery(searchFilter);

    const std::string expectedQuery = "SELECT id, name, color, timestamp FROM Tag "
                                      "INNER JOIN Tagged ON Tag.id = Tagged.tagId "
                                      "WHERE memoId IN (1) GROUP BY id;";
    EXPECT_EQ(expectedQuery, actualQuery);
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TEST(TestSqlite3Functions, test_BuildMemoQuery_Use_all_filter_options)
{
    MemoSearchFilter filter;
    filter.titlePrefix = "title_prefix";
    filter.titleKeywords = {"key1", "key2", "key3"};
    filter.memoKeywords = {"mKey1", "mKey2", "mKey3"};
    filter.tagIds = {1, 2, 3};
    filter.filterFromDate = 10001;
    filter.filterUntilDate = 20003;

    const auto query = BuildMemoQuery(filter);
    const std::string expectedQuery =
            "SELECT id, title, description, timestamp FROM Memo "
            "INNER JOIN Tagged ON Memo.id = Tagged.memoId "
            "WHERE title LIKE 'title_prefix%' "
            "AND (title LIKE '%key1%' OR title LIKE '%key2%' OR title LIKE '%key3%') "
            "AND (description LIKE '%mKey1%' OR description LIKE '%mKey2%' OR description LIKE '%mKey3%') "
            "AND timestamp >= 10001 AND timestamp <= 20003 "
            "AND tagId IN (1,2,3) GROUP BY id;";

    EXPECT_EQ(expectedQuery, query);
}

TEST(TestSqlite3Functions, test_BuildMemoQuery_returns_simple_SELECT_query_if_no_filter_is_applied)
{
    const auto query = BuildMemoQuery({});
    const std::string expectedQuery = "SELECT id, title, description, timestamp FROM Memo GROUP BY id;";

    EXPECT_EQ(expectedQuery, query);
}

TEST(TestSqlite3Functions, test_BuildMemoQuery_Use_title_prefix_filter_only)
{
    MemoSearchFilter filter;
    filter.titlePrefix = "title_prefix";

    const auto query = BuildMemoQuery(filter);
    const std::string expectedQuery = "SELECT id, title, description, timestamp FROM Memo "
                                      "WHERE title LIKE 'title_prefix%' GROUP BY id;";

    EXPECT_EQ(expectedQuery, query);
}

TEST(TestSqlite3Functions, test_BuildMemoQuery_Use_only_title_keywords_filter)
{
    MemoSearchFilter filter;
    filter.titleKeywords = {"key1", "key2", "key3"};

    const auto query = BuildMemoQuery(filter);
    const std::string expectedQuery = "SELECT id, title, description, timestamp FROM Memo "
                                      "WHERE (title LIKE '%key1%' OR title LIKE '%key2%' OR title LIKE '%key3%') "
                                      "GROUP BY id;";
    EXPECT_EQ(expectedQuery, query);
}

TEST(TestSqlite3Functions, test_BuildMemoQuery_ignores_keyword_duplicates_for_title)
{
    MemoSearchFilter filter;
    filter.titleKeywords = {"key1", "key2", "key3", "key2", "key1"};

    const auto query = BuildMemoQuery(filter);
    const std::string expectedQuery = "SELECT id, title, description, timestamp FROM Memo "
                                      "WHERE (title LIKE '%key1%' OR title LIKE '%key2%' OR title LIKE '%key3%') "
                                      "GROUP BY id;";
    EXPECT_EQ(expectedQuery, query);
}

TEST(TestSqlite3Functions, test_BuildMemoQuery_Use_only_description_keywords_filter)
{
    MemoSearchFilter filter;
    filter.memoKeywords = {"mKey1", "mKey2", "mKey3"};

    const auto query = BuildMemoQuery(filter);
    const std::string expectedQuery =
            "SELECT id, title, description, timestamp FROM Memo "
            "WHERE (description LIKE '%mKey1%' OR description LIKE '%mKey2%' OR description LIKE '%mKey3%') "
            "GROUP BY id;";
    EXPECT_EQ(expectedQuery, query);
}

TEST(TestSqlite3Functions, test_BuildMemoQuery_ignores_duplicate_keywords_for_description)
{
    MemoSearchFilter filter;
    filter.memoKeywords = {"mKey1", "mKey2", "mKey3", "mKey2", "mKey1"};

    const auto query = BuildMemoQuery(filter);
    const std::string expectedQuery =
            "SELECT id, title, description, timestamp FROM Memo "
            "WHERE (description LIKE '%mKey1%' OR description LIKE '%mKey2%' OR description LIKE '%mKey3%') "
            "GROUP BY id;";
    EXPECT_EQ(expectedQuery, query);
}

TEST(TestSqlite3Functions, test_BuildMemoQuery_creates_JOIN_query_if_filter_contains_tag_ids)
{
    MemoSearchFilter filter;
    filter.tagIds = {1, 2, 3};

    const auto query = BuildMemoQuery(filter);
    const std::string expectedQuery = "SELECT id, title, description, timestamp FROM Memo "
                                      "INNER JOIN Tagged ON Memo.id = Tagged.memoId "
                                      "WHERE tagId IN (1,2,3) GROUP BY id;";

    EXPECT_EQ(expectedQuery, query);
}

TEST(TestSqlite3Functions, test_BuildMemoQuery_Filter_by_start_date_only)
{
    MemoSearchFilter filter;
    filter.filterFromDate = 10001;

    const auto query = BuildMemoQuery(filter);
    const std::string expectedQuery = "SELECT id, title, description, timestamp FROM Memo "
                                      "WHERE timestamp >= 10001 GROUP BY id;";

    EXPECT_EQ(expectedQuery, query);
}

TEST(TestSqlite3Functions, test_BuildMemoQuery_Filter_by_end_date_only)
{
    MemoSearchFilter filter;
    filter.filterUntilDate = 20002;

            const auto query = BuildMemoQuery(filter);
    const std::string expectedQuery = "SELECT id, title, description, timestamp FROM Memo "
                                      "WHERE timestamp <= 20002 GROUP BY id;";

    EXPECT_EQ(expectedQuery, query);
}

TEST(TestSqlite3Functions, test_BuildMemoQuery_Specify_only_one_tag_id_in_filter)
{
    MemoSearchFilter filter;
    filter.tagIds = {1};

    const auto query = BuildMemoQuery(filter);
    const std::string expectedQuery = "SELECT id, title, description, timestamp FROM Memo "
                                      "INNER JOIN Tagged ON Memo.id = Tagged.memoId "
                                      "WHERE tagId IN (1) GROUP BY id;";

    EXPECT_EQ(expectedQuery, query);
}

TEST(TestSqlite3Functions, test_BuildMemoQuery_Specify_only_one_title_keyword_in_filter)
{
    MemoSearchFilter filter;
    filter.titleKeywords = {"key1"};

    const auto query = BuildMemoQuery(filter);
    const std::string expectedQuery = "SELECT id, title, description, timestamp FROM Memo "
                                      "WHERE (title LIKE '%key1%') GROUP BY id;";
    EXPECT_EQ(expectedQuery, query);
}

TEST(TestSqlite3Functions, test_BuildMemoQuery_Specify_only_one_description_keyword_in_filter)
{
    MemoSearchFilter filter;
    filter.memoKeywords = {"mKey1"};

    const auto query = BuildMemoQuery(filter);
    const std::string expectedQuery =
            "SELECT id, title, description, timestamp FROM Memo "
            "WHERE (description LIKE '%mKey1%') GROUP BY id;";
    EXPECT_EQ(expectedQuery, query);
}
} // namespace memo
