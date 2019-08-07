#ifndef SYMENGINE_PARSER_SEM7_H
#define SYMENGINE_PARSER_SEM7_H

/*
   Computer 1:
   Computer 2:
*/

template <typename T, typename... Args>
inline std::unique_ptr<T> make_unique(Args &&... args)
{
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}


enum BinOpType
{
    Add, Sub, Mul, Div
};

class Base {
};

class BinOp : public Base {
    const BinOpType type;
    const std::unique_ptr<const Base> left, right;
public:
    BinOp(BinOpType type, const Base *x, const Base *y)
        : type{type}, left{x}, right{y} {
    }
};

class Pow : public Base {
    const std::unique_ptr<const Base> base, exp;
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
#define PRINT(x) std::cout << (long int)x << std::endl; //x->d.binop.right->type << std::endl


#endif
