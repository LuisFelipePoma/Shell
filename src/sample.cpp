#include <cstddef>
#include <llvm-15/llvm/IR/BasicBlock.h>
#include <llvm-15/llvm/IR/DerivedTypes.h>
#include <llvm-15/llvm/IR/IRBuilder.h>
#include <llvm-15/llvm/IR/Intrinsics.h>
#include <llvm-15/llvm/IR/LLVMContext.h>
#include <llvm-15/llvm/IR/Module.h>
#include <llvm-15/llvm/IR/Type.h>
#include <memory>
#include <vector>

using namespace llvm;

/**
 * @brief The main function of the program.
 *
 * @return int The exit status of the program.
 */
int main()
{
	// Create a new context and a new module to hold the function
	auto TheContext = std::make_unique<llvm::LLVMContext>();
	auto TheModule = std::make_unique<Module>("Mi cool jit", *TheContext);
	auto TheBuilder = std::make_unique<IRBuilder<>>(*TheContext);

	// Create the function prototype
	Type *Tys[] = {Type::getInt8PtrTy(*TheContext), Type::getInt32Ty(*TheContext)};

	// Create the function type
	std::vector<Type *> Doubles(2, Type::getDoubleTy(*TheContext));

	// Create the function type
	FunctionType *FT = FunctionType::get(Type::getDoubleTy(*TheContext), Doubles, false);

	// Create the function
	Function *Func = Function::Create(FT, Function::ExternalLinkage, "func", TheModule.get());

	// Set names for all arguments
	BasicBlock *BB = BasicBlock::Create(*TheContext, "entry", Func);
	TheBuilder->SetInsertPoint(BB);

	// Get the arguments of the function
	// Function::arg_iterator args = Func->arg_begin();
	// Value *x = args++;
	// Value *y = args++;
	Value *x = ConstantInt::get(Type::getInt32Ty(*TheContext), 1);
	Value *y = ConstantInt::get(Type::getInt32Ty(*TheContext), 2);

	// Create an addition operation
	Value *sum = TheBuilder->CreateAdd(x, y, "addtmp");

	// Set the result of the addition as the return value of the function
	TheBuilder->CreateRet(sum);

	TheModule->print(errs(), nullptr);

	return 0;
}