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

int main(int argc, char **argv)
{
	shellVisitor visitor;
	std::string line;
	std::string program = "";
	const char *username = getenv("USER"); // Get the username
	if (username == nullptr)
	{ // If the USER environment variable is not set, use a default value
		username = "user";
	}
	while (true)
	{
		std::cout << "[" << username << "] $ "; // Print the prompt with the username
		// Print the prompt.
		std::getline(std::cin, line);
		if (line == "exit")
		{
			break;
		}

		if (line.empty())
		{
			continue;
		}

		antlr4::ANTLRInputStream input(line);
		ShellExprLexer lexer(&input);
		antlr4::CommonTokenStream tokens(&lexer);
		ShellExprParser parser(&tokens);
		auto tree = parser.start();
		visitor.visitStart(tree);
		visitor.test();
	}
}