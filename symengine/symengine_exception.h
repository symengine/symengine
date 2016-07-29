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

class NotImplemented : public std::exception
{
    virtual const char *what() const throw()
    {
        return "Not Implemented";
    }
};

class ResultIsComplex : public std::exception
{
    virtual const char *what() const throw()
    {
        return "Result is Complex. Recompile with MPC support";
    }
};

class Undefined : public std::exception
{
    virtual const char *what() const throw()
    {
        return "Undefined";
    }
};

}
#endif // SYMENGINE_EXCEPTION_H
