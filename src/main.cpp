#include <cstdio>
#include <fstream>
#include <iostream>
#include <ostream>
#include <string>

#include "grammar/libs/ShellExprLexer.h"
#include "grammar/libs/ShellExprParser.h"
#include "grammar/shellVisitor.h"
#include "antlr4-runtime.h"

#include <cstdlib> // for getenv

#define RED_TEXT "\033[31m"
#define GREEN_TEXT "\033[32m"
#define YELLOW_TEXT "\033[33m"
#define BLUE_TEXT "\033[34m"
#define MAGENTA_TEXT "\033[35m"
#define CYAN_TEXT "\033[36m"
#define WHITE_TEXT "\033[37m"
#define RESET_COLOR "\033[0m"

#define BOLD_TEXT "\033[1m"
#define RESET_BOLD_TEXT "\033[22m"

int main(int argc, char **argv)
{
	std::string colors[] = {BLUE_TEXT, GREEN_TEXT, YELLOW_TEXT, RED_TEXT, MAGENTA_TEXT, WHITE_TEXT};
	int colorIndex = 0;
	shellVisitor visitor;
	std::string line;
	std::string lineCP;
	const char *username = getenv("USER"); // Get the username
	if (username == nullptr)
	{ // If the USER environment variable is not set, use a default value
		username = "user";
	}
	bool singleLine = true;
	std::string mulStr;

	while (true)
	{
		lineCP = "";
		std::cout << BOLD_TEXT;
		if (singleLine)
		{
			std::cout << colors[colorIndex] << "Pambi" << username << " ~ " << WHITE_TEXT;
			std::getline(std::cin, line);

			if (line == "**")
			{
				singleLine = false;
				continue;
			}
			lineCP = line;
		}
		else
		{
			std::cout << colors[colorIndex] << "     " << username << " ~ " << WHITE_TEXT;
			std::getline(std::cin, line);

			if (line != "**")
			{
				mulStr += line + " ";
				continue;
			}
			else
			{
				singleLine = true;
				lineCP = mulStr;
				mulStr = "";
			}
		}

		if (line == "exit")
			break;
		if (line.empty())
			continue;
		if (line == "!!")
		{
			colorIndex == std::size(colors) - 1 ? colorIndex = 0 : colorIndex++;
			continue;
		}

		antlr4::ANTLRInputStream input(lineCP);
		ShellExprLexer lexer(&input);
		antlr4::CommonTokenStream tokens(&lexer);
		ShellExprParser parser(&tokens);
		auto tree = parser.start();
		visitor.visitStart(tree);
		visitor.test();
	}
}