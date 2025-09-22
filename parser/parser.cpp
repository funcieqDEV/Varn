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

std::unique_ptr<VarDecl> Parser::parseVarDecl() {
    /*
        let name = <expr>;
        let name: type;
        let name: type = <expr>;
    */
    consume(TokenType::LET);
    
    std::string id = consume(TokenType::ID, "Expected variable name.").value;
    if (peek().type == TokenType::COLON) {
		advance(); // consume ':'
		auto type = parseType();
        if (peek().type == TokenType::ASSIGN) {
            advance(); 
			// consume '='
			auto expr = parseExpression();
			consume(TokenType::SEMICOLON, "Expected ';' after variable declaration.");
            return std::make_unique<VarDecl>(id, type, expr);
        } else {
			consume(TokenType::SEMICOLON, "Expected ';' after variable declaration.");
            return std::make_unique<VarDecl>(id, std::move(type), nullptr);
		}
    }
}

std::unique_ptr<ExpressionNode> Parser::parseExpression() {
    
	return parseTerm();
}

std::unique_ptr<ExpressionNode> Parser::parseTerm() {
	std::unique_ptr<ExpressionNode> left = parseFactor();
    while (peek().type == TokenType::PLUS || peek().type == TokenType::MINUS) {
        Token operatorToken = next();
        std::unique_ptr<ExpressionNode> right = parseFactor();
        left = std::make_unique<BinExpr>(std::move(left), operatorToken.value, std::move(right));
	}

    return left;
}

std::unique_ptr<ExpressionNode> Parser::parseFactor() {
    std::unique_ptr<ExpressionNode> left = parseUnary();
    while (peek().type == TokenType::MULTIPLY || peek().type == TokenType::DIVIDE) {
        Token operatorToken = next();
        std::unique_ptr<ExpressionNode> right = parseUnary();
        left = std::make_unique<BinExpr>(std::move(left), operatorToken.value, std::move(right));
    }
    return left;
}

std::unique_ptr<ExpressionNode> Parser::parseUnary() {
    if (peek().type == TokenType::MINUS || peek().type == TokenType::NOT) {
        Token operatorToken = next();
        std::unique_ptr<ExpressionNode> right = parseUnary();
        return right;
    }
    return parsePrimary();
}

std::unique_ptr<ExpressionNode> Parser::parsePrimary() {
    Token current = peek();
    if (current.type == TokenType::INTEGER) {
        advance();
        return std::make_unique<ConstInt>(std::stoi(current.value));
    } else if (current.type == TokenType::FLOAT) {
        advance();
        return std::make_unique<ConstFloat>(std::stof(current.value));
    } else if (current.type == TokenType::LPAREN) {
        advance(); // consume '('
        std::unique_ptr<ExpressionNode> expr = parseExpression();
        consume(TokenType::RPAREN, "Expected ')' after expression.");
        return expr;
    } else if (current.type == TokenType::ID) {
        advance();
        return std::make_unique<ConstId>(current.value);
    } else {
        printf("Unexpected token in primary expression: %s at position %d\n", current.value.c_str(), current.pos);
        exit(-1); // Handle unexpected tokens
    }
}

std::unique_ptr<BodyNode> Parser::parseBody(){
    auto body = std::make_unique<BodyNode>();
    consume(TokenType::LBRACE);
    while(!isAtEnd() && peek().type != TokenType::RBRACE){
		body->addStatement(parseStatement());
    }
    consume(TokenType::RBRACE, "Expected '}' when funtion ends.");
    return body;
}

std::unique_ptr<StatementNode> Parser::parseStatement() {
        if(peek().type == TokenType::LET){
            return parseVarDecl();
        }
        else {
			printf("Unexpected token in statement: %s at position %d\n", peek().value.c_str(), peek().pos);
			exit(-1); // Handle unexpected tokens
        }
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
            pos_++;
            break;
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

void Parser::advance() {
    if (!isAtEnd()) {
        pos_++;
    }
    else {
        printf("parser can't advance! \n");
    }
}

Token Parser::next() {
    if (!isAtEnd()) {
        return toks_[pos_++];
    }
    else {
        printf("parser can't get next! \n");
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