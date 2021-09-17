#pragma once
#include "model/Color.hpp"
#include "db/Tools.hpp"
#include <vector>
#include <gtest/gtest.h>

namespace memo {

TEST(TestTools, test_ColorToInt_PureColors)
{
    EXPECT_EQ(tools::ColorToInt({ 255, 255, 255 }), 0xFFFFFF) << "White";
    EXPECT_EQ(tools::ColorToInt({ 0, 0, 0 }), 0x0) << "Black";
    EXPECT_EQ(tools::ColorToInt({ 255, 0, 0 }), 0xFF0000) << "Red";
    EXPECT_EQ(tools::ColorToInt({ 0, 255, 0 }), 0x00FF00) << "Green";
    EXPECT_EQ(tools::ColorToInt({ 0, 0, 255 }), 0x0000FF) << "Blue";
}

TEST(TestTools, test_ColorToInt_RandomColor)
{
    EXPECT_EQ(tools::ColorToInt({ 173, 6, 219 }), 0xAD06DB);
}

TEST(TestTools, test_IntToColor_PureColors)
{
    EXPECT_EQ(tools::IntToColor(0xFFFFFF), model::Color(255, 255, 255)) << "White";
    EXPECT_EQ(tools::IntToColor(0x0), model::Color(0, 0, 0)) << "Black";
    EXPECT_EQ(tools::IntToColor(0xFF0000), model::Color(255, 0, 0)) << "Red";
    EXPECT_EQ(tools::IntToColor(0x00FF00), model::Color(0, 255, 0)) << "Green";
    EXPECT_EQ(tools::IntToColor(0x0000FF), model::Color(0, 0, 255)) << "Blue";
}

TEST(TestTools, test_IntToColor_RandomColor)
{
    EXPECT_EQ(tools::IntToColor(0x01FD61), model::Color(1, 253, 97));
}

TEST(TestTools, test_IntToColor_and_back_ColorToInt_roundcheck)
{
    std::vector<int> colorValues { 0xF1365F, 0xF01EA2, 0xB95653, 0x113157, 0x21C6BF, 0x31060F };
    for (auto startingColorValue : colorValues)
    {
        const auto color = tools::IntToColor(startingColorValue);
        EXPECT_EQ(tools::ColorToInt(color), startingColorValue) << color;
    }
}


} // namespace memo
