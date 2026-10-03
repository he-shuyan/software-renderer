#include "line.h"

#include <cstdlib>//abs在其中，cmath的只管double和float
#include <cmath>

//DDA画线
void drawLineDDA(Image& image, const Vec2& from, const Vec2& to,
    const Color& c0, const Color& c1) {

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
            image.setPixel(px, py, c0.r, c0.g, c0.b);
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

        //加入两行，画一条固定颜色的线”升级成了“画一条颜色渐变的线
        const double t = static_cast<double>(i) / steps;
        const Color c = lerp(c0, c1, t);

        const int px = static_cast<int>(std::lround(x));//std::lround：四舍五入到最近的整数，返回 long
        const int py = static_cast<int>(std::lround(y));

        //检查：意思是"x 在 0 到 width-1 之间，并且 y 在 0 到 height-1 之间"
        if (px >= 0 && px < image.width() && py >= 0 && py < image.height()) {
            image.setPixel(px, py, c.r, c.g, c.b);
        }
    

        //当前位置加上每一步的增量，准备画下一个像素。
        x += xStep;
        y += yStep;
    }
}

//Bresenham画线
void drawLineBresenham(Image& image, const Vec2& from, const Vec2& to, const Color& c0, const Color& c1) {
    //起点可变，不加const
    int x0 = static_cast<int>(std::lround(from.x));
    int y0 = static_cast<int>(std::lround(from.y));
    const int x1 = static_cast<int>(std::lround(to.x));
    const int y1 = static_cast<int>(std::lround(to.x));

    //计算增量
    const int dx = std::abs(x1 - x0);
    const int dy = std::abs(y1 - y0);

    //确定前进方向
    const int sx = (x0 < x1) ? 1 : -1;
    const int sy = (y0 < y1) ? 1 : -1;

    //计算步数
    const int steps = std::max(dx, dy);

    //核心 err 表示当前点离理想直线的偏差,err<0，向x轴走
    int err = dx - dy;
    int i = 0;//t=0，该颜色不显，i=steps该颜色最显
    while (true) {
        const double t = (steps == 0) ? 0.0 : static_cast<double>(i) / steps;
        const Color c = lerp(c0, c1, t);

        if (x0 >= 0 && x0 < image.width() && y0 >= 0 && y0 < image.height()) {
            image.setPixel(x0, y0, c.r, c.g, c.b);
        }

        //到达终点结束循环
        if (x0 == x1 && y0 == y1) {
            break;
        }

        //计算两倍误差：因为原式里的判断要跟"一半"比，会引入小数；乘 2 之后全部变成整数比较。
        const int e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x0 += sx;
        }
        else if (e2 < dx) {
            err += dx;
            y0 += sy;
        }
        ++i;
    }
}