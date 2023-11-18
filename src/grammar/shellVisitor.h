// #pragma once
#ifndef __PCDOS_VISITOR_IMPL__
#define __PCDOS_VISITOR_IMPL__

// Include the missing file here
#include "libs/ShellExprBaseVisitor.h"
#include "libs/ShellExprParser.h"

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
		  builder(std::make_unique<llvm::IRBuilder<>>(*context)) {}

	// start
	virtual std::any visitStart(ShellExprParser::StartContext *ctx) override;
	// complete_comand
	// TODO
	// list
	// TODO
	// command
	virtual std::any visitSimpleStmt(ShellExprParser::SimpleStmtContext *ctx) override;
	// compound_command
	// TODO
	// simple_command
	virtual std::any visitCmdSuff(ShellExprParser::CmdSuffContext *ctx) override;
	virtual std::any visitCmd(ShellExprParser::CmdContext *ctx) override;
	// cmd_prefix
	// TODO
	// cmd_suffix
	virtual std::any visitCmdSuffBody(ShellExprParser::CmdSuffBodyContext *ctx) override;

	// Functions of the Class Shell Visitor
	void test()
	{
		std::error_code error;
		llvm::raw_fd_stream outLL("test.ll", error);
		module->print(outLL, nullptr);
	}

private:
	std::map<std::string, std::any> memory;
	std::unique_ptr<llvm::LLVMContext> context;
	std::unique_ptr<llvm::Module> module;
	std::unique_ptr<llvm::IRBuilder<>> builder;
};

#endif