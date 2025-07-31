// src/differentiator.cpp

#include "differentiator.hpp"
#include "parser.hpp"
#include "ast.hpp"



// Returns (f, f', f'') as tuple
std::tuple<Func, Func, Func> differentiate(const std::string &mathExpr) {
    Parser parser(mathExpr);

    NodePtr originalTree = parser.parse();
    NodePtr firstDerivativeTree = originalTree -> deriv();
    NodePtr secondDerivativeTree = firstDerivativeTree -> deriv();
    
    Func f = [originalTree](Complex x) {
        return originalTree -> eval(x);
    };
    
    Func f1 = [firstDerivativeTree](Complex x) {
        return firstDerivativeTree -> eval(x);
    };
    
    Func f2 = [secondDerivativeTree](Complex x) {
        return secondDerivativeTree -> eval(x);
    };
    
    return std::make_tuple(f, f1, f2);
}
