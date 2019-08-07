#ifndef SYMENGINE_PARSER_SEM2_H
#define SYMENGINE_PARSER_SEM2_H

// Computer 1: 8ms 72ms
// Computer 2: 8 - 9ms

#define TYPE int
#define ADD(x, y) x+y
#define SUB(x, y) x-y
#define MUL(x, y) x*y
#define DIV(x, y) x/y
#define POW(x, y) std::pow(x,y)
#define SYMBOL(x) 1
#define INTEGER(x) std::stoi(x)
#define PRINT(x) std::cout << x << std::endl


#endif
