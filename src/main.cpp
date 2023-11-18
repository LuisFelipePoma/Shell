#include <cstdio>
#include <fstream>
#include <iostream>
#include <ostream>
#include <string>

#include "grammar/libs/ShellExprLexer.h"
#include "grammar/libs/ShellExprParser.h"
#include "grammar/shellVisitor.h"
#include "antlr4-runtime.h"

int main(int argc, char **argv)
{
	std::string line;
	std::string program;
	while (true)
	{
		// Print the prompt.
		std::getline(std::cin, line);
		if (line == "exit")
		{
			break;
		}

		if (line.empty())
		{
			system("clear");
			std::cout << "ready> ";
		}
		else
		{
			// Otherwise, add the line to the program.
			program += line + "\n";
		}
		antlr4::ANTLRInputStream input(line);
		ShellExprLexer lexer(&input);
		antlr4::CommonTokenStream tokens(&lexer);
		ShellExprParser parser(&tokens);
		auto tree = parser.start();
		shellVisitor eval;
		eval.visitStart(tree);
	}
	// eval.test();
	// std::cout << tree->toStringTree(true) << std::endl;
}

//------------------------ MAIN --------------------------------------//

// int main(int argc, char const *argv[])
// {
// 	std::string line;
// 	std::string program;
// 	IR vm;

// 	std::cout << "ready> ";
// 	while (true)
// 	{
// 		// Print the prompt.
// 		std::getline(std::cin, line);

// 		// If the user entered "exit", quit the program.
// 		if (line == "exit")
// 		{
// 			break;
// 		}

// 		// If the user entered an empty line, execute the program.
// 		if (line.empty())
// 		{
// 			// vm.exec(program);
// 			system("clear");
// 			// program.clear();
// 			std::cout << "ready> ";
// 		}
// 		else
// 		{
// 			// Otherwise, add the line to the program.
// 			program += line + "\n";
// 		}
// 	}

// 	return 0;
// }