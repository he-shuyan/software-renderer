#pragma once

#include "Vec2.h"

struct Mat3 {
	/*             Mat3           Vec2
	[ x' ]   [ m00  m01  m02 ]   [ x ]
    [ y' ] = [ m10  m11  m12 ] * [ y ]
    [ 1  ]   [ 0    0    1   ]   [ 1 ]*/
	double m[3][3] = {};

	static Mat3 identity();//单位矩阵
	static Mat3 translation(double tx, double ty);//平移
	static Mat3 scaling(double sx, double sy);//缩放
	static Mat3 rotation(double radions);//旋转
};

Mat3 operator*(const Mat3& a, const Mat3& b);//矩阵乘法

Vec2 transformPoint(const Mat3& mat, const Vec2& p);//用矩阵变换一个点