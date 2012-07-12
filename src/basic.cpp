#include "basic.h"

std::ostream& operator<<(std::ostream& out, const CSymPy::Basic& p)
{
    out << p.__str__();
    return out;
}

