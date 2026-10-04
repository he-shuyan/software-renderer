#include "Mat3.h"

/* 完成初始化，平移，缩放
                  Mat3           Vec2
	[ x' ]   [ sx   0    tx  ]   [ x ]
	[ y' ] = [ 0    sy   ty  ] * [ y ]
	[ 1  ]   [ 0    0    1   ]   [ 1 ]
	x'=sx*x+tx y'=sy*y+ty
*/


//初始化单位矩阵
/*
[ 1  0  0 ]
[ 0  1  0 ]
[ 0  0  1 ]
*/
Mat3 Mat3::identity() {
	Mat3 result;

	result.m[0][0] = 1.0;
	result.m[1][1] = 1.0;
	result.m[2][2] = 1.0;

	return result;
}


//平移

Mat3 Mat3::translation(double tx, double ty) {
	Mat3 result = identity();
	result.m[0][2] = tx;
	result.m[1][2] = ty;

	return result;
}

//缩放 
Mat3 Mat3::scaling(double sx, double sy) {
	//sx和sy分别是x y缩放倍数
	Mat3 result ;
	result.m[0][0] = sx;
	result.m[1][1] = sy;
	result.m[2][2] = 1.0;

	return result;
}

//旋转
/*
[ cos  -sin   0 ]
[ sin   cos   0 ]
[  0     0    1 ]
x'= r·cos(φ+θ) = r·cosφ·cosθ − r·sinφ·sinθ = x·cosθ − y·sinθ
y' = r·sin(φ+θ) = r·cosφ·sinθ + r·sinφ·cosθ = x·sinθ + y·cosθ
*/
Mat3 Mat3::rotation(double radions) {
	const double c = std::cos(radions);
	const double s = std::sin(radions);

	Mat3 result = identity();

	result.m[0][0] = c;
	result.m[0][1] = -s;
	result.m[1][0] = s;
	result.m[1][1] = c;

	return result;
}

//两个矩阵相乘
Mat3 operator*(const Mat3& a, const Mat3& b) {
	Mat3 result;

	for (int row = 0; row < 3; ++row) {
		for (int col = 0; col < 3; ++col) {

			double sum = 0.0;

			for (int k = 0; k < 3; ++k) {
				sum += a.m[row][k] * b.m[k][col];
			}

			result.m[row][col] = sum;
		}
	}
	return result;
}

/*变换
1. 把 2D 点 (x, y) 升维成 (x, y, 1)          ← 加一个 1
2. 用 3x3 矩阵乘以 (x, y, 1)，得到 (x', y', w')  ← 矩阵乘法
3. 除以 w'，得到 (x'/w', y'/w')                ← 透视除法
4. 丢掉第三维，得到最终的 2D 点 (x, y)          ← 降回 2D
*/
Vec2 transformPoint(const Mat3& mat, const Vec2& p) {
	const double x = mat.m[0][0] * p.x + mat.m[0][1] * p.y + mat.m[0][2];
	const double y = mat.m[1][0] * p.x + mat.m[1][1] * p.y + mat.m[1][2];
	const double w = mat.m[2][0] * p.x + mat.m[2][1] * p.y + mat.m[2][2];

	//防止除数为0
	if (w == 0.0) {
		return Vec2(0.0, 0.0);
	}
	//透视除法：除以 w，把齐次坐标变回普通坐标
	return Vec2(x / w, y/w);
}