#include "line.h"

#include <cmath>

void drawLine(Image& image, const Vec2& from, const Vec2& to,
    std::uint8_t r, std::uint8_t g, std::uint8_t b) {

    //计算增量
    const double dx = to.x - from.x;
    const double dy = to.y - from.y;


    // 确定步数取最大值，确保每一步增量小于1
    const int steps = static_cast<int>(std::fabs(dx) > std::fabs(dy)) ? std::fabs(dx) : std::fabs(dy);

    if (steps == 0) {
        const int px = static_cast<int>(std::lround(from.x));
        const int py = static_cast<int>(std::lround(from.y));

        //如果起点和终点是同一个点，steps 会是 0，接下来 dx / steps 就是除以零，整个函数会算出 NaN 并可能在 lround 那里直接崩
        if (px >= 0 && px < image.width() && py >= 0 && py < image.height()) {
            image.setPixel(px, py, r, g, b);
        }

        return;
    }
    //计算每一步的增量
    const double xStep = dx / steps;
    const double yStep = dy / steps;


    //初始化当前位置
    double x = from.x;
    double y = from.y;

    /*第 0 次：(a.x, a.y)，起点。
第 steps 次：(b.x, b.y)，终点。
中间共 steps - 1 个点。
总共 steps + 1 个像素。*/
    for (int i = 0; i <= steps; ++i) {
        const int px = static_cast<int>(std::lround(x));//std::lround：四舍五入到最近的整数，返回 long
        const int py = static_cast<int>(std::lround(y));

        //检查：意思是"x 在 0 到 width-1 之间，并且 y 在 0 到 height-1 之间"
        if (px >= 0 && px < image.width() && py >= 0 && py < image.height()) {
            image.setPixel(px, py, r, g, b);
        }
    

        //当前位置加上每一步的增量，准备画下一个像素。
        x += xStep;
        y += yStep;
    }
}