// Libraries from the system
#include <any>
#include <memory>
#include <string>

// Libraries from antlr4 files generated
#include "libs/ShellExprLexer.h"
#include "libs/ShellExprVisitor.h"
#include "libs/ShellExprParser.h"
#include "shellVisitor.h"

// Utils
#include "../utils/export.h"
#include "../utils/echo.h"
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

// function_definition	: DEF ID LPAREN function_args RPAREN LBRACE compound_list RBRACE		# functionDef
// TODO

// _____________________________________________________________________________
// |							function_args									|
// -----------------------------------------------------------------------------

// function_args 		: ID (',' ID)* 															# functionArgs
// TODO

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
	handleExecutionCmd(command + " " + args);

	return std::any();
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
	// Read the command name
	std::string command = ctx->cmd_name()->ID()->getText();

	// Execute the command of the system without args
	handleExecutionCmd(command);
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
	auto left = std::any_cast<std::string>(visit(ctx->expr(0)));
	auto right = std::any_cast<std::string>(visit(ctx->expr(1)));

	// Verify the operation (only accepts numbers operations for now)
	switch (ctx->opt->getType())
	{
	case ShellExprLexer::MUL:
		return std::any(std::to_string(std::stoi(left) * std::stoi(right)));
	case ShellExprLexer::DIV:
		return std::any(std::to_string(std::stoi(left) / std::stoi(right)));
	}

	return std::any();
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
	auto left = std::any_cast<std::string>(visit(ctx->expr(0)));
	auto right = std::any_cast<std::string>(visit(ctx->expr(1)));

	// Verify the operation (only accepts numbers operations for now)
	switch (ctx->opt->getType())
	{
	case ShellExprLexer::PLUS:
		return std::any(std::to_string(std::stoi(left) + std::stoi(right)));
	case ShellExprLexer::MINUS:
		return std::any(std::to_string(std::stoi(left) - std::stoi(right)));
	}
	return std::any();
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
	auto left = std::any_cast<std::string>(visit(ctx->expr(0)));
	auto right = std::any_cast<std::string>(visit(ctx->expr(1)));

	// Verify the operation (only accepts numbers operations for now)
	switch (ctx->opt->getType())
	{
	case ShellExprLexer::LESS:
		return std::any(std::to_string(std::stoi(left) < std::stoi(right)));
	case ShellExprLexer::GREAT:
		return std::any(std::to_string(std::stoi(left) > std::stoi(right)));
	case ShellExprLexer::LESS_EQ:
		return std::any(std::to_string(std::stoi(left) <= std::stoi(right)));
	case ShellExprLexer::GREAT_EQ:
		return std::any(std::to_string(std::stoi(left) >= std::stoi(right)));
	case ShellExprLexer::NOT_EQ:
		return std::any(std::to_string(std::stoi(left) != std::stoi(right)));
	case ShellExprLexer::EQUALS:
		return std::any(std::to_string(std::stoi(left) == std::stoi(right)));
	}
	return std::any();
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
	std::string id = ctx->ID()->getText();

	// Verify if the variable exists
	if (memory.find(id) != memory.end())
		// If the variable exists, return it
		return memory[id];

	// If the variable doesn't exists, return the same name
	return std::any(id);
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
	for (size_t i = 0; i < ctx->children.size(); ++i)
	{
		visit(ctx->children[i]);
	}

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

// and_or: pipeline (AND_IF pipeline | OR_IF pipeline)* #andOrBody
// TODO

// _____________________________________________________________________________
// |							    pipeline									|
// -----------------------------------------------------------------------------

// pipeline: simple_command (('|'| io_redirect) simple_command)* 		# pipelineBody
// TODO

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