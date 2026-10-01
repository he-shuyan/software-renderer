#pragma once

#include <cstdint>

#include "image.h"
#include "Vec2.h"

void drawLine(Image& image, const Vec2& from, const Vec2& to,
    std::uint8_t r, std::uint8_t g, std::uint8_t b);