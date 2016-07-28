#ifndef SYMENGINE_EXCEPTION_H
#define SYMENGINE_EXCEPTION_H

namespace SymEngine
{

class DivisionByZero : public std::exception
{
    virtual const char *what() const throw()
    {
        return "Division By Zero";
    }
};
}
#endif // SYMENGINE_EXCEPTION_H
