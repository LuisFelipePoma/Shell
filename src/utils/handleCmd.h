#ifndef __HANDLE_CMD_H__
#define __HANDLE_CMD_H__

#include <cstdlib>
#include <iostream>
#include <string>
#include <filesystem>
#include <unistd.h>

inline bool handleExecutionCmd(const std::string &command)
{
	if (command.substr(0, 3) == "cd ")
	{
		std::string dir = command.substr(3);
		if ((chdir(dir.c_str()) != 0))
		{
			std::perror("Error changing directory");
			return false;
		}
		return true;
	}
	else
	{
		int result = std::system(command.c_str());
		if (result == -1)
		{
			std::cerr << "Failed to execute command: " << command << "\n";
			return false;
		}
		else if (result != 0)
		{
			//std::cerr << "Command exited with status: " << result << "\n";
			return false;
		}
	}
	return true;
}
#endif