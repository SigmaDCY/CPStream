#include "commands.hpp"
#include "server.hpp"
#include "showHelp.hpp"
#include "ui.hpp"
#include <iostream>
#include <limits>
#include <stdexcept>
#include <string>

void cmds()
{
	while (true)
	{
		std::cout << "cps> ";
		std::string inst;
		std::getline(std::cin, inst);
		if (inst.empty())
		{
			continue;
		}
		else if (inst == "exit")
		{
			break;
		}
		else if (inst == "help")
		{
			showHelp();
		}
		else if (std::string(inst) == "fetch")
		{
			std::cout << "请输入监听端口号> ";
			int port;
			if (!(std::cin >> port)) {
				std::cin.clear();  // 清除错误状态
				std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');  // 丢弃错误输入
				ui::warning("请输入数字！\n");
				continue;
			}
			std::cin.ignore();
			try
			{
				fetch(port);
			}
			catch (const std::exception& e)
			{
				ui::error(e.what());
			}
		}
		else
		{
			ui::warning("未知命令！\n");
		}
	}
	return;
}