#include "db/TestSqlite3Wrapper.inl"
#include "db/TestSqlite3Functions.inl"
#include "db/TestSqlite3Database.inl"
#include "db/TestTools.inl"
#include <gtest/gtest.h>
#include "logger/Logger.hpp"

INITIALIZE_EASYLOGGINGPP

int main()
{
    testing::InitGoogleTest();
    return RUN_ALL_TESTS();
}