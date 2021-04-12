#include <symengine/ntheory.h>
#include <symengine/ntheory_funcs.h>
#include <symengine/prime_sieve.h>

namespace SymEngine
{

PrimePi::PrimePi(const RCP<const Basic> &arg) : OneArgFunction(arg)
{
    SYMENGINE_ASSIGN_TYPEID()
    SYMENGINE_ASSERT(is_canonical(arg))
}

bool PrimePi::is_canonical(const RCP<const Basic> &arg) const
{
    if (is_a_Number(*arg) or is_a<Constant>(*arg)) {
        return false;
    } else {
        return true;
    }
}

RCP<const Basic> PrimePi::create(const RCP<const Basic> &arg) const
{
    return primepi(arg);
}

RCP<const Basic> primepi(const RCP<const Basic> &arg)
{
    if (is_a_Number(*arg)) {
        if (is_a<NaN>(*arg)) {
            return arg;
        } else if (is_a<Infty>(*arg)) {
            if (down_cast<const Infty &>(*arg).is_negative_infinity()) {
                return make_rcp<const Integer>(integer_class(0));
            } else {
                return arg;
            }
        } else if (down_cast<const Number &>(*arg).is_complex()) {
            throw SymEngineException("Complex can't be passed to primepi!");
        } else if (down_cast<const Number &>(*arg).is_negative()) {
            return make_rcp<const Integer>(integer_class(0));
        }
    }
    if (is_a_Number(*arg) or is_a<Constant>(*arg)) {
        unsigned int num
            = (unsigned int)down_cast<const Integer &>(*SymEngine::floor(arg))
                  .as_uint();
        Sieve::iterator pi(num);
        unsigned long int p = 0;
        while ((pi.next_prime()) <= num) {
            p++;
        }
        return make_rcp<const Integer>(integer_class(p));
    }
    return make_rcp<const PrimePi>(arg);
}

Primorial::Primorial(const RCP<const Basic> &arg) : OneArgFunction(arg)
{
    SYMENGINE_ASSIGN_TYPEID()
    SYMENGINE_ASSERT(is_canonical(arg))
}

bool Primorial::is_canonical(const RCP<const Basic> &arg) const
{
    if (is_a_Number(*arg) or is_a<Constant>(*arg)) {
        return false;
    } else {
        return true;
    }
}

RCP<const Basic> Primorial::create(const RCP<const Basic> &arg) const
{
    return primorial(arg);
}

RCP<const Basic> primorial(const RCP<const Basic> &arg)
{
    if (is_a_Number(*arg)) {
        if (is_a<NaN>(*arg)) {
            return arg;
        }
        if (down_cast<const Number &>(*arg).is_positive()) {
            if (is_a<Infty>(*arg)) {
                return arg;
            }
        } else {
            throw SymEngineException(
                "Only positive numbers are allowed for primorial!");
        }
    }
    if (is_a_Number(*arg) or is_a<Constant>(*arg)) {
        unsigned long n
            = down_cast<const Integer &>(*SymEngine::floor(arg)).as_uint();
        return make_rcp<const Integer>(mp_primorial(n));
    }
    return make_rcp<const Primorial>(arg);
}

} // namespace SymEngine
