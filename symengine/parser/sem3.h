#ifndef SYMENGINE_PARSER_SEM3_H
#define SYMENGINE_PARSER_SEM3_H

// 10 - 13ms
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


#endif
