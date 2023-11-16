#include "IR.h"
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/Support/raw_ostream.h>
#include <memory>
#include <string>
#include <system_error>

// private
void IR::moduleInit()
{
	context = std::make_unique<llvm::LLVMContext>();
	module = std::make_unique<llvm::Module>("MyExample", *context);
	builder = std::make_unique<llvm::IRBuilder<>>(*context);
}

void IR::saveModuleToFile(const std::string &fileName)
{
	std::error_code errorCode;
	llvm::raw_fd_stream outLL(fileName, errorCode);
	module->print(outLL, nullptr);
}

// public
IR::IR()
{
	moduleInit();
}

void IR::exec(const std::string &program)
{
	saveModuleToFile("./out.ll");
}