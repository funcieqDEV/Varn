#pragma once
#include "parser.hpp"
#include <vector>

std::unique_ptr<RootNode> Parser::parse(std::vector<Token> toks){
    this->pos_= 0;
    this->toks_ = toks;
    auto root = std::make_unique<RootNode>();
    while(!isAtEnd()){
        Token current = peek();

        if(current.type == TokenType::FUN){
            root->addChild(parseFun());
        }else {
            printf("Unexpected token: %s at position %d\n", current.value.c_str(), current.pos);
            exit(-1); // Handle unexpected tokens
        }
    }
    return root;
}
std::unique_ptr<FunDecl> Parser::parseFun() {
    auto func = std::make_unique<FunDecl>();
    consume(TokenType::FUN);
    func->name_ = consume(TokenType::ID, "Expected function name.").value;

    consume(TokenType::LPAREN, "Expected '(' after function name.");
    func->params_ = parseParameters();
    consume(TokenType::RPAREN, "Expected ')' after params");

    func->body_ = parseBody();
    return func;
}


std::unique_ptr<BodyNode> Parser::parseBody(){
    auto body = std::make_unique<BodyNode>();
    consume(TokenType::LBRACE);
    while(!isAtEnd() && peek().type != TokenType::RBRACE){
        //parseStatement should be here
    }
    consume(TokenType::RBRACE, "Expected '}' when funtion ends.");
    return body;
}
std::vector<std::unique_ptr<ParameterNode>> Parser::parseParameters(){
    std::vector<std::unique_ptr<ParameterNode>> params;
    for(;;){
        if(peek().type == TokenType::ID){
            Token name = consume(TokenType::ID);
            consume(TokenType::COLON, "Expected ':' after param name.");
            std::unique_ptr<TypeNode> type = parseType();
            auto paramNode = std::make_unique<ParameterNode>();
            paramNode->name = name.value;
            paramNode->type = std::move(type);
            params.push_back(std::move(paramNode));
            if(peek().type != TokenType::COMMA){
                break;
            }else {
                consume(TokenType::COMMA);
            }
        }else {
            consume(TokenType::RPAREN, "Expected ')' after parameters.");
        }
    }
    return params;
}

std::unique_ptr<TypeNode> Parser::parseType(){
    //for now only ID
    //todo: [], [size] for example [4], <T>
    auto typeName = consume(TokenType::ID, "type must be an identifier.");
    TypeNode type = TypeNode(typeName.value);
    return std::make_unique<TypeNode>(type);
}
Token Parser::consume(TokenType expected, std::string err_msg){
    Token tok = peek();
    if(tok.type != expected){
        printf("%s\n", err_msg.c_str());//todo: add errors to list to allow multiple error handling
        exit(-1);
    }else {
    pos_++;
    return tok;
    }
}
Token Parser::peek(){
    if(!isAtEnd()){
        return toks_[pos_];
    }else {
        printf("parser can't peek! \n");
        //todo: add errors to list to allow multiple error handling
        exit(-1);
    }
}

bool Parser::isAtEnd(){
    return this->pos_ >= this->toks_.size();
}