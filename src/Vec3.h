//将2D变为3D
#pragma once

#include <cmath>

struct Vec3 {
	double x = 0.0;
	double y = 0.0;
	double z = 0.0;

	Vec3() = default;
	Vec3(double x_,double y_,double z_):x(x_),y(y_),z(z_){}

	Vec3 operator+(const Vec3& other)const {
		return Vec3(x + other.x, y + other.y, z + other.z);
	}

	Vec3 operator-(const Vec3& other)const {
		return Vec3(x - other.x, y - other.y, z - other.z);
	}

	double dot(const Vec3& other)const {
		return x * other.x + y * other.y + z * other.z;
	}

	double length()const {
		return std::sqrt(x * x + y * y + z * z);
	}

	//归一化 = 把向量缩放到长度为 1，方向不变，即求单位向量
	Vec3 normalized() const {
		const double len = length();
		if (len == 0.0) {
			return Vec3(0.0, 0.0, 0.0);
		}
		return Vec3(x / len, y / len, z / len);
	}

	//叉积  a × b得到新矩阵c,垂直于a b
	Vec3 cross(const Vec3& other) const {
		return Vec3(y * other.z - z * other.y,
			z * other.x - x * other.z,
			x * other.y - y * other.x);
	}
};