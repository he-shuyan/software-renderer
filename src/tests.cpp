#include <iostream>

#include "Color.h"
#include "Mat3.h"
#include "Vec2.h"
#include "image.h"
#include "line.h"


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

int main() {
	std::cout << "=== 画线算法验证 ===\n";
	dumpSmallLine();

	std::cout << "\n=== 两算法差分对比 ===\n";
	compareLineAlgorithms(650, 400);

	std::cout << "\n=== Mat3 矩阵验证 ===\n";
	testMat3();

	return 0;
}