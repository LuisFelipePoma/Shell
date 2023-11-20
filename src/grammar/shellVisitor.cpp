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
// start : comand EOF;
std::any shellVisitor::visitStart(ShellExprParser::StartContext *ctx)
{
	// std::cout << "visitProg\n";
	return visitChildren(ctx);
}

// _____________________________________________________________________________
// |								command										|
// -----------------------------------------------------------------------------

// command : simple_command   							# simpleStmt
std::any shellVisitor::visitSimpleStmt(ShellExprParser::SimpleStmtContext *ctx)
{
	// std::cout << "simple Comand\n";
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

// _____________________________________________________________________________
// |							simple_command									|
// -----------------------------------------------------------------------------

// simple_command: cmd_name args						# cmdArgs
std::any shellVisitor::visitCmdArgs(ShellExprParser::CmdArgsContext *ctx)
{
	// std::cout << "cmd_name cmd_suffix\n";
	std::string command = ctx->cmd_name()->ID()->getText();
	auto args = std::any_cast<std::string>(visit(ctx->args()));

	handleExecutionCmd(command + " " + args);

	return std::any();
}

// simple_command: | cmd_name							# cmd
std::any shellVisitor::visitCmd(ShellExprParser::CmdContext *ctx)
{
	// std::cout << "cmd_name\n";
	std::string command = ctx->cmd_name()->ID()->getText();

	handleExecutionCmd(command);
	return std::any();
}

// simple_command: | operation                         # operationStmt
std::any shellVisitor::visitOperationStmt(ShellExprParser::OperationStmtContext *ctx)
{
	visit(ctx->operation());
	return std::any();
}

// _____________________________________________________________________________
// |								  args									   |
// -----------------------------------------------------------------------------

// args: ID+								# argsBody
std::any shellVisitor::visitArgsBody(ShellExprParser::ArgsBodyContext *ctx)
{
	std::string args;
	for (auto i : ctx->ID())
	{
		args.append((i->getText()));
		args.append(" ");
	}
	args.pop_back();
	return std::any(args);
}

// _____________________________________________________________________________
// |								operation									|
// -----------------------------------------------------------------------------

// operation : ID '=' expr                         # assign
std::any shellVisitor::visitAssing(ShellExprParser::AssingContext *ctx)
{
	std::string assignation = ctx->ID()->getText();
	auto value = std::any_cast<std::string>(visit(ctx->expr()));
	if (memory.find(assignation) == memory.end())
	{
		std::cout << "Variable not found..." << std::endl;
	}
	else
		memory[assignation] = value;
	return std::any();
}

// operation : | 'export' ID '=' expr		       # export
std::any shellVisitor::visitExport(ShellExprParser::ExportContext *ctx)
{
	std::string id = ctx->ID()->getText();

	std::any valueAny = visit(ctx->expr());
	if (valueAny.type() == typeid(std::vector<std::any>))
	{
		// Handle vector case
		std::cout << "Cannot export a vector\n";
	}
	else if (valueAny.type() == typeid(std::string))
	{
		std::string value = std::any_cast<std::string>(valueAny);
		export_command(id.c_str(), value.c_str());
	}
	else
	{
		// Handle other types or throw an error
		std::cout << "Unsupported type for export\n";
	}

	return std::any();
}

// operation : | 'let' ID '=' expr	                # declaration
std::any shellVisitor::visitDeclaration(ShellExprParser::DeclarationContext *ctx)
{
	std::string id = ctx->ID()->getText();
	auto value = visit(ctx->expr());
	if (memory.find(id) != memory.end())
	{
		std::cout << "Variable already exists..." << std::endl;
	}
	else
		memory.insert(std::pair<std::string, std::any>(id, value));
	return std::any();
}

// _____________________________________________________________________________
// |								expr										|
// -----------------------------------------------------------------------------

// expr : expr ('*'|'/') expr                     # mulDivOpe
std::any shellVisitor::visitMulDivOpe(ShellExprParser::MulDivOpeContext *ctx)
{
	auto left = std::any_cast<std::string>(visit(ctx->expr(0)));
	auto right = std::any_cast<std::string>(visit(ctx->expr(1)));

	switch (ctx->opt->getType())
	{
	case ShellExprLexer::MUL:
		return std::any(std::to_string(std::stoi(left) * std::stoi(right)));
	case ShellExprLexer::DIV:
		return std::any(std::to_string(std::stoi(left) / std::stoi(right)));
	}

	return std::any();
}

// expr : | expr ('+'|'-') expr                   # sumMinOpe
std::any shellVisitor::visitSumMinOpe(ShellExprParser::SumMinOpeContext *ctx)
{
	auto left = std::any_cast<std::string>(visit(ctx->expr(0)));
	auto right = std::any_cast<std::string>(visit(ctx->expr(1)));

	switch (ctx->opt->getType())
	{
	case ShellExprLexer::PLUS:
		return std::any(std::to_string(std::stoi(left) + std::stoi(right)));
	case ShellExprLexer::MINUS:
		return std::any(std::to_string(std::stoi(left) - std::stoi(right)));
	}
	return std::any();
}

// expr : | expr ('<'|'>'|'>='|'<='| '==') expr   # compOpe
std::any shellVisitor::visitCompOpe(ShellExprParser::CompOpeContext *ctx)
{
	auto left = std::any_cast<std::string>(visit(ctx->expr(0)));
	auto right = std::any_cast<std::string>(visit(ctx->expr(1)));

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
	case ShellExprLexer::EQUALS:
		return std::any(std::to_string(std::stoi(left) == std::stoi(right)));
	}
	return std::any();
}

// expr : | ID                                    # idStmt
std::any shellVisitor::visitIdStmt(ShellExprParser::IdStmtContext *ctx)
{
	std::string id = ctx->ID()->getText();
	if (memory.find(id) != memory.end())
		return memory[id];
	return std::any(id);
}

// expr : | LIST                                  # listStmt
std::any shellVisitor::visitListStmt(ShellExprParser::ListStmtContext *ctx)
{
	std::string list = ctx->LIST()->getText();
	std::vector<std::any> arr;
	// parse "list" to "arr"
	// Extract the list part of the string
	std::size_t start = list.find('[') + 1;
	std::size_t end = list.find(']');
	std::string listPart = list.substr(start, end - start);

	// Split the list part by commas
	std::istringstream iss(listPart);
	for (std::string s; std::getline(iss, s, ',');)
	{
		// Convert each element to int and store it as std::any
		arr.push_back(std::stoi(s));
	}

	return std::any(arr);
}

// _____________________________________________________________________________
// |							compound_list									|
// -----------------------------------------------------------------------------

// compound_list: (command|and_or) (separator (command|and_or))* separator? # compoundListBody
// TODO

// _____________________________________________________________________________
// |							  for_clause									|
// -----------------------------------------------------------------------------

// for_clause: FOR ID IN ID+ do_group          # forBody
// TODO

// _____________________________________________________________________________
// |							  brace_group									|
// -----------------------------------------------------------------------------

// brace_group: LBRACE compound_list RBRACE    # braceBody
// TODO

// _____________________________________________________________________________
// |								do_group									|
// -----------------------------------------------------------------------------

// do_group: DO compound_list DONE             # doBody
// TODO

// _____________________________________________________________________________
// |								and_or										|
// -----------------------------------------------------------------------------

// and_or: pipeline (AND_IF pipeline | OR_IF pipeline)* #andOrBody
// TODO

// _____________________________________________________________________________
// |							    pipeline									|
// -----------------------------------------------------------------------------

// pipeline: simple_command (('|'| io_redirect) simple_command)* #pipelineBody
// TODO

// _____________________________________________________________________________
// |								if_clause									|
// -----------------------------------------------------------------------------

// if_clause: IF expr DO compound_list else_part DONE         #ifElseBody
// TODO

// if_clause: | IF expr do_group                              #ifBody
// TODO

// _____________________________________________________________________________
// |								else_part									|
// -----------------------------------------------------------------------------

// else_part : ELSE IF expr DO command* else_part              #elseIfBody
// TODO

// else_part : | ELSE compound_list                            #elseBody
// TODO

// _____________________________________________________________________________
// |							   while_clause									|
// -----------------------------------------------------------------------------

// while_clause: WHILE expr do_group                   #whileBody
// TODO
