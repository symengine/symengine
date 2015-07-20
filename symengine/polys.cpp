#include <symengine/polys.h>
#include <symengine/add.h>
#include <symengine/mul.h>
#include <symengine/pow.h>
#include <symengine/symbol.h>

namespace SymEngine {

Polynomial::Polynomial(const vec_symbol &vars, hash_set polys_set) {
    //! TODO: Use initializer list
    vars_ = vars;
    polys_set_ = polys_set;
    //SYMENGINE_ASSERT(is_canonical(vars_, polys_set_))
}

Polynomial::Polynomial(const RCP<const Basic> &p, umap_basic_num &vars) {
    //! TODO: Make more robust
    umap_vec_mpz P;
    if (is_a<Add>(*p)) {
        int n = vars.size();
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
                    if (!is_a<Integer>(*vars.at(sym)))
                            throw std::runtime_error("Not implemented.");
                    int i = rcp_static_cast<const Integer>(vars.at(sym))->as_int();
                    if (is_a<Integer>(*q.second)) {
                        exp[i] = rcp_static_cast<const Integer>(q.second)->as_int();
                    } else {
                        throw std::runtime_error("Cannot convert symbolic exponents to sparse polynomials with integer exponents.");
                    }
                }
            } else if (is_a<Pow>(*p.first)) {
                RCP<const Basic> sym = rcp_static_cast<const Pow>(p.first)->base_;
                RCP<const Basic> exp_ = rcp_static_cast<const Pow>(p.first)->exp_;
                if (!is_a<Integer>(*vars.at(sym)))
                        throw std::runtime_error("Not implemented.");
                int i = rcp_static_cast<const Integer>(vars.at(sym))->as_int();
                if (!is_a<Integer>(*exp_))
                    throw std::runtime_error("Not implemented.");
                exp[i] = rcp_static_cast<const Integer>(exp_)->as_int();
            } else if (is_a<Symbol>(*p.first)) {
                RCP<const Basic> sym = p.first;
                if (!is_a<Integer>(*vars.at(sym)))
                        throw std::runtime_error("Not implemented.");
                int i = rcp_static_cast<const Integer>(vars.at(sym))->as_int();
                exp[i] = 1;
            } else {
                throw std::runtime_error("Not implemented.");
            }

            P[exp] = coef;
        }
    } else {
        throw std::runtime_error("Not implemented.");
    }
    
    for (auto &a: vars) {
        vars_.push_back(rcp_static_cast<const Symbol>(a.first));
    }

    for (auto &a: P) {
        m_pair temp;
        using ka = piranha::kronecker_array<long long>;
        temp.first = ka::encode(a.first);
        temp.second = piranha::integer{a.second.get_str()};
        polys_set_.insert(temp);
    }
}

bool Polynomial::is_canonical(const hash_set& set)
{
    for(auto &a: set) {
        for(auto &b: set) {
            if (b.first == a.first)
                return false;
        }
    }
    return true;
}

std::size_t Polynomial::__hash__() const 
{
    std::hash<std::string> hash_string;
    std::size_t seed = POLYNOMIAL;

    for (auto &it: vars_)
        seed += hash_string(it->get_name());

    for (auto &it : polys_set_)
    {   
        std::size_t temp = POLYNOMIAL;
        hash_combine<long long>(temp, it.first);
        hash_combine<long long>(temp, mpz_get_si(it.second.get_mpz_view()));
        seed += temp;
    }
    return seed;
}

bool Polynomial::__eq__(const Basic &o) const
{
    const Polynomial &s = static_cast<const Polynomial &>(o);

    for (uint i = 0; i < vars_.size(); i++) {
        if (!vars_[i]->__eq__(*s.vars_[i]))
            return false;
    }

    if (hash_set_eq(polys_set_, s.polys_set_))
        return true;

    return false;
}

int Polynomial::compare(const Basic &o) const
{
    const Polynomial &s = static_cast<const Polynomial &>(o);

    if (polys_set_.size() != s.polys_set_.size())
        return (polys_set_.size() < s.polys_set_.size()) ? -1 : 1;

    for (uint i = 0; i < vars_.size(); i++) {
        int cmp = vars_[i]->compare(*s.vars_[i]);
        if (cmp != 0)
            return cmp;
    }

    return hash_set_compare(polys_set_, s.polys_set_);
}

vec_basic Polynomial::get_args() const
{
    vec_basic args;
    return args;
}

RCP<const Basic> Polynomial::diff(const RCP<const Symbol> &x) const
{
    return x;
}

} // SymEngine
