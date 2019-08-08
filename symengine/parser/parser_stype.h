#ifndef SYMENGINE_PARSER_STYPE_H
#define SYMENGINE_PARSER_STYPE_H

//#include "sem1.h"
//#include "sem2.h"
//#include "sem3.h"
//#include "sem4.h"
//#include "sem5.h"
#include "sem6.h"
//#include "sem7.h"



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
