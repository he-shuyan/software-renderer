#include <cstdint>//引入 uint8_t 这类"固定宽度整数"的定义,写死颜色分量字节数为1
#include <iostream>
#include <cmath>
//#include <vector>

#include "Vec2.h"
#include "image.h"
#include "line.h"
#include "Color.h"
#include "Mat3.h"
#include "triangle.h"
#include "model.h"
#include "depth.h"



void drawBoxes(Image& image) {
	const Vec2 canvasCenter(325.0, 200.0);
	const int count = 8;
	const double radius = 120.0;//半径
	const double halfSize = 40.0;//方框半边

	// 局部坐标：方框的四角，中心在原点
	const Vec2 local[4] = {
		Vec2(-halfSize, -halfSize),
		Vec2(halfSize, -halfSize),
		Vec2(halfSize,  halfSize),
		Vec2(-halfSize,  halfSize),
	};

	const Color edge0(255, 170, 70);//橙色
	const Color edge1(90, 200, 255);//蓝色

	//画八个方框
	for (int i = 0; i < count; ++i) {
		//theta 是当前方框在圆周上的角度，均匀分布在[0, 2π)
		const double theta = 2.0 * kPi * i / count;

		// 在圆周上算出这个方框的位置
		const Vec2 center(canvasCenter.x + radius * std::cos(theta),
			canvasCenter.y + radius * std::sin(theta));

		// 先绕自己的中心旋转，再搬到圆周上的位置
		const Mat3 model = Mat3::translation(center.x, center.y)
			* Mat3::rotation(theta);

		//把四个变成世界坐标，world[j] 就是方框在画布上的实际坐标。
		Vec2 world[4];
		for (int j = 0; j < 4; ++j) {
			world[j] = transformPoint(model, local[j]);
		}

		for (int j = 0; j < 4; ++j) {
			drawLineBresenham(image, world[j], world[(j + 1) % 4], edge0, edge1);
		}
	}
}

//生成色轮
/*
| 角度  | r    | g    | b    | 看上去 |
| ---   | ---  | ---  | ---  | ---    |
| 0°   | 1.0  | 0.25 | 0.25 | 偏红   |
| 120° | 0.25 | 1.0  | 0.25 | 偏绿   |
| 240° | 0.25 | 0.25 | 1.0  | 偏蓝   |*/
static Color hueColor(double angle) {
	const double r = 0.5 + 0.5 * std::cos(angle);
	const double g = 0.5 + 0.5 * std::cos(angle - 2.0 * kPi / 3.0);
	const double b = 0.5 + 0.5 * std::cos(angle + 2.0 * kPi / 3.0);
	return Color(static_cast<std::uint8_t>(std::lround(255.0 * r)),
		static_cast<std::uint8_t>(std::lround(255.0 * g)),
		static_cast<std::uint8_t>(std::lround(255.0 * b)));
}

//把法线映射成颜色
static Color normalColor(const Vec3& n) {
	return Color(static_cast<std::uint8_t>(std::lround(255.0 * (0.5 + 0.5 * n.x))),
		static_cast<std::uint8_t>(std::lround(255.0 * (0.5 + 0.5 * n.y))),
		static_cast<std::uint8_t>(std::lround(255.0 * (0.5 + 0.5 * n.z))));
}

//画圆盘
void drawColorWheel(Image& image) {
	const Vec2 center(325.0, 200.0);
	const int slices = 12;//扇形个数
	const double radius = 150.0;//色环半径

	//全局旋转15°
	const double wheelAngle = 15.0 * kPi / 180.0;
	const Mat3 model = Mat3::translation(center.x, center.y)
		* Mat3::rotation(wheelAngle);//先平移（平移到画布中心）再旋转15°

	const Color centerColor(240, 240, 240);

	//画12个扇形
	for (int i = 0; i < slices; ++i) {
		const double a0 = 2.0 * kPi * i / slices;//起始度数
		const double a1 = 2.0 * kPi * (i + 1) / slices;//结束度数

		//将极坐标转为二维坐标
		//x = r * cos(θ)
		//y = r * sin(θ)
		const Vec2 local0(0.0, 0.0);                                    // 圆心
		const Vec2 local1(radius * std::cos(a0), radius * std::sin(a0)); // 弧上起点
		const Vec2 local2(radius * std::cos(a1), radius * std::sin(a1)); // 弧上终点

		drawTriangle(image,
			transformPoint(model, local0),
			transformPoint(model, local1),
			transformPoint(model, local2),
			centerColor,
			hueColor(a0),
			hueColor(a1));
	}
}

//投影与线框渲染
//把三维顶点变成画布上的二维点，然后画线。
void drawModelWireframe(Image& image, const Model& model) {//wireframe线框模型
	if (model.vertices.empty() || model.faces.empty()) {
		return;
	}
	/*
	const Vec2 center(325.0, 200.0);
	const double scale = 90.0;//scale：缩放因子。3D 坐标通常是 -1.0 ~ 1.0 这种小范围，乘以 90 后变成像素大小。
	*/
	//旋转矩阵
	//旋转模型，产生立体感
	//绕Y轴转30°，绕X轴转-20°
	const Mat3 spin=Mat3::rotationY(30.0 * kPi / 180.0) * Mat3::rotationX(-20.0 * kPi / 180.0);

	// ---------- 第一遍：量出模型投影后的包围盒 ----------
	double minX = 0.0;
	double maxX = 0.0;
	double minY = 0.0;
	double maxY = 0.0;
	bool first = true;

	for (const Vec3& v : model.vertices) {
		const Vec3 r = transformVector(spin, v);

		const double px = r.x;
		const double py = -r.y;          // 画布 y 向下，这里先取反

		if (first) {
			minX = maxX = px;
			minY = maxY = py;
			first = false;
		}
		else {
			minX = std::min(minX, px);
			maxX = std::max(maxX, px);
			minY = std::min(minY, py);
			maxY = std::max(maxY, py);
		}
	}

	const double modelW = maxX - minX;
	const double modelH = maxY - minY;

	if (modelW <= 0.0 || modelH <= 0.0) {
		return;                          // 模型退化成一个点或一条线，没法摆
	}
	

	// ---------- 算缩放和位置 ----------
	const double margin = 0.9;           // 四周留 10% 空白

	// 等比缩放：取两个方向里较小的比例
	const double scale = std::min(image.width() * margin / modelW,
		image.height() * margin / modelH);

	//算模型中心和画布中心

	//modelCenterX / Y：模型投影后的几何中心
	const double modelCenterX = 0.5 * (minX + maxX);
	const double modelCenterY = 0.5 * (minY + maxY);

	//canvasCenterX / Y：画布的几何中心。
	const double canvasCenterX = image.width() * 0.5;
	const double canvasCenterY = image.height() * 0.5;

	const Color edge0(255, 180, 90);
	const Color edge1(90, 200, 255);

	// ---------- 第二遍：投影并连线 ----------
	//遍历模型每个三角形面
	for (const Face& face : model.faces) {
		const int idx[3] = { face.a,face.b,face.c };//idx[3]把这三个索引存到数组里，方便循环。

		Vec2 p[3];//用来存投影到2D后的三个点

		//3D → 2D 投影（核心步骤）
		for (int i = 0; i < 3; ++i) {
			const Vec3 world = transformVector(spin, model.vertices[idx[i]]);//旋转成3D坐坐标

			//p[i]：1.缩放 2.平移到画布中心 3.y轴翻转
			//p[i] = Vec2(center.x + world.x * scale, center.y - world.y * scale);//3D 模型空间：Y 轴向上（符合数学和 3D 软件的惯例）

			/*
			1. r.x - modelCenterX   → 把模型中心移到原点
            2. * scale              → 缩放到屏幕尺度
            3. + canvasCenterX      → 平移到画布中心*/
			p[i] = Vec2(canvasCenterX + (world.x - modelCenterX) * scale,canvasCenterY + (-world.y - modelCenterY) * scale);                                                                 //2D 画布空间：Y 轴向下（屏幕像素坐标）
			
		}

		//画三角形三条边
		for (int i = 0; i < 3; ++i) {
			drawLineBresenham(image, p[i], p[(i + 1) % 3], edge0, edge1);
		}
	}
	
}

//3D实心渲染
void drawModelDepth(Image& image, DepthBuffer& depth, const Model& model) {
	
	//空模型检查
	if (model.vertices.empty() || model.faces.empty()) {
		return;
	}

	//旋转矩阵
	//先绕 X 轴转 -20 度，再绕 Y 轴转 30 度
	const Mat3 spin= Mat3::rotationY(30.0 * kPi / 180.0)* Mat3::rotationX(-20.0 * kPi / 180.0);

	// ---------- 第一遍：求包围盒（x、y 和深度都要） ----------
	double minX = 0.0, maxX = 0.0;
	double minY = 0.0, maxY = 0.0;
	double minDist = 0.0, maxDist = 0.0;
	bool first = true;

	for (const Vec3& v : model.vertices) {
		const Vec3 r = transformVector(spin, v);

		const double px = r.x;
		const double py = -r.y;
		const double pd = -r.z;          // 深度：越小越近

		if (first) {
			minX = maxX = px;
			minY = maxY = py;
			minDist = maxDist = pd;
			first = false;
		}
		else {
			minX = std::min(minX, px);
			maxX = std::max(maxX, px);
			minY = std::min(minY, py);
			maxY = std::max(maxY, py);
			minDist = std::min(minDist, pd);
			maxDist = std::max(maxDist, pd);
		}
	}
	
	//计算模型尺寸和缩放(留10%空白）

	const double modelW = maxX - minX;
	const double modelH = maxY - minY;

	if (modelW <= 0.0 || modelH <= 0.0) {
		return;
	}
	const double margin = 0.9;
	const double scale = std::min(image.width() * margin / modelW,image.height() * margin / modelH);

	//模型中心和画布中心
	const double modelCenterX = 0.5 * (minX + maxX);
	const double modelCenterY = 0.5 * (minY + maxY);

	const double canvasCenterX = image.width() * 0.5;
	const double canvasCenterY = image.height() * 0.5;

	//深度范围和颜色映射
	const double depthSpan = maxDist - minDist;

	//近处暖亮，远处冷暗
	const Color nearColor(255, 200, 120);//暖橙色
	const Color farColor(40, 60, 140);//冷蓝色

	// ---------- 第二遍：投影并填充 ----------

	int frontFaces = 0;
	int culledFaces = 0;

	for (const Face& face : model.faces) {

		const int idx[3] = { face.a, face.b, face.c };

		// 先用世界坐标算出这个面的法线
		const Vec3 a = transformVector(spin, model.vertices[face.a]);
		const Vec3 b = transformVector(spin, model.vertices[face.b]);
		const Vec3 c = transformVector(spin, model.vertices[face.c]);

		const Vec3 n = (b - a).cross(c - a).normalized();

		if (n.z <= 0.0) {
			++culledFaces;
			continue;//背对观察者，跳过
		}

		++frontFaces;

		ScreenVertex v[3];

		const Color faceColor = normalColor(n);

		for (int i = 0; i < 3; ++i) {
			const Vec3 r = transformVector(spin, model.vertices[idx[i]]);

			//投影到屏幕
			/*
			1. r.x - modelCenterX   → 把模型中心移到原点
			2. * scale              → 缩放到屏幕尺度
			3. + canvasCenterX      → 平移到画布中心*/
			v[i].pos = Vec2(canvasCenterX + (r.x - modelCenterX) * scale,
				canvasCenterY + (-r.y - modelCenterY) * scale);

			//写入深度
			v[i].depth = -r.z;

			//整个面共用一个颜色，颜色由面的朝向决定
			v[i].color = faceColor;
			/*
			const double t = (depthSpan > 0.0)
				? (v[i].depth - minDist) / depthSpan//把深度归一化到 [0, 1]。
				: 0.0;

			v[i].color = lerp(nearColor, farColor, t);*/
		}

		drawTriangleDepth(image, depth, v[0], v[1], v[2]);
	}
	std::cout << "正面三角形 = " << frontFaces
		<< "，被剔除 = " << culledFaces << "（应为 6 / 6）\n";
}
int main() {
	//测试Vec2.h，可删除
/*
	const Vec2 a(3.0, 4.0);
	const Vec2 b(1.0, 2.0);
	std::cout << "a = (" << a.x << ", " << a.y << ")\n";
	std::cout << "b = (" << b.x << ", " << b.y << ")\n";
	std::cout << "a + b = (" << (a + b).x << ", " << (a + b).y << ")\n";
	std::cout << "a - b = (" << (a - b).x << ", " << (a - b).y << ")\n";
	std::cout << "a * 2 = (" << (a * 2).x << ", " << (a * 2).y << ")\n";
	std::cout << "a . b = " << a.dot(b) << "\n";
	std::cout << "|a|   = " << a.length() << "\n";

	const Vec2 n = a.normalized();
	std::cout << "a 的单位向量 = (" << n.x << ", " << n.y << ")\n";
	std::cout << "|单位向量| = " << n.length() << "\n";


	//测试Color.h，可删除
	const Color black(0, 0, 0);
	const Color white(255, 255, 255);
	const Color mid = lerp(black, white, 0.5);

	std::cout << "混合色 = (" << static_cast<int>(mid.r) << ", "
		<< static_cast<int>(mid.g) << ", "
		<< static_cast<int>(mid.b) << ")\n";
*/
	//1.1 建立空画布
	const int width = 650;//画布列数
	const int height = 400;//画布行数

	const int previewStep = 10;

	Image image (width, height);

	

/*
- 建一块连续内存。
- 第一个参数是元素个数：width * height * 3
- 第二个参数 0 表示每个字节都初始化为 0（黑色）
- std::uint8_t：意思是：一个只能表示 0 到 255 的整数类型，通常占 1 个字节。
u：unsigned，无符号
int：integer，整数
8：8 位
_t：type，表示这是一个类型名
*/
//	std::vector<std::uint8_t>canvas(static_cast<std::size_t>(width) * height * 3, 0);//static_cast<std::size_t>将int类型改为size_t类型
//	std::cout << "画布字节总数" << canvas.size() << "，应为" << width * height * 3 << std::endl;

	
	
    //1.2 学会往画布上写颜色、读回来
//就地定义一个函数
/*
auto 让编译器自己推断这个变量是什么类型
[&] 是"捕获列表"，意思是"我要用外面的 canvas 和 width，用引用的方式借用"。不写 [&]，函数里面就看不到这两个变量
定义完就能像普通函数一样调用：setPixel(0, 0, 255, 0, 0)
*/
/*	auto setPixel = [&](int x, int y, std::uint8_t r, std::uint8_t g, std::uint8_t b) {
		const std::size_t index = (static_cast<std::size_t>(y) * width + x) * 3;
		canvas[index] = r;
		canvas[index + 1] = g;
		canvas[index + 2] = b;
		//y * width + x：把二维坐标压成一个一维序号。第 y 行前面已经放满了 y 行，每行 width 个，所以先跳过 y * width 个；再加上这一行里的偏移 x
* 3：因为每个像素占 3 个字节，序号要乘 3 才是字节下标
	};
	auto getPixel = [&](int x, int y, int channel)->int {//-> int：显式指定返回类型是 int
		return canvas[(static_cast<std::size_t>(y) * width + x) * 3 + channel];//channel 是 0、1、2，分别代表 R、G、B
	};
*/

	/*脚手架：为了验证前边是否正确
	setPixel(0, 0, 255, 0, 0);
	std::cout << "(0,0) 现在是：" << getPixel(0, 0, 0) << ","
		<< getPixel(0, 0, 1) << ","
		<< getPixel(0, 0, 2) << std::endl;*/




	//1.3 逐行扫描，把整块画布填满
/*  for (int y = 0; y < height; ++y) {
		for (int x = 0; x < width; ++x) {
			const auto r = static_cast<std::uint8_t>(255 * x / (width - 1));//随列变化，红色亮度从0到255
			const auto g = static_cast<std::uint8_t>(255 * y / (height - 1));//随行变化，绿色亮度从0到255
			const auto b = static_cast<std::uint8_t>(128);//蓝色固定半亮，b取值不同，所要效果不一样
			image.setPixel(x, y, r, g, b);
		}
	}
	
	std::cout << "(0,0) 填完之后：" << getPixel(0, 0, 0) << ","
		<< getPixel(0, 0, 1) << ","
		<< getPixel(0, 0, 2) << std::endl;
	*/
	// 背景：整块填成深灰蓝
	for (int y = 0; y < height; ++y) {
		for (int x = 0; x < width; ++x) {
			image.setPixel(x, y, 20, 20, 30);
		}
	}
    /*
	drawLineBresenham(image, Vec2(50.0, 50.0), Vec2(600.0, 50.0), Color(255, 80, 80), Color(0, 255, 80));
	drawLineBresenham(image, Vec2(50.0, 60.0), Vec2(600.0, 360.0), Color(80, 255, 120), Color(120, 255, 80));
	drawLineBresenham(image, Vec2(320.0, 30.0), Vec2(320.0, 370.0), Color(120, 160, 255), Color(255, 230, 80));
	drawLineBresenham(image, Vec2(100.0, 20.0), Vec2(140.0, 380.0), Color(255, 230, 120), Color(160, 120, 80));
	

	drawBoxes(image);
	drawTriangle(image, Vec2(120.0, 80.0), Vec2(520.0, 140.0), Vec2(300.0, 360.0),
		Color(255, 80, 80), Color(80, 255, 120), Color(80, 140, 255));*/

	/*画色轮
	drawColorWheel(image);
	*/

	//模型加载与渲染
	Model model;

	if (!loadObj("assets/cube.obj", model)) {
		std::cout << "模型加载失败\n";
		return 1;
	}

	//drawModelWireframe(image, model);
	DepthBuffer depth;
	depth.resize(width, height);

	drawModelDepth(image, depth, model);
	//1.4 把整块画布打印出来
	/*数字打印用于验证
	for (int y = 0; y < height; ++y) {
		for (int x = 0; x < width; ++x) {
			std::cout << "(" << getPixel(x, y, 0) << ","
				<< getPixel(x, y, 1) << ","
				<< getPixel(x, y, 2) << ") ";
		}
		std::cout << std::endl;
	}*/
	const char* ramp = " .:-=+*#%@";
	//用符号表示亮度：第一个字符是空格（最暗的像素显示成空白），最后是 @（最亮的）。中间那串符号是美术上的惯例，密度递增，看起来就像渐变的灰度。
	for (int y = 0; y < height; y+=previewStep) {
		for (int x = 0; x < width; x+=previewStep) {
			const int brightness = (image.getPixel(x, y, 0) + image.getPixel(x, y, 1) + image.getPixel(x, y, 2)) / 3;//把 R、G、B 加起来除以 3，得到一个 0~255 的"亮度"。
			const int level = brightness * 9 / 255;
		std:: cout << ramp[level];
		}
		std::cout << std::endl;
	}
	if (image.saveBMP("out.bmp")) {
		std::cout << "已写出 out.bmp\n";
	}
	else {
		std::cout << "写文件失败\n";
	}
	return 0;
}