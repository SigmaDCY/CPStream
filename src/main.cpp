#include "commands.hpp"
#include "ui.hpp"
#include <iostream>

#ifdef _WIN32
#include <windows.h>
#endif

int main(int argc, char *argv[])
{
	welcome();
	cmds();
	return 0;
}