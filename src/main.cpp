#include <iostream>
#include <filesystem>
#include <fstream>
#include <regex>
#include <cstdlib>
#include "httplib.h"
#include "json.hpp"
namespace fs = std::filesystem;
using json = nlohmann::json;

struct Problem
{
	std::string name, group, url, id, inputType, outputType, subtaskSize;
	int memoryLimit = 0, timeLimit = 0;
	struct TestCase
	{
		std::string input, output;
	};
	std::vector<TestCase> tests;

	void extractIdFromUrl()
	{
		if (group == "Luogu")
		{
			// /P1001
			size_t lastSlash = url.find_last_of('/');
			if (lastSlash != std::string::npos)
			{
				id = url.substr(lastSlash + 1);
			}
		}
		else if (group == "Codeforces")
		{
			// /A/1
			std::regex re("/problem/(\\d+)/([A-Za-z0-9]+)$");
			std::smatch match;
			if (std::regex_search(url, match, re))
			{
				id = match[1].str() + match[2].str();
			}
		}
		else if (group == "AtCoder")
		{
			// /abc123_a
			std::regex re("/tasks/([a-z0-9_]+)$");
			std::smatch match;
			if (std::regex_search(url, match, re))
			{
				id = match[1].str(); // 得到 "abc123_a"
			}
		}
		// 在算出 id 之后，把所有不适合做文件夹名字的字符替换成下划线 '_'
		for (char& c : id)
		{
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
		for (size_t i = 0; i < tests.size(); i++) {
			fs::path inPath = dirPath / (std::to_string(i + 1) + ".in");
			fs::path outPath = dirPath / (std::to_string(i + 1) + ".out");

			std::ofstream inFile(inPath);
			if (inFile.is_open())
			{
				inFile << tests[i].input;
				inFile.close();
			}

			std::ofstream outFile(outPath);
			if (outFile.is_open())
			{
				outFile << tests[i].output;
				outFile.close();
			}
		}
	}
};

int main(int argc, char* argv[])
{
	const char* logo = R"( ██████╗██████╗ ███████╗████████╗██████╗ ███████╗ █████╗ ███╗   ███╗
██╔════╝██╔══██╗██╔════╝╚══██╔══╝██╔══██╗██╔════╝██╔══██╗████╗ ████║
██║     ██████╔╝███████╗   ██║   ██████╔╝█████╗  ███████║██╔████╔██║
██║     ██╔═══╝ ╚════██║   ██║   ██╔══██╗██╔══╝  ██╔══██║██║╚██╔╝██║
╚██████╗██║     ███████║   ██║   ██║  ██║███████╗██║  ██║██║ ╚═╝ ██║
 ╚═════╝╚═╝     ╚══════╝   ╚═╝   ╚═╝  ╚═╝╚══════╝╚═╝  ╚═╝╚═╝     ╚═╝
                                                                    )";
	std::cout << logo << std::endl;
	while (true)
	{
		std::cout << "cps> ";
		std::string inst;
		std::cin >> inst;
		if (inst == "")
		{
			break;
		}
		if (inst[0] == '\0')
		{
			continue;
		}
		if (std::string(inst) == "fetch")
		{
			int port;
			std::cin >> port;
			if (port < 0 || port>65535)
			{
				std::cout << "请输入合法端口号！";
			}
			httplib::Server server;// 声明监听服务器
			server.Post("/", [](const httplib::Request& req, httplib::Response& res) // 注册 POST 路由
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
		else
		{
			std::cout << "未知命令" << std::endl;
		}
	}
	return 0;
}