#include <iostream>
#include "IR/IR.h"

//------------------------ MAIN --------------------------------------//
int main(int argc, char const *argv[])
{
	std::string program = R"(
		42
	)";

	IR vm;
	vm.exec(program);

	return 0;
}