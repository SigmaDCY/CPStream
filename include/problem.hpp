#pragma once

#include <string>
#include <vector>

struct Problem
{
	std::string name, group, url, id, inputType, outputType, subtaskSize;
	int memoryLimit = 0, timeLimit = 0;

	struct TestCase
	{
		std::string input, output;
	};
	std::vector<TestCase> tests;

	void extractIdFromUrl();
	void save();
	static Problem fromJson(const std::string& jsonString);
};