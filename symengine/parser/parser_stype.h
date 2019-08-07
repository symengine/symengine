#ifndef SYMENGINE_PARSER_STYPE_H
#define SYMENGINE_PARSER_STYPE_H

//#include "sem1.h"
//#include "sem2.h"
//#include "sem3.h"
#include "sem4.h"

/******************************************************************************/

/*
enum BinOpType
{
    Add, Sub, Mul, Div
};

class Base {
};

class BinOp : public Base {
    BinOp type;
public:
    BinOp(NodeType type, PNode x, PNode y) {
    }
};

typedef struct Node *PNode;
struct Node {
    NodeType type;
    union {
        struct { PNode left; PNode right; } binop;
        struct { PNode base; PNode exp; } pow;
        struct { char *name; } symbol;
        struct { char *i; } integer;
    } d;
};

static struct Node* make_binop(NodeType type, PNode x, PNode y) {
    PNode n;
    n = new Node;
    n->type = type;
    n->d.binop.left = x;
    n->d.binop.right = y;
    return n;
}

static struct Node* make_pow(PNode x, PNode y) {
    PNode n;
    n = new Node;
    n->type = NodeType::Pow;
    n->d.pow.base = x;
    n->d.pow.exp = y;
    return n;
}

static struct Node* make_symbol(std::string s) {
    PNode n;
    n = new Node;
    n->type = NodeType::Symbol;
    n->d.symbol.name = &s[0];
    return n;
}

static struct Node* make_integer(std::string s) {
    PNode n;
    n = new Node;
    n->type = NodeType::Integer;
    n->d.integer.i = &s[0];
    return n;
}

#define TYPE PNode
#define ADD(x, y) make_binop(NodeType::Add, x, y)
#define SUB(x, y) make_binop(NodeType::Sub, x, y)
#define MUL(x, y) make_binop(NodeType::Mul, x, y)
#define DIV(x, y) make_binop(NodeType::Div, x, y)
#define POW(x, y) make_pow(x, y)
#define SYMBOL(x) make_symbol(x)
#define INTEGER(x) make_integer(x)
#define PRINT(x) std::cout << x->d.binop.right->type << std::endl
*/


namespace SymEngine
{

struct YYSTYPE {
    TYPE basic;
    std::string string;
    // Constructor
    YYSTYPE() = default;
    // Destructor
    ~YYSTYPE() = default;
    // Copy constructor and assignment
    YYSTYPE(const YYSTYPE &) = default;
    YYSTYPE &operator=(const YYSTYPE &) = default;
    // Move constructor and assignment
    YYSTYPE(YYSTYPE &&) = default;
    YYSTYPE &operator=(YYSTYPE &&) = default;
};

} // namespace SymEngine



#endif
