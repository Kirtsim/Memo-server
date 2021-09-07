#pragma once
#include "db/SqLiteDatabase.hpp"
#include <filesystem>
#include <cstdio>
#include <gtest/gtest.h>

namespace memo {

TEST(TestSQLiteDatabase, test_constructor_Check_database_does_not_automatically_open)
{
    SQLiteDatabase database("/fake/path/to/db.db");
    EXPECT_FALSE(database.isOpen());
}

TEST(TestSQLiteDatabase, test_open_Create_new_db_file_when_it_does_not_exist)
{
    const auto filePath = std::filesystem::current_path().string() + "/test.db";
    std::remove(filePath.c_str());

    SQLiteDatabase database(filePath);
    EXPECT_TRUE(database.open());
    EXPECT_TRUE(database.isOpen());
    EXPECT_TRUE(std::filesystem::exists(filePath));
    std::remove(filePath.c_str());
}

TEST(TestSQLiteDatabase, test_close_Database_file_should_not_be_deleted)
{
    const auto filePath = std::filesystem::current_path().string() + "/test.db";
    std::remove(filePath.c_str());

    SQLiteDatabase database(filePath);
    EXPECT_TRUE(database.open());
    EXPECT_TRUE(database.isOpen());
    EXPECT_TRUE(database.close());
    EXPECT_FALSE(database.isOpen());
    EXPECT_TRUE(std::filesystem::exists(filePath));
    std::remove(filePath.c_str());
}

TEST(TestSQLiteDatabase, test_exec_Perform_create_table_cmd_without_callback_Return_success)
{
    const auto filePath = std::filesystem::current_path().string() + "/test.db";
    std::remove(filePath.c_str());
    const std::string sqlCommand = "CREATE TABLE IF NOT EXISTS test_table (id INTEGER PRIMARY KEY);";

    SQLiteDatabase database(filePath);
    database.open();
    const auto succeeded = database.exec(sqlCommand, nullptr);
    EXPECT_TRUE(succeeded);
    EXPECT_TRUE(database.close());
    std::remove(filePath.c_str());
}

TEST(TestSQLiteDatabase, test_exec_Perform_create_table_cmd_Return_success)
{
    const auto filePath = std::filesystem::current_path().string() + "/test.db";
    std::remove(filePath.c_str());

    SQLiteDatabase database(filePath);
    database.open();
    const bool succeeded = database.exec("CREATE TABLE test (id INTEGER PRIMARY KEY);",  nullptr);
    EXPECT_TRUE(succeeded);
    EXPECT_TRUE(database.close());
    std::remove(filePath.c_str());
}

TEST(TestSQLiteDatabase, test_exec_Perform_create_table_Check_callback_is_called_once)
{
    const auto filePath = std::filesystem::current_path().string() + "/test.db";
    std::remove(filePath.c_str());

    int callbackCallCount = 0;
    auto callback = [&](const std::vector<std::string>&/*values*/, const std::vector<std::string>&/*colNames*/) {
        ++callbackCallCount;
        return true;
    };

    SQLiteDatabase database(filePath);
    database.open();
    const bool succeeded = database.exec("CREATE TABLE test (id INTEGER PRIMARY KEY);",  callback);
    EXPECT_TRUE(succeeded);
    EXPECT_EQ(callbackCallCount, 0);
    EXPECT_TRUE(database.close());
    std::remove(filePath.c_str());
}

} // namespace memo
