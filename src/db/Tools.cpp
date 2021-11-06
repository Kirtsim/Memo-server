#include "db/Tools.hpp"

namespace memo::tools {

int ColorToInt(const model::Color& color)
{
    int ones = ~0;
    int redValue = (ones & color.red) << 16;
    int greenValue = (ones & color.green) << 8;
    int blueValue = (ones & color.blue);

    return (redValue | greenValue | blueValue );
}

model::Color IntToColor(int numericColor)
{
    int ones = 0xFF; // 0000 0000  0000 0000  0000 0000  1111 1111
    model::Color color;
    color.blue = (numericColor & ones);
    numericColor = numericColor >> 8;
    color.green = (numericColor & ones);
    numericColor = numericColor >> 8;
    color.red = (numericColor & ones);
    return color;
}

std::string EscapeSingleQuotes(const std::string& text)
{
    std::string result;
    for (const char character : text)
    {
        if (character == '\'')
            // double the quote (this is how it works in SQL).
            result.push_back(character);
        result.push_back(character);
    }
    return result;
}

} // namespace memo::tools