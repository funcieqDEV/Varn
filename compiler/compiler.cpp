#include "compiler.hpp"
#include "../ast/nodes.hpp"
#include "../ast/const/const_id.hpp"
#include "llvm_h.hpp"
#include <unordered_map>
#include <vector>
using namespace llvm;
Compiler::Compiler(){
    this->module = std::make_unique<llvm::Module>("Main", context);
    this->builder = std::make_unique<llvm::IRBuilder<>>(context);
    scopes.emplace_back();
}
void Compiler::compile(std::unique_ptr<RootNode> root){

    for(auto & child : root->children){
        if(auto funDecl = dynamic_cast<FunDecl*>(child.get())){
            if(funDecl->isExtern_){
                compileExternFun(funDecl);
            }
            else {
                compileFun(funDecl);
            }
        }
    }
std::error_code EC;
llvm::raw_fd_ostream outFile("temp.ll", EC, llvm::sys::fs::OF_None);
if (EC) {
    llvm::errs() << "Could not open temp.ll for writing: " << EC.message() << "\n";
    return;
}
this->module->print(outFile, nullptr);
outFile.close();

int ret = std::system("clang temp.ll -o output");
if (ret != 0) {
    llvm::errs() << "Clang compilation failed.\n";
}
}

void Compiler::compileExternFun(FunDecl* funDecl){
    std::vector<llvm::Type*> argTypes;
    bool isVarArg = false;
    if (!funDecl->params_.empty() && funDecl->params_.back()->type && funDecl->params_.back()->type->isVarArgs) {
        isVarArg = true;
     
        for (size_t i = 0; i < funDecl->params_.size() - 1; ++i) {
            argTypes.push_back(VarnType2LLVMType(std::make_unique<TypeNode>(*funDecl->params_[i]->type)));
        }
    } else {
        for (const auto& param : funDecl->params_) {
            argTypes.push_back(VarnType2LLVMType(std::make_unique<TypeNode>(*param->type)));
        }
    }
    llvm::FunctionType *funcType = llvm::FunctionType::get(
        VarnType2LLVMType(std::make_unique<TypeNode>(*funDecl->returnType_)),
        argTypes,
        isVarArg
    );

    llvm::Function *function = llvm::Function::Create(
        funcType,
        llvm::Function::ExternalLinkage,
        funDecl->name_,
        module.get()
    );
}

void Compiler::compileFun(FunDecl* funDecl){
    std::vector<llvm::Type*> argTypes;
    for (const auto& param : funDecl->params_) {
        argTypes.push_back(VarnType2LLVMType(std::make_unique<TypeNode>(*param->type)));
    }
    llvm::FunctionType *funcType = llvm::FunctionType::get(
        VarnType2LLVMType(std::make_unique<TypeNode>(*funDecl->returnType_)),
        argTypes,
        false 
    );

    llvm::Function *function = llvm::Function::Create(
        funcType,
        llvm::Function::ExternalLinkage,
        funDecl->name_,
        module.get()
    );

    unsigned idx = 0;

    scopes.emplace_back();
    for (auto& arg : function->args()) {
        arg.setName(funDecl->params_[idx]->name);
      
        scopes.back()[funDecl->params_[idx]->name] = &arg;
        idx++;
    }

    llvm::BasicBlock *entry = llvm::BasicBlock::Create(context, "entry", function);
    builder->SetInsertPoint(entry);

    if (funDecl->body_) {
        compileBody(funDecl->body_.get());
    }
    llvm::verifyFunction(*function);
  
    scopes.pop_back();
}
void Compiler::compileBody(BodyNode* body) {
   
    scopes.emplace_back();
    for (auto& stmt : body->statements) {
        if (auto funCall = dynamic_cast<FunCall*>(stmt.get())) {
            std::vector<llvm::Value*> args;
            for (auto& argExpr : funCall->arguments_) {
                llvm::Value* val = compileExpression(argExpr.get());
                if (val) {
                    args.push_back(val);
                } else {
                    llvm::errs() << "Unsupported argument type in function call.\n";
                }
            }
            llvm::Function* callee = module->getFunction(funCall->funcName_);
            if (callee) {
                builder->CreateCall(callee, args);
            } else {
                llvm::errs() << "Function not found: " << funCall->funcName_ << "\n";
            }
        }
        else if (auto retStmt = dynamic_cast<ReturnNode*>(stmt.get())) {
            if (retStmt->returnValue) {
                llvm::Value* retVal = compileExpression(retStmt->returnValue.get());
                if (retVal) {
                    builder->CreateRet(retVal);
                } else {
                    llvm::errs() << "Unsupported return expression type.\n";
                }
            } else {
                builder->CreateRetVoid();
            }
        }
        else if (auto varDecl = dynamic_cast<VarDecl*>(stmt.get()))
        {
            llvm::Type* varType = VarnType2LLVMType(std::make_unique<TypeNode>(*varDecl->type_));
            llvm::AllocaInst* alloca = builder->CreateAlloca(varType, nullptr, varDecl->name_);
           
            scopes.back()[varDecl->name_] = alloca;
            if (varDecl->initializer_) {
                llvm::Value* initVal = compileExpression(varDecl->initializer_.get());
                if (initVal) {
                    builder->CreateStore(initVal, alloca);
                } else {
                    llvm::errs() << "Unsupported initializer expression type.\n";
                }
            }
        }
        else {
            llvm::errs() << "Unsupported statement type in function body.\n";
        }
    }
    scopes.pop_back();
}
llvm::Value* Compiler::compileExpression(ExpressionNode* expr) {
    if (auto constStr = dynamic_cast<ConstString*>(expr)) {
        return builder->CreateGlobalStringPtr(constStr->value_);
    } else if (auto constInt = dynamic_cast<ConstInt*>(expr)) {
        if (constInt->typeName_ == "i64") {
            return llvm::ConstantInt::get(llvm::Type::getInt64Ty(context), constInt->value_);
        } else {
            return llvm::ConstantInt::get(llvm::Type::getInt32Ty(context), constInt->value_);
        }
    } else if (auto constId = dynamic_cast<ConstId*>(expr)) {
        for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
            auto found = it->find(constId->value_);
            if (found != it->end()) {
                llvm::Value* val = found->second;
                
                if (auto alloca = llvm::dyn_cast<llvm::AllocaInst>(val)) {
                    return builder->CreateLoad(alloca->getAllocatedType(), alloca, constId->value_);
                } else if (llvm::isa<llvm::Argument>(val)) {
                    return val;
                } else {
                    llvm::errs() << "Unknown variable value type for: " << constId->value_ << "\n";
                    return nullptr;
                }
            }
        }
        llvm::errs() << "Variable not found: " << constId->value_ << "\n";
        return nullptr;
    } else if (auto funCall = dynamic_cast<FunCall*>(expr)) {
        std::vector<llvm::Value*> args;
        for (auto& argExpr : funCall->arguments_) {
            llvm::Value* val = compileExpression(argExpr.get());
            if (val) {
                args.push_back(val);
            } else {
                llvm::errs() << "Unsupported argument type in nested function call.\n";
            }
        }
        llvm::Function* callee = module->getFunction(funCall->funcName_);
        if (callee) {
            return builder->CreateCall(callee, args);
        } else {
            llvm::errs() << "Function not found: " << funCall->funcName_ << "\n";
            return nullptr;
        }
    }
    // Support for binary operations: +, -, *, /
    else if (auto binOp = dynamic_cast<BinExpr*>(expr)) {
        llvm::Value* lhs = compileExpression(binOp->left_.get());
        llvm::Value* rhs = compileExpression(binOp->right_.get());
        if (!lhs || !rhs) {
            llvm::errs() << "Invalid operands for binary operation.\n";
            return nullptr;
        }
    
        llvm::Type* lhsType = lhs->getType();
        llvm::Type* rhsType = rhs->getType();
        if (lhsType != rhsType) {
           
            if (lhsType->isIntegerTy(64) && rhsType->isIntegerTy(32)) {
                rhs = builder->CreateSExt(rhs, lhsType, "sexttmp");
                rhsType = lhsType;
            } else if (lhsType->isIntegerTy(32) && rhsType->isIntegerTy(64)) {
                lhs = builder->CreateSExt(lhs, rhsType, "sexttmp");
                lhsType = rhsType;
            } else {
                llvm::errs() << "Unsupported operand type combination in binary operation.\n";
                return nullptr;
            }
        }
        if (binOp->op_ == "+") {
            return builder->CreateAdd(lhs, rhs, "addtmp");
        } else if (binOp->op_ == "-") {
            return builder->CreateSub(lhs, rhs, "subtmp");
        } else if (binOp->op_ == "*") {
            return builder->CreateMul(lhs, rhs, "multmp");
        } else if (binOp->op_ == "/") {
            return builder->CreateSDiv(lhs, rhs, "divtmp");
        } else {
            llvm::errs() << "Unsupported binary operator: " << binOp->op_ << "\n";
            return nullptr;
        }
    }
    else {
        llvm::errs() << "Unsupported expression type.\n";
        return nullptr;
    }
}



llvm::Type* Compiler::VarnType2LLVMType(std::unique_ptr<TypeNode> typeNode) {
    llvm::Type* baseType = nullptr;
    if (typeNode->typeName == "i32") {
        baseType = llvm::Type::getInt32Ty(context);
    } else if (typeNode->typeName == "i8") {
        baseType = llvm::Type::getInt8Ty(context);
    } else if (typeNode->typeName == "str") {
        // str is always a pointer to i8
        return llvm::PointerType::getUnqual(llvm::IntegerType::get(context, 8));
    } else if (typeNode->typeName == "usize") {
        baseType = llvm::Type::getInt64Ty(context);
    } else if (typeNode->typeName == "i16") {
        baseType = llvm::Type::getInt16Ty(context);
    } else if (typeNode->typeName == "i1" || typeNode->typeName == "bool") {
        baseType = llvm::Type::getInt1Ty(context);
    } else if (typeNode->typeName == "i64") {
        baseType = llvm::Type::getInt64Ty(context);
    } else if (typeNode->typeName == "f64") {
        baseType = llvm::Type::getDoubleTy(context);
    } else if (typeNode->typeName == "f32") {
        baseType = llvm::Type::getFloatTy(context);
    } else if (typeNode->typeName == "ch") {
        baseType = llvm::Type::getInt8Ty(context);
    } else if (typeNode->typeName == "void") {
        baseType = llvm::Type::getVoidTy(context);
    } else {
        printf("Unknown type: %s\n", typeNode->typeName.c_str());
        exit(-1);
    }

    // Handle pointer/reference types (except for 'str', which is handled above)
    if ( typeNode->typeName != "str" && typeNode->isPointer || typeNode->isReference) {
        // Special case: pointer to void
        if (typeNode->typeName == "void") {
            return llvm::PointerType::getUnqual(llvm::Type::getVoidTy(context));
        }
        return llvm::PointerType::getUnqual(baseType);
    }
    return baseType;
}