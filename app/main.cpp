// Implementing symbolic differentiation for complex-valued functions

// Inspired by "Finding Derivatives" Kata on CodeWars


// --------------------------------------------------------------------------------------------------------------------------------------------------------------------
// --- INPUT TYPE ---
// --------------------------------------------------------------------------------------------------------------------------------------------------------------------
// - The input is a string which is described by the following BNF (Backus-Naur Form): -
// --------------------------------------------------------------------------------------------------------------------------------------------------------------------
// <constant>  ::= [0-9]+ ( "." [0-9]+ )?
// <variable>  ::= "x"
// <func_name> ::= "sin" | "cos" | "tan" | "cot" | "log"
// 
// <expression> ::= <term> ( ( "+" | "-" ) <term> )*
// <term>       ::= <factor> ( ( "*" | "/" ) <factor> )*
// <factor>     ::= <basic> ( "^" <basic> )*
// <func_call>  ::= <func_name> "(" <expression> ")"
// <basic>      ::= <constant> | <variable> | <func_call> | ( "(" <expression> ")" )
// --------------------------------------------------------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------------------------------------------------------
// --- OUTPUT TYPE ---
// --------------------------------------------------------------------------------------------------------------------------------------------------------------------
// - The outputs are the parsed function, its first and second derivatives. All of them should receive and return a complex number. For example: -
// --------------------------------------------------------------------------------------------------------------------------------------------------------------------
// const auto f = differentiate("2 * x^3");
// 
// std::get<0>(f)({ 2, 2 }) == (-32, 32)
// /*
//   Because: 
//   f(x) = 2 * x^3
//   f(2 + 2i) = 2 * (2 + 2i)^3 = -32 + 32i
// */
// 
// std::get<1>(f)({ 2, 2 }) == (0, 48)
// /*
//   Because: 
//   f'(x) = (2 * x^3)' = 6 * x^2
//   f'(2 + 2i) = 6 * (2 + 2i)^2 = 48i
// */
// 
// std::get<2>(f)({ 2, 2 }) == (24, 24)
// /*
//   Because:
//   f''(x) = (6 * x^2)' = 12 * x
//   f''(2 + 2i) = 12 * (2 + 2i)^2 = 24 + 24i
// */
// ---------------------------------------------------------------------------------------------------------------------------



/*
1. **Parsing**: Convert the input string into an Abstract Syntax Tree (AST) using a recursive-descent parser.  
   - The grammar supports constants, variable 'x', function calls ('sin', 'cos', 'tan', 'cot', 'log'), binary operations ('+', '-', '*', '/'), and exponentiation ('^').

2. **AST Nodes**: Each node type ('ConstNode', 'VarNode', 'BinaryNode', 'PowerNode', 'FuncNode') implements two methods:
   - 'eval(x)': Evaluate the expression represented by the subtree for a complex input 'x'.
   - 'deriv()': Symbolically compute and return a new AST representing the derivative.

3. **Differentiation Rules**:
   - **Linearity**: " (f ± g)' = f' ± g' "
   - **Product Rule**: " (f·g)' = f'·g + f·g' "
   - **Quotient Rule**: " (f/g)' = (f'·g - f·g') / g² "
   - **Chain Rule**: " (f∘g)' = f'(g(x)) · g'(x) "
   - **Power Rule (General)**: " d(u^v) = u^v · [v'·ln(u) + v·(u'/u)] "

4. **Lambda Wrapping**: After building the AST for f, f', and f", wrap each in a 'std::function<Complex(Complex)>' lambda that invokes 'eval(x)'.  

5. **Usage**: 'differentiate(expr)' returns a tuple (f, f', f''), each callable on complex inputs.
*/


// main.cpp
#include <iostream>
#include <vector>
#include <string>

#include "differentiator.hpp"



void printUsage();


int main(int argc, char *argv[]) {
    // Expected: (program, expr, real) or (program, expr, real, imag).
    if ( argc != 3 && argc != 4 ) {
        printUsage();
        return 1;
    }

    try {
        std::string mathExpr = argv[1];

        long double real = std::stold(argv[2]);
        long double imag = (argc == 4) ? std::stold(argv[3]) : 0.0; 

        Complex z(real, imag);


        std::cout << "Function: f(x) = " << mathExpr << std::endl;
        std::cout << "Point:    z    = " << z << std::endl;
        std::cout << "------------------------------------" << std::endl;

        auto [f, f1, f2] = differentiate(mathExpr);
        
        // Evaluate and print
        std::cout << "f(z)   = " << f(z) << std::endl;
        std::cout << "f'(z)  = " << f1(z) << std::endl;
        std::cout << "f''(z) = " << f2(z) << std::endl;
    }
    catch (const std::invalid_argument &e) {
        std::cerr << "Error: Invalid number format for the point. Please provide valid numbers." << std::endl;
        printUsage();
        return 1;
    }
    catch (const std::runtime_error &e) {
        std::cerr << "An error occurred: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}



void printUsage() {
    std::cerr << "Usage:\n"
              << "  ./differentiate <\"expression\"> <real_part>\n"
              << "  ./differentiate <\"expression\"> <real_part> <imag_part>\n\n";
}



/*
// Test points
    Complex z(2, 2);
    Complex z1(1.63, -2.11);
    Complex z2(1, 0);
    Complex z3(-32, 32);
    Complex z4(-4.6, -9.47);
    Complex z5(5.89, 6.23);
    Complex z6(-9.17, 2.23);
    Complex z7(-1.73, 3);

    // Test expressions
    std::vector<std::string> expressions = {
        "2 * x^3",
        "sin(x)",
        "x^2 + 3*x",
        "log(x)",
        "x^x",
        "x^2 * cos(x) + log(x) * cos(x - 1)",   // (???)
        "22.5+log(sin(77.6)^(50.2^42.1)/(x/x)-(81.9/x^x*58.8)*(18*57.2^x-x^x+81.9*x^5.2*x^62)^x)",
        "x^93.2 - x*cos( x^(x^x / x^28.8 / x) + x - x^60.3 * 0.5^x / cos(48.6^x + 24.9^80.3 / 60 - x*15.9))*44.4",
        "x^88.3 / x^81.8 / x",
        "x+cot(89.4*x^(x/62.5^x*36.5^52.5-63/x^37+27.3)+x/x)+99.5/x/(x+x^69.1*(8.4^x*x^37.6/x)/x^sin(57.7^x*x^x+x-x))^x"
        // Expected in z7 = (-1.73, 3): (5.61228e+103,9.20117e+103) (+/- (1.7498e+100,3.4996e+100))
    };

    for ( const auto &expr : expressions ) {
        std::cout << "Expression: " << expr << std::endl;
        auto [f, f1, f2] = differentiate(expr);
        
        try {
            // z = (2, 2)
            std::cout << "  f(z)   = " << f(z)  << std::endl;
            std::cout << "  f\'(z)  = " << f1(z) << std::endl;
            std::cout << "  f\"(z) = " << f2(z) << std::endl;

            // z1 = (1.63, -2.11)
            std::cout << std::endl;
            std::cout << "  f(z1)   = " << f(z1)  << std::endl;
            std::cout << "  f\'(z1)  = " << f1(z1) << std::endl;
            std::cout << "  f\"(z1) = " << f2(z1) << std::endl;

            // z2 = (1, 0)
            std::cout << std::endl;
            std::cout << "  f(z2)   = " << f(z2)  << std::endl;
            std::cout << "  f\'(z2)  = " << f1(z2) << std::endl;
            std::cout << "  f\"(z2) = " << f2(z2) << std::endl;

            // z3 = (-32, 32)
            std::cout << std::endl;
            std::cout << "  f(z3)   = " << f(z3)  << std::endl;
            std::cout << "  f\'(z3)  = " << f1(z3) << std::endl;
            std::cout << "  f\"(z3) = " << f2(z3) << std::endl;

            // z4 = (-4.6, -9.47)
            std::cout << std::endl;
            std::cout << "  f(z4)   = " << f(z4)  << std::endl;
            std::cout << "  f\'(z4)  = " << f1(z4) << std::endl;
            std::cout << "  f\"(z4) = " << f2(z4) << std::endl;
        
            // z5 = (5.89, 6.23)
            std::cout << std::endl;
            std::cout << "  f(5.89, 6.23)   = " << f(z5)  << std::endl;
            std::cout << "  f\'(5.89, 6.23)  = " << f1(z5) << std::endl;
            std::cout << "  f\"(5.89, 6.23) = " << f2(z5) << std::endl;

            // z6 = (-9.17, 2.23)
            std::cout << std::endl;
            std::cout << "  f(z6)   = " << f(z6)  << std::endl;
            std::cout << "  f\'(z6)  = " << f1(z6) << std::endl;
            std::cout << "  f\"(z6) = " << f2(z6) << std::endl;

            // z7 = (-1.73, 3)
            std::cout << std::endl;
            std::cout << "  f(z7)   = " << f(z7)  << std::endl;
            std::cout << "  f\'(z7)  = " << f1(z7) << std::endl;
            std::cout << "  f\"(z7) = " << f2(z7) << std::endl;
        } 
        catch (const std::exception &e) {
            std::cout << "  Error evaluating: " << e.what() << std::endl;
        }
        
        std::cout << std::string(40, '-') << std::endl;
    }
*/