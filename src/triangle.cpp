#include "triangle.h"

#include <algorithm>
#include <cmath>
#include <utility>


//外积：向量叉乘a X b=a.x*b.y-a.y*b.x;
double edge(const Vec2& a, const Vec2& b, const Vec2& p) {
	return (b.x - a.x) * (p.y - a.y) - (b.y - a.y) * (p.x - a.x);
}

//颜色混合
static Color mix3(const Color& c0, const Color& c1, const Color& c2,
	double w0, double w1, double w2) {
	const double r = c0.r * w0 + c1.r * w1 + c2.r * w2;
	const double g = c0.g * w0 + c1.g * w1 + c2.g * w2;
	const double b = c0.b * w0 + c1.b * w1 + c2.b * w2;
	return Color(static_cast<std::uint8_t>(std::lround(r)),
		static_cast<std::uint8_t>(std::lround(g)),
		static_cast<std::uint8_t>(std::lround(b)));
}

//包围盒+颜色填充
void drawTriangle(Image& image, Vec2 p0, Vec2 p1, Vec2 p2, Color c0, Color c1, Color c2) {
	//area是以p0,p1,p2为顶点的三角形有向面积的二倍
	double area2 = edge(p0, p1, p2);

	//三点共线，画不出三角形
	if (area2 == 0) {
		return;
	}

	//统一绕序，保证area2为正
	if (area2 < 0.0) {
		std::swap(p1, p2);
		std::swap(c1, c2);
		area2 = -area2;
	}

	//计算包围盒，包围盒就是能完全包住三角形的最小矩形。
	const double minXd = std::min({ p0.x, p1.x, p2.x });
	const double maxXd = std::max({ p0.x, p1.x, p2.x });
	const double minYd = std::min({ p0.y, p1.y, p2.y });
	const double maxYd = std::max({ p0.y, p1.y, p2.y });

	//再用 floor 向下取整、ceil 向上取整，转成整数像素范围。
	const int minX = std::max(0, static_cast<int>(std::floor(minXd)));
	const int maxX = std::min(image.width() - 1, static_cast<int>(std::ceil(maxXd)));
	const int minY = std::max(0, static_cast<int>(std::floor(minYd)));
	const int maxY = std::min(image.height() - 1, static_cast<int>(std::ceil(maxYd)));

	//遍历包围盒
	for (int y = minY; y < maxY; ++y) {
		for (int x = minX; x < maxX; ++x) {
			const Vec2 p(static_cast<double>(x) + 0.5,static_cast<double>(y) + 0.5);
			//用像素中心做判定,计算重心坐标
			//像素 (x, y) 覆盖的区域是 [x, x+1) × [y, y+1)，它的中心是(x + 0.5, y + 0.5)。
			const double w0 = edge(p1, p2, p) / area2;
			const double w1 = edge(p2, p0, p) / area2;
			const double w2 = edge(p0, p1, p) / area2;
			//判定是否在三角形内
			if (w0 >= 0.0 && w1 >= 0.0 && w2 >= 0.0) {
				const Color c = mix3(c0, c1, c2, w0, w1, w2);
				image.setPixel(x, y, c.r, c.g, c.b);
			}
		}
	}
}
