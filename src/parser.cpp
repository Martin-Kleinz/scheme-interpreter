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
using std ::pair;
using std ::string;
using std ::vector;

extern std ::map<std ::string, ExprType> primitives;
extern std ::map<std ::string, ExprType> reserved_words;

Expr Syntax ::parse(Assoc &env)
{
    List *p1 = dynamic_cast<List *>(this->get());
    if (p1) return p1->parse(env);
    Number *p = dynamic_cast<Number *>(this->get());
    if (p) return p->parse(env);
    FalseSyntax *fstx = dynamic_cast<FalseSyntax*>(this->get());
    if(fstx) return fstx->parse(env);
    TrueSyntax *tstx = dynamic_cast<TrueSyntax*>(this->get());
    if(tstx) return tstx->parse(env);
    Identifier *idtf = dynamic_cast<Identifier*>(this->get());
    if(idtf) return idtf->parse(env);
}

Expr Number ::parse(Assoc &env)
{
    return Expr(new Fixnum(this->n));
}

Expr Identifier ::parse(Assoc &env)
{
    return Expr(new Var(this->s));
}

Expr TrueSyntax ::parse(Assoc &env)
{
    return Expr(new True());
}

Expr FalseSyntax ::parse(Assoc &env)
{
    return Expr(new False());
}

Expr List ::parse(Assoc &env)
{
    if (this->stxs.empty())
        throw(RuntimeError(""));
    Identifier *identifierPtr = dynamic_cast<Identifier *>(this->stxs[0].get());
    if (!identifierPtr)
        throw(RuntimeError(""));
    std::vector<Expr> bg;    
    switch (reserved_words[identifierPtr->s])
    {
    case E_QUOTE:
        return Expr(new Quote(this->stxs[1]));
    case E_BEGIN:
        for(int i = 1; i < this->stxs.size(); ++i)
        {
            Expr expr = this->stxs[i].parse(env);
            bg.push_back(expr);
        }
        return Expr(new Begin(bg));
    case E_IF:
        if(this->stxs.size() != 4) throw(RuntimeError(""));
        Expr expr1 = this->stxs[1].parse(env);
        Expr expr2 = this->stxs[2].parse(env);
        Expr expr3 = this->stxs[3].parse(env);
        return Expr(new If(expr1, expr2, expr3));
    }
    switch (primitives[identifierPtr->s])
    {
    case E_VOID:
        return new MakeVoid();
    case E_EXIT:
        if (this->stxs.size() == 1)
            return Expr(new Exit());
        throw(RuntimeError(""));
    case E_CONS:
        if (this->stxs.size() != 3)
            throw(RuntimeError(""));
        else
        {
            Expr expr3 = this->stxs[1].parse(env);
            Expr expr4 = this->stxs[2].parse(env);
            return Expr(new Cons(expr3, expr4));
        }
    case E_CAR:
        if(this->stxs.size() != 2) throw(RuntimeError(""));
        else 
        {
            Expr expr = this->stxs[1]->parse(env);
            return Expr(new Car(expr));
        }
    case E_CDR:
        if(this->stxs.size() != 2) throw(RuntimeError(""));
        else 
        {
            Expr expr = this->stxs[1]->parse(env);
            return Expr(new Cdr(expr));
        }
    }
    Expr expr1 = this->stxs[1].parse(env);
    Expr expr2 = this->stxs[2].parse(env);
    switch (primitives[identifierPtr->s])
    {
    case E_PLUS:
        return Expr(new Plus(expr1, expr2));
    case E_MINUS:
        return Expr(new Minus(expr1, expr2));
    case E_MUL:
        return Expr(new Mult(expr1, expr2));
    }
}

#endif