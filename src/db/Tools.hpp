#pragma once
#include "model/Color.hpp"

namespace memo::tools {

int ColorToInt(const model::Color& color);

model::Color IntToColor(int numericColor);

} // namespace memo::tools
