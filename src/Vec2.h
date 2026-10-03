//RGB坐标

#pragma once
#include <cmath>


//定义一个二维向量结构体
struct Vec2
{
	double x = 0.0;
	double y = 0.0;

	Vec2() = default;//= default 告诉编译器生成一个默认构造函数
	Vec2(double x_,double y_):x(x_),y(y_){}

	//重载运算符，为画图使用
	Vec2 operator+(const Vec2& other)const {
		return Vec2(x + other.x, y + other.y);
	}

	Vec2 operator-(const Vec2& other)const {
		return Vec2(x - other.x, y - other.y);
	}

	Vec2 operator*(double k)const {
		return Vec2(x * k, y * k);
	}


	//向量点乘法
	double dot(const Vec2& other)const {
		return x * other.x + y * other.y;
	}

	//欧几里得长度|v| = √(x² + y²)即勾股定理
	double length()const {
		return std::sqrt(x * x + y * y);
	}

	//归一化 = 把向量缩放到长度为 1，方向不变，即求单位向量
	Vec2 normalized()const {
		const double len = length();
		if (len == 0.0) {
			return Vec2(0.0, 0.0);
		}
		return Vec2(x / len, y / len);
	}
};