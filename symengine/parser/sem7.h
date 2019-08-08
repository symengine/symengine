#ifndef SYMENGINE_PARSER_SEM7_H
#define SYMENGINE_PARSER_SEM7_H

#include <variant>

/*
   Computer 1: 12ms 139ms
   Computer 2: --   78ms   92ms
*/

enum BinOpType
{
    Add, Sub, Mul, Div
};

struct Base;

struct BinOp {
    const BinOpType type;
    const Base *left, *right;
    BinOp(BinOpType type, const Base *x, const Base *y)
        : type{type}, left{x}, right{y} {
    }
};

struct Pow {
    const Base *base, *exp;
    Pow(const Base *x, const Base *y)
        : base{x}, exp{y} {
    }
};

struct Symbol {
    const char *name;
    Symbol(const std::string s) : name{s.c_str()} { }
};

struct Integer {
    const char *i;
    Integer(const std::string s) : i{s.c_str()} { }
};

struct Base {
    std::variant<BinOp, Pow, Symbol, Integer> u;
    template<typename A> Base(A &&x) : u{std::move(x)}  {}
};

template<typename... LAMBDAS> struct visitors : LAMBDAS... {
  using LAMBDAS::operator()...;
};
template<typename... LAMBDAS> visitors(LAMBDAS... x)->visitors<LAMBDAS...>;


static int count(const Base &b) {
    return std::visit(
        visitors{
            [](const Symbol &x) { return 1; },
            [](const BinOp &x) {
                int c = 0;
                c += count(*x.left);
                c += count(*x.right);
                return c; },
            [](const Pow &x) {
                int c = 0;
                c += count(*x.base);
                c += count(*x.exp);
                return c; },
            [](const auto &x) { return 0; },
        },
        b.u);
}


#define TYPE Base*
#define ADD(x, y) new Base(BinOp(BinOpType::Add, x, y))
#define SUB(x, y) new Base(BinOp(BinOpType::Sub, x, y))
#define MUL(x, y) new Base(BinOp(BinOpType::Mul, x, y))
#define DIV(x, y) new Base(BinOp(BinOpType::Div, x, y))
#define POW(x, y) new Base(Pow(x, y))
#define SYMBOL(x) new Base(Symbol(x))
#define INTEGER(x) new Base(Integer(x))
//#define PRINT(x) std::cout << (long int)x << std::endl; //x->d.binop.right->type << std::endl
#define PRINT(x) std::cout << count(*x) << std::endl;


#endif
