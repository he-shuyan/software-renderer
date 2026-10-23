#pragma once


#include "Color.h"
#include "Vec2.h"
#include "image.h"
#include "depth.h"

struct ScreenVertex {
	Vec2 pos;//坐标
	double depth = 0.0;//深度
	Color color;//颜色
	Vec2 uv;
	double invW = 1.0;      // 透视投影下是 1/w；正交投影下恒为 1
};

double edge(const Vec2& a, const Vec2& b, const Vec2& p);

//2D填充
void drawTriangle(Image& iamge, Vec2 p0, Vec2 p1, Vec2 p2, Color c0, Color c1, Color c2);

//3D填充
void drawTriangleDepth(Image& image, DepthBuffer& depth,
	ScreenVertex v0, ScreenVertex v1, ScreenVertex v2,
	const Image* texture = nullptr);