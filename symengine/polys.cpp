#include <symengine/polys.h>
#include <symengine/add.h>
#include <symengine/mul.h>
#include <symengine/pow.h>
#include <symengine/symbol.h>

namespace SymEngine {

Polynomial::Polynomial(const vec_symbol &vars, hash_set polys_set)
    : vars_{vars}, polys_set_(polys_set) {

    SYMENGINE_ASSERT(is_canonical(polys_set_))
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

//TODO
vec_basic Polynomial::get_args() const
{
    vec_basic args;
    return args;
}

//TODO
RCP<const Basic> Polynomial::diff(const RCP<const Symbol> &x) const
{
    return x;
}

RCP<const Polynomial> add_poly(const Polynomial &a, const Polynomial &b) {
    hash_set res = a.polys_set_;
    m_pair temp;
    for (auto &it : b.polys_set_) {
        temp.first = it.first;
        auto it2 = res.find(temp);
        if (it2 == res.end()) {
            temp.second = it.second;
            res.insert(temp);
        }
        else {
            it2->second = it2->second + it.second;
        }
    }

    RCP<const Polynomial> c = polynomial(a.vars_, std::move(res));
    return c;
}

RCP<const Polynomial> sub_poly(const Polynomial &a, const Polynomial &b) {
    hash_set res = a.polys_set_;
    m_pair temp;
    for (auto &it : b.polys_set_) {
        temp.first = it.first;
        auto it2 = res.find(temp);
        if (it2 == res.end()) {
            temp.second = it.second;
            res.insert(temp);
        }
        else {
            it2->second = it2->second - it.second;
        }
    }

    RCP<const Polynomial> c = polynomial(a.vars_, std::move(res));
    return c;
}

RCP<const Polynomial> neg_poly(const Polynomial &a) {
    hash_set res;
    m_pair temp;
    for (auto &it: a.polys_set_) {
        temp.first = it.first;
        temp.second = -1 * it.second;
        res.insert(temp);
    }
    return polynomial(a.vars_, res);
}

RCP<const Polynomial> mul_poly(RCP <const Polynomial> a, RCP <const Polynomial> b) {
    hash_set C, A = a->polys_set_, B = b->polys_set_;
    m_pair temp;
    for (auto &a: A) {
        for (auto &b: B) {
            temp.first = a.first + b.first;
            size_t bucket = C._bucket(temp);
            auto it = C._find(temp, bucket);
            if (it == C.end()) {
                // Check it the load factor of C is too large.
                if ((double(C.size()) + 1) / C.bucket_count() > C.max_load_factor()) {
                    // Increase the size of the table.
                    C._increase_size();
                    // Recompute the bucker.
                    bucket = C._bucket(temp);
                }
                temp.second = a.second;
                temp.second *= b.second;
                C._unique_insert(temp, bucket);
                C._update_size(C.size() + 1u);
            } else {
                piranha::math::multiply_accumulate(it->second,a.second,b.second);
            }
        }
    }
    return polynomial(a->vars_, C);
}


} // SymEngine
