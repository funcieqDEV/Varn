#pragma once
#include "../ast/nodes.hpp"
#include "../token/token.hpp"
#include <vector>

class Parser {
public:
    unsigned long long int pos_;
    std::vector<Token> toks_;
    Parser() {

    }
    std::unique_ptr<RootNode>parse(std::vector<Token> toks);
    Token consume(TokenType expected, std::string err_msg="");
    //statements parsing
    std::unique_ptr<FunDecl>parseFun();
    std::vector<std::unique_ptr<ParameterNode>> parseParameters();
    std::unique_ptr<TypeNode> parseType();
    std::unique_ptr<BodyNode> parseBody();
    Token peek();
    bool isAtEnd();
};