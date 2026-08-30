#include "showHelp.hpp"
#include <iostream>

void showHelp()
{
	std::cout << "可用命令:\n";
	std::cout << "  fetch    - 获取题目（需要Competitive Companion浏览器扩展）\n";
	std::cout << "  help     - 显示此帮助信息\n";
	std::cout << "  exit     - 退出程序\n";
	std::cout << "\n";
}