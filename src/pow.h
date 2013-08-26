#ifndef CSYMPY_POW_H
#define CSYMPY_POW_H

#include "basic.h"
#include "dict.h"
#include "mul.h"
#include "integer.h"

namespace CSymPy {

class Pow : public Basic {
public: // TODO: make this private
    RCP<Basic> base_, exp_; // base^exp

public:
    Pow(const RCP<Basic> &base, const RCP<Basic> &exp);
    virtual std::size_t __hash__() const;
    virtual bool __eq__(const Basic &o) const;
    virtual int compare(const Basic &o) const;
    virtual std::string __str__() const;

    bool is_canonical(const RCP<Basic> &base,
            const RCP<Basic> &exp);

    virtual RCP<Basic> diff(const RCP<Symbol> &x) const;
    virtual RCP<Basic> subs(const map_basic_basic &subs_dict) const;
};

RCP<Basic> pow(const RCP<Basic> &a,
        const RCP<Basic> &b);


void multinomial_coefficients(int m, int n, map_vec_int &r);

RCP<Basic> pow_expand(const RCP<Pow> &self);

inline RCP<Basic> sqrt(const RCP<Basic> &x) {
    return pow(x, div(one, integer(2)));
}

} // CSymPy

#endif
