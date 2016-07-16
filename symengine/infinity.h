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
class Infinit : public Number
{
    RCP<const Number> _direction;

public:
    IMPLEMENT_TYPEID(INFINIT)
    //! Constructors

    //! Default Constructor gives +infinity(direction = +1)
    Infinit();
    //! Constructs Infinit using the sign of `_direction`
    Infinit(const RCP<const Number> &direction);
    Infinit(const int val);
    //! Copy Constructor
    Infinit(const Infinit &inf);
    static RCP<const Infinit>
    from_direction(const RCP<const Number> &direction);
    //! Constructs Infinit using sign of `val`
    static RCP<const Infinit> from_int(const int val);

    //! \return true if canonical
    bool is_canonical(const RCP<const Number> &num) const;
    //! \return size of the hash
    std::size_t __hash__() const;

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

inline RCP<const Infinit> infinit(const int &n = 1)
{
    return make_rcp<Infinit>(n);
}

inline RCP<const Infinit> infinit(const RCP<const Number> &direction)
{
    return make_rcp<Infinit>(direction);
}

} // SymEngine
#endif
