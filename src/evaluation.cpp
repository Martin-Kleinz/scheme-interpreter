#include "Def.hpp"
#include "value.hpp"
#include "expr.hpp"
#include "RE.hpp"
#include "syntax.hpp"
#include <cstring>
#include <vector>
#include <map>

extern std :: map<std :: string, ExprType> primitives;
extern std :: map<std :: string, ExprType> reserved_words;

Value Let::eval(Assoc &env) {} // let expression

Value Lambda::eval(Assoc &env) {} // lambda expression

Value Apply::eval(Assoc &e) {} // for function calling

Value Letrec::eval(Assoc &env) {} // letrec expression

Value Var::eval(Assoc &e) 
{
    
} // evaluation of variable

Value Fixnum::eval(Assoc &e) 
{
    return IntegerV(this->n);
} // evaluation of a fixnum

Value If::eval(Assoc &e) 
{

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

} // begin expression

Value Quote::eval(Assoc &e) 
{
    Number* ptr1 = dynamic_cast<Number*>(this->s.get());
    if(ptr1 != nullptr) return IntegerV(ptr1->n);
    TrueSyntax* ptr2 = dynamic_cast<TrueSyntax*>(this->s.get());
    if(ptr2 != nullptr) return BooleanV(true);
    FalseSyntax* ptr3 = dynamic_cast<FalseSyntax*>(this->s.get());
    if(ptr3 != nullptr) return BooleanV(false);
    Identifier* ptr4 = dynamic_cast<Identifier*>(this->s.get());
    if(ptr4) return SymbolV(ptr4->s);
    Var* ptr = dynamic_cast<Var*>(this->s.get());
    if(ptr) return SymbolV(ptr->x);
    List* ptr5 = dynamic_cast<List*>(this->s.get());
    if(ptr5) {
        if (ptr5->stxs.empty()) 
            return NullV();
        else 
        {
            Quote* qt1 = new Quote(ptr5->stxs[0]);
            Value first = qt1->eval(e);
            List* restList = new List();
            for (auto it = ptr5->stxs.begin() + 1; it != ptr5->stxs.end(); ++it)
                restList->stxs.push_back(*it);
            Quote* restQuote = new Quote(restList);
            Value rest = restQuote->eval(e);
            return PairV(first, rest);
        }
    }
    throw(RuntimeError(""));
} // quote expression

Value MakeVoid::eval(Assoc &e) {} // (void)

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

Value Unary::eval(Assoc &e) {} // evaluation of single-operator primitive

Value Mult::evalRator(const Value &rand1, const Value &rand2) 
{
    Integer* Ptr1 = dynamic_cast<Integer*>(rand1.get());
    Integer* Ptr2 = dynamic_cast<Integer*>(rand2.get());
    if(Ptr1 != nullptr && Ptr2 != nullptr)
    return IntegerV(Ptr1->n * Ptr2->n);
    else throw(RuntimeError(""));
} // *

Value Plus::evalRator(const Value &rand1, const Value &rand2) 
{
    Integer* Ptr1 = dynamic_cast<Integer*>(rand1.get());
    Integer* Ptr2 = dynamic_cast<Integer*>(rand2.get());
    if(Ptr1 != nullptr && Ptr2 != nullptr)
    return IntegerV(Ptr1->n + Ptr2->n);
    else throw(RuntimeError(""));
} // +

Value Minus::evalRator(const Value &rand1, const Value &rand2) 
{
    Integer* Ptr1 = dynamic_cast<Integer*>(rand1.get());
    Integer* Ptr2 = dynamic_cast<Integer*>(rand2.get());
    if(Ptr1 != nullptr && Ptr2 != nullptr)
    return IntegerV(Ptr1->n - Ptr2->n);
    else throw(RuntimeError(""));
} // -

Value Less::evalRator(const Value &rand1, const Value &rand2) {} // <

Value LessEq::evalRator(const Value &rand1, const Value &rand2) {} // <=

Value Equal::evalRator(const Value &rand1, const Value &rand2) {} // =

Value GreaterEq::evalRator(const Value &rand1, const Value &rand2) {} // >=

Value Greater::evalRator(const Value &rand1, const Value &rand2) {} // >

Value IsEq::evalRator(const Value &rand1, const Value &rand2) {} // eq?

Value Cons::evalRator(const Value &rand1, const Value &rand2) 
{

} // cons

Value IsBoolean::evalRator(const Value &rand) {} // boolean?

Value IsFixnum::evalRator(const Value &rand) {} // fixnum?

Value IsSymbol::evalRator(const Value &rand) {} // symbol?

Value IsNull::evalRator(const Value &rand) {} // null?

Value IsPair::evalRator(const Value &rand) {} // pair?

Value IsProcedure::evalRator(const Value &rand) {} // procedure?

Value Not::evalRator(const Value &rand) {} // not

Value Car::evalRator(const Value &rand) {} // car

Value Cdr::evalRator(const Value &rand) {} // cdr
