#include <iostream>

#include "Color.h"
#include "Mat3.h"
#include "Vec2.h"
#include "image.h"
#include "line.h"
#include "triangle.h"
#include "Vec3.h"
#include "model.h"
#include "Mat4.h"


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

void testVec3() {
	const Vec3 a(3.0, 4.0, 0.0);
	const Vec3 b(0.0, 3.0, 4.0);

	std::cout << "|a| = " << a.length() << "（应为 5）\n";
	std::cout << "|b| = " << b.length() << "（应为 5）\n";
	std::cout << "a·b = " << a.dot(b) << "（应为 12）\n";

	const Vec3 n = b.normalized();
	std::cout << "b 单位化 = (" << n.x << ", " << n.y << ", " << n.z << ")"
		<< "（应为 0, 0.6, 0.8）\n";
	std::cout << "|单位向量| = " << n.length() << "（应为 1）\n";

	const Vec3 cx = Vec3(1.0, 0.0, 0.0).cross(Vec3(0.0, 1.0, 0.0));
	const Vec3 cy = Vec3(0.0, 1.0, 0.0).cross(Vec3(1.0, 0.0, 0.0));

	std::cout << "x 叉乘 y = (" << cx.x << ", " << cx.y << ", " << cx.z << ")"
		<< "（应为 0, 0, 1）\n";
	std::cout << "y 叉乘 x = (" << cy.x << ", " << cy.y << ", " << cy.z << ")"
		<< "（应为 0, 0, -1）\n";
}

void testVec3Rotation() {
	const double halfPi = kPi / 2.0;

	const Vec3 rx = transformVector(Mat3::rotationX(halfPi), Vec3(0.0, 1.0, 0.0));
	std::cout << "绕 X 转 90 度，(0,1,0) -> (" << rx.x << ", " << rx.y << ", " << rx.z << ")"
		<< "  应为 (0, 0, 1)\n";

	const Vec3 ry = transformVector(Mat3::rotationY(halfPi), Vec3(1.0, 0.0, 0.0));
	std::cout << "绕 Y 转 90 度，(1,0,0) -> (" << ry.x << ", " << ry.y << ", " << ry.z << ")"
		<< "  应为 (0, 0, -1)\n";

	const Vec3 rz = transformVector(Mat3::rotationZ(halfPi), Vec3(1.0, 0.0, 0.0));
	std::cout << "绕 Z 转 90 度，(1,0,0) -> (" << rz.x << ", " << rz.y << ", " << rz.z << ")"
		<< "  应为 (0, 1, 0)\n";
}

void testLoadObj() {
	Model model;

	if (!loadObj("assets/cube.obj", model)) {
		std::cout << "加载失败（检查路径和当前工作目录）\n";
		return;
	}

	std::cout << "顶点数 = " << model.vertices.size() << "（应为 8）\n";
	std::cout << "三角形面数 = " << model.faces.size() << "（应为 12）\n";

	bool indexOk = true;
	const int vertexCount = static_cast<int>(model.vertices.size());

	for (const Face& face : model.faces) {
		if (face.a < 0 || face.a >= vertexCount ||
			face.b < 0 || face.b >= vertexCount ||
			face.c < 0 || face.c >= vertexCount) {
			indexOk = false;
		}
	}

	std::cout << "索引范围检查 = " << (indexOk ? "通过" : "失败") << "\n";

	std::cout << "第 1 个顶点 = (" << model.vertices[0].x << ", "
		<< model.vertices[0].y << ", " << model.vertices[0].z << ")"
		<< "（应为 -1, -1, -1）\n";
}

void testLoadObjCases() {
	Model model;

	const bool okCube = loadObj("assets/cube.obj", model);
	std::cout << "cube.obj        -> " << (okCube ? "成功" : "失败（错）")
		<< "，顶点 " << model.vertices.size()
		<< "，面 " << model.faces.size() << "（应为 8 / 12）\n";

	const bool okQuad = loadObj("assets/test_quad.obj", model);
	std::cout << "test_quad.obj   -> " << (okQuad ? "成功" : "失败（错）")
		<< "，顶点 " << model.vertices.size()
		<< "，面 " << model.faces.size() << "（应为 4 / 2）\n";

	const bool okBroken = loadObj("assets/test_broken.obj", model);
	std::cout << "test_broken.obj -> " << (okBroken ? "成功（错）" : "被拒绝（对）") << "\n";

	const bool okMissing = loadObj("assets/not_exist.obj", model);
	std::cout << "不存在的文件    -> " << (okMissing ? "成功（错）" : "被拒绝（对）") << "\n";
}

void testDepthBuffer() {
	DepthBuffer depth;
	depth.resize(4, 2);

	std::cout << "初始深度 = " << depth.data[1] << "（应为 1e+30）\n";

	const bool a = depth.testAndSet(1, 0, 5.0);
	const bool b = depth.testAndSet(1, 0, 9.0);
	const bool c = depth.testAndSet(1, 0, 2.0);
	const bool d = depth.testAndSet(1, 0, 7.0);

	std::cout << "写 5.0 -> " << (a ? "通过" : "被拒") << "（应通过）\n";
	std::cout << "写 9.0 -> " << (b ? "通过" : "被拒") << "（应被拒：更远）\n";
	std::cout << "写 2.0 -> " << (c ? "通过" : "被拒") << "（应通过：更近）\n";
	std::cout << "写 7.0 -> " << (d ? "通过" : "被拒") << "（应被拒：比 2.0 远）\n";
	std::cout << "最终深度 = " << depth.data[1] << "（应为 2）\n";
}

void testMat4Basics() {
	const Mat4 identity = Mat4::identity();

	std::cout << "单位矩阵对角线 = "
		<< identity.m[0][0] << " " << identity.m[1][1] << " "
		<< identity.m[2][2] << " " << identity.m[3][3] << "\n";

	std::cout << "单位矩阵 [0][3] = " << identity.m[0][3] << "（应为 0）\n";

	const Mat4 trans = Mat4::translation(3.0, 4.0, 5.0);

	std::cout << "平移矩阵第 3 列 = "
		<< trans.m[0][3] << " " << trans.m[1][3] << " " << trans.m[2][3] << "\n";

	

	const Vec4 p(1.0, 2.0, 3.0, 1.0);

	const Vec4 a = transform(identity, p);
	std::cout << "单位矩阵: (" << a.x << ", " << a.y << ", " << a.z << ", " << a.w << ")"
		<< "  应为 (1, 2, 3, 1)\n";

	const Vec4 b = transform(trans, p);
	std::cout << "平移(3,4,5): (" << b.x << ", " << b.y << ", " << b.z << ", " << b.w << ")"
		<< "  应为 (4, 6, 8, 1)\n";

	const Mat4 it = identity * trans;
	const Vec4 c = transform(it, p);
	std::cout << "I × T 作用于 p: (" << c.x << ", " << c.y << ", " << c.z << ", " << c.w << ")"
		<< "  应为 (4, 6, 8, 1)\n";

	const Vec4 homogeneous(2.0, 4.0, 6.0, 2.0);
	const Vec3 cart = toCartesian(homogeneous);
	std::cout << "齐次 (2,4,6,2) -> 笛卡尔: (" << cart.x << ", " << cart.y << ", " << cart.z << ")"
		<< "  应为 (1, 2, 3)\n";
}

void testMat4Rotation() {
	const double halfPi = kPi / 2.0;
	const Vec4 p(1.0, 0.0, 0.0, 1.0);

	const Vec4 rz = transform(Mat4::rotationZ(halfPi), p);
	std::cout << "绕 Z 转90度: (" << rz.x << ", " << rz.y << ", " << rz.z << ")"
		<< "  应为 (0, 1, 0)\n";

	const Vec4 ry = transform(Mat4::rotationY(halfPi), p);
	std::cout << "绕 Y 转90度: (" << ry.x << ", " << ry.y << ", " << ry.z << ")"
		<< "  应为 (0, 0, -1)\n";

	const Mat4 tr = Mat4::translation(3.0, 4.0, 5.0) * Mat4::rotationZ(halfPi);
	const Vec4 a = transform(tr, p);
	std::cout << "先转再平移: (" << a.x << ", " << a.y << ", " << a.z << ")"
		<< "  应为 (3, 5, 5)\n";

	const Mat4 rt = Mat4::rotationZ(halfPi) * Mat4::translation(3.0, 4.0, 5.0);
	const Vec4 b = transform(rt, p);
	std::cout << "先平移再转: (" << b.x << ", " << b.y << ", " << b.z << ")"
		<< "  应为 (-4, 4, 5)\n";
}

void testLookAt() {
	const Mat4 view = Mat4::lookAt(Vec3(0.0, 0.0, 5.0),
		Vec3(0.0, 0.0, 0.0),
		Vec3(0.0, 1.0, 0.0));

	const Vec4 o = transform(view, Vec4(0.0, 0.0, 0.0, 1.0));
	std::cout << "相机在(0,0,5)、看向原点：原点 -> (" << o.x << ", " << o.y << ", " << o.z << ")"
		<< "  应为 (0, 0, -5)\n";

	const Vec4 e = transform(view, Vec4(0.0, 0.0, 5.0, 1.0));
	std::cout << "相机自己的位置 -> (" << e.x << ", " << e.y << ", " << e.z << ")"
		<< "  应为 (0, 0, 0)\n";

	const Vec4 x2 = transform(view, Vec4(2.0, 0.0, 0.0, 1.0));
	std::cout << "(2,0,0) -> (" << x2.x << ", " << x2.y << ", " << x2.z << ")"
		<< "  应为 (2, 0, -5)\n";
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

	std::cout << "\n=== Vec3 三维向量验证 ===\n";
	testVec3();

	std::cout << "\n=== 三个旋转测试 ===\n";
	testVec3Rotation();

	std::cout << "\n=== 解析器验证 ===\n";
	testLoadObj();

	std::cout << "\n=== 解析器小测试 ===\n";
	testLoadObjCases();

	std::cout << "\n=== 深度检测测试 ===\n";
	testDepthBuffer();

	std::cout << "\n=== Mat4 矩阵验证 ===\n";
	testMat4Basics();
	
	std::cout << "\n=== Mat4 旋转验证 ===\n";
	testMat4Rotation();

	std::cout << "\n=== 相机检测 ===\n";
	testLookAt();

	return 0;
}
