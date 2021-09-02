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
    SQLiteDatabase database(filePath);
    EXPECT_TRUE(database.open());
    EXPECT_TRUE(database.isOpen());
    std::remove(filePath.c_str());
}

} // namespace memo
