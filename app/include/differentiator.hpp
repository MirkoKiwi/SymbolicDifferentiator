#ifndef DIFFERENTIATOR_HPP
#define DIFFERENTIATOR_HPP



#include <functional>
#include <string>
#include <tuple>

#include "ast.hpp" // Provides the Complex type definition



using Func = std::function<Complex(const Complex&)>;


std::tuple<Func, Func, Func> differentiate(const std::string &mathExpr);



#endif // DIFFERENTIATOR_HPP