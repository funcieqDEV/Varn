#include "lexer.hpp"
#include <cstdlib>
std::vector<Token> Lexer::tokenize(const std::string& input) {
    std::vector<Token> tokens;
    _pos = 0;
    _input = input;
    while (!isAtEnd()) {
        char _cur = _input[_pos];
        if (_cur == '('){
            tokens.push_back(Token(_pos, TokenType::LPAREN, "("));
        }
        else if (_cur == ')') {
            tokens.push_back(Token(_pos, TokenType::RPAREN, ")"));
        }
        else if (_cur == '{') {
            tokens.push_back(Token(_pos, TokenType::LBRACE, "{"));
        }
        else if (_cur == '}') {
            tokens.push_back(Token(_pos, TokenType::RBRACE, "}"));
        }
        else if (_cur == '[') {
            tokens.push_back(Token(_pos, TokenType::SQUARE_LBRACE, "["));
        }
        else if (_cur == ']') {
            tokens.push_back(Token(_pos, TokenType::SQUARE_RBRACE, "]"));
        }
        else if (_cur == ';') {
            tokens.push_back(Token(_pos, TokenType::SEMICOLON, ";"));
        }
        else if (_cur == ',') {
            tokens.push_back(Token(_pos, TokenType::COMMA, ","));
        }
        else if (_cur == ':') {
            if(Peek2() == ':') {
                _pos++;
                tokens.push_back(Token(_pos, TokenType::DOUBLECOLON, "::"));
            }else {
                tokens.push_back(Token(_pos, TokenType::COLON, ":"));
            }
        }
        else if (_cur == '.') {
            tokens.push_back(Token(_pos, TokenType::DOT, "."));
        }
        else if (_cur == '?') {
            tokens.push_back(Token(_pos, TokenType::QUESTION, "?"));
        }
        else if (_cur == '+') {
            tokens.push_back(Token(_pos, TokenType::PLUS, "+"));
        }
        else if (_cur == '*'){
            tokens.push_back(Token(_pos, TokenType::MULTIPLY, "*"));
        }
        else if (_cur == '-') {
            tokens.push_back(Token(_pos, TokenType::MINUS, "-"));
        }
        else if (_cur == '/') {
            tokens.push_back(Token(_pos, TokenType::DIVIDE, "/"));
        }
        else if (_cur == '%') {
            tokens.push_back(Token(_pos, TokenType::MODULO, "%"));
        }
        else if (_cur == '^') {
            tokens.push_back(Token(_pos, TokenType::POW, "^"));
        }
        else if (_cur == '&') {
            if(Peek2() == '&') {
                _pos++;
                tokens.push_back(Token(_pos, TokenType::AND, "&&"));
            } else {
                tokens.push_back(Token(_pos, TokenType::REFERENCE, "&"));
            }
        }
        else if (_cur == '|') {
            if (Peek2() == '|') {
                _pos++;
                tokens.push_back(Token(_pos, TokenType::OR, "||"));
            }
            else{
                printf("Lexer Error: Unexpected character '%c' at position %u\n", _cur, _pos);
                exit(EXIT_FAILURE);
            }
            tokens.push_back(Token(_pos, TokenType::OR, "|"));
        }
        else if (_cur == '=') {
            if (Peek2() == '=') {
                _pos++;
                tokens.push_back(Token(_pos, TokenType::EQUAL, "=="));
            }else {
                 tokens.push_back(Token(_pos, TokenType::ASSIGN, "="));
            }
        }
        else if (_cur == '!') {
            if (Peek2() == '=') {
                _pos++;
                tokens.push_back(Token(_pos, TokenType::LESSEQUAL, "!="));
            }else {
                   tokens.push_back(Token(_pos, TokenType::NOT, "!"));
            }
        }
        else if (_cur == '<') {
            if (Peek2() == '=') {
                _pos++;
                tokens.push_back(Token(_pos, TokenType::LESSEQUAL, "<="));
            }else {
                            tokens.push_back(Token(_pos, TokenType::LESS, "<"));
            }
        }
        else if (_cur == '>') {
            if (Peek2() == '=') {
                _pos++;
                tokens.push_back(Token(_pos, TokenType::GREATEREQUAL, ">="));
            }else {
                tokens.push_back(Token(_pos, TokenType::GREATER, ">"));
            }
        }
        else if (std::isdigit(_cur)) {
            std::string numStr;
            bool isFloat = false;
            unsigned int startPos = _pos;
            while (!isAtEnd() && std::isdigit(_input[_pos])) {
            numStr += _input[_pos++];
            }
            if (!isAtEnd() && _input[_pos] == '.') {
            isFloat = true;
            numStr += _input[_pos++];
            while (!isAtEnd() && std::isdigit(_input[_pos])) {
                numStr += _input[_pos++];
            }
            }
            if (isFloat) {
            tokens.push_back(Token(startPos, TokenType::FLOAT, numStr));
            } else {
            tokens.push_back(Token(startPos, TokenType::INTEGER, numStr));
            }
        }
        else if (std::isalpha(_cur) || _cur == '_') {
            std::string identStr;
            unsigned int startPos = _pos;
            while (!isAtEnd() && (std::isalnum(_input[_pos]) || _input[_pos] == '_')) {
                identStr += _input[_pos++];
            }
            // Check for keywords
            if (identStr == "if") {
                tokens.push_back(Token(startPos, TokenType::IF, identStr));
            } else if (identStr == "else") {
                tokens.push_back(Token(startPos, TokenType::ELSE, identStr));
            } else if (identStr == "while") {
                tokens.push_back(Token(startPos, TokenType::WHILE, identStr));
            } else if (identStr == "for") {
                tokens.push_back(Token(startPos, TokenType::FOR, identStr));
            } else if (identStr == "return") {
                tokens.push_back(Token(startPos, TokenType::RETURN, identStr));
            } else if (identStr == "true") {
                tokens.push_back(Token(startPos, TokenType::BOOL, identStr));
            } else if (identStr == "false") {
                tokens.push_back(Token(startPos, TokenType::BOOL, identStr));
            } else if (identStr == "fun") {
                tokens.push_back(Token(startPos, TokenType::FUN, identStr));
            } else if(identStr == "import"){
                tokens.push_back(Token(startPos, TokenType::IMPORT, identStr));
            } else if(identStr == "match") {
                tokens.push_back(Token(startPos, TokenType::MATCH, identStr));
            } else if(identStr == "let") {
                tokens.push_back(Token(startPos, TokenType::LET, identStr));
            } else if(identStr == "extern") {
                tokens.push_back(Token(startPos, TokenType::EXTERN, identStr));
            } else if(identStr == "module") {
                tokens.push_back(Token(startPos, TokenType::MODULE, identStr));
            } else if(identStr == "pub") {
                tokens.push_back(Token(startPos, TokenType::PUB, identStr));
            } else if(identStr == "priv") {
                tokens.push_back(Token(startPos, TokenType::PRIV, identStr));
            } else if(identStr == "struct") {
                tokens.push_back(Token(startPos, TokenType::STRUCT, identStr));
            } else if(identStr == "impl") {
                tokens.push_back(Token(startPos, TokenType::IMPL, identStr));
            } else if(identStr == "static") {
                tokens.push_back(Token(startPos, TokenType::STATIC, identStr));
            } else if(identStr == "enum") {
                tokens.push_back(Token(startPos, TokenType::ENUM, identStr));
            }
            else {
                tokens.push_back(Token(startPos, TokenType::ID, identStr));
            }
            _pos--; // adjust for the loop's extra increment
        }
        else if (std::isspace(_cur)) {
            _pos++; 
            continue; 
        }
        else {
            printf("Lexer Error: Unexpected character '%c' at position %u\n", _cur, _pos);
            exit(EXIT_FAILURE);
        }
        ++_pos;
        
    }
    return tokens;
}

char Lexer::Peek2(){
    if (_pos + 1 < _input.size()) {
        return _input[_pos + 1];
    }
    return '\0';
}

bool Lexer::isAtEnd() const {
    return _pos >= _input.size();
}

