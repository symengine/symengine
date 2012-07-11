#include "add.h"
#include <stdexcept>

namespace CSymPy {

Add::Add(std::unordered_map<int, int> &dict)
{
    this->dict = dict;
}

std::size_t Add::__hash__() const
{
    throw std::runtime_error("Not implemented yet.");
}

} // CSymPy
