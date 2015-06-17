#include <stdexcept>

#include "add.h"
#include "mul.h"
#include "pow.h"
#include "symbol.h"
#include "rings.h"
#include "monomials.h"

namespace SymEngine {

void expr2poly(const RCP<const Basic> &p, umap_basic_num &syms, umap_vec_mpz &P)
{
    if (is_a<Add>(*p)) {
        int n = syms.size();
        const umap_basic_num &d = rcp_static_cast<const Add>(p)->dict_;
        vec_int exp;
        mpz_class coef;
        for (auto &p: d) {
            if (!is_a<Integer>(*p.second))
                    throw std::runtime_error("Not implemented.");
            coef = rcp_static_cast<const Integer>(p.second)->as_mpz();
            exp.assign(n, 0); // Initialize to [0]*n
            if (is_a<Mul>(*p.first)) {
                const map_basic_basic &term = rcp_static_cast<const Mul>(p.first)->dict_;
                for (auto &q: term) {
                    RCP<const Basic> sym = q.first;
                    if (!is_a<Integer>(*syms.at(sym)))
                            throw std::runtime_error("Not implemented.");
                    int i = rcp_static_cast<const Integer>(syms.at(sym))->as_int();
                    if (is_a<Integer>(*q.second)) {
                        exp[i] = rcp_static_cast<const Integer>(q.second)->as_int();
                    } else {
                        throw std::runtime_error("Cannot convert symbolic exponents to sparse polynomials with integer exponents.");
                    }
                }
            } else if (is_a<Pow>(*p.first)) {
                RCP<const Basic> sym = rcp_static_cast<const Pow>(p.first)->base_;
                RCP<const Basic> exp_ = rcp_static_cast<const Pow>(p.first)->exp_;
                if (!is_a<Integer>(*syms.at(sym)))
                        throw std::runtime_error("Not implemented.");
                int i = rcp_static_cast<const Integer>(syms.at(sym))->as_int();
                if (!is_a<Integer>(*exp_))
                    throw std::runtime_error("Not implemented.");
                exp[i] = rcp_static_cast<const Integer>(exp_)->as_int();
            } else if (is_a<Symbol>(*p.first)) {
                RCP<const Basic> sym = p.first;
                if (!is_a<Integer>(*syms.at(sym)))
                        throw std::runtime_error("Not implemented.");
                int i = rcp_static_cast<const Integer>(syms.at(sym))->as_int();
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

void poly2packed(const umap_vec_mpz &A, umap_ull_mpz &B)
{
    for (auto &a: A) {
        B[pack_vec_int(a.first)] = piranha::integer{a.second.get_str()};
    }
}

void poly_mul2(const umap_ull_mpz &A, const umap_ull_mpz &B, umap_ull_mpz &C)
{
    for (auto &a: A) {
        for (auto &b: B) {
            C[a.first + b.first] += a.second*b.second;
        }
    }
}

void packed2hashset(const umap_ull_mpz &A, hash_set &B)
{
    for (auto &a: A) {
        std::pair<unsigned long long, piranha::integer> temp;
        temp.first = a.first;
        auto v = a.second.get_mpz_view();
        temp.second = mpz_class(v).get_ui();
        B.insert(temp);
    }
}

void poly_mul3(const hash_set &A, const hash_set &B, hash_set &C)
{   
    std::pair<unsigned long long, piranha::integer> temp;
    for (auto &a: A) {
        for (auto &b: B) {
            temp.first = a.first + b.first;
            temp.second = a.second*b.second;
            C.insert(temp);
        }
    }
}

void hashset2packed(const hash_set &A, umap_ull_mpz &B)
{
    for (auto &a: A) {
        B[a.first] = a.second;
    }
}

void packed2poly(const umap_ull_mpz &A, const umap_basic_num &syms, umap_vec_mpz &P)
{
    uint b = 0;
    uint n_var = syms.size();
    while (b*(n_var + 1) < 64)
        b++;

    unsigned long long n = 1<<b;

    for (auto &a: A) {
        vec_int exp;
        unsigned long long ull = a.first;
        for (uint i = 0; i < n_var; i++) {
            exp.push_back(ull % n);
            ull = ull / n;
        }
        auto v = a.second.get_mpz_view();  
        P[exp] = mpz_class(v);
    }
}

} // SymEngine
