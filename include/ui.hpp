#pragma once
#include <iostream>
#include <string>

void welcome();

namespace ui {
	const std::string RESET = "\033[0m";
	const std::string CYAN = "\033[36m";
	const std::string GREEN = "\033[32m";
	const std::string RED = "\033[31m";
	const std::string YELLOW = "\033[33m";
	const std::string BOLD = "\033[1m";

	void welcome();

	// 通用输出函数
	inline void info(const std::string& msg)
	{
		std::cout << CYAN << msg << RESET << std::endl;
	}

	inline void success(const std::string& msg)
	{
		std::cout << GREEN << msg << RESET << std::endl;
	}

	inline void error(const std::string& msg)
	{
		std::cout << RED << msg << RESET << std::endl;
	}

	inline void warning(const std::string& msg)
	{
		std::cout << YELLOW << msg << RESET << std::endl;
	}
}