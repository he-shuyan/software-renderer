//升级4D

#pragma once

#include "Vec3.h"

struct Vec4 {
	double x = 0.0;
	double y = 0.0;
	double z = 0.0;
	double w = 0.0;

	Vec4() = default;
	Vec4(double x_, double y_, double z_, double w_) : x(x_), y(y_), z(z_), w(w_){}

};

inline Vec3 toCartesian(const Vec4& v) {

	//w==0,说明是方向向量
	if (v.w == 0.0) {
		return Vec3(v.x, v.y, v.z);
	}
	return Vec3(v.x / v.w, v.y / v.w, v.z / v.w);
}