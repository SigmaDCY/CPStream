#include "commands.hpp"
#include "ui.hpp"
#include <iostream>

#ifdef _WIN32
#include <windows.h>
#endif

int main(int argc, char *argv[])
{
#ifdef _WIN32
	SetConsoleCP(CP_UTF8);
	SetConsoleOutputCP(CP_UTF8);
#endif
	welcome();
	cmds();
	return 0;
}