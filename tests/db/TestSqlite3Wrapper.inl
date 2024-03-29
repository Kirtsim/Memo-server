#pragma once
#include "db/Sqlite3Wrapper.hpp"
#include "db/Sqlite3TestHelperFunctions.hpp"
#include "TestingUtils.hpp"
#include <memory>
#include <sstream>
#include <filesystem>
#include <cstdio>
#include <gtest/gtest.h>

namespace memo {

using StringVector = std::vector<std::string>;

namespace {
    std::string testDbFilePath();

    std::shared_ptr<Sqlite3Wrapper> DatabaseWithEmptyTable();

    StringVector ValuesToStringVector(const test::MemoValues& values);

} // namespace

TEST(TestSqlite3Wrapper, test_constructor_Check_database_does_not_automatically_open)
{
    Sqlite3Wrapper database(testDbFilePath());
    EXPECT_FALSE(database.isOpen());
}

TEST(TestSqlite3Wrapper, test_open_Create_new_db_file_when_it_does_not_exist)
{
    test::RemoveFile(testDbFilePath());

    Sqlite3Wrapper database(testDbFilePath());
    EXPECT_TRUE(database.open());
    EXPECT_TRUE(database.isOpen());
    EXPECT_TRUE(std::filesystem::exists(testDbFilePath()));
    test::RemoveFile(testDbFilePath());
}

TEST(TestSqlite3Wrapper, test_close_Database_file_should_not_be_deleted)
{
    test::RemoveFile(testDbFilePath());

    Sqlite3Wrapper database(testDbFilePath());
    EXPECT_TRUE(database.open());
    EXPECT_TRUE(database.isOpen());
    EXPECT_TRUE(database.close());
    EXPECT_FALSE(database.isOpen());
    EXPECT_TRUE(std::filesystem::exists(testDbFilePath()));
    test::RemoveFile(testDbFilePath());
}

TEST(TestSqlite3Wrapper, test_exec_Perform_create_table_cmd_without_callback_Return_success)
{
    test::RemoveFile(testDbFilePath());
    const std::string sqlCommand = "CREATE TABLE IF NOT EXISTS test_table (id INTEGER PRIMARY KEY);";

    Sqlite3Wrapper database(testDbFilePath());
    database.open();
    const auto succeeded = database.exec(sqlCommand, nullptr);
    EXPECT_TRUE(succeeded);
    EXPECT_TRUE(database.close());
    test::RemoveFile(testDbFilePath());
}

TEST(TestSqlite3Wrapper, test_exec_Perform_create_table_cmd_Return_success)
{
    test::RemoveFile(testDbFilePath());

    Sqlite3Wrapper database(testDbFilePath());
    database.open();
    const bool succeeded = database.exec("CREATE TABLE test (id INTEGER PRIMARY KEY);",  nullptr);
    EXPECT_TRUE(succeeded);
    EXPECT_TRUE(database.close());
    test::RemoveFile(testDbFilePath());
}

TEST(TestSqlite3Wrapper, test_exec_Perform_create_table_Check_callback_is_called_once)
{
    test::RemoveFile(testDbFilePath());

    int callbackCallCount = 0;
    auto callback = [&](const StringVector&/*values*/, const StringVector&/*colNames*/) {
        ++callbackCallCount;
        return false;
    };

    Sqlite3Wrapper database(testDbFilePath());
    database.open();
    const bool succeeded = database.exec("CREATE TABLE test (id INTEGER PRIMARY KEY);",  callback);
    EXPECT_TRUE(succeeded);
    EXPECT_EQ(callbackCallCount, 0);
    EXPECT_TRUE(database.close());
    test::RemoveFile(testDbFilePath());
}

TEST(TestSqlite3Wrapper, test_exec_Perform_SELECT_ALL_on_empty_table_Callback_shouldnt_be_called_Expect_success)
{
    auto database = DatabaseWithEmptyTable();
    int callbackCallCount = 0;
    auto callback = [&](const StringVector&/*values*/, const StringVector&/*colNames*/) {
        ++callbackCallCount;
        return false;
    };

    const bool succeeded = database->exec("SELECT * FROM Memo;",  callback);
    EXPECT_TRUE(succeeded);
    EXPECT_EQ(callbackCallCount, 0);
    EXPECT_TRUE(database->close());
    test::RemoveFile(testDbFilePath());
}

TEST(TestSqlite3Wrapper, test_exec_Perform_INSERT_Callback_shouldnt_be_called_Expect_success)
{
    auto database = DatabaseWithEmptyTable();
    int callbackCallCount = 0;
    auto callback = [&](const StringVector&/*values*/, const StringVector&/*colNames*/) {
        ++callbackCallCount;
        return false;
    };

    const bool succeeded = database->exec("INSERT INTO Memo VALUES(1, 'Memo1', 'MemoDesc1', 12345);",  callback);
    EXPECT_TRUE(succeeded);
    EXPECT_EQ(callbackCallCount, 0);
    EXPECT_TRUE(database->close());
    test::RemoveFile(testDbFilePath());
}

TEST(TestSqlite3Wrapper, test_exec_Perform_INSERT_with_duplicate_id_Expect_failure)
{
    auto database = DatabaseWithEmptyTable();
    ASSERT_TRUE(test::InsertMemoRow(*database, {1, "Memo1", "MemoDesc1", 11}));

    int callbackCallCount = 0;
    auto callback = [&](const StringVector&/*values*/, const StringVector&/*colNames*/) {
        ++callbackCallCount;
        return false;
    };

    const bool succeeded = database->exec("INSERT INTO Memo VALUES(1, 'Memo2', 'MemoDesc2', 22345);",  callback);
    EXPECT_FALSE(succeeded);
    EXPECT_EQ(callbackCallCount, 0);
    EXPECT_TRUE(database->close());
    test::RemoveFile(testDbFilePath());
}

TEST(TestSqlite3Wrapper, test_exec_Perform_SELECT_ALL_Callback_should_be_called_for_each_row_Expect_success)
{
    auto database = DatabaseWithEmptyTable();
    test::MemoValues row1 {1, "Memo1", "MemoDesc1", 11};
    test::MemoValues row2 {2, "Memo2", "MemoDesc2", 22};
    test::MemoValues row3 {3, "Memo3", "MemoDesc3", 33};
    ASSERT_TRUE(test::InsertMemoRow(*database, row1));
    ASSERT_TRUE(test::InsertMemoRow(*database, row2));
    ASSERT_TRUE(test::InsertMemoRow(*database, row3));

    std::vector<StringVector> expectedValueVectors {
        ValuesToStringVector(row1),
        ValuesToStringVector(row2),
        ValuesToStringVector(row3)
    };
    std::vector<StringVector> actualValueVectors;
    auto callback = [&](const StringVector& values, const StringVector&/*colNames*/) {
        actualValueVectors.emplace_back(values);
        return false;
    };

    const bool succeeded = database->exec("SELECT * FROM Memo;",  callback);
    EXPECT_TRUE(succeeded);
    EXPECT_EQ(actualValueVectors.size(), 3ul);
    for (size_t i = 0; i < std::min(actualValueVectors.size(), 3ul); ++i)
    {
        const auto& actualValues = actualValueVectors[i];
        const auto& expectedValues = expectedValueVectors[i];
        EXPECT_EQ(actualValues, expectedValues);
    }
    EXPECT_TRUE(database->close());
    test::RemoveFile(testDbFilePath());
}

TEST(TestSqlite3Wrapper, test_exec_Perform_DELETE_Callback_shouldnt_be_called_Expect_success)
{
    auto database = DatabaseWithEmptyTable();
    int callbackCallCount = 0;
    auto callback = [&](const StringVector&/*values*/, const StringVector&/*colNames*/) {
        ++callbackCallCount;
        return false;
    };
    ASSERT_TRUE(test::InsertMemoRow(*database, {1, "Memo1", "MemoDesc1", 11}));
    ASSERT_TRUE(test::InsertMemoRow(*database, {2, "Memo2", "MemoDesc2", 22}));
    ASSERT_TRUE(test::InsertMemoRow(*database, {3, "Memo3", "MemoDesc3", 33}));

    const bool deleteSucceeded = database->exec("DELETE FROM Memo WHERE id IN (1, 2, 3);",  callback);
    EXPECT_TRUE(deleteSucceeded);
    EXPECT_EQ(callbackCallCount, 0);

    const bool selectSucceeded = database->exec("SELECT * FROM Memo;",  callback);
    EXPECT_TRUE(selectSucceeded);
    EXPECT_EQ(callbackCallCount, 0);
    EXPECT_TRUE(database->close());
    test::RemoveFile(testDbFilePath());
}

TEST(TestSqlite3Wrapper, test_exec_Perform_DELETE_on_non_existing_row_Expect_success)
{
    auto database = DatabaseWithEmptyTable();
    int callbackCallCount = 0;
    auto callback = [&](const StringVector&/*values*/, const StringVector&/*colNames*/) {
        ++callbackCallCount;
        return false;
    };
    ASSERT_TRUE(test::InsertMemoRow(*database, {1, "Memo1", "MemoDesc1", 11}));

    const bool deleteSucceeded = database->exec("DELETE FROM Memo WHERE id = 10;",  callback);
    EXPECT_TRUE(deleteSucceeded);
    EXPECT_EQ(callbackCallCount, 0);

    const bool selectSucceeded = database->exec("SELECT * FROM Memo;",  callback);
    EXPECT_TRUE(selectSucceeded);
    EXPECT_EQ(callbackCallCount, 1);
    EXPECT_TRUE(database->close());
    test::RemoveFile(testDbFilePath());
}

TEST(TestSqlite3Wrapper, test_exec_Perform_UPDATE_Callback_shouldnt_be_called_Expect_success)
{
    auto database = DatabaseWithEmptyTable();
    int callbackCallCount = 0;
    StringVector fetchedValues;
    auto callback = [&](const StringVector& values, const StringVector&/*colNames*/) {
        ++callbackCallCount;
        fetchedValues = values;
        return false;
    };
    ASSERT_TRUE(test::InsertMemoRow(*database, {1, "Memo1", "MemoDesc1", 11}));

    const bool succeeded = database->exec("UPDATE Memo SET title='NewMemo' WHERE id = 1;",  callback);
    EXPECT_TRUE(succeeded);
    EXPECT_EQ(callbackCallCount, 0);

    callbackCallCount = 0;
    const bool selectSucceeded = database->exec("SELECT title FROM Memo WHERE id = 1;",  callback);
    EXPECT_TRUE(selectSucceeded);
    EXPECT_EQ(callbackCallCount, 1);
    ASSERT_EQ(fetchedValues.size(), 1ul);
    EXPECT_EQ(fetchedValues.front(), "NewMemo");
    EXPECT_TRUE(database->close());
    test::RemoveFile(testDbFilePath());
}

TEST(TestSqlite3Wrapper, test_exec_Perform_UPDATE_on_data_with_non_existing_id_Expect_success)
{
    auto database = DatabaseWithEmptyTable();
    int callbackCallCount = 0;
    StringVector fetchedValues;
    auto callback = [&](const StringVector& values, const StringVector&/*colNames*/) {
        ++callbackCallCount;
        fetchedValues = values;
        return false;
    };
    ASSERT_TRUE(test::InsertMemoRow(*database, {1, "Memo1", "MemoDesc1", 11}));

    const bool succeeded = database->exec("UPDATE Memo SET title='NewMemo' WHERE id=10;",  callback);
    EXPECT_TRUE(succeeded);
    EXPECT_EQ(callbackCallCount, 0);

    callbackCallCount = 0;
    const bool selectSucceeded = database->exec("SELECT title FROM Memo WHERE id=1;",  callback);
    EXPECT_TRUE(selectSucceeded);
    EXPECT_EQ(callbackCallCount, 1);
    ASSERT_EQ(fetchedValues.size(), 1ul);
    EXPECT_EQ(fetchedValues.front(), "Memo1");
    EXPECT_TRUE(database->close());
    test::RemoveFile(testDbFilePath());
}

namespace {

std::string testDbFilePath()
{
    return test::TestFilePath("temp.db");
}

std::shared_ptr<Sqlite3Wrapper> DatabaseWithEmptyTable()
{
    const std::string createTableCmd =
            "CREATE TABLE IF NOT EXISTS Memo("
                "id INTEGER PRIMARY KEY,"
                "title TEXT NOT NULL,"
                "description TEXT,"
                "timestamp INTEGER NOT NULL);";
    test::RemoveFile(testDbFilePath());
    auto database = std::make_shared<Sqlite3Wrapper>(testDbFilePath());
    database->open();
    database->exec(createTableCmd, nullptr);
    return database;
}


StringVector ValuesToStringVector(const test::MemoValues& values)
{
    return {
        std::to_string(values.id),
        values.title,
        values.description,
        std::to_string(values.timestamp)
    };
}

} // namespace

} // namespace memo
