//将代码封装成一个类
#pragma once//个文件只允许被包含一次"

#include <vector>
#include <cstddef>// std::size_t基础尺寸、指针差、空指针等定义
#include <cstdint>//std::uint8_t固定宽度整数类型,避免 int、long 在不同平台长度不同的问题。
#include <string>

// 一张图片 = 一块连续内存里的像素数据。
// 约定：像素按行存储，每行从左到右，行从上到下（y = 0 在顶部）。
// 每个像素 3 个字节：R、G、B。
//定义一个类
class Image {
public:
	Image(int width, int height);//无返回值的构造函数

	int width()const { return width_; };
	int heigth()const { return height_; };

	void setPixel(int x, int y, std::uint8_t r, std::uint8_t g, std::uint8_t b);
	int getPixel(int x, int y, int chamel)const;

	bool saveBMP(const std::string& path)const;
private:
	std::size_t indexof(int x, int y, int chamel)const;//把 (x, y, channel) 转成 data_ 的下标
	int width_;
	int height_;
	std::vector<std::uint8_t> data_;
};