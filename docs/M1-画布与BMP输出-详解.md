# M1 详解 · 从内存字节到一张图片

> 这是给自己看的版本。和提交进仓库的 [M1 · 画布与 BMP 输出](M1-画布与BMP输出.md) 是同一件事，
> 但把每个概念、每条命令、每次报错都拆开讲透。
> 配套：[M0 环境搭建详解](M0-环境搭建-详解.md)

## 怎么用这份文档

- 第一遍：按目录跳读，只看你当时卡住的那一段。
- 第二遍：把第七部分「代码对照」打开，对着自己的文件逐行看一遍。
- 第三遍：做第十部分的自测题，错哪补哪。

**别从头读到尾**——这份文档是字典，不是小说。

---

## 第一部分 · 这一节到底在干什么

一句话：**让渲染器有一块自己的画布，并且能把画布存成一张真正的图片。**

渲染器的本质是：在内存里维护一块矩形区域，每个格子记一个颜色。所有图形算法（画线、画三角形、贴纹理、光照）最终都归结为「往某些格子里写颜色」。最后把整块格子按某种文件格式写进磁盘，就成了图片。

所以这一节是整个项目的地基，之后所有内容都长在它上面。三个子问题：

1. 格子怎么编号、怎么找 → 内存布局与索引公式
2. 怎么快速看到结果 → 字符画预览
3. 怎么让 Windows 和浏览器认得 → BMP 文件格式

---

## 第二部分 · 画布与内存布局

### 2.1 一个像素 = 三个字节

每个颜色分量取值 0 ~ 255，正好用一个字节装下，所以用 `std::uint8_t`。

`std::uint8_t` 的意思是「恰好 8 位的无符号整数」。`unsigned char` 通常也是 8 位，但 `uint8_t` 把「8 位」这件事写死在类型名里，语义更明确，也不会因为换平台而变化。

### 2.2 行优先（row-major）

内存是一维的，图片是二维的，所以要把二维「摊平」成一维。我们的选择是**一行一行接着存**：

```
第 0 行的 0~649 号像素 → 第 1 行的 0~649 号像素 → 第 2 行 → ...
```

这叫行优先。另一种（列优先）在 Fortran、MATLAB 里常见，主流图像里几乎都是行优先。

### 2.3 索引公式怎么来的

要从 `(x, y)` 找到它在 vector 里的下标，分三步想：

1. 先跳过前面 `y` 整行：每行 `width` 个像素，每个像素 3 字节 → `y * width * 3`
2. 再跳过本行前面的 `x` 个像素 → `x * 3`
3. 最后加上通道号 0 / 1 / 2

合起来：

```
index(x, y, c) = (y * width + x) * 3 + c
```

**注意是 `y * width` 而不是 `x * height`。** 写反了就等于「按列摊平」，图会整体歪掉。这是初学者最常见的错误之一。

### 2.4 为什么用 `std::vector` 而不是数组

- 普通数组的长度必须是编译期常量，而画布宽高是我们运行时决定的。
- vector 自动管理内存：构造、析构、复制都不用你操心，不会泄漏。
- vector 保证元素在内存里**连续**，所以可以一次性把整块数据交给文件系统（这正是 `saveBMP` 高效的前提）。

---

## 第三部分 · `Image` 类的设计

### 3.1 为什么要封装成类

如果不封装，外界会直接拿到那个 `vector` 和宽高，然后**每个人按自己的理解去算下标**。一旦有人记错公式，就是随机花屏。封装之后：

- 下标公式只写一遍，只有一处可能出错；
- 以后想把 RGB 改成 RGBA、把 `uint8_t` 改成 `float`，只改类内部，调用方一行都不用动。

这就是「封装」的实际价值——**把「容易出错的细节」关进一个盒子里，只留几个不会出错的接口出来。**

### 3.2 `const` 的三种出现位置

```cpp
const int width = 650;                              // ① 局部常量
bool saveBMP(const std::string& path) const;        // ② 参数 + ③ 尾部
```

- **① 局部 const**：告诉编译器也告诉自己「这个值后面不会改」。
- **② 参数里的 `const std::string&`**：`&` 是引用，避免复制一份字符串；`const` 是承诺「我不改你传进来的东西」。
- **③ 函数尾部的 `const`**：承诺「这个成员函数不会修改对象自己」。好处有二：一是编译器帮你检查（真改了会报错），二是 `const Image` 对象也能调用它。

存文件只是「读数据 + 写文件」，不该改对象，所以尾部必须加 `const`。

### 3.3 构造函数与初始化列表

```cpp
Image::Image(int width, int height)
    : width_(width),
      height_(height),
      data_(static_cast<std::size_t>(width) * static_cast<std::size_t>(height) * 3, 0) {
}
```

- `: 成员(值), 成员(值)` 这一段叫**初始化列表**。它在函数体执行**之前**就把成员构造好，比在函数体里赋值更早、更正确（对 const 成员、或者没有默认构造函数的成员，是唯一可行的写法）。
- `width_(width)` 里，**括号外的 `width_` 是成员，括号里的 `width` 是参数**。名字长得像也没关系，编译器分得清。
- `data_(个数, 0)` 是 vector 的构造函数：开「个数」个元素，每个都填 0。因为我们用 `[]` 读写，所以必须**提前开好足够的大小**——vector 不会自己长大。

### 3.4 类型选择：这一节最贵的一课

出事的写法：

```cpp
static_cast<std::uint8_t>(width)      // 危险：8 位，最大只能放 255
static_cast<std::size_t>(width)       // 正确
```

把 650 塞进 8 位整数会发生什么：

```
650 = 2 × 256 + 138      → 低 8 位是 138，超出的部分被丢掉（截断）
data_ = 138 × 400 × 3    = 165,600 字节        ← 实际只开了这么多
最远的访问 = (399 × 650 + 649) × 3 + 2 = 779,999   ← 想用到这么远
```

差了约 4.7 倍，于是越界写，段错误。

**为什么以前（64 × 32）没炸？** 因为 64 和 32 都小于 256，截断不改变数值，算式碰巧是对的。也就是说：这个 bug 从写下那一行代码起就存在，只是潜伏着，直到画布变大才现形。

结论（以后每次声明变量都用这三问）：

1. 这个值**最大**能到多少？
2. 哪种类型**装得下**它，又不会浪费？
3. 参与运算时，会不会**溢出**？

具体到本项目：颜色分量最大 255 → `uint8_t`；尺寸、下标、字节数可能很大 → `std::size_t`。

---

## 第四部分 · 字符画预览

### 4.1 亮度

```cpp
const int brightness = (r + g + b) / 3;
```

三个通道求平均，得到 0 ~ 255 的「亮度」。这是最朴素的算法——人眼对绿色最敏感、对蓝色最不敏感，工业界更常用 `0.299R + 0.587G + 0.114B`。这里为了简单不较真。

因为我们的 b 固定是 128，所以画布里最暗的像素亮度也不是 0，而是约 42。

### 4.2 映射到符号

```cpp
const char* ramp = " .:-=+*#%@";
const int level = brightness * 9 / 255;
```

`ramp` 有 10 个字符（下标 0 ~ 9），从「空格」到「@」密度递增。`brightness * 9 / 255` 把 0 ~ 255 压到 0 ~ 9：最暗显示成空格，最亮显示成 `@`。

注意这里也是**先乘后除**。

### 4.3 降采样（downsampling）

650 × 400 如果每个像素都打印，就是 400 行、每行 650 字符——刷屏，而且什么信息都看不出来。每 10 个像素取一个，变成 65 × 40，刚好一屏，明暗走向一目了然。

「降采样」这个词在图形学里到处都会出现：纹理缩小、生成 mipmap、抗锯齿、缩小窗口……今天先认识名字和行为。

### 4.4 整数除法陷阱

```cpp
255 * x / (width - 1)      // ✓ 先乘后除
255 / (width - 1) * x      // ✗ 先除后乘：255 / 649 = 0，整张图全黑
```

C++ 里两个整数相除，结果**直接截断**（不是四舍五入）。所以「先乘后除」是整数运算的一条铁律——乘完再除，精度损失最小。

（顺带：`x * 255` 在 x 很大时会溢出 int。650 宽完全安全；如果以后画布宽到十万级，就该写成 `255.0 * x / (width - 1)`。）

---

## 第五部分 · BMP 文件格式

### 5.1 为什么需要「格式」

图片软件不认识你的 vector，它只认字节。所以必须约定：**第几个字节到第几个字节表示什么**。这个约定就是文件格式。

BMP 是所有常见图片格式里最简单的：不压缩、结构固定、全部是明文。学它不是为了用它，是为了**第一次亲手把「内存里的数据结构」变成「磁盘上的字节流」**——这个动作叫序列化（serialize），你以后做网络传输、存档、纹理上传，做的都是同一件事。

### 5.2 文件头（14 字节）

见表（同仓库文档，略）。

写的时候注意：`54` 这个数字是「两个头加起来的大小」，也就是说像素数据从第 54 个字节开始。

### 5.3 信息头（40 字节）

见表（同仓库文档，略）。

### 5.4 小端序

所有多字节整数，**低字节在前**。

举例：文件总大小 780,854 = 0x000BEA36

```
在文件里的字节顺序：36 EA 0B 00
```

这就是为什么我们要自己写 `writeU16` / `writeU32`：把整数按位拆开、低位先写。

```cpp
static void writeU32(std::ofstream& out, uint32_t v) {
    out.put(static_cast<char>(v & 0xFF));           // 最低 8 位
    out.put(static_cast<char>((v >> 8) & 0xFF));    // 次低 8 位
    out.put(static_cast<char>((v >> 16) & 0xFF));
    out.put(static_cast<char>((v >> 24) & 0xFF));
}
```

`static` 加在函数前面 = 这个函数只属于本文件，别的 .cpp 看不见它。这两个是纯粹的内部小工具。

### 5.5 三条铁律

**① 通道顺序 B、G、R。** 没道理，是历史。PNG 用的才是 R、G、B（因为它出现得晚，可以重新选）。

**② 行序自下而上。** 文件里的第一个像素属于图片的**最后一行**。因为早期图形系统的坐标原点在左下角，BMP 沿用了这个约定。所以我们的循环必须从 `y = height_ - 1` 往下走到 `0`。

**③ 每行补齐到 4 字节边界。**

```
padding = (4 - (rowBytes % 4)) % 4
```

推导一下为什么是这个式子。假设每行原始字节数是 `rowBytes`：

| rowBytes % 4 | 需要补几个字节 | 4 - (rowBytes % 4) |
| --- | --- | --- |
| 0 | 0 | 4 ← 多了 |
| 1 | 3 | 3 |
| 2 | 2 | 2 |
| 3 | 1 | 1 |

「已经对齐」那一行会算出 4，所以最外面再套一个 `% 4`，把 4 变成 0。**这就是那个看着多余的 `% 4` 存在的全部理由。**

对 650 宽：`1950 % 4 = 2` → 补 2 个 0 → 一行 1952 字节。

---

## 第六部分 · 完整流程串一遍

1. 建画布：`Image image(650, 400)` → 开 780,000 字节，全部填 0
2. 填颜色：双重循环遍历每个像素，算 r / g / b，`setPixel`
3. 终端预览：降采样 + 亮度映射，打印 65 × 40 字符画
4. 存文件：
   1. 打开文件（二进制模式）
   2. 写 14 字节文件头
   3. 写 40 字节信息头
   4. `y` 从最后一行开始往上，每行 `x` 从左到右，每个像素写 B、G、R；行尾补 padding 个 0
   5. 关闭，返回是否成功

---

## 第七部分 · 代码对照

> 以你本地能跑通的版本为准；下面是 M1 完成时的形态。

### `src/image.h`

```cpp
#pragma once

#include <cstdint>
#include <string>
#include <vector>

class Image {
public:
    Image(int width, int height);

    int width() const;
    int height() const;

    void setPixel(int x, int y, std::uint8_t r, std::uint8_t g, std::uint8_t b);

    int getPixel(int x, int y, int channel) const;

    bool saveBMP(const std::string& path) const;

private:
    int indexOf(int x, int y) const;

    int width_;
    int height_;
    std::vector<std::uint8_t> data_;
};
```

### `src/image.cpp` 关键部分

```cpp
Image::Image(int width, int height)
    : width_(width),
      height_(height),
      data_(static_cast<std::size_t>(width) * static_cast<std::size_t>(height) * 3, 0) {
}

int Image::indexOf(int x, int y) const {
    return (y * width_ + x) * 3;
}
```

`saveBMP` 的骨架：

```cpp
bool Image::saveBMP(const std::string& path) const {
    std::ofstream out(path, std::ios::binary);
    if (!out) {
        return false;
    }

    const int rowBytes = width_ * 3;
    const int padding = (4 - (rowBytes % 4)) % 4;
    const int stride = rowBytes + padding;
    const uint32_t pixelBytes =
        static_cast<uint32_t>(stride) * static_cast<uint32_t>(height_);

    // 14 字节文件头
    out.put('B');
    out.put('M');
    writeU32(out, 54 + pixelBytes);
    writeU16(out, 0);
    writeU16(out, 0);
    writeU32(out, 54);

    // 40 字节信息头
    writeU32(out, 40);
    writeU32(out, static_cast<uint32_t>(width_));
    writeU32(out, static_cast<uint32_t>(height_));
    writeU16(out, 1);
    writeU16(out, 24);
    writeU32(out, 0);
    writeU32(out, pixelBytes);
    writeU32(out, 2835);
    writeU32(out, 2835);
    writeU32(out, 0);
    writeU32(out, 0);

    // 像素：自下而上，BGR，行尾补 padding
    for (int y = height_ - 1; y >= 0; --y) {
        for (int x = 0; x < width_; ++x) {
            const int i = (y * width_ + x) * 3;
            out.put(static_cast<char>(data_[i + 2]));
            out.put(static_cast<char>(data_[i + 1]));
            out.put(static_cast<char>(data_[i + 0]));
        }
        for (int p = 0; p < padding; ++p) {
            out.put(0);
        }
    }

    out.close();
    return out.good();
}
```

### `src/main.cpp` 的骨架

```cpp
const int width = 650;
const int height = 400;
const int previewStep = 10;

Image image(width, height);

// 填色：每个像素都要
for (int y = 0; y < height; ++y) {
    for (int x = 0; x < width; ++x) {
        const auto r = static_cast<std::uint8_t>(255 * x / (width - 1));
        const auto g = static_cast<std::uint8_t>(255 * y / (height - 1));
        const auto b = static_cast<std::uint8_t>(128);
        image.setPixel(x, y, r, g, b);
    }
}

// 预览：降采样
for (int y = 0; y < height; y += previewStep) {
    for (int x = 0; x < width; x += previewStep) {
        // 亮度 → ramp 下标 → 打印
    }
}
```

### `CMakeLists.txt`

```cmake
cmake_minimum_required(VERSION 3.16)
project(renderer CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(app
    src/main.cpp
    src/image.cpp
)
```

**注意最后那段**：只要新增了 .cpp 文件，就必须写进 `add_executable`，否则会出现 `undefined reference`（链接错误，不是语法错误）。

---

## 第八部分 · 命令手册

这一节用到的所有命令，按用途分类。**每条都记住「作用 + 你要看什么」就够了，不用背参数。**

### 构建

| 命令 | 作用 |
| --- | --- |
| `cmake -S . -B ~/build-renderer` | 配置：读当前目录（`-S`）的 CMakeLists，把构建系统生成到 `~/build-renderer`（`-B`）。只需做一次，改了 CMakeLists 才需要重做 |
| `cmake --build ~/build-renderer` | 编译：让构建系统把源码编成程序 |
| `touch src/main.cpp` | 把文件时间戳刷新成「现在」，用来强制触发重新编译 |

**看什么**：构建输出里有 `Building CXX object ...` 才说明真的重编了。只有 `[100%] Built target app` 就是没编。

**为什么构建目录放在 `~/build-renderer`**：共享文件夹（`/mnt/hgfs`）是 FUSE 文件系统，读写慢，编译产物扔在那里会很卡；而且编译产物本来就**不该进仓库**。

### 运行与查看

| 命令 | 作用 |
| --- | --- |
| `~/build-renderer/app` | 运行程序 |
| `ls -l out.bmp` | 看文件大小、时间 |
| `stat -c '%y  %n' 文件...` | 看详细时间戳（排查「为什么没重编」用） |
| `cat -n src/main.cpp` | 带行号看整个文件 |
| `grep -n "关键词" 文件` | 找出关键词出现在哪几行（**不认注释**！） |
| `sed -n '55,65p' src/main.cpp` | 只看第 55 ~ 65 行 |

### 编辑文件

| 命令 | 作用 |
| --- | --- |
| `nano .gitignore` | 打开 nano 编辑器 |

nano 里：`Ctrl + O` 保存（再按回车确认文件名），`Ctrl + X` 退出，`Ctrl + W` 搜索。底部那排 `^O` `^X` 就是提示，`^` 代表 Ctrl。

### Git

| 命令 | 作用 |
| --- | --- |
| `git status --short` | 看当前状态。左列=已暂存，右列=未暂存；`M` 改、`A` 新增、`D` 删除、`??` 未跟踪 |
| `git diff` | 看「相对上次提交改了哪些行」，`-` 是旧，`+` 是新 |
| `git diff --stat` | 只看每个文件改了几行（快速扫一眼用） |
| `git add 文件...` | 把改动放进「下次要提交的清单」 |
| `git commit -m "信息"` | 提交，信息前缀：`feat` 新功能 / `fix` 修 bug / `refactor` 重构 / `docs` 文档 / `chore` 杂项 |
| `git log --oneline --decorate -5` | 看最近 5 条提交（`--decorate` 会显示标签） |
| `git tag -a m1 -m "说明"` | 给当前提交打一个带说明的里程碑标签 |
| `git rm --cached 文件` | 把文件从版本库里摘出来，**磁盘上的文件保留** |
| `git check-ignore -v 文件` | 查是 `.gitignore` 的哪一行挡住了它（默认跳过已跟踪文件） |
| `git push origin m1` | 把标签 `m1` 推到远端（只能在 Windows 上执行，见下） |

**为什么 push 只能在 Windows 做**：主机和虚拟机都连不上 GitHub（21 秒超时），而 Windows 上的 GitHub Desktop 有加速器配合。所以约定是：**add / commit 在 Linux 做（纯本地），push 在 Windows 做。**

---

## 第九部分 · 三个坑的复盘

### 坑 1 · 段错误：`uint8_t` 截断

**现象**：程序一个字都没打印就 `段错误（核心已转储）`。

**推理链**：
1. 「一个字都没打印」→ 崩在打印**之前** → 位置在填色阶段或对象构造阶段
2. 填色是循环写内存，最可能越界
3. 越界说明「分配的容量」小于「使用的下标」
4. 分配容量由构造函数决定 → 检查那一行
5. 发现 `static_cast<std::uint8_t>(width)` → 650 变 138 → 容量只剩 1/4

**教训**：**能跑 ≠ 对**。原来 64 × 32 能跑，只是因为数值恰好小于 256。

**定位手法**：把可疑的值打印出来。

```cpp
std::cout << image.width() << " x " << image.height() << std::endl;
std::cout << image.dataSize() << std::endl;   // 需要临时加个接口，或直接在构造函数里打印
```

### 坑 2 · 图上下颠倒、红蓝互换

**现象**：图能打开，但绿色从上往下变暗（本该变亮），蓝色随 x 变亮（本该是红色）。

**推理链**：
1. 蓝色随 x 变亮 ⟹ 我们的 R 通道被读成了 B ⟹ **通道顺序写反了**
2. 绿色越往下越暗 ⟹ 行序被翻了 ⟹ **行序写反了**
3. 用四个角的颜色验算：内存 (0,0,128) → 显示 (128,255,0) 黄绿 ✓ 与截图一致

**教训**：**从现象反推内存内容**是调试图形程序的基本功。这个技能在后面的光照、纹理部分会一直用。

### 坑 3 · 改了代码但行为完全没变

**现象**：改完代码、`cmake --build` 只输出 `[100%] Built target app`，跑起来还是老行为；连 `touch` 都无效。

**根因**：make 判断「要不要重编」的唯一依据是**比时间戳**。源文件在共享文件夹里，时间戳由 Windows 主机提供；`.o` 在虚拟机本地磁盘，时间戳由虚拟机时钟提供。两套时钟不同步时，make 就会误判。（vmhgfs-fuse 对 `touch` 的响应也有已知怪癖。）

**解决办法**：删掉构建目录重来。

```bash
rm -rf ~/build-renderer
cmake -S /mnt/hgfs/code -B ~/build-renderer
cmake --build ~/build-renderer
```

**教训（值得背下来）**：

> **代码改了但没生效，一律先删构建目录重来。** 重建十几秒，比分析快得多。

### 附：工具的两个「你不知道它不知道」

- `grep` **不认识注释**——被 `//` 和 `/* */` 包起来的代码照样会被打印出来。
- `git check-ignore` **默认跳过已跟踪文件**——所以对已经提交过的文件不输出任何东西，看起来像「规则没生效」。想单纯测规则，加 `--no-index`。

**通用教训**：用工具得出的结论，要先想清楚「它到底看得见什么」。工具不会告诉你它的盲区。

---

## 第十部分 · 自测题

先自己写答案，再对照。

1. 800 × 600 的画布，`data_` 需要多少字节？
2. 上题中，像素 (10, 20) 的 R 分量在 vector 里的下标是多少？
3. 索引公式里为什么是 `y * width_` 而不是 `x * height_`？
4. 400 × 300 的 24 位 BMP：一行原始字节多少？padding 多少？stride 多少？像素数据多大？文件总共多大？
5. BMP 为什么是 BGR 不是 RGB？
6. 为什么 BMP 要自下而上存？
7. `static_cast<std::uint8_t>(650)` 等于多少？为什么？
8. 亮度 200，`ramp = " .:-=+*#%@"`，显示哪个字符？
9. `out.good()` 什么时候会是 false？
10. 打开文件时为什么必须加 `std::ios::binary`？
11. `.gitignore` 能挡住已经提交过的文件吗？
12. `git check-ignore -v out.bmp` 对已跟踪文件为什么没有输出？

### 参考答案

1. `800 × 600 × 3 = 1,440,000` 字节。
2. `(20 × 800 + 10) × 3 = 48,030`（R 就是 `+0`，所以不再加）。
3. 行优先：先跳过 y 个整行，每行 width 个像素。写反就变列优先，图会歪。
4. 一行 `400 × 3 = 1200`；`1200 % 4 = 0`，padding 0；stride 1200；像素 `1200 × 300 = 360,000`；文件 `360,000 + 54 = 360,054` 字节。
5. 历史原因：早期 Windows 图形接口内部就是这个顺序，规范冻结后所有软件都得跟随。PNG 是后来者，才用了 RGB。
6. 早期图形系统的坐标原点在左下角。想自上而下，可以把高度写成负数（进阶）。
7. `138`。650 = 2 × 256 + 138，8 位只留最低 8 位，高位被截断。
8. `level = 200 × 9 / 255 = 7` → `ramp[7] = '#'`。
9. 流进入错误状态时——例如写失败、磁盘满、文件被占用。
10. 避免文本模式下把 `\n` 自动翻译成 `\r\n`（Windows 上尤其明显），那样文件就坏了。
11. 不能。**忽略规则只对未跟踪文件生效。** 已经提交过的要先 `git rm --cached`。
12. 因为它默认跳过已跟踪文件，只对未跟踪文件求值。加 `--no-index` 就能看到规则。

---

## 第十一部分 · 扩展练习（有余力再做）

1. 给 `Image` 加一个 `clear(r, g, b)` 方法：一次把整块画布填成同一颜色。提示：循环调 `setPixel` 就行，先别想优化。
2. 把渐变改成**径向渐变**：亮度由「到中心的距离」决定（`dx = x - width/2`，`dy = y - height/2`，`d = sqrt(dx*dx + dy*dy)`，注意需要 `<cmath>`）。
3. 把画布尺寸做成命令行参数：`./app 800 600 out.bmp`，用 `argc` / `argv` 读。（这是走向「真正的工具」的第一步。）
4. 用负高度让 BMP 自上而下存（高写 `-height`），验证图和原来完全一样。提示：需要按有符号数写 `writeU32`。

---

## 附录 · 术语表

| 中文 | 英文 | 含义 |
| --- | --- | --- |
| 帧缓冲 | framebuffer | 存放整帧画面的内存区域 |
| 像素 | pixel | 画面上的最小单位 |
| 通道 | channel | 一个颜色分量（R / G / B） |
| 行优先 | row-major | 二维数据按「一行接一行」摊平成一维 |
| 步长 | stride | 一行的实际字节数（含填充） |
| 填充 | padding | 行尾为对齐而补的字节 |
| 小端序 | little-endian | 多字节整数低字节在前 |
| 序列化 | serialize | 把内存中的数据结构按约定格式写入字节流 |
| 降采样 | downsampling | 按间隔抽样，把图像变小 |
| 亮度 | luminance | 表示明暗程度的数值 |
| 魔数 | magic number | 文件开头的标识字节（如 BMP 的 `BM`） |
| 位深 | bit depth | 每个像素占多少位 |
| 段错误 | segmentation fault | 访问了不属于自己的内存 |
| 链接 | linking | 把多个目标文件合成可执行文件 |
| 里程碑标签 | tag | 给某个提交起的名字，如 `m1` |