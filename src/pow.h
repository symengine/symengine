#ifndef CSYMPY_POW_H
#define CSYMPY_POW_H

#include "basic.h"
#include "dict.h"
#include "mul.h"
#include "integer.h"

namespace CSymPy {

class Pow : public Basic {
public: // TODO: make this private
    RCP<const Basic> base_, exp_; // base^exp

public:
    Pow(const RCP<const Basic> &base, const RCP<const Basic> &exp);
    virtual std::size_t __hash__() const;
    virtual bool __eq__(const Basic &o) const;
    virtual int compare(const Basic &o) const;
    virtual std::string __str__() const;

    bool is_canonical(const RCP<const Basic> &base,
            const RCP<const Basic> &exp);
    inline RCP<const Basic> get_base() const { return base_; }
    inline RCP<const Basic> get_exp() const { return exp_; }

    virtual RCP<const Basic> diff(const RCP<const Symbol> &x) const;
    virtual RCP<const Basic> subs(const map_basic_basic &subs_dict) const;
};

RCP<const Basic> pow(const RCP<const Basic> &a,
        const RCP<const Basic> &b);


void multinomial_coefficients(int m, int n, map_vec_int &r);

RCP<const Basic> pow_expand(const RCP<const Pow> &self);

inline RCP<const Basic> sqrt(const RCP<const Basic> &x) {
    return pow(x, div(one, integer(2)));
}

} // CSymPy

#endif
