#ifndef SYMENGINE_EXCEPTION_H
#define SYMENGINE_EXCEPTION_H

enum symengine_exceptions_t {
    NO_EXCEPTION,
    DIV_BY_ZERO,
    NOT_IMPLEMENTED,
    UNDEFINED
};

namespace SymEngine
{

class SymEngineException : public std::exception
{
    std::string m_msg;
    symengine_exceptions_t ec;

public:
    SymEngineException(const std::string &msg, symengine_exceptions_t error)
        : m_msg(msg), ec(error)
    {
    }
    virtual const char *what() const throw()
    {
        return m_msg.c_str();
    }
    symengine_exceptions_t error_code()
    {
        return ec;
    }
};
}
#endif // SYMENGINE_EXCEPTION_H
