// ls.h
#ifndef CD_H
#define CD_H

#include <unistd.h>
#include <iostream>

inline bool changeDirectory(const std::string &dir)
{
	if (chdir(dir.c_str()) != 0)
	{
		std::perror("Error changing directory");
		return false;
	}
	return true;
}

#endif