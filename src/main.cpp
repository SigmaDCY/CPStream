#include <iostream>
#include <filesystem>
#include <fstream>
#include "CLI11.hpp"
#include "httplib.h"
#include "json.hpp"
namespace fs = std::filesystem;
using json = nlohmann::json;

struct Problem
{
	std::string name, group, url, id, inputType, outputType, subtaskSize;
	int memoryLimit, timeLimit;
	struct TestCase
	{
		std::string input, output;
	};
	std::vector<TestCase> tests;

	void extractIdFromUrl()
	{
		if (group == "Luogu") {
			size_t lastSlash = url.find_last_of('/');
			if (lastSlash != std::string::npos) {
				id = url.substr(lastSlash + 1);
			}
		}
		// 在算出 id 之后，把所有不适合做文件夹名字的字符替换成下划线 '_'
		for (char& c : id) {
			if (c == '/' || c == '\\' || c == ':' || c == '*' || c == '?' || c == '"' || c == '<' || c == '>' || c == '|') {
				c = '_';
			}
		}
	}

	void save()
	{
		fs::path dirPath = fs::path(".") / group / id;
		fs::create_directories(dirPath);
		fs::path cppPath = dirPath / (id + ".cpp");
		std::ofstream cppFile(cppPath);
		if (cppFile.is_open())
		{
			cppFile << "/*\n";
			cppFile << " * 题目来源: " << group << "\n";
			cppFile << " * 题目序号: " << id << "\n";
			cppFile << " * 题目名称: " << name << "\n";
			cppFile << " * 题目链接: " << url << "\n";
			cppFile << " * 时间限制: " << timeLimit << " ms\n";
			cppFile << " * 空间限制: " << memoryLimit << " MB\n";
			cppFile << "*/\n";
			cppFile.close();
		}
	}
};

int main(int argc, char* argv[])
{
	CLI::App app{ "CPCodeManager" };
	auto fetchCmd = app.add_subcommand("fetch", "fetch problems' info");

	int port = 12345;
	fetchCmd->add_option("-p", port, "listening port"); //监听端口

	try
	{
		app.parse(argc, argv);
	}
	catch (const CLI::ParseError& e)
	{
		return app.exit(e);
	}

	if (app.get_subcommand(fetchCmd))
	{
		httplib::Server server; //声明监听服务器
		server.Post("/", [](const httplib::Request& req, httplib::Response& res) //注册 POST 路由
			{
				res.set_content("OK", "text/plain");

				try
				{
					Problem prob;
					json data = json::parse(req.body);
					prob.name = data.value("name", "unknown");
					prob.group = data.value("group", "unknown");
					prob.url = data.value("url", "unknown");
					prob.memoryLimit = data.value("memoryLimit", -1);
					prob.timeLimit = data.value("timeLimit", -1);
					prob.extractIdFromUrl();
					if (data.contains("input") && data["input"].is_object()) {
						prob.inputType = data["input"].value("type", "unknown");
					}
					if (data.contains("output") && data["output"].is_object()) {
						prob.outputType = data["output"].value("type", "unknown");
					}
					if (data.contains("tests") && data["tests"].is_array()) {
						for (const auto& test : data["tests"]) {
							Problem::TestCase tc;
							tc.input = test.value("input", "");
							tc.output = test.value("output", "");
							prob.tests.push_back(tc);
						}
					}
					prob.save();
				}
				catch (const json::parse_error& e)
				{
					std::cout << "Failed to parse json file!" << std::endl;
				}
			});
		std::cout << "Ready to listening on 127.0.0.1:" << port << std::endl;
		if (!server.listen("127.0.0.1", port))
		{
			std::cout << "Failed to set handler!" << std::endl;
		}
	}
	return 0;
}