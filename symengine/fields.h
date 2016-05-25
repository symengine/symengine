/**
 *  \file fields.h
 *
 **/
#ifndef SYMENGINE_FIELDS_H
#define SYMENGINE_FIELDS_H

#include <symengine/basic.h>

namespace SymEngine
{
class GaloisField : public Basic
{
public:
    std::map<unsigned, int> dict_;
    unsigned modulo_;

public:
    IMPLEMENT_TYPEID(GALOISFIELD)
    GaloisField(std::map<unsigned, int> &dict, unsigned modulo);
    GaloisField(std::vector<int> &dict, unsigned modulo);
    virtual std::size_t __hash__() const;
    virtual bool __eq__(const Basic &o) const;
    virtual int compare(const Basic &o) const;
    virtual vec_basic get_args() const
    {
        return {};
    }

    int mod(int a, int b) const;
    RCP<const GaloisField> gf_neg() const;
    RCP<const GaloisField> gf_add_ground(int a) const;
    RCP<const GaloisField> gf_sub_ground(int a) const;
    RCP<const GaloisField> gf_mul_ground(int a) const;

    RCP<const GaloisField> gf_add(const RCP<const GaloisField> &o) const;
    RCP<const GaloisField> gf_sub(const RCP<const GaloisField> &o) const;
    RCP<const GaloisField> gf_mul(const RCP<const GaloisField> &o) const;
};

//! \return RCP<const GaloisField>
inline RCP<const GaloisField> gf(std::vector<int> &dict, unsigned modulo)
{
    return make_rcp<const GaloisField>(dict, modulo);
}

//! \return RCP<const GaloisField>
inline RCP<const GaloisField> gf(std::map<unsigned, int> &dict, unsigned modulo)
{
    return make_rcp<const GaloisField>(dict, modulo);
}
}

#endif
