#include "image.h"

Image::Image(int width,int heigth)
	:width_(width),
	heigth_(heigth),
	data_(static_cast<std::uint8_t>(width)*heigth*3,0){}


//把二维坐标 + 通道号转成一维数组下标。
std::size_t Image::indexof(int x, int y, int chamel)const {
	return (static_cast<std::size_t>(y) * width_ + x) * 3 + chamel;
}

void Image::setPixel(int x, int y, std::uint8_t r, std::uint8_t g, std::uint8_t b) {
	data_[indexof(x, y, 0)] = r;
	data_[indexof(x, y, 1)] = g;
	data_[indexof(x, y, 2)] = b;
}

int Image::getPixel(int x, int y, int chamel)const {
	return data_[indexof(x, y, chamel)];
}