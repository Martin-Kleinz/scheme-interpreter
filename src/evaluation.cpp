#include "Def.hpp"
#include "value.hpp"
#include "expr.hpp"
#include "RE.hpp"
#include "syntax.hpp"
#include <cstring>
#include <vector>
#include <map>

extern std ::map<std ::string, ExprType> primitives;
extern std ::map<std ::string, ExprType> reserved_words;

Value Let::eval(Assoc &env) {} // let expression

Value Lambda::eval(Assoc &env) {} // lambda expression

Value Apply::eval(Assoc &e) {} // for function calling

Value Letrec::eval(Assoc &env) {} // letrec expression

Value Var::eval(Assoc &e)
{
    // to do
    throw(RuntimeError(""));
} // evaluation of variable

Value Fixnum::eval(Assoc &e)
{
    return IntegerV(this->n);
} // evaluation of a fixnum

Value If::eval(Assoc &e)
{
    Value v1 = this->cond->eval(e);
    Boolean *blv = dynamic_cast<Boolean *>(v1.get());
    if (blv && blv->b == false)
        return this->alter->eval(e);
    else
        return this->conseq->eval(e);
} // if expression

Value True::eval(Assoc &e)
{
    return BooleanV(true);
} // evaluation of #t

Value False::eval(Assoc &e)
{
    return BooleanV(false);
} // evaluation of #f

Value Begin::eval(Assoc &e)
{
    return this->es.back()->eval(e);
} // begin expression

Value Quote::eval(Assoc &e)
{
    Number *ptr1 = dynamic_cast<Number *>(this->s.get());
    if (ptr1 != nullptr)
        return IntegerV(ptr1->n);
    TrueSyntax *ptr2 = dynamic_cast<TrueSyntax *>(this->s.get());
    if (ptr2 != nullptr)
        return BooleanV(true);
    FalseSyntax *ptr3 = dynamic_cast<FalseSyntax *>(this->s.get());
    if (ptr3 != nullptr)
        return BooleanV(false);
    Identifier *ptr4 = dynamic_cast<Identifier *>(this->s.get());
    if (ptr4)
        return SymbolV(ptr4->s);
    List *ptr5 = dynamic_cast<List *>(this->s.get());
    if (ptr5)
    {
        if (ptr5->stxs.empty())
            return NullV();
        else
        {
            Identifier *id = dynamic_cast<Identifier*>(ptr5->stxs[0].get());
            if(id && id->s == ".")
            {
                if(ptr5->stxs.size() != 2) throw(RuntimeError(""));
                Quote *rest = new Quote(ptr5->stxs[1]);
                Value r = rest->eval(e);
                return r;
            }
            Quote *qt1 = new Quote(ptr5->stxs[0]);
            Value first = qt1->eval(e);
            List *restList = new List();
            for (auto it = ptr5->stxs.begin() + 1; it != ptr5->stxs.end(); ++it)
                restList->stxs.push_back(*it);
            Quote *restQuote = new Quote(restList);
            Value rest = restQuote->eval(e);
            return PairV(first, rest);
        }
    }
    throw(RuntimeError(""));
} // quote expression

Value MakeVoid::eval(Assoc &e)
{
    return VoidV();
} // (void)

Value Exit::eval(Assoc &e)
{
    return TerminateV();
} // (exit)

Value Binary::eval(Assoc &e)
{
    Value v1 = this->rand1->eval(e);
    Value v2 = this->rand2->eval(e);
    return evalRator(v1, v2);
} // evaluation of two-operators primitive

Value Unary::eval(Assoc &e)
{
    Value v = this->rand->eval(e);
    return evalRator(v);
} // evaluation of single-operator primitive

Value Mult::evalRator(const Value &rand1, const Value &rand2)
{
    Integer *Ptr1 = dynamic_cast<Integer *>(rand1.get());
    Integer *Ptr2 = dynamic_cast<Integer *>(rand2.get());
    if (Ptr1 != nullptr && Ptr2 != nullptr)
        return IntegerV(Ptr1->n * Ptr2->n);
    else
        throw(RuntimeError(""));
} // *

Value Plus::evalRator(const Value &rand1, const Value &rand2)
{
    Integer *Ptr1 = dynamic_cast<Integer *>(rand1.get());
    Integer *Ptr2 = dynamic_cast<Integer *>(rand2.get());
    if (Ptr1 != nullptr && Ptr2 != nullptr)
        return IntegerV(Ptr1->n + Ptr2->n);
    else
        throw(RuntimeError(""));
} // +

Value Minus::evalRator(const Value &rand1, const Value &rand2)
{
    Integer *Ptr1 = dynamic_cast<Integer *>(rand1.get());
    Integer *Ptr2 = dynamic_cast<Integer *>(rand2.get());
    if (Ptr1 != nullptr && Ptr2 != nullptr)
        return IntegerV(Ptr1->n - Ptr2->n);
    else
        throw(RuntimeError(""));
} // -

Value Less::evalRator(const Value &rand1, const Value &rand2)
{
    Integer *itg1 = dynamic_cast<Integer *>(rand1.get());
    Integer *itg2 = dynamic_cast<Integer *>(rand2.get());
    if (!itg1 || !itg2)
        throw(RuntimeError(""));
    if (itg1->n < itg2->n)
        return BooleanV(true);
    return BooleanV(false);
} // <

Value LessEq::evalRator(const Value &rand1, const Value &rand2)
{
    Integer *itg1 = dynamic_cast<Integer *>(rand1.get());
    Integer *itg2 = dynamic_cast<Integer *>(rand2.get());
    if (!itg1 || !itg2)
        throw(RuntimeError(""));
    if (itg1->n <= itg2->n)
        return BooleanV(true);
    return BooleanV(false);
} // <=

Value Equal::evalRator(const Value &rand1, const Value &rand2)
{
    Integer *itg1 = dynamic_cast<Integer *>(rand1.get());
    Integer *itg2 = dynamic_cast<Integer *>(rand2.get());
    if (!itg1 || !itg2)
        throw(RuntimeError(""));
    if (itg1->n == itg2->n)
        return BooleanV(true);
    return BooleanV(false);
} // =

Value GreaterEq::evalRator(const Value &rand1, const Value &rand2)
{
    Integer *itg1 = dynamic_cast<Integer *>(rand1.get());
    Integer *itg2 = dynamic_cast<Integer *>(rand2.get());
    if (!itg1 || !itg2)
        throw(RuntimeError(""));
    if (itg1->n >= itg2->n)
        return BooleanV(true);
    return BooleanV(false);
} // >=

Value Greater::evalRator(const Value &rand1, const Value &rand2)
{
    Integer *itg1 = dynamic_cast<Integer *>(rand1.get());
    Integer *itg2 = dynamic_cast<Integer *>(rand2.get());
    if (!itg1 || !itg2)
        throw(RuntimeError(""));
    if (itg1->n > itg2->n)
        return BooleanV(true);
    return BooleanV(false);
} // >

Value IsEq::evalRator(const Value &rand1, const Value &rand2)
{
    if (rand1->v_type != rand2->v_type)
        return BooleanV(false);
    Integer *i1 = dynamic_cast<Integer *>(rand1.get());
    Integer *i2 = dynamic_cast<Integer *>(rand2.get());
    Boolean *p1 = dynamic_cast<Boolean *>(rand1.get());
    Boolean *p2 = dynamic_cast<Boolean *>(rand2.get());
    Symbol *s1 = dynamic_cast<Symbol *>(rand1.get());
    Symbol *s2 = dynamic_cast<Symbol *>(rand2.get());
    switch (rand1->v_type)
    {
    case V_INT:
        if (i1->n == i2->n)
            return BooleanV(true);
        return BooleanV(false);
    case V_BOOL:
        if (p1->b == p2->b)
            return BooleanV(true);
        return BooleanV(false);
    case V_SYM:
        if (s1->s == s2->s)
            return BooleanV(true);
        return BooleanV(false);
    case V_VOID:
    case V_NULL:
        return BooleanV(true);
    default:
        if (rand1.get() == rand2.get())
            return BooleanV(true);
        return BooleanV(false);
    }
    throw(RuntimeError(""));
} // eq?

Value Cons::evalRator(const Value &rand1, const Value &rand2)
{
    return PairV(rand1, rand2);
} // cons

Value IsBoolean::evalRator(const Value &rand)
{
    if (rand->v_type != V_BOOL)
        return BooleanV(false);
    else
        return BooleanV(true);
} // boolean?

Value IsFixnum::evalRator(const Value &rand)
{
    if (rand->v_type != V_INT)
        return BooleanV(false);
    else
        return BooleanV(true);
} // fixnum?

Value IsSymbol::evalRator(const Value &rand)
{
    if (rand->v_type != V_SYM)
        return BooleanV(false);
    else
        return BooleanV(true);
} // symbol?

Value IsNull::evalRator(const Value &rand)
{
    if (rand->v_type != V_NULL)
        return BooleanV(false);
    else
        return BooleanV(true);
} // null?

Value IsPair::evalRator(const Value &rand)
{
    if (rand->v_type != V_PAIR)
        return BooleanV(false);
    else
        return BooleanV(true);
} // pair?

Value IsProcedure::evalRator(const Value &rand) {} // procedure?

Value Not::evalRator(const Value &rand)
{
    Boolean *bl = dynamic_cast<Boolean *>(rand.get());
    if (bl && bl->b == false)
        return BooleanV(true);
    else
        return BooleanV(false);
} // not

Value Car::evalRator(const Value &rand)
{
    Pair *pr = dynamic_cast<Pair *>(rand.get());
    if (!pr)
        throw(RuntimeError(""));
    return pr->car;
} // car

Value Cdr::evalRator(const Value &rand)
{
    Pair *pr = dynamic_cast<Pair *>(rand.get());
    if (!pr)
        throw(RuntimeError(""));
    return pr->cdr;
} // cdr
