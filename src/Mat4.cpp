#include "Mat4.h"

Mat4 Mat4::identity() {
    Mat4 result;

    for (int i = 0; i < 4; ++i) {
        result.m[i][i] = 1.0;
    }

    return result;
}

Mat4 Mat4::translation(double tx, double ty, double tz) {
    Mat4 result = identity();

    result.m[0][3] = tx;
    result.m[1][3] = ty;
    result.m[2][3] = tz;

    return result;
}

//旋转
Mat4 Mat4::rotationX(double radians) {
    const double c = std::cos(radians);
    const double s = std::sin(radians);

    Mat4 result = identity();

    result.m[1][1] = c;
    result.m[1][2] = -s;
    result.m[2][1] = s;
    result.m[2][2] = c;

    return result;
}

Mat4 Mat4::rotationY(double radians) {
    const double c = std::cos(radians);
    const double s = std::sin(radians);

    Mat4 result = identity();

    result.m[0][0] = c;
    result.m[0][2] = s;
    result.m[2][0] = -s;
    result.m[2][2] = c;

    return result;
}

Mat4 Mat4::rotationZ(double radians) {
    const double c = std::cos(radians);
    const double s = std::sin(radians);

    Mat4 result = identity();

    result.m[0][0] = c;
    result.m[0][1] = -s;
    result.m[1][0] = s;
    result.m[1][1] = c;

    return result;
}

//eye：相机位置 target：相机看向的点  up：相机的上方向
Mat4 Mat4::lookAt(const Vec3& eye, const Vec3& target, const Vec3& up) {
    //相机有三个方向：前、右、上。它们必须互相垂直，组成一个正交基。
    
    //前方向
    const Vec3 f = (target - eye).normalized();
    const Vec3 r = f.cross(up).normalized();
    const Vec3 u = r.cross(f);

    Mat4 result = identity();

    result.m[0][0] = r.x;
    result.m[0][1] = r.y;
    result.m[0][2] = r.z;

    result.m[1][0] = u.x;
    result.m[1][1] = u.y;
    result.m[1][2] = u.z;

    result.m[2][0] = -f.x;
    result.m[2][1] = -f.y;
    result.m[2][2] = -f.z;

    result.m[0][3] = -r.dot(eye);
    result.m[1][3] = -u.dot(eye);
    result.m[2][3] = f.dot(eye);

    return result;

}

Mat4  operator*(const Mat4& a, const Mat4& b) {
    Mat4 result;

    for (int row = 0; row < 4; ++row) {
        for (int col = 0; col < 4; ++col) {
            double sum = 0;
            for (int k = 0; k < 4; ++k) {
                sum += a.m[row][k] * b.m[k][col];
            }
            result.m[row][col] = sum;
        }
    }

    return result;
}


Vec4 transform(const Mat4& mat, const Vec4& v) {
    return Vec4(mat.m[0][0] * v.x + mat.m[0][1] * v.y + mat.m[0][2] * v.z + mat.m[0][3] * v.w,
        mat.m[1][0] * v.x + mat.m[1][1] * v.y + mat.m[1][2] * v.z + mat.m[1][3] * v.w,
        mat.m[2][0] * v.x + mat.m[2][1] * v.y + mat.m[2][2] * v.z + mat.m[2][3] * v.w,
        mat.m[3][0] * v.x + mat.m[3][1] * v.y + mat.m[3][2] * v.z + mat.m[3][3] * v.w);
}