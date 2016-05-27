/**
 *  \file fields.h
 *
 **/
#ifndef SYMENGINE_FIELDS_H
#define SYMENGINE_FIELDS_H

#include <symengine/basic.h>
#include <symengine/ntheory.h>
#include <symengine/dict.h>

namespace SymEngine
{
class GaloisField : public Basic
{
public:
    map_uint_mpz dict_;
    integer_class modulo_;

public:
    IMPLEMENT_TYPEID(GALOISFIELD)
    GaloisField(const map_uint_mpz &dict, const integer_class modulo);
    GaloisField(const std::vector<integer_class> &v,
                const integer_class modulo);
    virtual std::size_t __hash__() const;
    virtual bool __eq__(const Basic &o) const;
    virtual int compare(const Basic &o) const;
    virtual vec_basic get_args() const
    {
        return {};
    }

    RCP<const GaloisField> gf_neg() const;
    RCP<const GaloisField> gf_add_ground(const integer_class a) const;
    RCP<const GaloisField> gf_sub_ground(const integer_class a) const;
    RCP<const GaloisField> gf_mul_ground(const integer_class a) const;
    RCP<const GaloisField> gf_quo_ground(const integer_class a) const;

    RCP<const GaloisField> gf_add(const RCP<const GaloisField> &o) const;
    RCP<const GaloisField> gf_sub(const RCP<const GaloisField> &o) const;
    RCP<const GaloisField> gf_mul(const RCP<const GaloisField> &o) const;
};

//! \return RCP<const GaloisField>
inline RCP<const GaloisField> gf(const map_uint_mpz &dict,
                                 const integer_class modulo)
{
    return make_rcp<const GaloisField>(dict, modulo);
}

//! \return RCP<const GaloisField>
inline RCP<const GaloisField> gf(const std::vector<integer_class> &v,
                                 const integer_class modulo)
{
    return make_rcp<const GaloisField>(v, modulo);
}
}

#endif
