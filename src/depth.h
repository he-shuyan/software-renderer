#pragma once

#include <vector>

//深度缓冲：每个像素记录“目前最近的深度”
//约定：depth越小表示越近

struct DepthBuffer {
	int width = 0;
	int height = 0;

	//data[y * width + x] = 深度,逻辑上是2D的，但用一维存储
	std::vector<double>data;//一维数组，长度为 width * height

	void resize(int w, int h);
	void clear();

	// 若这个深度比已记录的更近，则更新并返回 true；否则返回 false（被遮挡）
	bool testAndSet(int x, int y, double depth);
};