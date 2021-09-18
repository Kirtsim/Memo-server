#include "db/TestSqlite3Wrapper.inl"
#include "db/TestSqlite3Functions.inl"
#include "db/TestTools.inl"
#include <gtest/gtest.h>

int main()
{
    testing::InitGoogleTest();
    return RUN_ALL_TESTS();
}