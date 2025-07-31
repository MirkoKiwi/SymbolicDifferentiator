#ifndef PARSER_HPP
#define PARSER_HPP

#include <string>

#include "ast.hpp" 



/**
 * @brief Recursive descent parser for math expressions in BNF.
 *
 * Takes a math expression and builds an AST
 * The parser handles basic arithmetic,
 * exponentiation, parentheses, trig functions
 */
class Parser {
public:
    explicit Parser(std::string mathExpr);

    NodePtr parse();

private:
    std::string inputString; // Input expression string.
    size_t currPos;

    void skipWhitespace();

    // <expression> ::= <term> ( ( "+" | "-" ) <term> )*
    NodePtr parseExpression();

    // <term> ::= <factor> { ('*' | '/') <factor> }
    NodePtr parseTerm();

    // <factor> ::= <basic> ( "^" <basic> )*
    NodePtr parseFactor();

    // <basic> ::= <number> | 'x' | <func_call> | '(' <expression> ')'
    NodePtr parseBasic();
};



#endif // PARSER_HPP