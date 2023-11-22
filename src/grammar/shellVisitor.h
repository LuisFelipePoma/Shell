// #pragma once
#ifndef __PCDOS_VISITOR_IMPL__
#define __PCDOS_VISITOR_IMPL__

// Include the missing file here
#include "libs/ShellExprBaseVisitor.h"
#include "libs/ShellExprLexer.h"
#include "libs/ShellExprParser.h"

#include "antlr4-runtime.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include <algorithm>
#include <any>
#include <iostream>
#include <llvm/Support/raw_ostream.h>
#include <map>
#include <memory>
#include <string>
#include <system_error>

/* added the Impl at the end of the class to avoid it being .gitignored sorry */
class shellVisitor : ShellExprBaseVisitor
{
public:
	shellVisitor()
		: context(std::make_unique<llvm::LLVMContext>()),
		  module(std::make_unique<llvm::Module>("LaPC2", *context)),
		  builder(std::make_unique<llvm::IRBuilder<>>(*context)),
		  isPipeline(false) {}

	// _____________________________________________________________________________
	// |	/	/	/	/	/	VISITORS FUNCTIONS	/	/	/	/	/	/	/	/	|
	// -----------------------------------------------------------------------------

	// <------------------- start ------------------------->
	virtual std::any visitStart(ShellExprParser::StartContext *ctx) override;

	// <------------------- command ----------------------->
	virtual std::any visitSimpleStmt(ShellExprParser::SimpleStmtContext *ctx) override;

	// <------------------- compound_command -------------->
	// TODO

	// <------------------- function_definition ----------->
	virtual std::any visitFunctionDef(ShellExprParser::FunctionDefContext *ctx) override;

	// <------------------- function_args ----------------->
	virtual std::any visitFunctionArgs(ShellExprParser::FunctionArgsContext *ctx) override;

	// <------------------- simple_command ---------------->
	virtual std::any visitCmdArgs(ShellExprParser::CmdArgsContext *ctx) override;
	virtual std::any visitCmd(ShellExprParser::CmdContext *ctx) override;
	virtual std::any visitOperationStmt(ShellExprParser::OperationStmtContext *ctx) override;
	// <------------------- args -------------------------->
	virtual std::any visitArgsBody(ShellExprParser::ArgsBodyContext *ctx) override;

	// <------------------- operation --------------------->
	virtual std::any visitAssing(ShellExprParser::AssingContext *ctx) override;
	virtual std::any visitExport(ShellExprParser::ExportContext *ctx) override;
	virtual std::any visitDeclaration(ShellExprParser::DeclarationContext *ctx) override;
	virtual std::any visitShow(ShellExprParser::ShowContext *ctx) override;
	virtual std::any visitCallFunction(ShellExprParser::CallFunctionContext *ctx) override;

	// <------------------- expr -------------------------->
	virtual std::any visitMulDivOpe(ShellExprParser::MulDivOpeContext *ctx) override;
	virtual std::any visitSumMinOpe(ShellExprParser::SumMinOpeContext *ctx) override;
	virtual std::any visitCompOpe(ShellExprParser::CompOpeContext *ctx) override;
	virtual std::any visitIdStmt(ShellExprParser::IdStmtContext *ctx) override;
	virtual std::any visitListStmt(ShellExprParser::ListStmtContext *ctx) override;

	// <------------------- compound_list ----------------->
	virtual std::any visitCompoundListBody(ShellExprParser::CompoundListBodyContext *ctx) override;

	// <------------------- for_clause -------------------->
	virtual std::any visitForBody(ShellExprParser::ForBodyContext *ctx) override;

	// <------------------- brace_group ------------------->
	// TODO

	// <------------------- do_group ---------------------->
	// TODO

	// <------------------- and_or ------------------------>
	virtual std::any visitAndOrBody(ShellExprParser::AndOrBodyContext *ctx) override;

	// <------------------- pipeline ---------------------->
	virtual std::any visitPipelineBody(ShellExprParser::PipelineBodyContext *ctx) override;

	// <------------------- if_clause --------------------->
	virtual std::any visitIfElseBody(ShellExprParser::IfElseBodyContext *ctx) override;
	virtual std::any visitIfBody(ShellExprParser::IfBodyContext *ctx) override;

	// <------------------- else_part --------------------->
	virtual std::any visitElseIfBody(ShellExprParser::ElseIfBodyContext *ctx) override;
	virtual std::any visitElseBody(ShellExprParser::ElseBodyContext *ctx) override;

	// <------------------- while_clause ------------------>
	virtual std::any visitWhileBody(ShellExprParser::WhileBodyContext *ctx) override;

	// _____________________________________________________________________________
	// |	/	/	/	/	/	BUILD-IN FUNCTIONS	/	/	/	/	/	/	/	/	|
	// -----------------------------------------------------------------------------
	// Functions of the Class Shell Visitor
	void test()
	{
		std::error_code error;
		llvm::raw_fd_stream outLL("test.ll", error);
		module->print(outLL, nullptr);
	}

private:
	struct Function
	{
		std::vector<std::string> args;
	};

	bool isAndOr;
	bool isPipeline;
	// Create a map to store variables
	std::map<std::string, std::any> memory;
	// Create a map to store functions
	std::map<std::string, Function> functions;
	std::unique_ptr<llvm::LLVMContext> context;
	std::unique_ptr<llvm::Module> module;
	std::unique_ptr<llvm::IRBuilder<>> builder;
};

#endif