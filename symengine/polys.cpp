#include <symengine/polys.h>
#include <symengine/add.h>
#include <symengine/mul.h>
#include <symengine/pow.h>
#include <symengine/symbol.h>
#include <symengine/dict.h>

namespace SymEngine {

#ifdef HAVE_SYMENGINE_PIRANHA
Polynomial::Polynomial(const vec_symbol &vars, hash_set polys_set)
    : vars_{vars}, polys_set_(polys_set) {

    SYMENGINE_ASSERT(is_canonical(polys_set_))
}
#else
Polynomial::Polynomial(const vec_symbol &vars, unordered_set polys_set)
    : vars_{vars}, polys_set_(polys_set) {

    SYMENGINE_ASSERT(is_canonical(polys_set_))
}
#endif

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
                RCP<const Basic> sym = rcp_static_cast<const Pow>(p.first)->get_base();
                RCP<const Basic> exp_ = rcp_static_cast<const Pow>(p.first)->get_exp();
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
#ifdef HAVE_SYMENGINE_PIRANHA
        using ka = piranha::kronecker_array<long long>;
        temp.first = ka::encode(a.first);
        temp.second = piranha::integer{a.second.get_str()};
#else
        temp.first = vec_encode(a.first);
        temp.second = (a.second).get_ui();
#endif
        polys_set_.insert(temp);
    }
}

#ifdef HAVE_SYMENGINE_PIRANHA
bool Polynomial::is_canonical(const hash_set& set)
#else
bool Polynomial::is_canonical(const unordered_set& set)
#endif
{
    for(auto &a: set) {
        int count = 0;
        for(auto &b: set) {
            if (b.first == a.first)
                count++;
            if (count == 2)
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
#ifdef HAVE_SYMENGINE_PIRANHA
        hash_combine<long long>(temp, mpz_get_si(it.second.get_mpz_view()));
#else
        hash_combine<long long>(temp, (it.second).get_ui());
#endif
        seed += temp;
    }
    return seed;
}

bool Polynomial::__eq__(const Basic &o) const
{
    const Polynomial &s = static_cast<const Polynomial &>(o);

    for (unsigned int i = 0; i < vars_.size(); i++) {
        if (!vars_[i]->__eq__(*s.vars_[i]))
            return false;
    }

    #ifdef HAVE_SYMENGINE_PIRANHA
    if (hash_set_eq(polys_set_, s.polys_set_))
        return true;
    #else
    if (unordered_set_eq(polys_set_, s.polys_set_))
        return true;
    #endif

    return false;
}

int Polynomial::compare(const Basic &o) const
{
    const Polynomial &s = static_cast<const Polynomial &>(o);

    if (polys_set_.size() != s.polys_set_.size())
        return (polys_set_.size() < s.polys_set_.size()) ? -1 : 1;

    for (unsigned int i = 0; i < vars_.size(); i++) {
        int cmp = vars_[i]->compare(*s.vars_[i]);
        if (cmp != 0)
            return cmp;
    }

    #ifdef HAVE_SYMENGINE_PIRANHA
    return hash_set_compare(polys_set_, s.polys_set_);
    #else
    throw::std::runtime_error("Not Implemented");
    #endif
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
#ifdef HAVE_SYMENGINE_PIRANHA
    hash_set res = a.polys_set_;
#else
    unordered_set res = a.polys_set_;
#endif

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
#ifdef HAVE_SYMENGINE_PIRANHA
    hash_set res = a.polys_set_;
#else
    unordered_set res = a.polys_set_;
#endif
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
#ifdef HAVE_SYMENGINE_PIRANHA
    hash_set res;
#else
    unordered_set res;
#endif
    m_pair temp;
    for (auto &it: a.polys_set_) {
        temp.first = it.first;
        temp.second = -1 * it.second;
        res.insert(temp);
    }
    return polynomial(a.vars_, res);
}

#ifdef HAVE_SYMENGINE_PIRANHA
void _normalise_polymul(const Polynomial &p, const Polynomial &q, vec_symbol &vars_,
    hash_set &p_new, hash_set &q_new) {
    using ka = piranha::kronecker_array<long long>;
    vars_ = p.vars_;
    int c_1 = p.vars_.size();
    int c_2 = 0;
    bool present = false;
    for (auto &i: q.vars_) {
        for (auto &j: p.vars_) {
            if (i->get_name() == j->get_name()) {
                present = true;
            }
        }
        if (present == false) {
            vars_.push_back(i);
            c_2++;
        }
        present = false;
    }
    for (auto &a: p.polys_set_) {
        std::vector<long long> old(p.vars_.size());
        m_pair temp;

        ka::decode(old, a.first);
        for (int i = 0; i < c_2; i++) {
            old.push_back(0);
        }
        temp.first = ka::encode(old);
        temp.second = a.second;
        p_new.insert(temp);
    }
    for (auto &a: q.polys_set_) {
        if (p.vars_.empty()) {
            q_new = q.polys_set_;
        } else {
            std::vector<long long> new_(c_1);
            std::vector<long long> old(q.vars_.size());
            m_pair temp;
            ka::decode(old, a.first);
            auto i = new_.begin();
            bool present = false;
            int j_c = 0, k_c = 0;
            for (auto &j: vars_) {
                for (auto &k: q.vars_) {
                    if (j->get_name() == k->get_name()) {
                        present = true;
                        new_[j_c] = old[k_c];
                    }
                    k_c++;
                }
                if (present == false)
                    new_.insert(i, 0);
                if (i != new_.end())
                    i++;
                present = false;
                j_c++;
                k_c = 0;
            }            
            temp.first = ka::encode(new_);
            temp.second = a.second;
            q_new.insert(temp);
        }
    }
}

void _mul_hashset(const hash_set &A, const hash_set &B, hash_set &C) {
    auto t1 = std::chrono::high_resolution_clock::now();
    m_pair temp;
    for (auto &a: A) {
        for (auto &b: B) {
            temp.first = a.first + b.first;
            size_t bucket = C._bucket(temp);
            auto it = C._find(temp, bucket);
            if (it == C.end()) {
                // Check if the load factor of C is too large.
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
    auto t2 = std::chrono::high_resolution_clock::now();
    std::cout << std::chrono::duration_cast<std::chrono::nanoseconds>(t2-t1).count()
    << "ns" << std::endl;
}

RCP<const Polynomial> mul_poly(const Polynomial &p, const Polynomial &q) {
    hash_set a, b, C;
    C.rehash(10000);
    vec_symbol vars_;
    if (!vec_symbol_eq(p.vars_, q.vars_)) {
        _normalise_polymul(p, q, vars_, a, b);
    } else {
        a = p.polys_set_;
        b = q.polys_set_;
    }
    _mul_hashset(a, b, C);
    return polynomial(vars_, C);
}
#else
RCP<const Polynomial> mul_poly(const Polynomial &p, const Polynomial &q) {   
    unordered_set C, A = p.polys_set_, B = q.polys_set_;
    m_pair temp;
    for (auto &a: A) {
        for (auto &b: B) {
            temp.first = a.first + b.first;
            temp.second = a.second*b.second;
            C.insert(temp);
        }
    }
    return polynomial(p.vars_, C);
}
#endif

} // SymEngine
