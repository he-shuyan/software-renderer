//深度缓冲


#include "depth.h"

#include <algorithm>

constexpr double kFarDepth = 1e30;//定义一个"无穷远"的深度值

void DepthBuffer::resize(int w, int h) {
	width = w;
	height = h;
	data.assign(static_cast<std::size_t>(w) * static_cast<std::size_t>(h), kFarDepth);
}

void DepthBuffer::clear() {
	std::fill(data.begin(), data.end(),kFarDepth);//利用fill填充
}

bool DepthBuffer::testAndSet(int x, int y, double depth) {

    //将二维坐标转为一维
    const std::size_t index = static_cast<std::size_t>(y) * static_cast<std::size_t>(width)
        + static_cast<std::size_t>(x);

    if (depth < data[index]) {//data[index] 是当前像素已记录的最浅深度
        data[index] = depth;
        return true;
    }

    return false;
}