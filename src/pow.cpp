#include <stdexcept>

#include "pow.h"
#include "symbol.h"
#include "integer.h"

using Teuchos::RCP;
using Teuchos::Ptr;
using Teuchos::outArg;
using Teuchos::rcp;
using Teuchos::rcp_dynamic_cast;

namespace CSymPy {

Pow::Pow(const Teuchos::RCP<Basic> &base, const Teuchos::RCP<Basic> &exp)
{
    this->base = base;
    this->exp = exp;
}

std::size_t Pow::__hash__() const
{
    std::size_t seed = 0;
    hash_combine<Basic>(seed, *(this->base));
    hash_combine<Basic>(seed, *(this->exp));
    return seed;
}

bool Pow::__eq__(const Basic &o) const
{
    if (is_a<Pow>(o)) {
        const Pow &s = static_cast<const Pow &>(o);
        if (*(this->base) == *(s.base) && *(this->exp) == *(s.exp)) {
            return true;
        }
    }
    return false;
}

std::string Pow::__str__() const
{
    std::ostringstream o;
    o << *(this->base) << "^" << *(this->exp);
    return o.str();
}

typedef std::vector<int> vec_int;
// TODO: this "int" needs to be converted to Integer
typedef std::map<vec_int, int> multi_dict;

std::ostream& operator<<(std::ostream& out, const vec_int& d)
{
    out << "[";
    for (auto &p: d)
        //out << p->__str__() << ", ";
        out << p << ", ";
    out << "]";
    return out;
}

std::ostream& operator<<(std::ostream& out, const multi_dict& d)
{
    out << "{";
    for (auto &p: d)
        out << (p.first) << ": " << (p.second) << ", ";
    out << "}";
    return out;
}

// TODO: m, n should stay ints, but the calculted coefficients must be
// Integers
void multinomial_coefficients(int m, int n)
{
    multi_dict r;
    vec_int t;
    int j, tj, start, v, k;
    t.assign(m, 0);
    t[0] = n;
    r[t] = 1;
    j = 0;
    while (j < m - 1) {
        tj = t[j];
        if (j) {
            t[j] = 0;
            t[0] = tj;
        }
        if (tj > 1) {
            t[j+1] += 1;
            j = 0;
            start = 1;
            v = 0;
        } else {
            j += 1;
            start = j + 1;
            v = r[t];
            t[j] += 1;
        }
        for (k=start; k<m; k++) {
            if (t[k]) {
                t[k] -= 1;
                v += r[t];
                t[k] += 1;
            }
        }
        t[0] -= 1;
        r[t] = (v*tj) / (n-t[0]);
    }
    std::cout << r << std::endl;
}

} // CSymPy
