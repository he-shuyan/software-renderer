#include <cstdint>//引入 uint8_t 这类"固定宽度整数"的定义,写死颜色分量字节数为1
#include <iostream>
//#include <vector>

#include "Vec2.h"
#include "image.h"
int main() {

	//测试Vec2.h
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
	for (int y = 0; y < height; ++y) {
		for (int x = 0; x < width; ++x) {
			const auto r = static_cast<std::uint8_t>(255 * x / (width - 1));//随列变化，红色亮度从0到255
			const auto g = static_cast<std::uint8_t>(255 * y / (height - 1));//随行变化，绿色亮度从0到255
			const auto b = static_cast<std::uint8_t>(128);//蓝色固定半亮，b取值不同，所要效果不一样
			image.setPixel(x, y, r, g, b);
		}
	}
	/*
	std::cout << "(0,0) 填完之后：" << getPixel(0, 0, 0) << ","
		<< getPixel(0, 0, 1) << ","
		<< getPixel(0, 0, 2) << std::endl;
	*/




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