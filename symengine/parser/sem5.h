#ifndef SYMENGINE_PARSER_SEM5_H
#define SYMENGINE_PARSER_SEM5_H

/*
   Computer 2: 5ms
*/

enum BinOpType
{
    Add, Sub, Mul, Div
};

class Base {
};

class BinOp : public Base {
    const BinOpType type;
    const Base *left, *right;
public:
    BinOp(BinOpType type, const Base *x, const Base *y)
        : type{type}, left{x}, right{y} {
    }
};

class Pow : public Base {
    const Base *base, *exp;
public:
    Pow(const Base *x, const Base *y)
        : base{x}, exp{y} {
    }
};

class Symbol : public Base {
    const char *name;
public:
    Symbol(const std::string s) : name{s.c_str()} { }
};

class Integer : public Base {
    const char *i;
public:
    Integer(const std::string s) : i{s.c_str()} { }
};


#define TYPE Base*
#define ADD(x, y) new BinOp(BinOpType::Add, x, y)
#define SUB(x, y) new BinOp(BinOpType::Sub, x, y)
#define MUL(x, y) new BinOp(BinOpType::Mul, x, y)
#define DIV(x, y) new BinOp(BinOpType::Div, x, y)
#define POW(x, y) new Pow(x, y)
#define SYMBOL(x) new Symbol(x)
#define INTEGER(x) new Integer(x)
#define PRINT(x) std::cout << "OK" << std::endl; //x->d.binop.right->type << std::endl


#endif
