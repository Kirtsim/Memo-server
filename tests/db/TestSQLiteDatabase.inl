#pragma once

#include "db/SqLiteDatabase.hpp"
#include <gtest/gtest.h>

namespace memo {

TEST(TestSQLiteDatabase, test_constructor_Check_database_does_not_automatically_open)
{
    SQLiteDatabase database("/fake/path/to/db.db");
    EXPECT_FALSE(database.isOpen());
}

} // namespace memo
