#ifndef PARSER 
#define PARSER

// parser of myscheme 

#include "RE.hpp"
#include "Def.hpp"
#include "syntax.hpp"
#include "expr.hpp"
#include <map>
#include <cstring>
#include <iostream>
#define mp make_pair
using std :: string;
using std :: vector;
using std :: pair;

extern std :: map<std :: string, ExprType> primitives;
extern std :: map<std :: string, ExprType> reserved_words;

Expr Syntax :: parse(Assoc &env) 
{
    Number* p = dynamic_cast<Number*>(this->get());
    if(p != nullptr) return p->parse(env);
    List* p2 = dynamic_cast<List*>(this->get());
    if(p2 != nullptr) return p2->parse(env);
}

Expr Number :: parse(Assoc &env) 
{
    return Expr(new Fixnum(this->n));
}

Expr Identifier :: parse(Assoc &env) 
{
    return Expr(new Var(this->s));
}

Expr TrueSyntax :: parse(Assoc &env) 
{
    return Expr(new True());
}

Expr FalseSyntax :: parse(Assoc &env) 
{
    return Expr(new False());
}

Expr List :: parse(Assoc &env) 
{
    if (this->stxs.empty()) throw(RuntimeError(""));
    Identifier* identifierPtr = dynamic_cast<Identifier*>(this->stxs[0].get());
    if(identifierPtr->s == "quote") 
    {
        return Expr(new Quote(this->stxs[1]));
    }
    if(this->stxs.size() != 3)
    {
        if(this->stxs.size() == 1 && identifierPtr->s == "exit") return Expr(new Exit());
        throw(RuntimeError(""));
    }
    Expr expr1 = this->stxs[1].parse(env);
    Expr expr2 = this->stxs[2].parse(env);
    if(identifierPtr->s == "+")
        return Expr(new Plus(expr1, expr2));
    if(identifierPtr->s == "-")
        return Expr(new Minus(expr1, expr2));
    if(identifierPtr->s == "*") 
        return Expr(new Mult(expr1, expr2));
}

#endif