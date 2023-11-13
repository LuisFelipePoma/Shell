#include <unistd.h>
#include <iostream>

bool changeDirectory(const std::string &dir)
{
	if (chdir(dir.c_str()) != 0)
	{
		std::perror("Error changing directory");
		return false;
	}
	return true;
}

int main(int argc, char *argv[])
{
	if (argc < 2)
	{
		std::cerr << "Usage: " << argv[0] << " <directory>" << std::endl;
		return 1;
	}

	if (!changeDirectory(argv[1]))
	{
		return 1;
	}

	std::cout << "Hello, world!" << std::endl;
	return 0;
}