#pragma once

#include <string>
#include <vector>

#include "Vec3.h"
#include "Vec2.h"

//三角形面
struct Face{
	//a, b, c 不是坐标，而是顶点数组的索引。
	//比如 a = 0, b = 2, c = 4 表示这个三角形由第 0、2、4 个顶点组成
	int a = 0;
	int b = 0;
	int c = 0;

	//新增UV索引，默认-1
	int ta = -1;
	int tb = -1;
	int tc = -1;
};

//整个3D模型
/*举例，一个立方体：
vertices: 8 个 Vec3
faces:    12 个 Face（12 个三角形）
*/
struct Model{
	std::vector<Vec3>vertices;//所有顶点
	std::vector<Vec2>uvs;// 新增：UV 数组（用 Vec2 存）
	std::vector<Face> faces;//所有三角形面

};

//文件路径
bool loadObj(const std::string& path, Model& model);