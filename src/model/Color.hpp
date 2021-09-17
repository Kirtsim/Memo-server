#pragma once
#include <sstream>

namespace memo::model {

/// @brief Struct representing an RGB color. Consists of the
///        red, green and blue int color values.
struct Color
{
    int red = 0;
    int green = 0;
    int blue = 0;

    Color() = default;

    Color(int red, int green, int blue);

    bool operator==(const Color& other) const;

    bool operator!=(const Color& other) const;
};

std::ostream& operator<<(std::ostream& stream, const Color& color);

} // namespace memo::model
