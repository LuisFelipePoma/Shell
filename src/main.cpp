#include <cstdlib>
#include <iostream>
#include "IR/IR.h"

#include <fstream>
#include <cstdio>
#include <string>

#include "eval.h"
#include "antlr4-runtime.h"

// int main(int argc, char **argv)
// {
// 	if (argc <= 1)
// 		return -1;
// 	std::ifstream is;
// 	is.open(argv[1]);
// 	antlr4::ANTLRInputStream input(is);
// 	LabeledExprLexer lexer(&input);
// 	antlr4::CommonTokenStream tokens(&lexer);
// 	LabeledExprParser parser(&tokens);
// 	auto tree = parser.prog();
// 	EvalVisitorImpl eval;
// 	eval.visitProg(tree);
// 	return 0;
// }

//------------------------ MAIN --------------------------------------//

int main(int argc, char const *argv[])
{
	std::string line;
	std::string program;
	IR vm;

	std::cout << "ready> ";
	while (true)
	{
		// Print the prompt.
		std::getline(std::cin, line);

		// If the user entered "exit", quit the program.
		if (line == "exit")
		{
			break;
		}

		// If the user entered an empty line, execute the program.
		if (line.empty())
		{
			// vm.exec(program);
			system("clear");
			// program.clear();
			std::cout << "ready> ";
		}
		else
		{
			// Otherwise, add the line to the program.
			program += line + "\n";
		}
	}

	return 0;
}