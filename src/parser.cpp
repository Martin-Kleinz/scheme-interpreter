#ifndef PARSER
#define PARSER

// parser of myscheme

#include "RE.hpp"
#include "Def.hpp"
#include "syntax.hpp"
#include "expr.hpp"
#include "value.hpp"
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
    if (p1)
        return p1->parse(env);
    Number *p = dynamic_cast<Number *>(this->get());
    if (p)
        return p->parse(env);
    FalseSyntax *fstx = dynamic_cast<FalseSyntax *>(this->get());
    if (fstx)
        return fstx->parse(env);
    TrueSyntax *tstx = dynamic_cast<TrueSyntax *>(this->get());
    if (tstx)
        return tstx->parse(env);
    Identifier *idtf = dynamic_cast<Identifier *>(this->get());
    if (idtf)
        return idtf->parse(env);
    throw(RuntimeError(""));
}

Expr Number ::parse(Assoc &env)
{
    return Expr(new Fixnum(this->n));
}

Expr Identifier ::parse(Assoc &env)
{
    if (this->s[0] == '.' || this->s[0] == '@')
        throw(RuntimeError(""));
    if (this->s[0] >= '0' && this->s[0] <= '9')
        throw(RuntimeError(""));
    for (int i = 0; i < this->s.size(); ++i)
        if (this->s[i] == '#' || this->s[i] == '\'' || this->s[i] == '"')
            throw(RuntimeError(""));
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
    // if (this->stxs.empty())
    //     throw(RuntimeError(""));
    Identifier *identifierPtr = dynamic_cast<Identifier *>(this->stxs[0].get());
    if (!identifierPtr)
    {
        List *lst = dynamic_cast<List *>(this->stxs[0].get());
        if (!lst)
            throw(RuntimeError(""));
        Expr l = this->stxs[0].parse(env);
        std::vector<Expr> eps;
        for (int i = 1; i < this->stxs.size(); ++i)
        {
            Expr ep = this->stxs[i].parse(env);
            eps.push_back(ep);
        }
        return Expr(new Apply(l, eps));
    }
    std::vector<std::pair<std::string, Expr>> bind;
    std::vector<Expr> bg;
    std::vector<std::string> para;
    Expr expr1 = nullptr, expr2 = nullptr, expr3 = nullptr;
    if (identifierPtr->s == "if")
    {
        Assoc now = env;
        while (now.get() && now.get()->x != "if")
            now = now.get()->next;
        if (!now.get())
        {
            if (this->stxs.size() != 4)
                throw(RuntimeError(""));
            expr1 = this->stxs[1].parse(env);
            expr2 = this->stxs[2].parse(env);
            expr3 = this->stxs[3].parse(env);
            return Expr(new If(expr1, expr2, expr3));
        }
    }
    if (identifierPtr->s == "quote")
    {
        Assoc now = env;
        while (now.get() && now.get()->x != "quote")
            now = now.get()->next;
        if (!now.get())
        {
            if (this->stxs.size() != 2)
                throw(RuntimeError(""));
            return Expr(new Quote(this->stxs[1]));
        }
    }
    if (identifierPtr->s == "begin")
    {
        Assoc now = env;
        while (now.get() && now.get()->x != "begin")
            now = now.get()->next;
        if (!now.get())
        {
            if (this->stxs.size() < 2)
                throw(RuntimeError(""));
            for (int i = 1; i < this->stxs.size(); ++i)
            {
                Expr expr = this->stxs[i].parse(env);
                bg.push_back(expr);
            }
            return Expr(new Begin(bg));
        }
    }
    if (identifierPtr->s == "lambda")
    {
        Assoc now = env;
        while (now.get() && now.get()->x != "lambda")
            now = now.get()->next;
        if (!now.get())
        {
            now = env;
            if (this->stxs.size() != 3)
                throw(RuntimeError(""));
            List *p = dynamic_cast<List *>(this->stxs[1].get());
            if (!p)
                throw(RuntimeError(""));
            for (int i = 0; i < p->stxs.size(); ++i)
            {
                Expr v = p->stxs[i].parse(env);
                Var *var = dynamic_cast<Var *>(v.get());
                std::string s = var->x;
                para.push_back(s);
                now = Assoc(new AssocList(s, Value(nullptr), now));
            }
            Expr body = this->stxs[2].parse(now);
            return Expr(new Lambda(para, body));
        }
    }
    if (identifierPtr->s == "let")
    {
        Assoc now = env;
        while (now.get() && now.get()->x != "let")
            now = now.get()->next;
        if (!now.get())
        {
            now = env;
            if (this->stxs.size() != 3)
                throw(RuntimeError(""));
            List *lsts = dynamic_cast<List *>(this->stxs[1].get());
            if (!lsts)
                throw(RuntimeError(""));
            for (int i = 0; i < lsts->stxs.size(); ++i)
            {
                List *ls = dynamic_cast<List *>(lsts->stxs[i].get());
                if (!ls || ls->stxs.size() != 2)
                    throw(RuntimeError(""));
                Expr v = ls->stxs[0].parse(env);
                Var *varId = dynamic_cast<Var *>(v.get());
                now = Assoc(new AssocList(varId->x, Value(nullptr), now));
                if (!varId)
                    throw(RuntimeError(""));
                Expr ex = ls->stxs[1].parse(env);
                bind.push_back({varId->x, ex});
            }
            Expr bd = this->stxs[2].parse(now);
            return Expr(new Let(bind, bd));
        }
    }
    if (identifierPtr->s == "letrec")
    {
        Assoc now = env;
        while (now.get() && now.get()->x != "letrec")
            now = now.get()->next;
        if (!now.get())
        {
            now = env;
            if (this->stxs.size() != 3)
                throw(RuntimeError(""));
            List *lsts = dynamic_cast<List *>(this->stxs[1].get());
            if (!lsts)
                throw(RuntimeError(""));
            for (int i = 0; i < lsts->stxs.size(); ++i)
            {
                List *ls = dynamic_cast<List *>(lsts->stxs[i].get());
                if (!ls || ls->stxs.size() != 2)
                    throw(RuntimeError(""));
                Expr v = ls->stxs[0].parse(env);
                Var *varId = dynamic_cast<Var *>(v.get());
                if (!varId)
                    throw(RuntimeError(""));
                now = Assoc(new AssocList(varId->x, Value(nullptr), now));    
                Expr ex = ls->stxs[1].parse(env);
                bind.push_back({varId->x, ex});
            }
            Expr bd = this->stxs[2].parse(now);
            return Expr(new Letrec(bind, bd));
        }
    }
    Expr vr = this->stxs[0].parse(env);
    std::vector<Expr> es;
    for (int i = 1; i < this->stxs.size(); ++i)
    {
        Expr e = this->stxs[i].parse(env);
        es.push_back(e);
    }
    return Expr(new Apply(vr, es));
}

#endif