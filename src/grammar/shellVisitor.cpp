// Libraries from the system
#include <any>
#include <memory>
#include <string>

// Libraries from antlr4 files generated
#include "libs/ShellExprVisitor.h"
#include "libs/ShellExprParser.h"
#include "shellVisitor.h"

// Utils
#include "../utils/ls.h"
#include "../utils/cd.h"
#include "../utils/export.h"
#include "../utils/echo.h"
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

// simple_command: cmd_prefix cmd_word cmd_suffix	    # cmdPrefWordSuff
// TODO

// simple_command: | cmd_prefix cmd_word				# cmdPrefWord
// TODO

// simple_command: | cmd_prefix						    # cmdPref
// TODO

// simple_command: | cmd_name cmd_suffix				# cmdSuff
std::any shellVisitor::visitCmdSuff(ShellExprParser::CmdSuffContext *ctx)
{
	// std::cout << "cmd_name cmd_suffix\n";
	std::string command = ctx->cmd_name()->WORD()->getText();
	auto suffix = visit(ctx->cmd_suffix());
	if (command == "cd")
	{
		changeDirectory(std::any_cast<std::string>(suffix));
	}
	else if (command == "export")
	{
		std::string assignation = ctx->cmd_suffix()->getText();
		std::string name = assignation.substr(0, assignation.find("="));
		std::string value = assignation.substr(assignation.find("=") + 1, assignation.length());
		export_command(name.c_str(), value.c_str());
	}
	else if (command == "echo")
	{
		std::string variable = ctx->cmd_suffix()->getText();
		std::string name = variable.substr(variable.find("$") + 1, variable.length());
		echo_command(name.c_str());
	}

	return std::any();
}

// simple_command: | cmd_name							# cmd
std::any shellVisitor::visitCmd(ShellExprParser::CmdContext *ctx)
{
	// std::cout << "cmd_name\n";
	std::string command = ctx->cmd_name()->WORD()->getText();

	// Si el comando es "ls", ejecutar el comando del sistema
	std::system(command.c_str());
	return std::any();
}

// _____________________________________________________________________________
// |							cmd_prefix										|
// -----------------------------------------------------------------------------

// cmd_prefix : (io_redirect | assignment_word) (cmd_prefix)?
// TODO

// _____________________________________________________________________________
// |							cmd_suffix										|
// -----------------------------------------------------------------------------

// cmd_suffix : (io_redirect | WORD) (cmd_suffix)? 		# cmdSuffBody
std::any shellVisitor::visitCmdSuffBody(ShellExprParser::CmdSuffBodyContext *ctx)
{
	if (ctx->children.size() == 1)
	{
		return std::any(ctx->WORD()->getText());
	}
	return std::any();
}