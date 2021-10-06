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

class TestSqlite3Functions_WithSqlite3 : public testing::Test
{
    static const std::string dbFileName;
protected:
    TestSqlite3Functions_WithSqlite3();

    void SetUp() override;

    void TearDown() override;

protected:
    Sqlite3Wrapper sqlite3;
};

const std::string TestSqlite3Functions_WithSqlite3::dbFileName = "temp__.db";

TestSqlite3Functions_WithSqlite3::TestSqlite3Functions_WithSqlite3()
    : testing::Test()
    , sqlite3(test::TestFilePath(dbFileName))
{
}

void TestSqlite3Functions_WithSqlite3::SetUp()
{
    ASSERT_TRUE(sqlite3.open());
    ASSERT_TRUE(test::RecreateTables(sqlite3));
}

void TestSqlite3Functions_WithSqlite3::TearDown()
{
    test::RemoveFile(test::TestFilePath(dbFileName));
}

TEST_F(TestSqlite3Functions_WithSqlite3, test_SelectRows_Tags_only)
{
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

TEST_F(TestSqlite3Functions_WithSqlite3, test_SelectRows_Select_tag_rows_with_only_a_subset_of_attributes)
{
    ASSERT_TRUE(test::InsertTagRow(sqlite3, {1, "T1", 11, 1001}));
    ASSERT_TRUE(test::InsertTagRow(sqlite3, {2, "T2", 22, 2002}));
    ASSERT_TRUE(test::InsertTagRow(sqlite3, {3, "T3", 33, 3003}));

    const test::Sqlite3Rows expectedRows = { {"1", "T1"}, {"2", "T2"}, {"3", "T3"} };
    const auto actualRows = SelectRows("SELECT id, name FROM Tag;", sqlite3);
    EXPECT_EQ(expectedRows, actualRows);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TEST_F(TestSqlite3Functions_WithSqlite3, test_UpdateMemoTable)
{
    ASSERT_TRUE(test::InsertMemoRow(sqlite3, {1, "Old title", "Old desc", 11111}));

    const auto memo = test::CreateMemo({1, "New title", "New desc", 12345});

    EXPECT_TRUE(UpdateMemoTable(*memo, sqlite3));
    const auto expectedRows = test::ToStringVectors({memo});

    const auto rows = test::ExecCommand(sqlite3, "SELECT * FROM Memo;");
    EXPECT_EQ(expectedRows, rows);
}

TEST_F(TestSqlite3Functions_WithSqlite3, test_UpdateTagTable)
{
    ASSERT_TRUE(test::InsertTagRow(sqlite3, {1, "Old name", 12345, 11111}));

    const auto tag = test::CreateTag({1, "New name", tools::ColorToInt({10, 10, 10}), 22222});
    EXPECT_TRUE(UpdateTagTable(*tag, sqlite3));

    const auto expectedRows = test::ToStringVectors({tag});

    const auto rows = test::ExecCommand(sqlite3, "SELECT * FROM Tag;");
    EXPECT_EQ(expectedRows, rows);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TEST_F(TestSqlite3Functions_WithSqlite3, test_SelectMemoTagIds)
{
    const auto memo = test::CreateMemo({1, "New title", "New desc", 12345});
    ASSERT_TRUE(test::InsertTagRow(sqlite3, {1, "Tag 1", 11111, 1111111}));
    ASSERT_TRUE(test::InsertTagRow(sqlite3, {2, "Tag 2", 22222, 2222222}));
    ASSERT_TRUE(test::InsertTagRow(sqlite3, {3, "Tag 3", 33333, 3333333}));
    ASSERT_TRUE(test::InsertMemoRow(sqlite3, test::MemoToValues(*memo)));
    ASSERT_TRUE(test::InsertTaggedRow(sqlite3, {1, 1}));
    ASSERT_TRUE(test::InsertTaggedRow(sqlite3, {1, 3}));

    std::vector<unsigned long> expectedTagIds { 1, 3 };
    std::vector<unsigned long> selectedTagIds;
    EXPECT_TRUE(SelectMemoTagIds(memo->id(), selectedTagIds, sqlite3));
    EXPECT_EQ(selectedTagIds, expectedTagIds);
}

TEST_F(TestSqlite3Functions_WithSqlite3, test_SelectMemoTagIds_No_tags_found_Return_success)
{
    ASSERT_TRUE(test::InsertTagRow(sqlite3, {1, "Tag 1", 11111, 1111111}));
    ASSERT_TRUE(test::InsertMemoRow(sqlite3, {1, "Title", "Desc", 131313}));

    std::vector<unsigned long> selectedTagIds;
    EXPECT_TRUE(SelectMemoTagIds(1, selectedTagIds, sqlite3));
    EXPECT_EQ(std::vector<unsigned long>{}, selectedTagIds);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TEST_F(TestSqlite3Functions_WithSqlite3, test_InsertMemo_automatically_increments_and_assigns_id)
{
    const auto existingMemo = test::CreateMemo({3, "T", "D", 10000});
    ASSERT_TRUE(test::InsertMemoRow(sqlite3, test::MemoToValues(*existingMemo)));

    auto memo = test::CreateMemo({0, "Title", "Description", 11111});
    const bool success = InsertMemo(*memo, sqlite3);

    EXPECT_TRUE(success);
    memo->setId(4);
    const auto rows = test::ExecCommand(sqlite3, "SELECT * FROM Memo;");
    const auto expectedRows = test::ToStringVectors({existingMemo, memo});
    EXPECT_EQ(expectedRows, rows);
}

TEST_F(TestSqlite3Functions_WithSqlite3, test_InsertMemo_fails_if_memo_with_the_same_title_exists_in_the_db)
{
    const auto existingMemo = test::CreateMemo({3, "Title", "D", 10000});
    ASSERT_TRUE(test::InsertMemoRow(sqlite3, test::MemoToValues(*existingMemo)));

    auto memo = test::CreateMemo({0, "Title", "Description", 11111});
    const bool success = InsertMemo(*memo, sqlite3);

    EXPECT_FALSE(success);
    const auto rows = test::ExecCommand(sqlite3, "SELECT * FROM Memo;");
    const auto expectedRows = test::ToStringVectors({existingMemo});
    EXPECT_EQ(expectedRows, rows);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TEST_F(TestSqlite3Functions_WithSqlite3, test_InsertTag_automatically_increments_and_assigns_tag_id)
{
    const auto existingTag = test::CreateTag({2, "Tag1", 111, 11111});
    ASSERT_TRUE(test::InsertTagRow(sqlite3, test::TagToValues(*existingTag)));

    auto tag = test::CreateTag({0, "Tag2", 222, 22222});
    const bool success = InsertTag(*tag, sqlite3);
    EXPECT_TRUE(success);

    tag->setId(3);
    const auto expectedRows = test::ToStringVectors({existingTag, tag});
    const auto actualRows = test::ExecCommand(sqlite3, "SELECT * FROM Tag ORDER BY id;");
    EXPECT_EQ(expectedRows, actualRows);
}

TEST_F(TestSqlite3Functions_WithSqlite3, test_InsertTag_fails_if_tag_with_the_same_name_exists_in_the_db)
{
    const auto existingTag = test::CreateTag({2, "Tag1", 1111, 10000});
    ASSERT_TRUE(test::InsertTagRow(sqlite3, test::TagToValues(*existingTag)));

    auto tag = test::CreateTag({0, "Tag1", 2929, 33333});
    const bool success = InsertTag(*tag, sqlite3);

    EXPECT_FALSE(success);
    const auto rows = test::ExecCommand(sqlite3, "SELECT * FROM Tag;");
    const auto expectedRows = test::ToStringVectors({existingTag});
    EXPECT_EQ(expectedRows, rows);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TEST_F(TestSqlite3Functions_WithSqlite3, test_InsertMemoTagIds)
{
    const auto memo = test::CreateMemo({1, "New title", "New desc", 12345});
    ASSERT_TRUE(test::InsertTagRow(sqlite3, {1, "Tag 1", 11111, 1111111}));
    ASSERT_TRUE(test::InsertTagRow(sqlite3, {2, "Tag 2", 22222, 2222222}));
    ASSERT_TRUE(test::InsertTagRow(sqlite3, {3, "Tag 3", 33333, 3333333}));
    ASSERT_TRUE(test::InsertMemoRow(sqlite3, test::MemoToValues(*memo)));

    const std::vector<unsigned long> expectedTagIds { 1, 3 };
    EXPECT_TRUE(InsertMemoTagIds(memo->id(), expectedTagIds, sqlite3));

    const TableRows expectedRows = {{"1"}, {"3"}};
    const auto rows = test::ExecCommand(sqlite3, "SELECT tagId FROM Tagged WHERE memoId=1 ORDER BY tagId;");
    EXPECT_EQ(expectedRows, rows);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TEST_F(TestSqlite3Functions_WithSqlite3, test_DeleteMemoTagIds)
{
    const auto memo = test::CreateMemo({1, "New title", "New desc", 12345});
    ASSERT_TRUE(test::InsertTagRow(sqlite3, {1, "Tag 1", 11111, 1111111}));
    ASSERT_TRUE(test::InsertTagRow(sqlite3, {2, "Tag 2", 22222, 2222222}));
    ASSERT_TRUE(test::InsertTagRow(sqlite3, {3, "Tag 3", 33333, 3333333}));
    ASSERT_TRUE(test::InsertMemoRow(sqlite3, test::MemoToValues(*memo)));
    ASSERT_TRUE(test::InsertTaggedRow(sqlite3, {1, 1}));
    ASSERT_TRUE(test::InsertTaggedRow(sqlite3, {1, 2}));
    ASSERT_TRUE(test::InsertTaggedRow(sqlite3, {1, 3}));

    EXPECT_TRUE(DeleteMemoTagIds(memo->id(), {1, 3}, sqlite3));

    TableRows expectedRows = {{"2"}};
    const auto rows = test::ExecCommand(sqlite3, "SELECT tagId FROM Tagged WHERE memoId=1 ORDER BY tagId;");
    EXPECT_EQ(expectedRows, rows);
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
                                "AND memoId IN (1,2,3) GROUP BY id LIMIT 100;";
    const auto actualQuery = BuildTagQuery(searchFilter);
    EXPECT_EQ(expectedQuery, actualQuery);
}

TEST(TestSqlite3Functions, test_BuildTagQuery_excludes_name_prefix_and_keywords_if_exact_match_is_in_filter)
{
    TagSearchFilter searchFilter;
    searchFilter.exactNameMatch = "the_name";
    searchFilter.namePrefix = "name_prefix";
    searchFilter.nameContains = "name_contains";

    std::string expectedQuery = "SELECT id, name, color, timestamp FROM Tag "
                                "WHERE name LIKE 'the_name' GROUP BY id LIMIT 100;";
    const auto actualQuery = BuildTagQuery(searchFilter);
    EXPECT_EQ(expectedQuery, actualQuery);
}

TEST(TestSqlite3Functions, test_BuildTagQuery_returns_a_simple_select_query_if_filter_is_empty)
{
    const std::string expectedQuery = "SELECT id, name, color, timestamp FROM Tag GROUP BY id LIMIT 100;";
    const auto actualQuery = BuildTagQuery({});
    EXPECT_EQ(expectedQuery, actualQuery);
}

TEST(TestSqlite3Functions, test_BuildTagQuery_Filter_by_name_prefix_only)
{
    TagSearchFilter searchFilter;
    searchFilter.namePrefix = "name_prefix";
    const auto actualQuery = BuildTagQuery(searchFilter);

    const std::string expectedQuery = "SELECT id, name, color, timestamp FROM Tag WHERE name LIKE 'name_prefix%' GROUP BY id LIMIT 100;";
    EXPECT_EQ(expectedQuery, actualQuery);
}

TEST(TestSqlite3Functions, test_BuildTagQuery_Filter_by_name_keyword_only)
{
    TagSearchFilter searchFilter;
    searchFilter.nameContains = "name_contains";
    const auto actualQuery = BuildTagQuery(searchFilter);

    const std::string expectedQuery = "SELECT id, name, color, timestamp FROM Tag WHERE name LIKE '%name_contains%' GROUP BY id LIMIT 100;";
    EXPECT_EQ(expectedQuery, actualQuery);
}

TEST(TestSqlite3Functions, test_BuildTagQuery_Filter_by_colors_only)
{
    TagSearchFilter searchFilter;
    searchFilter.colors = {111, 222, 333};
    const auto actualQuery = BuildTagQuery(searchFilter);

    const std::string expectedQuery = "SELECT id, name, color, timestamp FROM Tag WHERE color IN (111,222,333) GROUP BY id LIMIT 100;";
    EXPECT_EQ(expectedQuery, actualQuery);
}

TEST(TestSqlite3Functions, test_BuildTagQuery_creates_a_JOIN_query_if_the_filter_contains_memo_ids)
{
    TagSearchFilter searchFilter;
    searchFilter.memoIds = {1, 2, 3};
    const auto actualQuery = BuildTagQuery(searchFilter);

    const std::string expectedQuery = "SELECT id, name, color, timestamp FROM Tag "
                                      "INNER JOIN Tagged ON Tag.id = Tagged.tagId "
                                      "WHERE memoId IN (1,2,3) GROUP BY id LIMIT 100;";
    EXPECT_EQ(expectedQuery, actualQuery);
}

TEST(TestSqlite3Functions, test_BuildTagQuery_Filter_only_contains_start_date)
{
    TagSearchFilter searchFilter;
    searchFilter.filterFromDate = 100001;
    const auto actualQuery = BuildTagQuery(searchFilter);

    const std::string expectedQuery = "SELECT id, name, color, timestamp FROM Tag WHERE timestamp >= 100001 GROUP BY id LIMIT 100;";
    EXPECT_EQ(expectedQuery, actualQuery);
}

TEST(TestSqlite3Functions, test_BuildTagQuery_Filter_only_contains_end_date)
{
    TagSearchFilter searchFilter;
    searchFilter.filterUntilDate = 200002;
    const auto actualQuery = BuildTagQuery(searchFilter);

    const std::string expectedQuery = "SELECT id, name, color, timestamp FROM Tag WHERE timestamp <= 200002 GROUP BY id LIMIT 100;";
    EXPECT_EQ(expectedQuery, actualQuery);
}

TEST(TestSqlite3Functions, test_BuildTagQuery_Filter_contains_only_one_color)
{
    TagSearchFilter searchFilter;
    searchFilter.colors = {111};
    const auto actualQuery = BuildTagQuery(searchFilter);

    const std::string expectedQuery = "SELECT id, name, color, timestamp FROM Tag WHERE color IN (111) GROUP BY id LIMIT 100;";
    EXPECT_EQ(expectedQuery, actualQuery);
}

TEST(TestSqlite3Functions, test_BuildTagQuery_Filter_contains_only_one_memo_id)
{
    TagSearchFilter searchFilter;
    searchFilter.memoIds = {1};
    const auto actualQuery = BuildTagQuery(searchFilter);

    const std::string expectedQuery = "SELECT id, name, color, timestamp FROM Tag "
                                      "INNER JOIN Tagged ON Tag.id = Tagged.tagId "
                                      "WHERE memoId IN (1) GROUP BY id LIMIT 100;";
    EXPECT_EQ(expectedQuery, actualQuery);
}

TEST(TestSqlite3Functions, test_BuildTagQuery_Filter_contains_only_max_result_count)
{
    TagSearchFilter searchFilter;
    searchFilter.maxCount = 3;
    const auto actualQuery = BuildTagQuery(searchFilter);

    const std::string expectedQuery = "SELECT id, name, color, timestamp FROM Tag GROUP BY id LIMIT 3;";
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
            "AND tagId IN (1,2,3) GROUP BY id LIMIT 100;";

    EXPECT_EQ(expectedQuery, query);
}

TEST(TestSqlite3Functions, test_BuildMemoQuery_excludes_title_prefix_and_keywords_if_exact_match_is_in_filter)
{
    MemoSearchFilter filter;
    filter.exactTitleMatch = "exact_title";
    filter.titlePrefix = "title_prefix";
    filter.titleKeywords = {"key1", "key2", "key3"};

    const auto query = BuildMemoQuery(filter);
    const std::string expectedQuery = "SELECT id, title, description, timestamp FROM Memo"
                                      " WHERE title LIKE 'exact_title'"
                                      " GROUP BY id LIMIT 100;";

    EXPECT_EQ(expectedQuery, query);
}

TEST(TestSqlite3Functions, test_BuildMemoQuery_returns_simple_SELECT_query_if_no_filter_is_applied)
{
    const auto query = BuildMemoQuery({});
    const std::string expectedQuery = "SELECT id, title, description, timestamp FROM Memo GROUP BY id LIMIT 100;";

    EXPECT_EQ(expectedQuery, query);
}

TEST(TestSqlite3Functions, test_BuildMemoQuery_Use_title_prefix_filter_only)
{
    MemoSearchFilter filter;
    filter.titlePrefix = "title_prefix";

    const auto query = BuildMemoQuery(filter);
    const std::string expectedQuery = "SELECT id, title, description, timestamp FROM Memo "
                                      "WHERE title LIKE 'title_prefix%' GROUP BY id LIMIT 100;";

    EXPECT_EQ(expectedQuery, query);
}

TEST(TestSqlite3Functions, test_BuildMemoQuery_Use_only_title_keywords_filter)
{
    MemoSearchFilter filter;
    filter.titleKeywords = {"key1", "key2", "key3"};

    const auto query = BuildMemoQuery(filter);
    const std::string expectedQuery = "SELECT id, title, description, timestamp FROM Memo "
                                      "WHERE (title LIKE '%key1%' OR title LIKE '%key2%' OR title LIKE '%key3%') "
                                      "GROUP BY id LIMIT 100;";
    EXPECT_EQ(expectedQuery, query);
}

TEST(TestSqlite3Functions, test_BuildMemoQuery_ignores_keyword_duplicates_for_title)
{
    MemoSearchFilter filter;
    filter.titleKeywords = {"key1", "key2", "key3", "key2", "key1"};

    const auto query = BuildMemoQuery(filter);
    const std::string expectedQuery = "SELECT id, title, description, timestamp FROM Memo "
                                      "WHERE (title LIKE '%key1%' OR title LIKE '%key2%' OR title LIKE '%key3%') "
                                      "GROUP BY id LIMIT 100;";
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
            "GROUP BY id LIMIT 100;";
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
            "GROUP BY id LIMIT 100;";
    EXPECT_EQ(expectedQuery, query);
}

TEST(TestSqlite3Functions, test_BuildMemoQuery_creates_JOIN_query_if_filter_contains_tag_ids)
{
    MemoSearchFilter filter;
    filter.tagIds = {1, 2, 3};

    const auto query = BuildMemoQuery(filter);
    const std::string expectedQuery = "SELECT id, title, description, timestamp FROM Memo "
                                      "INNER JOIN Tagged ON Memo.id = Tagged.memoId "
                                      "WHERE tagId IN (1,2,3) GROUP BY id LIMIT 100;";

    EXPECT_EQ(expectedQuery, query);
}

TEST(TestSqlite3Functions, test_BuildMemoQuery_Filter_by_start_date_only)
{
    MemoSearchFilter filter;
    filter.filterFromDate = 10001;

    const auto query = BuildMemoQuery(filter);
    const std::string expectedQuery = "SELECT id, title, description, timestamp FROM Memo "
                                      "WHERE timestamp >= 10001 GROUP BY id LIMIT 100;";

    EXPECT_EQ(expectedQuery, query);
}

TEST(TestSqlite3Functions, test_BuildMemoQuery_Filter_by_end_date_only)
{
    MemoSearchFilter filter;
    filter.filterUntilDate = 20002;

            const auto query = BuildMemoQuery(filter);
    const std::string expectedQuery = "SELECT id, title, description, timestamp FROM Memo "
                                      "WHERE timestamp <= 20002 GROUP BY id LIMIT 100;";

    EXPECT_EQ(expectedQuery, query);
}

TEST(TestSqlite3Functions, test_BuildMemoQuery_Specify_only_one_tag_id_in_filter)
{
    MemoSearchFilter filter;
    filter.tagIds = {1};

    const auto query = BuildMemoQuery(filter);
    const std::string expectedQuery = "SELECT id, title, description, timestamp FROM Memo "
                                      "INNER JOIN Tagged ON Memo.id = Tagged.memoId "
                                      "WHERE tagId IN (1) GROUP BY id LIMIT 100;";

    EXPECT_EQ(expectedQuery, query);
}

TEST(TestSqlite3Functions, test_BuildMemoQuery_Specify_only_one_title_keyword_in_filter)
{
    MemoSearchFilter filter;
    filter.titleKeywords = {"key1"};

    const auto query = BuildMemoQuery(filter);
    const std::string expectedQuery = "SELECT id, title, description, timestamp FROM Memo "
                                      "WHERE (title LIKE '%key1%') GROUP BY id LIMIT 100;";
    EXPECT_EQ(expectedQuery, query);
}

TEST(TestSqlite3Functions, test_BuildMemoQuery_Specify_only_one_description_keyword_in_filter)
{
    MemoSearchFilter filter;
    filter.memoKeywords = {"mKey1"};

    const auto query = BuildMemoQuery(filter);
    const std::string expectedQuery =
            "SELECT id, title, description, timestamp FROM Memo "
            "WHERE (description LIKE '%mKey1%') GROUP BY id LIMIT 100;";
    EXPECT_EQ(expectedQuery, query);
}

TEST(TestSqlite3Functions, test_BuildMemoQuery_Specify_only_max_result_count)
{
    MemoSearchFilter filter;
    filter.maxCount = 3;

    const auto query = BuildMemoQuery(filter);
    const std::string expectedQuery =
            "SELECT id, title, description, timestamp FROM Memo GROUP BY id LIMIT 3;";
    EXPECT_EQ(expectedQuery, query);
}
} // namespace memo
