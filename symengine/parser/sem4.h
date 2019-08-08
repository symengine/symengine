#ifndef SYMENGINE_PARSER_SEM4_H
#define SYMENGINE_PARSER_SEM4_H

// Computer 1: 12ms 128ms
// Computer 2: 13ms 74ms   83ms
enum NodeType
{
    Add, Sub, Mul, Div, Pow, Symbol, Integer
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

static int count(const Node &x) {
    switch (x.type) {
        case Add:
        case Sub:
        case Mul:
        case Div: {
                int c = 0;
                c += count(*x.d.binop.left);
                c += count(*x.d.binop.right);
                return c; }
        case Pow: {
                int c = 0;
                c += count(*x.d.pow.base);
                c += count(*x.d.pow.exp);
                return c; }
        case Symbol: return 1;
        case Integer: return 0;
    }
}

#define TYPE PNode
#define ADD(x, y) make_binop(NodeType::Add, x, y)
#define SUB(x, y) make_binop(NodeType::Sub, x, y)
#define MUL(x, y) make_binop(NodeType::Mul, x, y)
#define DIV(x, y) make_binop(NodeType::Div, x, y)
#define POW(x, y) make_pow(x, y)
#define SYMBOL(x) make_symbol(x)
#define INTEGER(x) make_integer(x)
//#define PRINT(x) std::cout << x->d.binop.right->type << std::endl
#define PRINT(x) std::cout << count(*x) << std::endl;


#endif
