// src/ast.cpp
#include <complex>
#include <memory>
#include <vector>
#include <string>
#include <stdexcept>

#include "ast.hpp"



// <constant>  ::= [0-9]+ ( "." [0-9]+ )?
// ConstNode constructor
ConstNode::ConstNode(long double v) : value(v) {}

Complex ConstNode::eval(const Complex &/*x*/) const {
    return Complex(value, 0.0);
}

NodePtr ConstNode::deriv() const {
    return std::make_shared<ConstNode>(0.0);
}


// Node for variable 'x' representation
// <variable>  ::= "x"
Complex VarNode::eval(const Complex &x) const {
    return x;
}

NodePtr VarNode::deriv() const {
    return std::make_shared<ConstNode>(1.0);
}




// Binary operations: +, -, *, /
// BinaryNode constructor
BinaryNode::BinaryNode(char o, NodePtr l, NodePtr r) : op(o), left(std::move(l)), right(std::move(r)) {}

// Evaluate by computing left and right, then apply op
Complex BinaryNode::eval(const Complex &x) const {
    Complex a = left -> eval(x); 
    Complex b = right -> eval(x);
    
    switch (op) {
        case '+': return a + b;
        case '-': return a - b;
        case '*': return a * b;
        case '/': return a / b;
    }
    
    throw std::runtime_error("Unknown binary op");
}

// Compute derivative via linearity, product rule, or quotient rule
NodePtr BinaryNode::deriv() const {
    if ( op == '+' || op == '-' ) {
        // (f ± g)' = f' ± g'
        return std::make_shared<BinaryNode>(op, left -> deriv(), right -> deriv());
    } 
    else if ( op == '*' ) {
        // (f * g)' = f'*g + f*g'
        return std::make_shared<BinaryNode>('+',
                std::make_shared<BinaryNode>('*', left -> deriv(), right),
                std::make_shared<BinaryNode>('*', left, right -> deriv()));
    } 
    else if ( op == '/' ) {
        // (f / g)' = (f'*g - f*g') / g^2
        auto num = std::make_shared<BinaryNode>('-',
                    std::make_shared<BinaryNode>('*', left -> deriv(), right),
                    std::make_shared<BinaryNode>('*', left, right -> deriv()));
        auto den = std::make_shared<PowerNode>(right, std::make_shared<ConstNode>(2.0));
        
        return std::make_shared<BinaryNode>('/', num, den);
    }

    throw std::runtime_error("Unknown binary deriv op");
}



// Exponentiation constructor
PowerNode::PowerNode(NodePtr b, NodePtr e) : base(std::move(b)), exp(std::move(e)) {}

// Evaluate u(x)^v(x) using std::pow for complexes
Complex PowerNode::eval(const Complex &x) const  {
    Complex baseVal = Complex(base -> eval(x));
    Complex expVal = Complex(exp -> eval(x));

    return std::pow(baseVal, expVal);

}

// Derivative: d(u^v) = u^v * [v'*ln(u) + v*(u'/u)]
NodePtr PowerNode::deriv() const {
    // u(x), v(x)   u^v
    NodePtr u = base;
    NodePtr v = exp;

    // Compute derivatives u'(x) and v'(x)
    NodePtr uDeriv = u -> deriv();
    NodePtr vDeriv = v -> deriv();

    // term1 = v'(x) * ln(u(x))
    auto ln_u = std::make_shared<FuncNode>("log", std::vector<NodePtr>{u});
    NodePtr term1 = std::make_shared<BinaryNode>('*', vDeriv, ln_u);

    // term2 = v(x) * (u'(x) / u(x))
    NodePtr quotient = std::make_shared<BinaryNode>('/', uDeriv, u);
    NodePtr term2 = std::make_shared<BinaryNode>('*', v, quotient);

    // Sum inside brackets: sumInside = term1 + term1
    NodePtr sumInsideBrackets = std::make_shared<BinaryNode>('+', term1, term2);

    // Final derivative: u^v * sumInsideBrackets
    return std::make_shared<BinaryNode>('*', std::make_shared<PowerNode>(u, v), sumInsideBrackets);
}



// Function calls (sin, cos, tan, cot, log)
// Constructor  -   name(arg)
FuncNode::FuncNode(std::string nm, std::vector<NodePtr> arg) : name(std::move(nm)), args(std::move(arg)) {}
    
// Evaluation by function
// <func_name> ::= "sin" | "cos" | "tan" | "cot" | "log"
Complex FuncNode::eval(const Complex &x) const {
    auto v = args[0] -> eval(x);

    if ( name == "sin" )  return std::sin(v);
    if ( name == "cos" )  return std::cos(v);
    if ( name == "tan" )  return std::tan(v);
    if ( name == "cot" )  return Complex(1.0) / std::tan(v);
    if ( name == "log" )  return std::log(v);
    
    throw std::runtime_error("Unknown func: " + name);
}

// Compute derivative via chain rule: f'(g) = f'_outer * g'
// (f∘inner)' = f'_outer(inner) * inner'
NodePtr FuncNode::deriv() const {
    // Retrieve inner function and its derivative
    NodePtr innerFunction = args[0]; 
    NodePtr innerDerivative = innerFunction -> deriv(); 
    NodePtr outerDerivative;
    
    if ( name == "sin" ) {
        // Outer derivative: d/dz sin(z) = cos(z)
        outerDerivative = std::make_shared<FuncNode>("cos", std::vector<NodePtr>{innerFunction});
    } 
    else if ( name == "cos" ) {
        // Outer derivative: d/dz cos(z) = -sin(z)
        NodePtr sinNode = std::make_shared<FuncNode>("sin", std::vector<NodePtr>{innerFunction});
        outerDerivative = std::make_shared<BinaryNode>('*', std::make_shared<ConstNode>(-1.0), sinNode);
    } 
    else if ( name == "tan" ) {
        // Outer derivative: d/dz tan(z) = 1 / cos(z)^2
        NodePtr cosNode = std::make_shared<FuncNode>("cos", std::vector<NodePtr>{innerFunction});
        NodePtr cosSquared = std::make_shared<PowerNode>(cosNode, std::make_shared<ConstNode>(2.0));
        outerDerivative = std::make_shared<BinaryNode>('/', std::make_shared<ConstNode>(1.0), cosSquared);
    } 
    else if ( name == "cot" ) {
        // Outer derivative: d/dz cot(z) = -1 / sin(z)^2
        NodePtr sinNode = std::make_shared<FuncNode>("sin", std::vector<NodePtr>{innerFunction});
        NodePtr sinSquared = std::make_shared<PowerNode>(sinNode, std::make_shared<ConstNode>(2.0));
        NodePtr reciprocal = std::make_shared<BinaryNode>('/', std::make_shared<ConstNode>(1.0), sinSquared);
        outerDerivative = std::make_shared<BinaryNode>('*', std::make_shared<ConstNode>(-1.0), reciprocal);
    } 
    else if ( name == "log" ) {
        // Outer derivative: d/dz log(z) = 1 / z
        outerDerivative = std::make_shared<BinaryNode>('/', std::make_shared<ConstNode>(1.0), innerFunction);
    } 
    else 
        throw std::runtime_error("Unknown func deriv: "+name);
    
    // Chain rule: outer(g) * g'
    // f'(x) = outerDerivative(inner(x)) * innerDerivative(x)
    return std::make_shared<BinaryNode>('*', outerDerivative, innerDerivative);
}