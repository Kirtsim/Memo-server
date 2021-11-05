#pragma once
#include "model/Color.hpp"

namespace memo::tools {

int ColorToInt(const model::Color& color);

model::Color IntToColor(int numericColor);

std::string EscapeSingleQuotes(const std::string& text);

} // namespace memo::tools
