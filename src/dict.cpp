#include "dict.h"

std::ostream& operator<<(std::ostream& out, const CSymPy::Dict_int& d)
{
    out << "{";
    for (auto &p: d)
        out << *(p.first) << ": " << *(p.second) << ", ";
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
    // TODO: figure out how to do this most efficiently
    if (a.size() != b.size()) return false;
    for (auto &p: a) {
        auto f = b.find(p.first);
        if (f == b.end()) return false; // no such element in "b"
        if (*(p.second) != *(f->second)) return false; // coefs not equal
    }
    return true;
}

}
