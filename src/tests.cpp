#include <iostream>

#include "Color.h"
#include "Mat3.h"
#include "Vec2.h"
#include "image.h"
#include "line.h"
#include "triangle.h"

void testMat3() {//测试函数
	const Vec2 p(3.0, 4.0);

	const Vec2 a = transformPoint(Mat3::identity(), p);
	std::cout << "单位矩阵:     (" << a.x << ", " << a.y << ")\n";

	const Vec2 b = transformPoint(Mat3::scaling(2.0, 2.0), p);
	std::cout << "放大两倍:     (" << b.x << ", " << b.y << ")\n";

	const Vec2 c = transformPoint(Mat3::scaling(2.0, 0.5), p);
	std::cout << "x 加倍 y 减半: (" << c.x << ", " << c.y << ")\n";

	const Vec2 d = transformPoint(Mat3::translation(10.0, 20.0), p);
	std::cout << "平移(10,20):  (" << d.x << ", " << d.y << ")\n";

	const double halfPi = kPi / 2;
	const Vec2 e = transformPoint(Mat3::rotation(halfPi), Vec2(1.0, 0.0));
	std::cout << "把(1,0)转90度: (" << e.x << ", " << e.y << ")\n";

	const Vec2 f = transformPoint(Mat3::rotation(halfPi), Vec2(0.0, 1.0));
	std::cout << "把(0,1)转90度: (" << f.x << ", " << f.y << ")\n";

	const Mat3 rot = Mat3::rotation(halfPi);
	const Mat3 trans = Mat3::translation(5.0, 0.0);
	const Vec2 q(1.0, 0.0);

	const Vec2 g = transformPoint(rot * trans, q);
	std::cout << "先平移再旋转: (" << g.x << ", " << g.y << ")\n";

	const Vec2 h = transformPoint(trans * rot, q);
	std::cout << "先旋转再平移: (" << h.x << ", " << h.y << ")\n";

	const Vec2 center(10.0, 0.0);
	const Mat3 pivot = Mat3::translation(center.x, center.y)
		* Mat3::rotation(halfPi)
		* Mat3::translation(-center.x, -center.y);

	const Vec2 r = transformPoint(pivot, Vec2(11.0, 0.0));
	std::cout << "点(11,0)绕(10,0)转90度: (" << r.x << ", " << r.y << ")\n";

	const Vec2 s = transformPoint(rot * trans, q);
	const Vec2 u = transformPoint(rot, transformPoint(trans, q));

	std::cout << "组合矩阵:  (" << s.x << ", " << s.y << ")\n";
	std::cout << "嵌套调用:  (" << u.x << ", " << u.y << ")\n";
}

void dumpSmallLine() {
	Image img(8, 4);

	for (int y = 0; y < 4; ++y) {
		for (int x = 0; x < 8; ++x) {
			img.setPixel(x, y, 0, 0, 0);
		}
	}

	const Color white(255, 255, 255);
	drawLineBresenham(img, Vec2(0.0, 0.0), Vec2(5.0, 2.0), white, white);

	std::cout << "Bresenham 在 8x4 画布上点亮的像素：";

	for (int y = 0; y < 4; ++y) {
		for (int x = 0; x < 8; ++x) {
			if (img.getPixel(x, y, 0) != 0) {
				std::cout << "(" << x << ", " << y << ") ";
			}
		}
	}

	std::cout << "\n";
}

void compareLineAlgorithms(int width, int height) {
	Image imgDDA(width, height);
	Image imgBres(width, height);

	for (int y = 0; y < height; ++y) {
		for (int x = 0; x < width; ++x) {
			imgDDA.setPixel(x, y, 0, 0, 0);
			imgBres.setPixel(x, y, 0, 0, 0);
		}
	}

	const Vec2 p0(40.0, 30.0);
	const Vec2 p1(600.0, 315.0);
	const Color white(255, 255, 255);

	drawLineDDA(imgDDA, p0, p1, white, white);
	drawLineBresenham(imgBres, p0, p1, white, white);

	int ddaCount = 0;
	int bresCount = 0;
	int diffCount = 0;

	for (int y = 0; y < height; ++y) {
		for (int x = 0; x < width; ++x) {
			const bool inDDA = imgDDA.getPixel(x, y, 0) != 0;
			const bool inBres = imgBres.getPixel(x, y, 0) != 0;

			if (inDDA) ++ddaCount;
			if (inBres) ++bresCount;
			if (inDDA != inBres) ++diffCount;
		}
	}

	std::cout << "DDA 点亮像素:      " << ddaCount << "\n";
	std::cout << "Bresenham 点亮像素: " << bresCount << "\n";
	std::cout << "两者不一致的像素:  " << diffCount << "\n";
}

//测试函数
void testEdge() {
	const Vec2 a(0.0, 0.0);
	const Vec2 b(10.0, 0.0);

	std::cout << "点(5, 3)  的 edge = " << edge(a, b, Vec2(5.0, 3.0)) << "\n";
	std::cout << "点(5, -3) 的 edge = " << edge(a, b, Vec2(5.0, -3.0)) << "\n";
	std::cout << "点(15, 0) 的 edge = " << edge(a, b, Vec2(15.0, 0.0)) << "\n";
	std::cout << "点(5, 0)  的 edge = " << edge(a, b, Vec2(5.0, 0.0)) << "\n";
}

void testTriangleArea(int width, int height) {
	Image img(width, height);

	const Vec2 p0(100.0, 50.0);
	const Vec2 p1(500.0, 150.0);
	const Vec2 p2(300.0, 350.0);

	drawTriangle(img, p0, p1, p2,
		Color(255, 255, 255), Color(255, 255, 255), Color(255, 255, 255));

	int count = 0;
	for (int y = 0; y < height; ++y) {
		for (int x = 0; x < width; ++x) {
			if (img.getPixel(x, y, 0) != 0) {
				++count;
			}
		}
	}

	const double expected = 50000.0;
	std::cout << "填充像素 = " << count
		<< "，理论面积 = " << expected
		<< "，比值 = " << static_cast<double>(count) / expected << "\n";
}

void testTriangleWinding(int width, int height) {
	Image a(width, height);
	Image b(width, height);

	const Vec2 p0(100.0, 50.0);
	const Vec2 p1(500.0, 150.0);
	const Vec2 p2(300.0, 350.0);

	const Color c0(255, 80, 80);
	const Color c1(80, 255, 120);
	const Color c2(80, 140, 255);

	drawTriangle(a, p0, p1, p2, c0, c1, c2);
	drawTriangle(b, p1, p0, p2, c1, c0, c2);   // 前两个顶点对调，绕序反过来

	int filled = 0;
	int diff = 0;

	for (int y = 0; y < height; ++y) {
		for (int x = 0; x < width; ++x) {
			// 只比"这一格被涂了没有"，不比颜色
			const bool inA = a.getPixel(x, y, 0) != 0 ||
				a.getPixel(x, y, 1) != 0 ||
				a.getPixel(x, y, 2) != 0;
			const bool inB = b.getPixel(x, y, 0) != 0 ||
				b.getPixel(x, y, 1) != 0 ||
				b.getPixel(x, y, 2) != 0;

			if (inA) {
				++filled;
			}
			if (inA != inB) {
				++diff;
			}
		}
	}

	const double ratio = (filled == 0) ? 0.0 : 100.0 * diff / filled;

	std::cout << "绕序一致性：差异 " << diff << " 像素（占填充的 "
		<< ratio << "%）"
		<< (ratio < 1.0 ? "  OK（边界浮点抖动）" : "  异常！")
		<< "\n";
}

void testTriangleDegenerate(int width, int height) {
	Image img(width, height);

	drawTriangle(img, Vec2(10.0, 10.0), Vec2(200.0, 10.0), Vec2(400.0, 10.0),
		Color(255, 0, 0), Color(0, 255, 0), Color(0, 0, 255));

	int count = 0;
	for (int y = 0; y < height; ++y) {
		for (int x = 0; x < width; ++x) {
			if (img.getPixel(x, y, 0) != 0) {
				++count;
			}
		}
	}

	std::cout << "退化三角形点亮像素 = " << count << "（应为 0）\n";
}
int main() {
	std::cout << "=== 画线算法验证 ===\n";
	dumpSmallLine();

	std::cout << "\n=== 两算法差分对比 ===\n";
	compareLineAlgorithms(650, 400);

	std::cout << "\n=== Mat3 矩阵验证 ===\n";
	testMat3();

	std::cout << "\n=== edge函数验证 ===\n";
	testEdge();

	std::cout << "\n=== 三角形：面积校验 ===\n";
	testTriangleArea(650, 400);

	std::cout << "\n=== 三角形：绕序一致性 ===\n";
	testTriangleWinding(650, 400);

	std::cout << "\n=== 三角形：退化情形 ===\n";
	testTriangleDegenerate(650, 400);;
	return 0;
}