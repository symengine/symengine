#include <stdexcept>

#include "add.h"

namespace CSymPy {

Add::Add(Dict_int& dict)
{
    this->dict = dict;
}

std::size_t Add::__hash__() const
{
    throw std::runtime_error("Not implemented yet.");
}

} // CSymPy
