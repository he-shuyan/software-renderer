#pragma once


#include "Color.h"
#include "Vec2.h"
#include "image.h"

double edge(const Vec2& a, const Vec2& b, const Vec2& p);

void drawTriangle(Image& iamge, Vec2 p0, Vec2 p1, Vec2 p2, Color c0, Color c1, Color c2);
