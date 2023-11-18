// ls.h
#ifndef LS_H
#define LS_H

#include <map>
#include <stdio.h>
#include <dirent.h>
#include <unordered_map>

const std::unordered_map<int, const char *> FILE_TYPES = {
	{4, "folder"},
	{8, "file"}};

// Body Functions
inline void ls_command(const char *dir_name)
{
	DIR *dir;
	struct dirent *entry;

	// Open the directory
	dir = opendir(dir_name);
	if (!dir)
	{
		printf("Error: Unable to open directory.\n");
		return;
	}

	/// Read entries from the directory
	while ((entry = readdir(dir)) != NULL)
	{
		printf("Type: %-10s Name: %s\n", FILE_TYPES.at(entry->d_type), entry->d_name);
	}

	// Close the directory
	closedir(dir);
}

#endif