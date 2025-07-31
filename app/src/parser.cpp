// src/parser.cpp
#include <cctype>
#include <stdexcept>
#include <utility>

#include "parser.hpp"
#include "ast.hpp"



// Grammar Parser

// Constructor
Parser::Parser(std::string mathExpr) : inputString(std::move(mathExpr)), currPos(0) {}

// Parse full expression and ensure end-of-string
NodePtr Parser::parse() {
    skipWhitespace();
    auto result = parseExpression();
    skipWhitespace();
    
    if ( currPos != inputString.size() ) {
        throw std::runtime_error("Unexpected: " + std::string(1, inputString[currPos]));
    }

    return result;
}

void Parser::skipWhitespace() { 
    while ( currPos < inputString.size() && isspace(inputString[currPos]) ) { 
        currPos++; 
    } 
}

// <expression> := <term> { ('+' | '-') <term> }
NodePtr Parser::parseExpression() {
    auto node = parseTerm(); 
    skipWhitespace();
    
    while ( currPos < inputString.size() && ( inputString[currPos] == '+' || inputString[currPos] == '-' ) ) {
        char op = inputString[currPos++]; 
        NodePtr rightFactor = parseTerm(); 
        node = std::make_shared<BinaryNode>(op, node, rightFactor); 
        skipWhitespace();
    }

    return node;
}

// <term> := <factor> { ('*' | '/') <factor> }
NodePtr Parser::parseTerm() {
    auto node = parseFactor(); 
    skipWhitespace();
    
    while ( currPos < inputString.size() && ( inputString[currPos] == '*' || inputString[currPos] == '/' ) ) {
        char op = inputString[currPos++]; 
        NodePtr rightFactor = parseFactor(); 
        node = std::make_shared<BinaryNode>(op, node, rightFactor); 
        skipWhitespace();
    }

    return node;
}

// <factor> := <primary> [ '^' <factor> ]
NodePtr Parser::parseFactor() {
    auto node = parseBasic();
    skipWhitespace();
    
    if ( currPos < inputString.size() && inputString[currPos] == '^' ) {
        ++currPos;
        auto exp = parseFactor();
        node = std::make_shared<PowerNode>(node, exp);
        skipWhitespace();
    }

    return node;
}

// <basic> := number | 'x' | func_call | '(' <expression> ')'
NodePtr Parser::parseBasic() {
    skipWhitespace();
    
    // Parse numeric constant
    if ( currPos < inputString.size() && isdigit(inputString[currPos]) ) {
        std::string num;
        
        while ( currPos < inputString.size() && (isdigit(inputString[currPos]) || inputString[currPos] == '.') ) {
            num += inputString[currPos++];
        }

        return std::make_shared<ConstNode>(std::stod(num));
    }

    // Parse variable 'x' or function name
    if ( currPos < inputString.size() && isalpha(inputString[currPos]) ) {
        std::string id;
        
        while ( currPos < inputString.size() && isalpha(inputString[currPos]) ) {
            id += inputString[currPos++];   
        }
        skipWhitespace();
        
        if ( id == "x" ) {
            return std::make_shared<VarNode>();
        }
        
        // Function call ( "sin" | "cos" | "tan" | "cot" | "log" )
        // name(expr)
        if ( currPos < inputString.size() && inputString[currPos] == '(' ) { 
            ++currPos;
            auto arg = parseExpression();
            skipWhitespace();
            
            if ( currPos >= inputString.size() || inputString[currPos] != ')' ) {
                throw std::runtime_error("Missing closing ')' for function call");
            }
            ++currPos;

            return std::make_shared<FuncNode>(id, std::vector<NodePtr>{arg});
        }

        throw std::runtime_error("Unknown identifier: " + id);
    }

    // Parenthesized sub-expression
    if ( currPos < inputString.size() && inputString[currPos] == '(' ) { 
        ++currPos;
        auto node = parseExpression();
        skipWhitespace(); 
        
        if ( currPos >= inputString.size() || inputString[currPos] != ')') {
            throw std::runtime_error("Missing closing ')' for sub-expression");
        }
        ++currPos;
        
        return node;
    }

    throw std::runtime_error(std::string("Unexpected char: ") + inputString[currPos]);
}