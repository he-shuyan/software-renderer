//RGB颜色渐变
#pragma once

#include <cmath>
#include <cstdint>

//颜色结构体
struct Color {
	//颜色初始化，初始化为0表示黑色
	std::uint8_t r = 0;
	std::uint8_t g = 0;
	std::uint8_t b = 0;

	Color() = default;//生成默认构造函数
	Color(std::uint8_t r_, std::uint8_t g_, std::uint8_t b_) :r(r_), g(g_), b(b_){}//有参构造

	
};

//linear interpolation（线性插值）结果 = a + (b - a) * t类似于y=y0+(y1-y0)*(x-x0)/(x1-x0)
	//加inline使得该函数可以在不需要cpp文件的情况下直接使用
inline Color lerp(const Color& a,const Color& b, double t) {
	const double r = a.r + (b.r - a.r) * t;
	const double g = a.g + (b.g - a.g) * t;
	const double b1 = a.b + (b.b - a.b) * t;

	return Color(static_cast<std::uint8_t>(std::lround(r)),
		static_cast<std::uint8_t>(std::lround(g)),
		static_cast<std::uint8_t>(std::lround(b1)));//std::lround：四舍五入到最近的整数
}