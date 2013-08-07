#include "integer.h"

namespace CSymPy {

Integer::Integer(int i)
{
    this->i = i;
}

std::size_t Integer::__hash__() const
{
    std::hash<long long int> hash_fn;
    return hash_fn(this->i);
}

bool Integer::__eq__(const Basic &o) const
{
    if (is_a<Integer>(o)) {
        const Integer &s = static_cast<const Integer &>(o);
        return this->i == s.i;
    }
    return false;
}

std::string Integer::__str__() const
{
    std::ostringstream s;
    s << this->i;
    return s.str();
}

} // CSymPy

Teuchos::RCP<CSymPy::Integer> operator-(const Teuchos::RCP<CSymPy::Integer> &a,
        const Teuchos::RCP<CSymPy::Integer> &b)
{
    return Teuchos::rcp(new CSymPy::Integer(a->i - b->i));
}

Teuchos::RCP<CSymPy::Integer> operator*(const Teuchos::RCP<CSymPy::Integer> &a,
        const Teuchos::RCP<CSymPy::Integer> &b)
{
    return Teuchos::rcp(new CSymPy::Integer(a->i * b->i));
}

Teuchos::RCP<CSymPy::Integer> operator/(const Teuchos::RCP<CSymPy::Integer> &a,
        const Teuchos::RCP<CSymPy::Integer> &b)
{
    return Teuchos::rcp(new CSymPy::Integer(a->i / b->i));
}

Teuchos::RCP<CSymPy::Integer> operator-(const Teuchos::RCP<CSymPy::Integer> &a)
{
    return Teuchos::rcp(new CSymPy::Integer(- (a->i)));
}
