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
	return std::any();
}

// simple_command: | cmd_name							# cmd
std::any shellVisitor::visitCmd(ShellExprParser::CmdContext *ctx)
{
	// std::cout << "cmd_name\n";
	std::string command = ctx->cmd_name()->WORD()->getText();

	// Si el comando es "ls", ejecutar el comando del sistema
	if (command == "ls")
	{
		ls_command(".");
	}
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

//! end
// std::any shellVisitor::visitPrintExpr(shellExprParser::PrintExprContext *ctx)
// {
// 	std::cout << "visitPrintExpr\n";
// 	std::vector<double> Args;
// 	std::vector<llvm::Type *> Doubles(Args.size(),
// 									  llvm::Type::getDoubleTy(*context));
// 	llvm::FunctionType *FT = llvm::FunctionType::get(
// 		llvm::Type::getDoubleTy(*context), Doubles, false);

// 	llvm::Function *F = llvm::Function::Create(
// 		FT, llvm::Function::ExternalLinkage, "_anon_", module.get());

// 	llvm::BasicBlock *BB = llvm::BasicBlock::Create(*context, "entry", F);
// 	builder->SetInsertPoint(BB);
// 	llvm::Value *val = std::any_cast<llvm::Value *>(visit(ctx->expr()));
// 	builder->CreateRet(val);

// 	return std::any();
// 	// return visitChildren(ctx);
// }

// std::any shellVisitor::visitAssign(shellExprParser::AssignContext *ctx)
// {
// 	std::cout << "visitAssign\n";
// 	return visitChildren(ctx);
// }

// std::any shellVisitor::visitStatdef(shellExprParser::StatdefContext *ctx)
// {
// 	std::cout << "visitStatdef\n";
// 	return visitChildren(ctx);
// }

// std::any
// shellVisitor::visitStatextern(shellExprParser::StatexternContext *ctx)
// {
// 	std::cout << "visitStatextern\n";
// 	return visitChildren(ctx);
// }

// std::any shellVisitor::visitBlank(shellExprParser::BlankContext *ctx)
// {
// 	std::cout << "visitBlank\n";
// 	return visitChildren(ctx);
// }

// std::any shellVisitor::visitCall(shellExprParser::CallContext *ctx)
// {
// 	std::cout << "visitCall\n";
// 	return visitChildren(ctx);
// }

// std::any shellVisitor::visitNumber(shellExprParser::NumberContext *ctx)
// {
// 	std::cout << "visitNumber\n";
// 	auto numVal = std::stod(ctx->NUMBER()->getText());
// 	llvm::Value *val = llvm::ConstantFP::get(*context, llvm::APFloat(numVal));
// 	return std::any(val);
// 	// return visitChildren(ctx);
// }

// std::any shellVisitor::visitMulDiv(shellExprParser::MulDivContext *ctx)
// {
// 	std::cout << "visitMulDiv\n";
// 	return visitChildren(ctx);
// }

// std::any shellVisitor::visitAddSub(shellExprParser::AddSubContext *ctx)
// {
// 	std::cout << "visitAddSub\n";
// 	llvm::Value *L = std::any_cast<llvm::Value *>(visit(ctx->expr(0)));
// 	llvm::Value *R = std::any_cast<llvm::Value *>(visit(ctx->expr(1)));
// 	if (ctx->op->getType() == shellExprParser::ADD)
// 	{
// 		return std::any(builder->CreateFAdd(L, R, "addTemp"));
// 	}
// 	else
// 	{
// 		return std::any(builder->CreateFSub(L, R, "subTemp"));
// 	}
// 	// return visitChildren(ctx);
// }

// std::any shellVisitor::visitParens(shellExprParser::ParensContext *ctx)
// {
// 	std::cout << "visitParens\n";
// 	return visitChildren(ctx);
// }

// std::any shellVisitor::visitId(shellExprParser::IdContext *ctx)
// {
// 	std::cout << "visitId\n";
// 	return visitChildren(ctx);
// }

// std::any shellVisitor::visitProto(shellExprParser::ProtoContext *ctx)
// {
// 	std::cout << "visitProto\n";
// 	return visitChildren(ctx);
// }

// std::any shellVisitor::visitDef(shellExprParser::DefContext *ctx)
// {
// 	std::cout << "visitDef\n";
// 	return visitChildren(ctx);
// }

// std::any shellVisitor::visitExtern(shellExprParser::ExternContext *ctx)
// {
// 	std::cout << "visitExtern\n";
// 	return visitChildren(ctx);
// }