//对于三维坐标，Mat3只能旋转和缩放，无法平移，但是对于二维坐标可以
//Mat4为了让三维坐标平移，要先将三维坐标升为四位

//4 x 4矩阵

#include "Vec3.h"
#include "Vec4.h"

#include <cmath>
struct Mat4{
	double m[4][4] = {};

	static Mat4 identity();//单位矩阵
	static Mat4 translation(double tx, double ty, double tz);//平移
	static Mat4 scaling(double sx, double sy, double sz);

	//旋转
	static Mat4 rotationX(double radians);
	static Mat4 rotationY(double radians);
	static Mat4 rotationZ(double radians);

	//相机核心
	static Mat4 lookAt(const Vec3& eye, const Vec3& target, const Vec3& up);

	//透视投影
	static Mat4 perspective(double fovYRadians, double aspect, double nearZ, double farZ);

};

Mat4 operator*(const Mat4& a, const Mat4& b);

Vec4 transform(const Mat4& mat, const Vec4& v);