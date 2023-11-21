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

std::string execAndCapture(const std::string &command);

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
		result = execAndCapture(command.c_str());
		std::cout << "\033[1;32m" << result << "\033[0m\n";
	}
	return result;
}

inline std::string execAndCapture(const std::string &command)
{
	std::array<char, 128> buffer;
	std::string result;
	std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(command.c_str(), "r"), pclose);
	if (!pipe)
	{
		std::cout << "popen() failed!";
		return "error";
	}
	while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr)
	{
		result += buffer.data();
	}
	return result;
}

inline void passStringToCommand(const std::string &str, const std::string &command)
{
	// Write the string to a temporary file
	std::ofstream tempFile("temp.txt");
	tempFile << str;
	tempFile.close();

	// Construct the command
	std::string fullCommand = command + " temp.txt";

	// Execute the command
	std::system(fullCommand.c_str());

	// Delete the temporary file
	std::remove("temp.txt");
}

inline void execAndRedirect(const std::string &command, const std::string &outputFile)
{
	// Construct the command
	std::string fullCommand = command + " > " + outputFile;

	// Execute the command
	std::system(fullCommand.c_str());
}
#endif