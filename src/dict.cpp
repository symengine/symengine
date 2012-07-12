#include "dict.h"

std::ostream& operator<<(std::ostream& out, const CSymPy::Dict_int& d)
{
    out << "{";
    for (auto &p: d)
        out << *(p.first) << ": " << p.second << ", ";
    out << "}";
    return out;
}

