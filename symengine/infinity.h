/**
 *  \file infinity.h
 *
 **/

#ifndef SYMENGINE_INFINITY_H
#define SYMENGINE_INFINITY_H

#include <symengine/basic.h>
#include <symengine/number.h>

namespace SymEngine
{

/** This class holds "infinity"
 *  It includes a direction (like -infinity).
 **/
class Infty : public Number
{
    RCP<const Number> _direction;

public:
    IMPLEMENT_TYPEID(INFTY)
    //! Constructs Infty using the sign of `_direction`
    Infty(const RCP<const Number> &direction);
    //! Copy Constructor
    Infty(const Infty &inf);
    static RCP<const Infty> from_direction(const RCP<const Number> &direction);
    //! Constructs Infty using sign of `val`
    static RCP<const Infty> from_int(const int val);

    //! \return true if canonical
    bool is_canonical(const RCP<const Number> &num) const;
    //! \return size of the hash
    hash_t __hash__() const;

    /*! Equality comparator
     * \param o - Object to be compared with
     * \return whether the 2 objects are equal
     * */
    // Implement these
    bool __eq__(const Basic &o) const;
    int compare(const Basic &o) const;

    virtual vec_basic get_args() const
    {
        return {_direction};
    }

    //! \return `true` if `0`
    inline bool is_zero() const
    {
        return false;
    }
    //! \return `true` if `1`
    inline bool is_one() const
    {
        return false;
    }
    //! \return `true` if `-1`
    inline bool is_minus_one() const
    {
        return false;
    }

    inline RCP<const Number> get_direction() const
    {
        return _direction;
    }

    bool is_unsigned_infinity() const;
    bool is_positive_infinity() const;
    bool is_negative_infinity() const;

    inline bool is_positive() const
    {
        return is_positive_infinity();
    }

    inline bool is_negative() const
    {
        return is_negative_infinity();
    }

    // Think about it again
    RCP<const Number> add(const Number &other) const;
    RCP<const Number> mul(const Number &other) const;
    RCP<const Number> div(const Number &other) const;  // TODO
    RCP<const Number> pow(const Number &other) const;  // TODO
    RCP<const Number> rpow(const Number &other) const; // TODO
};

inline RCP<const Infty> infty(int n = 1)
{
    return make_rcp<Infty>(integer(n));
}

RCP<const Infty> infty(const RCP<const Number> &direction);

} // SymEngine
#endif
