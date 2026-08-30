#include "httplib.h"
#include "json.hpp"
#include "problem.hpp"
#include "server.hpp"
#include "ui.hpp"
#include <csignal>
using json = nlohmann::json;

httplib::Server* server_g = nullptr; // 全局服务器指针，用于信号处理

void signalHandler(int signum)
{
	if ((signum == SIGINT || signum == SIGTERM) && server_g != nullptr)
	{
		ui::info("正在停止监听服务器......");
		server_g->stop();
	}
}

void fetch(int port)
{
	if (port < 0 || port>65535)
	{
		throw std::invalid_argument("端口号不合法");
	}
	else if (port < 1024)
	{
		ui::warning("警告：使用1024以下端口可能需要管理员权限");
	}
	httplib::Server server;// 声明监听服务器
	server_g = &server;

	// 注册信号处理器
	signal(SIGINT, signalHandler); // Windows
	signal(SIGTERM, signalHandler); // Linux

	server.Post("/", [](const httplib::Request& req, httplib::Response& res) // 注册 POST 路由
		{
			res.set_content("OK", "text/plain");
			try
			{
				Problem prob = Problem::fromJson(req.body);
				prob.save();
			}
			catch (const json::parse_error& e)
			{
				ui::error("json文件解析失败!");
			}
		});
	ui::info("开始在 127.0.0.1:" + std::to_string(port) + " 监听......");
	ui::info("按 Ctrl+C 停止监听");
	if (!server.listen("127.0.0.1", port))
	{
		ui::error("监听服务器启动失败！");
		throw std::runtime_error("无法在端口 " + std::to_string(port) + " 启动监听服务器");
	}

	server_g = nullptr;
	ui::info("监听服务器已停止");
	return;
}