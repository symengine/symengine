#include <stdexcept>

#include "add.h"
#include "symbol.h"

using Teuchos::RCP;
using Teuchos::rcp;

namespace CSymPy {

Add::Add(const Dict_int& dict)
{
    this->dict = dict;
}

std::size_t Add::__hash__() const
{
    throw std::runtime_error("Not implemented yet.");
}

bool Add::__eq__(const Basic &o) const
{
    throw std::runtime_error("Not implemented yet.");
}

std::string Add::__str__() const
{
    std::ostringstream s;
    for (auto &p: this->dict)
        s << p.second << *(p.first) << " + ";
    return s.str();
}

RCP<CSymPy::Basic> add_from_dict(const Dict_int &d)
{
    return rcp(new Add(d));
}

// Adds (coef*t) to the dict "d"
// Assumption: "t" does not have any numerical coefficients, those are in "coef"
void dict_add_term(Dict_int &d, int coef, const RCP<Basic> &t)
{
    if (d.find(t) == d.end()) {
        // Not found:
        d[t] = coef;
    } else {
        d[t] = d[t] + coef;
    }
}

} // CSymPy

using CSymPy::Basic;
using CSymPy::Add;

void as_coef_term(const RCP<Basic> &self, int *coef, RCP<Basic> *term)
{
    if (CSymPy::is_a<CSymPy::Symbol>(*self)) {
        *coef = 1;
        *term = self;
    } else {
        throw std::runtime_error("Not implemented yet.");
    }
}

RCP<Basic> operator+(const RCP<Basic> &a, const RCP<Basic> &b)
{
    CSymPy::Dict_int d;
    int coef;
    RCP<Basic> t;
    if (CSymPy::is_a<Add>(*a) && CSymPy::is_a<Add>(*b)) {
        // TODO: Fix these pointer hacks:
        d = ((Add*) a.get())->dict;
        for (auto &p: ((Add*) b.get())->dict)
            dict_add_term(d, p.second, p.first);
    } else if (CSymPy::is_a<Add>(*a)) {
        // TODO: Fix this pointer hacks:
        d = ((Add*) a.get())->dict;
        as_coef_term(b, &coef, &t);
        dict_add_term(d, coef, t);
    } else if (CSymPy::is_a<Add>(*b)) {
        // TODO: Fix this pointer hacks:
        d = ((Add*) b.get())->dict;
        as_coef_term(a, &coef, &t);
        dict_add_term(d, coef, t);
    } else {
        as_coef_term(a, &coef, &t);
        d[t] = coef;
        as_coef_term(b, &coef, &t);
        dict_add_term(d, coef, t);
    }
    return CSymPy::add_from_dict(d);
}

RCP<Basic> operator-(const RCP<Basic> &a, const RCP<Basic> &b)
{
//    return a + (-b);
    throw std::runtime_error("Not implemented yet.");
}
