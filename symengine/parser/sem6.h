#ifndef SYMENGINE_PARSER_SEM6_H
#define SYMENGINE_PARSER_SEM6_H

/*
   Computer 1: 12ms 122ms
   Computer 2: -    74ms   86ms
*/

#include "alloc.h"

static Allocator al(1000000000);

template <typename T, typename... Args>
inline std::unique_ptr<T> make_unique(Args &&... args)
{
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}


enum BinOpType
{
    Add, Sub, Mul, Div
};

class Base;
class Visitor;
class BinOp;
class Pow;
class Symbol;
class Integer;
static int count(const Base &b);

class Visitor
{
public:
    virtual ~Visitor() {};
    virtual void visit(const BinOp &x) = 0;
    virtual void visit(const Pow &x) = 0;
    virtual void visit(const Symbol &x) = 0;
    virtual void visit(const Integer &x) = 0;
};

class Base {
public:
    virtual ~Base() {};
    virtual void accept(Visitor &v) const = 0;
};

class BinOp : public Base {
public:
    const BinOpType type;
    const Base *left, *right;
    BinOp(BinOpType type, const Base *x, const Base *y)
        : type{type}, left{x}, right{y} {
    }
    virtual ~BinOp() {}
    virtual void accept(Visitor &v) const {
        v.visit(*this);
    }
};

class Pow : public Base {
public:
    const Base *base, *exp;
    Pow(const Base *x, const Base *y)
        : base{x}, exp{y} {
    }
    virtual ~Pow() {}
    virtual void accept(Visitor &v) const {
        v.visit(*this);
    }
};

class Symbol : public Base {
public:
    const char *name;
    Symbol(const std::string s) : name{s.c_str()} { }
    virtual ~Symbol() {}
    virtual void accept(Visitor &v) const {
        v.visit(*this);
    }
};

class Integer : public Base {
    const char *i;
public:
    Integer(const std::string s) : i{s.c_str()} { }
    virtual ~Integer() {}
    virtual void accept(Visitor &v) const {
        v.visit(*this);
    }
};

class CountVisitor : public Visitor
{
public:
    int c=0;
    virtual void visit(const BinOp &x) {
        c += count(*x.left);
        c += count(*x.right);
    }
    virtual void visit(const Pow &x) {
        c += count(*x.base);
        c += count(*x.exp);
    }
    virtual void visit(const Symbol &x) { c += 1; }
    virtual void visit(const Integer &x) { }
};

static int count(const Base &b) {
    CountVisitor v;
    b.accept(v);
    return v.c;
}


#define TYPE Base*
#define ADD(x, y) al.make_new<BinOp>(BinOpType::Add, x, y)
#define SUB(x, y) al.make_new<BinOp>(BinOpType::Sub, x, y)
#define MUL(x, y) al.make_new<BinOp>(BinOpType::Mul, x, y)
#define DIV(x, y) al.make_new<BinOp>(BinOpType::Div, x, y)
#define POW(x, y) al.make_new<Pow>(x, y)
#define SYMBOL(x) al.make_new<Symbol>(x)
#define INTEGER(x) al.make_new<Integer>(x)
#define PRINT(x) std::cout << count(*x) << std::endl;
//#define PRINT(x) std::cout << (long int)(x) << std::endl;


#endif
