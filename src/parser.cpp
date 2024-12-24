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
    // switch (reserved_words[identifierPtr->s])
    // {
    // case E_QUOTE:
    //
    // case E_BEGIN:
    //
    // case E_IF:
    //     if (this->stxs.size() != 4)
    //         throw(RuntimeError(""));
    //     expr1 = this->stxs[1].parse(env);
    //     expr2 = this->stxs[2].parse(env);
    //     expr3 = this->stxs[3].parse(env);
    //     return Expr(new If(expr1, expr2, expr3));
    // default:
    //     break;
    // }
    if (identifierPtr->s == "quote")
    {
        // if (this->stxs.size() != 2)
        //     throw(RuntimeError(""));
        // return Expr(new Quote(this->stxs[1]));
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
            now = Assoc(new AssocList(s, NullV(), now));
        }
        Expr body = this->stxs[2].parse(now);
        return Expr(new Lambda(para, body));
    }
    if (identifierPtr->s == "let")
    {
        Assoc now = env;
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
            Identifier *varId = dynamic_cast<Identifier *>(ls->stxs[0].get());
            if (!varId)
                throw(RuntimeError(""));
            Expr ex = ls->stxs[1].parse(env);
            bind.push_back({varId->s, ex});
            now = Assoc(new AssocList(varId->s, NullV(), now));
        }
        Expr bd = this->stxs[2].parse(now);
        return Expr(new Let(bind, bd));
    }
    // switch (primitives[identifierPtr->s])
    // {
    // case E_VOID:
    //     if(this->stxs.size() != 1) throw(RuntimeError(""));
    //     return new MakeVoid();
    // case E_EXIT:
    //     if (this->stxs.size() == 1)
    //         return Expr(new Exit());
    //     throw(RuntimeError(""));
    // case E_CAR:
    //     if(this->stxs.size() != 2) throw(RuntimeError(""));
    //     else
    //     {
    //         Expr expr = this->stxs[1]->parse(env);
    //         return Expr(new Car(expr));
    //     }
    // case E_CDR:
    //     if(this->stxs.size() != 2) throw(RuntimeError(""));
    //     else
    //     {
    //         Expr expr = this->stxs[1]->parse(env);
    //         return Expr(new Cdr(expr));
    //     }
    // case E_INTQ:
    //     if(this->stxs.size() != 2) throw(RuntimeError(""));
    //     return Expr(new IsFixnum(this->stxs[1].parse(env)));
    // case E_SYMBOLQ:
    //     if(this->stxs.size() != 2) throw(RuntimeError(""));
    //     return Expr(new IsSymbol(this->stxs[1].parse(env)));
    // case E_BOOLQ:
    //     if(this->stxs.size() != 2) throw(RuntimeError(""));
    //     return Expr(new IsBoolean(this->stxs[1].parse(env)));
    // case E_NULLQ:
    //     if(this->stxs.size() != 2) throw(RuntimeError(""));
    //     return Expr(new IsNull(this->stxs[1].parse(env)));
    // case E_PAIRQ:
    //     if(this->stxs.size() != 2) throw(RuntimeError(""));
    //     return Expr(new IsPair(this->stxs[1].parse(env)));
    // case E_NOT:
    //     if(this->stxs.size() != 2) throw(RuntimeError(""));
    //     return Expr(new Not(this->stxs[1].parse(env)));
    // case E_EQQ:
    //     if(this->stxs.size() != 3) throw(RuntimeError(""));
    //     return Expr(new IsEq(this->stxs[1].parse(env), this->stxs[2].parse(env)));
    // case E_PROCQ:
    //     if(this->stxs.size() != 2) throw(RuntimeError(""));
    //     Expr ep = this->stxs[1].parse(env);
    //     return Expr(new IsProcedure(ep));
    //}
    Expr vr = this->stxs[0].parse(env);
    std::vector<Expr> es;
    for (int i = 1; i < this->stxs.size(); ++i)
    {
        Expr e = this->stxs[i].parse(env);
        es.push_back(e);
    }
    //  if (primitives.find(v->x) != primitives.end() || this->stxs.size() == 2)
    return Expr(new Apply(vr, es));
    // switch (primitives[v->x])
    // {
    // case E_PLUS:
    // case E_MINUS:
    // case E_MUL:
    //     return Expr(new Apply(vr, es));
    // }
    // if(this->stxs.size() != 3) throw(RuntimeError(""));
    // expr1 = this->stxs[1].parse(env);
    // expr2 = this->stxs[2].parse(env);
    // switch (primitives[identifierPtr->s])
    // {
    // case E_LT:
    //     return Expr(new Less(expr1, expr2));
    // case E_LE:
    //     return Expr(new LessEq(expr1, expr2));
    // case E_EQ:
    //     return Expr(new Equal(expr1, expr2));
    // case E_GE:
    //     return Expr(new GreaterEq(expr1, expr2));
    // case E_GT:
    //     return Expr(new Greater(expr1, expr2));
    // case E_CONS:
    //     return Expr(new Cons(expr1, expr2));
    // }
    throw(RuntimeError(""));
}

#endif