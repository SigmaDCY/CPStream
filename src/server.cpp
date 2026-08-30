#include "server.hpp"
#include "problem.hpp"
#include "httplib.h"
#include "json.hpp"
#include "ui.hpp"
using json = nlohmann::json;

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
	ui::info("开始在127.0.0.1:" + std::to_string(port) + "监听......");
	if (!server.listen("127.0.0.1", port))
	{
		ui::error("服务器启动失败！");
		throw std::runtime_error("无法在端口 " + std::to_string(port) + " 启动服务器");
	}
	return;
}