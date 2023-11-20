// Libraries from the system
#include <any>
#include <memory>
#include <string>

// Libraries from antlr4 files generated
#include "libs/ShellExprVisitor.h"
#include "libs/ShellExprParser.h"
#include "shellVisitor.h"

// Utils
#include "../utils/cd.h"
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
	return std::any(args);
}

// _____________________________________________________________________________
// |								operation									|
// -----------------------------------------------------------------------------

// operation : ID '=' expr                         # assign
// TODO

// operation : | 'export' ID '=' expr		       # export
// TODO
// else if (command == "export")
// {
// 	std::string assignation = ctx->cmd_suffix()->getText();
// 	std::string name = assignation.substr(0, assignation.find("="));
// 	std::string value = assignation.substr(assignation.find("=") + 1, assignation.length());
// 	export_command(name.c_str(), value.c_str());
// }
// else if (command == "echo")
// {
// 	std::string variable = ctx->cmd_suffix()->getText();
// 	std::string name = variable.substr(variable.find("$") + 1, variable.length());
// 	echo_command(name.c_str());
// }

// operation : | 'let' ID '=' expr	                # declaration
// TODO

// _____________________________________________________________________________
// |								expr										|
// -----------------------------------------------------------------------------

// expr : expr ('*'|'/') expr                     # mulDivOpe
// TODO

// expr : | expr ('+'|'-') expr                   # sumMinOpe
// TODO

// expr : | expr ('<'|'>'|'>='|'<='| '==') expr   # compOpe
// TODO

// expr : | ID                                    # idStmt
// TODO

// expr : | LIST                                  # listStmt
// TODO

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
