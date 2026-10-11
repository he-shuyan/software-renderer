#include "model.h"

#include <cstddef>//std::size_t
#include <fstream>//读文件
#include <sstream>//把字符串当流处理（解析每行）

bool loadObj(const std::string& path, Model& model) {//path:文件路径，例如：assets/cube.obj
	//打开文件
	std::ifstream in(path);
	if (!in) {
		return false;
	}

	//清空文件中旧的数据模型
	model.vertices.clear();
	model.faces.clear();

	//逐行解析文本文件
	std::string line;//用来存放每行读取
	while (std::getline(in, line)) {
		std::istringstream iss(line);//把刚刚读到的这一行 line 包装成一个字符串输入流 iss

		std::string tag;//准备存放行首第一个单词
		iss >> tag;// 取行首的标记（v / f / # / vn ...）

		//上方内容俗话：读取line，把line当成输入源，iss当cin用，存每行第一个单词到tag

		if (tag == "v") {//遇到v开头，说明是顶点
			double x = 0.0;
			double y = 0.0;
			double z = 0.0;

			iss >> x >> y >> z;
			model.vertices.push_back(Vec3(x, y, z));
		}
		/*f 1 2 3                # 只给顶点索引
          f 1/1 2/2 3/3          # 顶点索引/纹理索引
          f 1//1 2//2 3//3       # 顶点索引//法线索引
          f 1/1/1 2/2/2 3/3/3    # 顶点索引/纹理索引/法线索引*/
		else if (tag == "f") {//当tag==f,找第一个/,取/之前为顶点索引
			std::vector<int>indices;
			std::string token;
				
			while (iss>>token){
				//找到就返回/第一次出现的下标索引，没有就返回npos
				const std::size_t slash = token.find('/');


				//判断是否找到，找到就取/之前部分，否则就取全部
				const std::string first = (slash == std::string::npos) ? token : token.substr(0, slash);

				//- 1 是因为 OBJ 文件的索引从 1 开始，而 C++ 数组从 0 开始
				indices.push_back(std::stoi(first) - 1);// 必须用std::stoi 把字符串转成整数即'1'->1


			}

			//多边形三角化：以第0个顶点为中心做扇形拆分
			for (std::size_t i = 1; i + 1 < indices.size(); ++i) {
				model.faces.push_back(Face{ indices[0], indices[i], indices[i + 1] });
			}
		}
	}
	return true;
}