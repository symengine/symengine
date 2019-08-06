#ifndef SYMENGINE_PARSER_STYPE_H
#define SYMENGINE_PARSER_STYPE_H

#include <string>
#include "symengine/basic.h"
#include "symengine/add.h"
#include "symengine/pow.h"

/******************************************************************************/

/*
using SymEngine::add;
using SymEngine::sub;
using SymEngine::mul;
using SymEngine::div;
using SymEngine::pow;
using SymEngine::symbol;
using SymEngine::integer;

#define TYPE SymEngine::RCP<const SymEngine::Basic>
#define ADD(x, y) add(x, y)
#define SUB(x, y) sub(x, y)
#define MUL(x, y) mul(x, y)
#define DIV(x, y) div(x, y)
#define POW(x, y) pow(x, y)
#define SYMBOL(x) symbol(x)
#define INTEGER(x) integer(x)
#define PRINT(x) std::cout << *x << std::endl
*/

/******************************************************************************/

/*
#define TYPE int
#define ADD(x, y) x+y
#define SUB(x, y) x-y
#define MUL(x, y) x*y
#define DIV(x, y) x/y
#define POW(x, y) std::pow(x,y)
#define SYMBOL(x) 1
#define INTEGER(x) std::stoi(x)
#define PRINT(x) std::cout << x << std::endl
*/

/******************************************************************************/

/*
enum NodeType
{
    Add, Sub, Mul, Div, Pow, Symbol, Integer
};

struct Node {
    NodeType type;
};

static struct Node* make_node(NodeType type) {
    struct Node *n;
    n = new Node;
    n->type = type;
    return n;
}

#define TYPE struct Node*
#define ADD(x, y) make_node(NodeType::Add)
#define SUB(x, y) make_node(NodeType::Sub)
#define MUL(x, y) make_node(NodeType::Mul)
#define DIV(x, y) make_node(NodeType::Div)
#define POW(x, y) make_node(NodeType::Pow)
#define SYMBOL(x) make_node(NodeType::Symbol)
#define INTEGER(x) make_node(NodeType::Integer)
#define PRINT(x) std::cout << x->type << std::endl
*/

/******************************************************************************/

enum NodeType
{
    Add, Sub, Mul, Div, Pow, Symbol, Integer
};

typedef struct Node *PNode;
struct Node {
    NodeType type;
    union {
        struct { PNode left; PNode right; } Add;
        struct { PNode left; PNode right; } Sub;
        struct { PNode left; PNode right; } Mul;
        struct { PNode left; PNode right; } Div;
        struct { PNode base; PNode exp; } Pow;
        //struct { std::string name; } Symbol;
        //struct { std::string i; } Integer;
    } d;
};

static struct Node* make_node(NodeType type) {
    struct Node *n;
    n = new Node;
    n->type = type;
    return n;
}

#define TYPE PNode
#define ADD(x, y) make_node(NodeType::Add)
#define SUB(x, y) make_node(NodeType::Sub)
#define MUL(x, y) make_node(NodeType::Mul)
#define DIV(x, y) make_node(NodeType::Div)
#define POW(x, y) make_node(NodeType::Pow)
#define SYMBOL(x) make_node(NodeType::Symbol)
#define INTEGER(x) make_node(NodeType::Integer)
#define PRINT(x) std::cout << x->type << std::endl


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
