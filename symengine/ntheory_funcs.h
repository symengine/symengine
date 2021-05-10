#ifndef SYMENGINE_NTHEORY_FUNCS_H
#define SYMENGINE_NTHEORY_FUNCS_H

#include <symengine/basic.h>
#include <symengine/symengine_casts.h>
#include <symengine/constants.h>
#include <symengine/functions.h>

namespace SymEngine
{

class PrimePi : public OneArgFunction
{
    /*! The prime counting function pi(x)
     * A function that takes a real value x and returns the number of
     * primes less than or equal to x.
     * https://en.wikipedia.org/wiki/Prime-counting_function
     **/

public:
    IMPLEMENT_TYPEID(SYMENGINE_PRIMEPI)
    PrimePi(const RCP<const Basic> &arg);
    bool is_canonical(const RCP<const Basic> &arg) const;
    RCP<const Basic> create(const RCP<const Basic> &arg) const;
};

RCP<const Basic> primepi(const RCP<const Basic> &arg);

class Primorial : public OneArgFunction
{
    /*! The primorial of n (n#)
     * The product all primes up to n
     * https://en.wikipedia.org/wiki/Primorial
     **/

public:
    IMPLEMENT_TYPEID(SYMENGINE_PRIMORIAL)
    Primorial(const RCP<const Basic> &arg);
    bool is_canonical(const RCP<const Basic> &arg) const;
    RCP<const Basic> create(const RCP<const Basic> &arg) const;
};

RCP<const Basic> primorial(const RCP<const Basic> &arg);

} // namespace SymEngine

#endif
