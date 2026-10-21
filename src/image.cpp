//一个 24 位 BMP 文件 = 54 字节的头 + 像素数据
/*
┌─────────────────────────────┐
│  BITMAPFILEHEADER(14 字节)  │  文件头
├─────────────────────────────┤
│  BITMAPINFOHEADER(40 字节)  │  信息头
├─────────────────────────────┤
│  像素数据(含每行填充)        │
└─────────────────────────────┘
*/
#include "image.h"
#include <cstdint>
#include <fstream>//提供写文件的 std::ofstream

/*
static 加在函数前，表示这个函数只属于本文件，别的 .cpp 看不见。
out.put(...) 往文件里塞一个字节。
v & 0xFF —— 0xFF 是二进制的 8 个 1，按位与就是"只要最低的 8 位"，其余丢掉。
v >> 8 —— 整体右移 8 位，把第二个字节挪到最低位，再 & 0xFF 取出来。
先取低位、后取高位，这就是小端序。
static_cast<char>(...) —— put 要的是 char 类型，显式转换一下，不靠编译器猜。
*/

//同一段代码，在不同机器上生成的文件内容不同，所以为了可移植、可控，我们手动拆字节
static void writeU16(std::ofstream& out, uint16_t v) {
	//out.put 和 out.write 就是输出流的两个基础写入函数：一个写单字节，一个写多字节。
	out.put(static_cast<char>(v & 0xFF));//取低位
	out.put(static_cast<char>((v >> 8) & 0xFF));//取高位
}

static void writeU32(std::ofstream& out, uint32_t v) {
	out.put(static_cast<char>(v & 0xFF));//取最低 v & 0xFF	掩码，只保留最低 8 位，其他位清零
	out.put(static_cast<char>((v >> 8) & 0xFF));//取次低v >> 8	右移 8 位，把第 2 个字节移到最低 8 位
	out.put(static_cast<char>((v >> 16) & 0xFF));//取次高v >> 16	右移 16 位，把第 3 个字节移到最低 8 位
	out.put(static_cast<char>((v >> 24) & 0xFF));//取最高v >> 24	右移 24 位，把第 4 个字节移到最低 8 位
}
Image::Image(int width,int height)
	:width_(width),
	height_(height),
	data_(static_cast<std::size_t>(width)*height*3,0){}


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

//把内部 RGB 像素数据保存成 24 位 BMP 文件
bool Image::saveBMP(const std::string& path)const {
	//打开文件
	std::ofstream out(path, std::ios::binary);//std::ios::binary：二进制模式，防止 Windows 把 \n 转成 \r\n。
	if (!out) {
		return false;
	}

	//计算每行字节数与填充
	const int rowBytes = width_ * 3;//每行像素占的字节数：width_ 个像素，每像素 3 字节（R、G、B）
	const int padding = (4 - (rowBytes % 4)) % 4;//BMP 规定：每行像素数据的字节数必须是 4 的倍数。不够用填充字节补齐
	const int stride = rowBytes + padding;//每行实际占用的字节数（含填充），一定是 4 的倍数
	const uint32_t pixelBytes = static_cast<uint32_t>(stride) * static_cast<uint32_t>(height_);//所有像素数据的总字节数 = 每行字节数 × 行数。


	//写文件头
	/* 
	| 偏移 | 字节数 | 写什么      | 含义 |
	| -- - | -- -   | -- -        | -- - |
	| 0    | 2      | `'B'` `'M'` | 魔数，BMP 的身份证 |
	| 2    | 4      | 54 + 像素字节数 | 整个文件多大 |
	| 6    | 2      | 0           | 保留，必须 0 |
	| 8    | 2      | 0           | 保留，必须 0 |
	| 10   | 4      | 54          | 像素数据从第 54 字节开始 |
	*/
	//BMP文件必须以B M两个字符开头
	out.put('B');
	out.put('M');

	writeU32(out, 54 + pixelBytes);//文件总大小：文件总字节数 = 文件头（14 字节）+ 信息头（40 字节）+ 像素数据
	writeU16(out, 0);
	writeU16(out, 0);//和上方一起，保留两个字段BMP规范填0
	writeU32(out, 54);//跳过文件头和信息头


	//些信息头
	/*
	| 偏移 | 字节数 | 写什么 | 含义 |
    | --- | --- | --- | --- |
    | 14 | 4 | 40 | 我这个结构自己占 40 字节 |
    | 18 | 4 | 宽 | 64 |
    | 22 | 4 | 高 | 32 |
    | 26 | 2 | 1 | 颜色平面数，永远是 1 |
    | 28 | 2 | 24 | 每像素 24 位 = 3 字节 |
    | 30 | 4 | 0 | 不压缩 |
    | 34 | 4 | 像素字节数 | 6144 |
    | 38 | 4 | 2835 | 水平分辨率（≈72 DPI） |
    | 42 | 4 | 2835 | 垂直分辨率 |
    | 46 | 4 | 0 | 调色板颜色数，24 位图不用 |
    | 50 | 4 | 0 | 重要颜色数，不用 |
	*/
	writeU32(out, 40);//BITMAPINFOHEADER 结构体大小是 40 字节。
	writeU32(out, static_cast<uint32_t>(width_));//图像宽度
	writeU32(out, static_cast<uint32_t>(height_));//图像高度
	writeU16(out, 1);//颜色平面数，固定为1
	writeU16(out, 24);//每像素位数，24 位 = 每像素 3 字节，正好是 R、G、B。
	writeU32(out, 0);//压缩方式，0表示不压缩
	writeU32(out, pixelBytes);//像素数据总字节数，含填充。
	writeU32(out, 2835);//水平分辨率
	writeU32(out, 2835);//垂直分辨率
	writeU32(out, 0);//调色板颜色数，24 位 BMP 不需要调色板，填 0。
	writeU32(out, 0);//重要颜色数，0表示全部重要

	//写像素数据
	for (int y = height_-1; y >=0; --y) {
		for (int x = 0; x < width_; ++x) {
			const int i = (y * width_ + x) * 3;
			out.put(static_cast<char>(data_[i + 2]));
			out.put(static_cast<char>(data_[i + 1]));
			out.put(static_cast<char>(data_[i + 0]));
		}
		//每行像素数据写完后，补 padding 个 0，让整行长度是 4 的倍数。
		for (int p = 0; p < padding; ++p) {
			out.put(0);
		}
	}
	out.close();//关闭文件
	return out.good();//如果流状态正常（没有错误），返回 true。
}

//这两个函数就是用来把 4 个字节或 2 个字节重新拼成一个整数。

//把四个字节分别左移 0, 8, 16, 24 位拼成 32 位整数。
static int readInt32(const unsigned char* p) {
	return p[0] | (p[1] << 8) | (p[2] << 16) | (p[3] << 24);
}

//p[0] 是低位，p[1] 左移 8 位变成高位，做按位或（|）拼起来。
static int readInt16(const unsigned char* p) {
	return p[0] | (p[1] << 8);
}

//文件操作
bool Image::loadBMP(const std::string& path, Image& out) {

	//以二进制模式打开文件
	std::ifstream in(path, std::ios::binary);
	if (!in) {
		return false;
	}

	//读取文件头
	unsigned char header[54] = {};//声明一个 54 字节的字节数组，用来存放 BMP 文件头信息
	in.read(reinterpret_cast<char*>(header), 54);

	//判断合法BMP文件
	if (header[0] != 'B' || header[1] != 'M') {
		return false;
	}

	//从刚读进来的 54 字节文件头里，把四个关键字段取出来。
	const int width = readInt32(header + 18);//读取图像宽
	const int height = readInt32(header + 22);//读取图像高
	const int bitCount = readInt16(header + 28);//每像素位数
	const int dataOffset = readInt32(header + 10);//素数据开始的文件偏移量


	if (width <= 0 || height <= 0 || bitCount != 24) {
		return false;
	}

	//BMP 规定，每一行像素数据的字节数，必须是 4 的倍数
	const int rowBytes = width * 3;//每一行像素字节数
	const int padding = (4 - (rowBytes % 4)) % 4;//补齐字节数

	in.seekg(dataOffset, std::ios::beg);//seekg 把文件读取指针直接跳到像素数据开始的地方beg:begin
	out = Image(width, height);

	//读取像素，处理上下翻转与BGR转换
	for (int fileRow = 0; fileRow < height; ++fileRow) {

		//BMP 文件是从下到上存储像素的，但是Image类约定y=0是顶部（y轴正方向朝下）
		//所以用 height - 1 - fileRow 把行号倒过来。
		const int y = height - 1 - fileRow;

		for (int x = 0; x < width; ++x) {
			//BMP 的第三个大坑：每个像素在文件里的排列顺序是 B（蓝）、G（绿）、R（红）
			//但是Image却是R，G，B
			unsigned char bgr[3] = {};
			in.read(reinterpret_cast<char*>(bgr), 3);
			out.setPixel(x, y, bgr[2], bgr[1], bgr[0]);
		}
		in.seekg(padding, std::ios::cur);//cur=current当前基准
	}
	return true;
}