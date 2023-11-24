// Libraries from the system
#include <any>
#include <llvm-c/ExecutionEngine.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Value.h>
#include <memory>
#include <string>
#include <cstdio>
#include <sys/wait.h>
#include <iostream>
#include <unistd.h>

// Libraries from antlr4 files generated
#include "libs/ShellExprLexer.h"
#include "libs/ShellExprParser.h"
#include "shellVisitor.h"

// Utils
#include "../utils/export.h"
#include "../utils/handleCmd.h"

// _____________________________________________________________________________
// |	/	/	/	/	/	IMPLEMENTATION VISITORS	 /	/	/	/	/	/	/	|
// -----------------------------------------------------------------------------

// start : comand EOF;
std::any shellVisitor::visitStart(ShellExprParser::StartContext *ctx)
{
	// Init of the program
	return visitChildren(ctx);
}

// _____________________________________________________________________________
// |								command										|
// -----------------------------------------------------------------------------

// command	: simple_command										# simpleStmt
/*
	----
	Visit a commands and operations

	```antlr
		simple_command
		: cmd_name args															# cmdArgs
		| cmd_name																# cmd
		| operation                         									# operationStmt
		;
	```
	Visit simple commands like:
		- `ls *args`
		- `cd *args`
		- `cp *args`
		- `etc`

	Also visit operations inline like:
		- `export abc = "abc"` : exports a enviroment variable
		- `let abc = "abc"` : creates a local variable
		- `abc = "cba"` : update a local variable
		- `show abc` : shows a local variable
*/
std::any shellVisitor::visitSimpleStmt(ShellExprParser::SimpleStmtContext *ctx)
{
	// Visit the children
	visitChildren(ctx);
	return std::any();
}

// command :| compound_command 							# compoundStmt
// TODO

// command :| pipeline 									# pipelineStmt
// TODO

// _____________________________________________________________________________
// |							compound_command								|
// -----------------------------------------------------------------------------

// compound_command : brace_group
// TODO

// compound_command : | for_clause
// TODO

// compound_command : | if_clause
// TODO

// compound_command : | while_clause
// TODO

// compound_command : | function_definition
// TODO

// _____________________________________________________________________________
// |							function_definition								|
// -----------------------------------------------------------------------------

// function_definition	: DEF ID LPAREN function_args? RPAREN LBRACE compound_list RBRACE		# functionDef
std::any shellVisitor::visitFunctionDef(ShellExprParser::FunctionDefContext *ctx)
{
	// Read the name of the function
	auto nameFunction = ctx->ID()->getText();
	// Verify if the function exists
	if (functions.find(nameFunction) != functions.end())
	{
		std::cout << "Function already exists..." << std::endl;
		return std::any();
	}

	std::vector<std::string> functionArgs;
	// Read args of the function in a vector
	if (ctx->function_args())
		functionArgs = std::any_cast<std::vector<std::string>>(visit(ctx->function_args()));

	// Create a Function instance
	Function function;
	function.args = functionArgs;
	// Save the function in the map
	functions[nameFunction] = function;

	return std::any();
}

// _____________________________________________________________________________
// |							function_args									|
// -----------------------------------------------------------------------------

// function_args 		: ID (',' ID)* 															# functionArgs
std::any shellVisitor::visitFunctionArgs(ShellExprParser::FunctionArgsContext *ctx)
{
	// Read and insert the args values to a vector
	std::vector<std::string> args;
	for (auto i : ctx->ID())
	{
		args.push_back(i->getText());
	}

	return std::any(args);
}

// _____________________________________________________________________________
// |							simple_command									|
// -----------------------------------------------------------------------------

// antlr: simple_command: cmd_name args									# cmdArgs
/*
	----
	Visit a command with args

	```antlr
		simple_command: cmd_name args									# cmdArgs
	```
	Visit commands like:
		- `ls *args`
		- `cd *args`
		- `cp *args`
		- `etc`
*/
std::any shellVisitor::visitCmdArgs(ShellExprParser::CmdArgsContext *ctx)
{
	// Read the command name
	std::string command = ctx->cmd_name()->ID()->getText();
	// Read the args like a string
	auto args = std::any_cast<std::string>(visit(ctx->args()));

	// Execute the command of the system with his args*
	std::string result = std::any_cast<std::string>(handleExecutionCmd(command + " " + args, isPipeline));
	// if (command.substr(0, 3) == "cd ")
	// {
	// 	command = command.substr(3);
	// 	executeCommandFunction = module->getFunction("chdir");
	// }
	return std::any(result);
}

// simple_command: | cmd_name											# cmd
/*
	----
	Visit a command without args
	```antlr
		simple_command: cmd_name											# cmd
	```
	Visit commands like:
		- `ls `
		- `pwd`
		- `etc`
*/
std::any shellVisitor::visitCmd(ShellExprParser::CmdContext *ctx)
{
	std::string command = ctx->cmd_name()->ID()->getText();

	// Parse command and verify
	if (isPipeline)
		return command;

	auto *executeCommandFunction = module->getFunction("system");

	llvm::Value *commandValue = builder->CreateGlobalStringPtr(llvm::StringRef(command), "command");
	std::vector<llvm::Value *>
		args = {commandValue};

	auto result = builder->CreateCall(executeCommandFunction, args);

	return std::any();
}

// simple_command: | operation                         					# operationStmt
/*
	----
	Visit a operation
	```antlr
		simple_command: operation                         # operationStmt
	```
*/
std::any shellVisitor::visitOperationStmt(ShellExprParser::OperationStmtContext *ctx)
{
	// Visit the operation
	visit(ctx->operation());
	return std::any();
}

// _____________________________________________________________________________
// |								  args									   |
// -----------------------------------------------------------------------------

// args: ID+															# argsBody
/*
	----
	Visit the args of the command
	```antlr
		args: ID+								# argsBody
	```
	Args like this:
		- `rm -r /path/folder`
		- `ls /path/folder`
		- `touch /path/to/create/file`
*/
std::any shellVisitor::visitArgsBody(ShellExprParser::ArgsBodyContext *ctx)
{
	// Read the args
	std::string args;
	for (auto i : ctx->ID())
	{
		// Add the args to a string
		args.append((i->getText()));
		// Add a space between args
		args.append(" ");
	}
	// Remove the last space
	args.pop_back();

	// Return the args
	return std::any(args);
}

// _____________________________________________________________________________
// |								operation									|
// -----------------------------------------------------------------------------

// operation : ID '=' expr                         						# assign
/*
	----
	Visit the assignation or update of a local variable (variable must exists)
	```antlr
		operation : ID '=' expr                         				# assign
	```
	Looks like this:
		- `b = 10`
		- `variable = 'alegria'`
		- `c10 = [1,2,3,4]`
*/
std::any shellVisitor::visitAssing(ShellExprParser::AssingContext *ctx)
{
	// Read the name of the variable
	std::string assignation = ctx->ID()->getText();

	// Read the value of the expression
	auto value = std::any_cast<std::string>(visit(ctx->expr()));

	// Verify if the variable exists
	if (memory.find(assignation) == memory.end())
	{
		std::cout << "Variable not found..." << std::endl;
	}
	else
		// If the variable exists, update the value
		memory[assignation] = value;
	return std::any();
}

// operation : | 'export' ID '=' expr		       						# export
/*
	----
	Creates a enviroment variable (not accepts lists)
	```antlr
		operation : 'export' ID '=' expr		       					# export
	```
	Looks like this:
		- `export b = 10`
		- `export variable = 'alegria'`
*/
std::any shellVisitor::visitExport(ShellExprParser::ExportContext *ctx)
{
	// Read the name of the variable
	std::string id = ctx->ID()->getText();

	// Read the value of the expression
	std::any valueAny = visit(ctx->expr());

	// Verify the type of the value
	if (valueAny.type() == typeid(std::vector<std::any>))
	{
		// Handle vector case
		std::cout << "Cannot export a vector\n";
	}
	else if (valueAny.type() == typeid(std::string))
	{
		// If the type is a string, export the variable
		std::string value = std::any_cast<std::string>(valueAny);

		// Run the export command
		export_command(id.c_str(), value.c_str());
	}
	else
	{
		// Handle other types an error
		std::cout << "Unsupported type for export\n";
	}

	return std::any();
}

// operation : | 'let' ID '=' expr	                					# declaration
/*
	----
	Creates a local variable
	```antlr
		operation : 'let' ID '=' expr	                				# declaration
	```
	Looks like this:
		- `let b = 10`
		- `let variable = 'alegria'`
		- `let variable = [1,4,12]`
*/
std::any shellVisitor::visitDeclaration(ShellExprParser::DeclarationContext *ctx)
{
	// Read the name of the variable
	std::string id = ctx->ID()->getText();

	// Read the value of the expression
	auto value = visit(ctx->expr());

	// Verify if the variable exists
	if (memory.find(id) != memory.end())
	{
		// If the variable exists, throw an error (cannot create two variables equals)
		std::cout << "Variable already exists..." << std::endl;
	}
	else
		// If the variable doesn't exists, create it
		memory.insert(std::pair<std::string, std::any>(id, value));
	return std::any();
}

// operation : 	| 'show' ID 	                						# show
/*
	----
	Shows a local variable
	```antlr
		operation : 'show' ID 	                						# show
	```
	Looks like this:
		- `show b `
		- `show variable`
*/
std::any shellVisitor::visitShow(ShellExprParser::ShowContext *ctx)
{
	// Read the name of the variable
	std::string id = ctx->ID()->getText();

	// Verify if the variable exists
	if (memory.find(id) != memory.end())
	{
		// If the variable exists, show it
		std::cout << std::any_cast<std::string>(memory.at(id)) << std::endl;
	}
	else
	{
		// If the variable doesn't exists, throw an error
		std::cout << "Variable not found..." << std::endl;
	}

	return std::any();
}

// 'fn' ID LPAREN function_args? RPAREN									# callFunction
std::any shellVisitor::visitCallFunction(ShellExprParser::CallFunctionContext *ctx)
{
	// Read the name of the function
	auto nameFunction = ctx->ID()->getText();

	if (functions.find(nameFunction) == functions.end())
	{
		std::cout << "Function not found..." << std::endl;
		return std::any();
	}
	std::vector<std::string> args;
	if (ctx->function_args())
		args = std::any_cast<std::vector<std::string>>(visit(ctx->function_args()));

	// Read the function
	auto function = functions[nameFunction];

	// Show Function
	std::cout << "Function: " << nameFunction << std::endl;
	std::cout << "Args: " << function.args.size() << std::endl;
	// Show args
	for (auto arg : function.args)
	{
		std::cout << arg << std::endl;
	}

	return std::any();
}
// _____________________________________________________________________________
// |								expr										|
// -----------------------------------------------------------------------------

// expr : expr ('*'|'/') expr                     						# mulDivOpe
/*
	----
	Operation of multiply and division
	```antlr
		expr : expr ('*'|'/') expr                     					# mulDivOpe
	```
	Looks like this:
		- `b = c * a `
		- `c = 1 / f`
*/
std::any shellVisitor::visitMulDivOpe(ShellExprParser::MulDivOpeContext *ctx)
{
	// Read the left and right part of the expression
	auto left = std::any_cast<llvm::Value *>(visit(ctx->expr(0)));
	auto right = std::any_cast<llvm::Value *>(visit(ctx->expr(1)));

	// Verify the operation (only accepts numbers operations for now)
	switch (ctx->opt->getType())
	{
	case ShellExprLexer::MUL:
		return std::any(builder->CreateFMul(left, right, "mulTemp"));
	case ShellExprLexer::DIV:
		return std::any(builder->CreateFDiv(left, right, "divTemp"));
	}
	return std::any(nullptr);
}

// expr : | expr ('+'|'-') expr                  						# sumMinOpe
/*
	----
	Operation of sum and subtraction
	```antlr
		expr : expr ('+'|'-') expr                   # sumMinOpe
	```
	Looks like this:
		- `b = c + a `
		- `c = 1 - f`
*/
std::any shellVisitor::visitSumMinOpe(ShellExprParser::SumMinOpeContext *ctx)
{
	// Read the left and right part of the expression
	auto left = std::any_cast<llvm::Value *>(visit(ctx->expr(0)));
	auto right = std::any_cast<llvm::Value *>(visit(ctx->expr(1)));

	// Verify the operation (only accepts numbers operations for now)
	switch (ctx->opt->getType())
	{
	case ShellExprLexer::PLUS:
		return std::any(builder->CreateFDiv(left, right, "addTemp"));
	case ShellExprLexer::MINUS:
		return std::any(builder->CreateFSub(left, right, "subTemp"));
	}
	return std::any(nullptr);
}

// expr : | expr ('<'|'>'|'>='|'<='| '==') expr   						# compOpe
/*
	----
	Operations of comparison
	```antlr
		expr : expr ('<'|'>'|'>='|'<='| '==') expr   # compOpe
	```
	Looks like this:
		- `b = c >= a `
		- `c == 10`
*/
std::any shellVisitor::visitCompOpe(ShellExprParser::CompOpeContext *ctx)
{
	// Read the left and right part of the expression
	auto left = std::any_cast<llvm::Value *>(visit(ctx->expr(0)));
	auto right = std::any_cast<llvm::Value *>(visit(ctx->expr(1)));

	// Verify the operation (only accepts numbers operations for now)
	switch (ctx->opt->getType())
	{
	case ShellExprLexer::LESS:
		return std::any(builder->CreateFCmpULT(left, right, "lessTemp"));
	case ShellExprLexer::GREAT:
		return std::any(builder->CreateFCmpUGT(left, right, "greatTemp"));
	case ShellExprLexer::LESS_EQ:
		return std::any(builder->CreateFCmpULE(left, right, "lessEqTemp"));
	case ShellExprLexer::GREAT_EQ:
		return std::any(builder->CreateFCmpUGE(left, right, "greatEqTemp"));
	case ShellExprLexer::NOT_EQ:
		return std::any(builder->CreateFCmpUNE(left, right, "notEqTemp"));
	case ShellExprLexer::EQUALS:
		return std::any(builder->CreateFCmpUEQ(left, right, "equalTemp"));
	}
	return std::any(nullptr);
}

// expr : | NUMBER                   										# number
/*
	----
	Base of a number
	```antlr
		expr : NUMBER                   									# number
	```
	Looks like this:
		- `100`
		- `-10.2`
*/
std::any shellVisitor::visitNumber(ShellExprParser::NumberContext *ctx)
{
	// Read the name of the variable
	auto numVal = std::stod(ctx->NUMBER()->getText());

	// If the variable doesn't exists, return the same name
	llvm::Value *val = llvm::ConstantFP::get(*context, llvm::APFloat(numVal));
	return std::any(val);
}

// expr : | ID                                    						# idStmt
/*
	----
	Base of a expression
	```antlr
		expr : ID                                    					# idStmt
	```
	Looks like this:
		- `b `
		- `c`
*/
std::any shellVisitor::visitIdStmt(ShellExprParser::IdStmtContext *ctx)
{
	// Read the name of the variable
	std::cout << "visitId\n";
	std::string idName = ctx->ID()->getText();

	llvm::AllocaInst *Alloca = CreateEntryBlockAlloca(idName);

	// Verify if the variable exists
	if (memory.find(idName) != memory.end())
		// If the variable exists, return it
		return memory[idName];

	// If the variable doesn't exists, return the same name
	memory[idName] = Alloca;
	return std::any(Alloca);
}

// expr : | LIST                                  						# listStmt
/*
	----
	Base of a list
	```antlr
		expr : LIST                                  					# listStmt
	```
	Looks like this:
		- `[1,3,4,1]`
		- `[pepito,felicidad,pov]`
*/
std::any shellVisitor::visitListStmt(ShellExprParser::ListStmtContext *ctx)
{
	// Read the list as a string
	std::string list = ctx->LIST()->getText();
	// Create a vector of any
	std::vector<std::any> arr;

	// Remove the brackets
	std::size_t start = list.find('[') + 1;
	std::size_t end = list.find(']');

	// Get the list part
	std::string listPart = list.substr(start, end - start);

	// Create a stream from the list part
	std::istringstream iss(listPart);

	// Iterate over the list and split the list part by commas
	for (std::string s; std::getline(iss, s, ',');)
	{
		// Store it as std::string
		arr.push_back(s);
	}

	// Return the vector of any
	return std::any(arr);
}

// _____________________________________________________________________________
// |							compound_list									|
// -----------------------------------------------------------------------------

// compound_list: (command|and_or) (separator (command|and_or))* separator? # compoundListBody
/*
	----
	Body of the Compound List
	```antlr
		compound_list: (command|and_or) (separator (command|and_or))* separator? # compoundListBody
	```
*/
std::any shellVisitor::visitCompoundListBody(ShellExprParser::CompoundListBodyContext *ctx)
{
	// Iterate over the children in order
	visitChildren(ctx);
	return std::any();
}

// _____________________________________________________________________________
// |							  for_clause									|
// -----------------------------------------------------------------------------

// for_clause: FOR ID IN expr do_group          						# forBody
/*
	----
	Body of the for structure
	```antlr
		for_clause: FOR ID IN expr do_group          # forBody
	```
	Looks like this:
		- `for i in x do show i done`
*/
std::any shellVisitor::visitForBody(ShellExprParser::ForBodyContext *ctx)
{
	// Read the name of the variable to iter
	auto iter_element = ctx->ID()->getText();

	// Read the iterable
	auto iterable = visit(ctx->expr());

	// If the Iterable is a list
	if (iterable.type() == typeid(std::vector<std::any>))
	{
		// Convert the iterable to vector of any
		std::vector<std::any> newIterable = std::any_cast<std::vector<std::any>>(iterable);

		// Body of the "For" structure
		for (auto i : newIterable)
		{
			// Verify the type of the iterable (i)
			if (i.type() == typeid(std::string))
			{
				// Creates the local variable (i) for the scope of the for
				memory.insert(std::pair<std::string, std::any>(iter_element, i));
				// Visit the body of the for
				visit(ctx->do_group());
				// Remove the local variable (i)
				memory.erase(iter_element);
			}
			else
			{
				// Handle other types and print the error
				std::cout << "Unsupported type in iterable\n";
				return std::any();
			}
		}
	}
	// If the Iterable is a string(number)
	else if (iterable.type() == typeid(std::string))
	{
		// Convert the iterable to int to be iterable in the for
		int newIterable = std::stoi(std::any_cast<std::string>(iterable));

		// Body of the "For" structure
		for (size_t i = 0; i < newIterable; ++i)
		{
			// Creates the local variable (i) for the scope of the for
			// Also send the "i" as a string
			memory.insert(std::pair<std::string, std::any>(iter_element, std::to_string(i)));
			// Visit the body of the for
			visit(ctx->do_group());
			// Remove the local variable (i)
			memory.erase(iter_element);
		}
	}
	else
	{
		// Handle other types as error
		std::cout << "Unsupported type for 'for'\n";
		return std::any();
	}

	return std::any();
}

// _____________________________________________________________________________
// |							  brace_group									|
// -----------------------------------------------------------------------------

// brace_group: LBRACE compound_list RBRACE    							# braceBody
// TODO

// _____________________________________________________________________________
// |								do_group									|
// -----------------------------------------------------------------------------

// do_group: DO compound_list DONE             							# doBody
// TODO

// _____________________________________________________________________________
// |								and_or										|
// -----------------------------------------------------------------------------

// and_or: pipeline (AND_IF pipeline | OR_IF pipeline)* 				#andOrBody
/*
	----
	Body of and and or inline
	```antlr
		and_or: pipeline (AND_IF pipeline | OR_IF pipeline)* 			#andOrBody
	```
	Looks like this:
		- `ls && pwd || echo dont work`
*/
std::any shellVisitor::visitAndOrBody(ShellExprParser::AndOrBodyContext *ctx)
{
	// Variable to store the pipeline
	std::string output = "";

	// If the size is 1, return the pipeline
	if (ctx->children.size() == 1)
	{
		// Read the pipeline
		output = std::any_cast<std::string>(visit(ctx->children[0]));
		return std::any(output);
	}

	// Set the flags
	isAndOr = true;

	// Parse all to a string
	for (int i = 0; i < ctx->children.size(); i++)
	{
		isPipeline = true;
		// If the child is a pipeline, add it to the string
		if (!ctx->children[i]->getText().compare("&&") ||
			!ctx->children[i]->getText().compare("||"))
			output += ctx->children[i]->getText();
		else
			// If the child is not a pipeline, visit it and add it to the string
			output += std::any_cast<std::string>(visit(ctx->children[i]));
		isPipeline = false;
	}
	isAndOr = false;

	// Run the command
	auto response = handleExecutionCmd(output, isPipeline);

	return std::any(response);
}

// _____________________________________________________________________________
// |							    pipeline									|
// -----------------------------------------------------------------------------

// pipeline: simple_command (io_redirect simple_command)*				# pipelineBody
/*
	----
	Body of the pipeline
	```antlr
		if_clause: IF expr DO compound_list else_part DONE         			# ifElseBody
	```
	Looks like this:
		- `ls | grep .cpp > list.txt`
*/
std::any shellVisitor::visitPipelineBody(ShellExprParser::PipelineBodyContext *ctx)
{
	// Variable to store the pipeline
	std::string output = "";

	// If the size is 1, return the pipeline
	if (ctx->children.size() == 1)
	{
		output = std::any_cast<std::string>(visit(ctx->children[0]));
		return std::any(output);
	}
	// Set the flags
	isPipeline = true;

	// Parse all to a string
	for (int i = 0; i < ctx->children.size(); i++)
	{
		// If the child is a pipeline, add it to the string
		if (!ctx->children[i]->getText().compare("<") ||
			!ctx->children[i]->getText().compare(">") ||
			!ctx->children[i]->getText().compare(">>") ||
			!ctx->children[i]->getText().compare("|"))
			output += ctx->children[i]->getText();
		else
			// If the child is not a pipeline, visit it and add it to the string
			output += std::any_cast<std::string>(visit(ctx->children[i]));
	}
	isPipeline = false;

	// Verify if the pipeline is a and/or
	if (isAndOr)
		return std::any(output);

	// Run the command
	auto response = handleExecutionCmd(output, isPipeline);

	// Return the response
	return std::any(response);
}

// _____________________________________________________________________________
// |								if_clause									|
// -----------------------------------------------------------------------------

// if_clause: IF expr DO compound_list else_part DONE         			# ifElseBody
/*
	----
	Body of the if/else structure
	```antlr
		if_clause: IF expr DO compound_list else_part DONE         		# ifElseBody
	```
	Looks like this:
		- `if x == 10 do show x else echo dont found done`
*/
std::any shellVisitor::visitIfElseBody(ShellExprParser::IfElseBodyContext *ctx)
{
	// Read the condition
	auto condition = std::any_cast<std::string>(visit(ctx->expr()));

	// Equals '1' -> true
	if (std::stoi(condition) == 1)
	{
		// Visit the body of the if
		visit(ctx->compound_list());
	}
	else
	{
		// Visit the body of the else
		visit(ctx->else_part());
	}
	return std::any();
}

// if_clause: | IF expr do_group                              			# ifBody
/*
	----
	Body of the if structure
	```antlr
		if_clause: | IF expr do_group                              		# ifBody
	```
	Looks like this:
		- `if x == 10 do show x done`
*/
std::any shellVisitor::visitIfBody(ShellExprParser::IfBodyContext *ctx)
{
	// Read the condition
	auto condition = std::any_cast<std::string>(visit(ctx->expr()));

	// Equals '1' -> true
	if (std::stoi(condition) == 1)
	{
		// Visit the body of the if
		visit(ctx->do_group());
	}
	return std::any();
}
// _____________________________________________________________________________
// |								else_part									|
// -----------------------------------------------------------------------------

// else_part : ELSE IF expr DO command* else_part              			# elseIfBody
/*
	----
	Body of the else if structure
	```antlr
		else_part : ELSE IF expr DO command* else_part              	# elseIfBody
	```
	Looks like this:
		- `if x == 10 do show x else if x < 5 do echo part done`
*/
std::any shellVisitor::visitElseIfBody(ShellExprParser::ElseIfBodyContext *ctx)
{
	// Read the condition
	auto condition = std::any_cast<std::string>(visit(ctx->expr()));

	// Equals '1' -> true
	if (std::stoi(condition) == 1)
	{
		// Visit the body of the if
		for (auto command : ctx->command())
		{
			// Visit the commands
			visit(command);
		}
	}
	else
	{
		// Visit the body of the else
		visit(ctx->else_part());
	}
	return std::any();
}

// else_part : ELSE compound_list                            			# elseBody
/*
	----
	Body of the if structure
	```antlr
		else_part : ELSE compound_list                            	# elseBody
	```
	Looks like this:
		- `else echo part`
*/
std::any shellVisitor::visitElseBody(ShellExprParser::ElseBodyContext *ctx)
{
	// Visit the body of the else
	visit(ctx->compound_list());
	return std::any();
}

// _____________________________________________________________________________
// |							   while_clause									|
// -----------------------------------------------------------------------------

// while_clause: WHILE expr do_group                   					# whileBody
/*
	----
	Body of the if structure
	```antlr
		while_clause: WHILE expr do_group                   			# whileBody
	```
	Looks like this:
		- `while x <= 10 do show x done`
*/
std::any shellVisitor::visitWhileBody(ShellExprParser::WhileBodyContext *ctx)
{
	// Read the condition
	auto condition = std::any_cast<std::string>(visit(ctx->expr()));

	// Equals '1' -> true
	while (std::stoi(condition) == 1)
	{
		// Visit the body of the while
		visit(ctx->do_group());

		// Update the condition
		condition = std::any_cast<std::string>(visit(ctx->expr()));
	}
	return std::any();
}

// _____________________________________________________________________________
// |	/	/	/	/	/	IMPLEMENTATION FUNCTIONS /	/	/	/	/	/	/	|
// -----------------------------------------------------------------------------

// Function to create the call to the system Function
void shellVisitor::IRFunctionSysCall(const char *nameFunction, std::vector<llvm::Type *> argTy)
{
	std::vector<llvm::Type *> argTypes = argTy; // Argument type is char*

	llvm::FunctionType *funcType = llvm::FunctionType::get(
		llvm::Type::getInt32Ty(*context), // Return type is int
		argTypes,						  // Argument types
		false							  // Is vararg?
	);

	llvm::Function *func = llvm::Function::Create(
		funcType,
		llvm::Function::ExternalLinkage,
		nameFunction, // Function name
		module.get()  // Get the raw pointer
	);
}
// Function to handle the execution of the commands
void shellVisitor::generateMainIR()
{
	int8Type = llvm::Type::getInt8Ty(*context);
	charPtrType = llvm::PointerType::get(int8Type, 0);

	// Create the function call to "system" -> int system(char*)
	IRFunctionSysCall("system", {charPtrType});
	IRFunctionSysCall("chdir", {charPtrType});

	// Create the main function ("main") -> double main()
	std::vector<llvm::Type *> paramsType(0, llvm::Type::getDoubleTy(*context));
	llvm::FunctionType *FT = llvm::FunctionType::get(
		llvm::Type::getVoidTy(*context), paramsType, false);
	F = llvm::Function::Create(
		FT, llvm::Function::ExternalLinkage, "main", module.get());
	builder->SetInsertPoint(
		llvm::BasicBlock::Create(*context, "entry", F));
}
