#ifndef AST_HPP
#define AST_HPP

#include <complex>
#include <memory>
#include <string>
#include <vector>



using Complex = std::complex<long double>;

struct Node;
using NodePtr = std::shared_ptr<Node>;



struct Node {
    virtual ~Node() = default;

    virtual Complex eval(const Complex &x) const = 0;

    virtual NodePtr deriv() const = 0;
};


// <constant>  ::= [0-9]+ ( "." [0-9]+ )?
struct ConstNode : Node {
    long double value;  // store real part, Im = 0

    explicit ConstNode(long double v);
    Complex eval(const Complex &x) const override;
    NodePtr deriv() const override;
};


// <variable>  ::= "x"
struct VarNode : Node {
    Complex eval(const Complex &x) const override;
    NodePtr deriv() const override;
};


// Binary operations: +, -, *, /
struct BinaryNode : Node {
    char op;        // operation symbol
    NodePtr left;   // left operand
    NodePtr right;  // right operand

    BinaryNode(char o, NodePtr l, NodePtr r);
    Complex eval(const Complex &x) const override;
    NodePtr deriv() const override;
};


// Exponentiation base^exp
struct PowerNode : Node {
    NodePtr base;   // Base function u(x)
    NodePtr exp;    // Exponent function v(x)

    // b^e
    PowerNode(NodePtr b, NodePtr e);
    
    // Evaluate u(x)^v(x)
    Complex eval(const Complex &x) const override;

    // Derivative: d(u^v) = u^v * [v'*ln(u) + v*(u'/u)]
    NodePtr deriv() const override;
};


// Function calls (sin, cos, tan, cot, log)
struct FuncNode : Node {
    std::string name;
    std::vector<NodePtr> args;  // single argument (args[0])

    // name(arg)
    FuncNode(std::string nm, std::vector<NodePtr> arg);

    // Evaluation by function
    // <func_name> ::= "sin" | "cos" | "tan" | "cot" | "log"
    Complex eval(const Complex &x) const override;

    // Compute derivative via chain rule: f'(g) = f'_outer * g'
    // (f∘inner)' = f'_outer(inner) * inner'
    NodePtr deriv() const override;
};



#endif // AST_HPP