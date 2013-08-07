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

int Integer::as_int()
{
    if (this->i > std::numeric_limits<int>::max()) {
        throw std::runtime_error("as_int: Integer larger than int");
    }
    if (this->i < std::numeric_limits<int>::min()) {
        throw std::runtime_error("as_int: Integer larger than int");
    }
    return this->i;
}

} // CSymPy
