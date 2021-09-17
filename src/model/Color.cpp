#include "model/Color.hpp"

namespace memo::model {

Color::Color(int red, int green, int blue)
    : red(red)
    , green(green)
    , blue(blue)
{
}

bool Color::operator==(const Color& other) const
{
    return red == other.red
        && green == other.green
        && blue == other.blue;
}

bool Color::operator!=(const Color& other) const
{
    return !(*this == other);
}

std::ostream& operator<<(std::ostream& stream, const Color& color)
{
    stream << "Color {" << color.red << ", " << color.green << ", " << color.blue << "}";
    return stream;
}

} // namespace memo::model