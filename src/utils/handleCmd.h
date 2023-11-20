#ifndef __HANDLE_CMD_H__
#define __HANDLE_CMD_H__

#include <any>
#include <cstdlib>
#include <iostream>
#include <string>

inline bool handleExecutionCmd(const std::string command)
{
	int result = std::system(command.c_str());
	if (result == -1)
	{
		std::cerr << "Failed to execute command: " << command << "\n";
		return false;
	}
	else if (result != 0)
	{
		std::cerr << "Command exited with status: " << result << "\n";
		return false;
	}
	return true;
}

#endif