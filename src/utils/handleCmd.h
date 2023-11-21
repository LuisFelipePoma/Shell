#ifndef __HANDLE_CMD_H__
#define __HANDLE_CMD_H__

#include <cstdlib>
#include <iostream>
#include <string>
#include <filesystem>
#include <unistd.h>
#include <array>
#include <memory>
#include <memory>
#include <fstream>

inline std::string handleExecutionCmd(const std::string &command, const bool isPipeline)
{
	std::string result = "";
	if (isPipeline)
		return command;
	if (command.substr(0, 3) == "cd ")
	{
		std::string dir = command.substr(3);
		if ((chdir(dir.c_str()) != 0))
		{
			std::perror("Error changing directory");
		}
		return "error";
	}
	else
	{
		std::system(command.c_str());
	}
	return result;
}

#endif