#pragma once

#include "Color.h"
#include "image.h"
#include "Vec2.h"

void drawLineDDA(Image& image, const Vec2& from, const Vec2& to,
    const Color& c0, const Color& c1);

void drawLineBresenham(Image& image, const Vec2& from, const Vec2& to, const Color& c0, const Color& c1);