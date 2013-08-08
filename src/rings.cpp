#include <stdexcept>

#include "add.h"
#include "mul.h"
#include "rings.h"
#include "monomials.h"

using Teuchos::RCP;
using Teuchos::Ptr;
using Teuchos::outArg;
using Teuchos::rcp;
using Teuchos::rcp_dynamic_cast;
using Teuchos::rcp_static_cast;

namespace CSymPy {

void expr2poly(const RCP<Basic> &p, Dict_int &syms, map_vec_mpz &P)
{
    if (is_a<Add>(*p)) {
        int n = syms.size();
        Dict_int &d = rcp_static_cast<Add>(p)->dict;
        vec_int exp;
        mpz_class coef;
        for (auto &p: d) {
            coef = p.second->as_mpz();
            exp.assign(n, 0); // Initialize to [0]*n
            if (is_a<Mul>(*p.first)) {
                Dict_int &term = rcp_static_cast<Mul>(p.first)->dict;
                for (auto &q: term) {
                    RCP<Basic> sym = q.first;
                    int i = syms[sym]->as_int();
                    exp[i] = q.second->as_int();
                }
            } else {
                throw std::runtime_error("Not implemented.");
            }

            P[exp] = coef;
        }
    } else {
        throw std::runtime_error("Not implemented.");
    }
}

void poly_mul(const map_vec_mpz &A, const map_vec_mpz &B, map_vec_mpz &C)
{
    vec_int exp;
    int n = (A.begin()->first).size();
    exp.assign(n, 0); // Initialize to [0]*n
    for (auto &a: A) {
        for (auto &b: B) {
            monomial_mul(a.first, b.first, exp);
            C[exp] = a.second*b.second;
        }
    }
}

} // CSymPy
