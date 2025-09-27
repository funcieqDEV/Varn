#include <vector>
#include <memory>
#include <string>
#include "../ast/nodes.hpp"
#include "llvm_h.hpp"

class Compiler{
public:
    Compiler();
    void compile(std::unique_ptr<RootNode> root);
    void compileExternFun(FunDecl* funDecl);
    void compileFun(FunDecl* funDecl);
    void compileBody(BodyNode* body);
    llvm::Value* compileExpression(ExpressionNode* expr);
private:
    llvm::LLVMContext context;
    std::unique_ptr<llvm::Module> module;
    std::unique_ptr<llvm::IRBuilder<>> builder;
    llvm::Type* VarnType2LLVMType(std::unique_ptr<TypeNode> typeNode);
    // Scopes
    std::vector<std::unordered_map<std::string, llvm::Value*>> scopes;
};