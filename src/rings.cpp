#include <stdexcept>

#include "add.h"
#include "mul.h"
#include "pow.h"
#include "symbol.h"
#include "rings.h"
#include "monomials.h"

namespace CSymPy {

void expr2poly(const RCP<Basic> &p, umap_basic_int &syms, umap_vec_mpz &P)
{
    if (is_a<Add>(*p)) {
        int n = syms.size();
        umap_basic_int &d = rcp_static_cast<Add>(p)->dict_;
        vec_int exp;
        mpz_class coef;
        for (auto &p: d) {
            if (!is_a<Integer>(*p.second))
                    throw std::runtime_error("Not implemented.");
            coef = rcp_static_cast<Integer>(p.second)->as_mpz();
            exp.assign(n, 0); // Initialize to [0]*n
            if (is_a<Mul>(*p.first)) {
                map_basic_basic &term = rcp_static_cast<Mul>(p.first)->dict_;
                for (auto &q: term) {
                    RCP<Basic> sym = q.first;
                    if (!is_a<Integer>(*syms[sym]))
                            throw std::runtime_error("Not implemented.");
                    int i = rcp_static_cast<Integer>(syms[sym])->as_int();
                    if (is_a<Integer>(*q.second)) {
                        exp[i] = rcp_static_cast<Integer>(q.second)->as_int();
                    } else {
                        throw std::runtime_error("Cannot convert symbolic exponents to sparse polynomials with integer exponents.");
                    }
                }
            } else if (is_a<Pow>(*p.first)) {
                RCP<Basic> sym = rcp_static_cast<Pow>(p.first)->base_;
                RCP<Basic> exp_ = rcp_static_cast<Pow>(p.first)->exp_;
                if (!is_a<Integer>(*syms[sym]))
                        throw std::runtime_error("Not implemented.");
                int i = rcp_static_cast<Integer>(syms[sym])->as_int();
                if (!is_a<Integer>(*exp_))
                    throw std::runtime_error("Not implemented.");
                exp[i] = rcp_static_cast<Integer>(exp_)->as_int();
            } else if (is_a<Symbol>(*p.first)) {
                RCP<Basic> sym = p.first;
                if (!is_a<Integer>(*syms[sym]))
                        throw std::runtime_error("Not implemented.");
                int i = rcp_static_cast<Integer>(syms[sym])->as_int();
                exp[i] = 1;
            } else {
                throw std::runtime_error("Not implemented.");
            }

            P[exp] = coef;
        }
    } else {
        throw std::runtime_error("Not implemented.");
    }
}

void poly_mul(const umap_vec_mpz &A, const umap_vec_mpz &B, umap_vec_mpz &C)
{
    vec_int exp;
    int n = (A.begin()->first).size();
    exp.assign(n, 0); // Initialize to [0]*n
    /*
    std::cout << "A: " << A.load_factor() << " " << A.bucket_count() << " " << A.size() << " "
        << A.max_bucket_count() << std::endl;
    std::cout << "B: " << B.load_factor() << " " << B.bucket_count() << " " << B.size() << " "
        << B.max_bucket_count() << std::endl;
    std::cout << "C: " << C.load_factor() << " " << C.bucket_count() << " " << C.size() << " "
        << C.max_bucket_count() << std::endl;
        */
    for (auto &a: A) {
        for (auto &b: B) {
            monomial_mul(a.first, b.first, exp);
            C[exp] += a.second*b.second;
        }
    }
    /*
    std::cout << "C: " << C.load_factor() << " " << C.bucket_count() << " " << C.size() << " "
        << C.max_bucket_count() << std::endl;
    for (std::size_t n=0; n < C.bucket_count(); n++) {
        std::cout << n << ": " << C.bucket_size(n) << "|";
        for (auto it = C.begin(n); it != C.end(n); ++it)
            std::cout << " " << it->first << myhash2(it->first) % C.bucket_count();
        std::cout << std::endl;
    }
    */
}

} // CSymPy
