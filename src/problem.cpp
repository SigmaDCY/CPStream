#include "json.hpp"
#include "problem.hpp"
#include "ui.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <regex>
using json = nlohmann::json;

namespace fs = std::filesystem;

void Problem::extractIdFromUrl()
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
	return;
}

Problem Problem::fromJson(const std::string& jsonString)
{
	Problem prob;
	json data = json::parse(jsonString);
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
	return prob;
}

void Problem::save()
{
	fs::path dirPath = fs::path(".") / group / id;
	try
	{
		fs::create_directories(dirPath);
	}
	catch (const fs::filesystem_error& e)
	{
		ui::error("创建目录失败:");
		ui::error(e.what());
		return;
	}
	fs::path cppPath = dirPath / (id + ".cpp");
	std::ofstream cppFile(cppPath);
	if (cppFile.is_open())
	{
		cppFile << u8"/*\n";
		cppFile << u8" * 题目来源: " << group << "\n";
		cppFile << u8" * 题目序号: " << id << "\n";
		cppFile << u8" * 题目名称: " << name << "\n";
		cppFile << u8" * 题目链接: " << url << "\n";
		cppFile << u8" * 时间限制: " << timeLimit << " ms\n";
		cppFile << u8" * 空间限制: " << memoryLimit << " MB\n";
		cppFile << u8"*/\n";
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
		else
		{
			ui::error("错误：无法创建文件！");
			continue;
		}
		std::ofstream outFile(outPath);
		if (outFile.is_open())
		{
			outFile << tests[i].output;
			outFile.close();
		}
		else
		{
			ui::error("错误：无法创建文件！");
			continue;
		}
	}
	ui::success("已成功爬取并创建题目！");
	return;
}