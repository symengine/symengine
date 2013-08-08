#include "dict.h"

std::ostream& operator<<(std::ostream& out, const CSymPy::Dict_int& d)
{
    out << "{";
    for (auto &p: d)
        out << *(p.first) << ": " << *(p.second) << ", ";
    out << "}";
    return out;
}

std::ostream& operator<<(std::ostream& out, const CSymPy::vec_int& d)
{
    out << "[";
    for (auto &p: d)
        out << p << ", ";
    out << "]";
    return out;
}

std::ostream& operator<<(std::ostream& out, const CSymPy::map_vec_int& d)
{
    out << "{";
    for (auto &p: d)
        out << (p.first) << ": " << (p.second) << ", ";
    out << "}";
    return out;
}


using Teuchos::RCP;
using Teuchos::Ptr;
using Teuchos::outArg;
using Teuchos::rcp;
using Teuchos::rcp_dynamic_cast;

namespace CSymPy {

bool dicts_equal(const Dict_int &a, const Dict_int &b)
{
    // This follows the same algorithm as Python's dictionary comparison
    // (a==b), which is implemented by "dict_equal" function in
    // Objects/dictobject.c.

    // Can't be equal if # of entries differ:
    if (a.size() != b.size()) return false;
    // Loop over keys in "a":
    for (auto &p: a) {
        // O(1) lookup of the key in "b":
        auto f = b.find(p.first);
        if (f == b.end()) return false; // no such element in "b"
        if (neq(p.second, f->second)) return false; // values not equal
    }
    return true;
}

}
