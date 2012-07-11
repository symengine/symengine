#include "basic.h"

std::ostream& operator<<(std::ostream& out, const CSymPy::Basic& p)
{
    out << "Basic()";
    return out;
}

