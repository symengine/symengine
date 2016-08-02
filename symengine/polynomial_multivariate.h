#ifndef SYMENGINE_POLYNOMIALS_MULTIVARIATE
#define SYMENGINE_POLYNOMIALS_MULTIVARIATE

#include <symengine/expression.h>
#include <symengine/monomials.h>
#include <symengine/polys/uintpoly.h>
#include <symengine/polys/uexprpoly.h>

namespace SymEngine
{
// reconciles the positioning of the exponents in the vectors in the
// Dict dict_ of the arguments
// with the positioning of the exponents in the correspondng vectors of the
// output of the function.
// f1 and f2 are vectors whose indices are the positions in the arguments and
// whose values are the
// positions in the output.  set_basic s is the set of symbols of the output,
// and
// s1 and s2 are the sets of the symbols of the inputs.
unsigned int reconcile(vec_uint &v1, vec_uint &v2, set_basic &s,
                       const set_basic &s1, const set_basic &s2);
// translates vectors from one polynomial into vectors for another.
template <typename Vec>
void translate(const Vec &original, Vec &changed, vec_uint translator,
               unsigned int size)
{
    changed.resize(size, 0);
    for (unsigned int i = 0; i < original.size(); i++) {
        changed[translator[i]] = original[i];
    }
}

// translates two vec_uints to the desired format and adds them together
// componentwise
template <typename Vec>
Vec translate_and_add(const Vec &v1, const Vec &v2, const vec_uint &translator1,
                      const vec_uint &translator2, const unsigned int size)
{
    Vec result;
    for (unsigned int i = 0; i < size; i++) {
        result.insert(result.end(), 0);
    }
    for (unsigned int i = 0; i < translator1.size(); i++) {
        result[translator1[i]] += v1[i];
    }
    for (unsigned int i = 0; i < translator2.size(); i++) {
        result[translator2[i]] += v2[i];
    }
    return result;
}

template <typename Poly, typename Coeff, typename Vec>
class MPolyBase : public Basic
{
public:
    // vars: set of variables for the polynomial
    // dict: dictionary for sparse represntation of polynomial, x**1 * y**2 +
    // 3 * x**4 * y ** 5 is represented as {(1,2):1,(4,5):3}
    using Dict = std::unordered_map<Vec, Coeff, vec_hash<Vec>>;

    set_basic vars_;
    Dict dict_;

    // constructor from components
    MPolyBase(const set_basic &vars, Dict &&dict)
        : vars_{std::move(vars)}, dict_{std::move(dict)}
    {
        SYMENGINE_ASSERT(is_canonical(vars_, dict_))
    }

    // Creates a Poly in cannonical form based on dictionary d
    inline static RCP<const Poly> from_dict(const set_basic &s, Dict &&d)
    {
        // Remove entries in d corresponding to terms with coefficient 0
        auto iter = d.begin();
        while (iter != d.end()) {
            if (iter->second == 0) {
                auto toErase = iter;
                iter++;
                d.erase(toErase);
            } else {
                iter++;
            }
        }
        return make_rcp<const Poly>(s, std::move(d));
    }

    // Public function for creating Poly:
    // vec_basic can contain symbols for the polynomial in any order, but the
    // symbols
    // will be sorted in the set_basic of the actual object.
    // The order of the exponenents in the vectors in the dictionary will also
    // be permuted accordingly.
    static RCP<const Poly> create(const vec_basic &v, Dict &&d)
    {
        set_basic s;
        // Symbols in the vector are sorted by placeing them in an std::map.
        // The image of the symbols in the map is their original location in the
        // vector.
        std::map<RCP<const Basic>, unsigned int, RCPBasicKeyLess> m;
        for (unsigned int i = 0; i < v.size(); i++) {
            s.insert(v[i]);
            m.insert(std::pair<RCP<const Basic>, unsigned int>(v[i], i));
        }

        // vec_uint translator represents the permutation of the exponents
        vec_uint translator;
        translator.resize(s.size());
        auto mptr = m.begin();
        for (unsigned int i = 0; i < s.size(); i++) {
            translator[mptr->second] = i;
            mptr++;
        }

        Dict dict;
        // Permute the exponents
        for (auto &bucket : d) {
            Vec changed;
            translate<Vec>(bucket.first, changed, translator, s.size());
            dict.insert(std::pair<Vec, Coeff>(changed, bucket.second));
        }
        return Poly::from_dict(s, std::move(dict));
    }

    bool is_canonical(const set_basic &vars, const Dict &dict)
    {
        for (auto bucket : dict) {
            if (bucket.second == 0)
                return false;
        }
        return true;
    }

    int compare(const Basic &o) const
    {
        SYMENGINE_ASSERT(is_a<Poly>(o))

        const Poly &s = static_cast<const Poly &>(o);

        if (vars_.size() != s.vars_.size())
            return vars_.size() < s.vars_.size() ? -1 : 1;
        if (dict_.size() != s.dict_.size())
            return dict_.size() < s.dict_.size() ? -1 : 1;

        int cmp = unified_compare(vars_, s.vars_);
        if (cmp != 0)
            return cmp;

        return unified_compare(dict_, s.dict_);
    }

    inline vec_basic get_args() const
    {
        return {};
    }

    bool __eq__(const Basic &o) const
    {
        // TODO : fix for when vars are different, but there is an intersection
        if (not is_a<Poly>(o))
            return false;
        const Poly &o_ = static_cast<const Poly &>(o);
        // compare constants without regards to vars
        if (1 == dict_.size() && 1 == o_.dict_.size()) {
            if (dict_.begin()->second != o_.dict_.begin()->second)
                return false;
            if (dict_.begin()->first == o_.dict_.begin()->first
                && unified_eq(vars_, o_.vars_))
                return true;
            Vec v1, v2;
            v1.resize(vars_.size(), 0);
            v2.resize(o_.vars_.size(), 0);
            if (dict_.begin()->first == v1 || o_.dict_.begin()->first == v2)
                return true;
            return false;
        } else if (0 == dict_.size() && 0 == o_.dict_.size()) {
            return true;
        } else {
            return (unified_eq(vars_, o_.vars_) && unified_eq(dict_, o_.dict_));
        }
    }

    RCP<const Poly> add(const Poly &b) const
    {
        vec_uint v1;
        vec_uint v2;
        set_basic s;
        Dict dict;
        umap_basic_uint degs;
        unsigned int size = reconcile(v1, v2, s, vars_, b.vars_);
        for (auto bucket : dict_) {
            Vec changed;
            translate<Vec>(bucket.first, changed, v1, size);
            dict.insert(std::pair<Vec, Coeff>(changed, bucket.second));
        }
        for (auto bucket : b.dict_) {
            Vec changed;
            translate<Vec>(bucket.first, changed, v2, size);
            auto target = dict.find(changed);
            if (target != dict.end()) {
                target->second += bucket.second;
            } else {
                dict.insert(std::pair<Vec, Coeff>(changed, bucket.second));
            }
        }
        return Poly::from_dict(s, std::move(dict));
    }
    RCP<const Poly> neg() const
    {
        Dict dict;
        set_basic s = vars_;
        for (auto bucket : dict_) {
            dict.insert(std::pair<Vec, Coeff>(bucket.first, -bucket.second));
        }
        return Poly::from_dict(s, std::move(dict));
    }
    RCP<const Poly> sub(const Poly &b) const
    {
        return this->add(*b.neg());
    }
    RCP<const Poly> mul(const Poly &b) const
    {
        // Naive algorithm
        vec_uint v1;
        vec_uint v2;
        set_basic s;
        Dict dict;
        unsigned int size = reconcile(v1, v2, s, vars_, b.vars_);
        for (auto a_bucket : dict_) {
            for (auto b_bucket : b.dict_) {
                Vec target = translate_and_add<Vec>(
                    a_bucket.first, b_bucket.first, v1, v2, size);
                if (dict.find(target) == dict.end()) {
                    dict.insert(std::pair<Vec, Coeff>(
                        target, a_bucket.second * b_bucket.second));
                } else {
                    dict.find(target)->second
                        += a_bucket.second * b_bucket.second;
                }
            }
        }
        return Poly::from_dict(s, std::move(dict));
    }
};

// MultivariateIntPolynomial
class MultivariateIntPolynomial
    : public MPolyBase<MultivariateIntPolynomial, integer_class, vec_uint>
{
public:
    MultivariateIntPolynomial(const set_basic &vars, umap_uvec_mpz &&dict)
        : MPolyBase(std::move(vars), std::move(dict))
    {
    }
    IMPLEMENT_TYPEID(MULTIVARIATEINTPOLYNOMIAL);
    std::size_t __hash__() const;
    integer_class eval(
        std::map<RCP<const Basic>, integer_class, RCPBasicKeyLess> &vals) const;
    static inline RCP<const MultivariateIntPolynomial>
    convert(const MultivariateIntPolynomial &o)
    {
        return o.rcp_from_this_cast<MultivariateIntPolynomial>();
    }
    static RCP<const MultivariateIntPolynomial> convert(const UIntPoly &o);
    RCP<const Basic> as_symbolic() const;
};

// MultivariatePolynomial
class MultivariatePolynomial
    : public MPolyBase<MultivariatePolynomial, Expression, vec_int>
{
public:
    MultivariatePolynomial(const set_basic &vars, umap_vec_expr &&dict)
        : MPolyBase(std::move(vars), std::move(dict))
    {
    }
    IMPLEMENT_TYPEID(MULTIVARIATEPOLYNOMIAL);
    std::size_t __hash__() const;
    Expression
    eval(std::map<RCP<const Basic>, Expression, RCPBasicKeyLess> &vals) const;
    static inline RCP<const MultivariatePolynomial>
    convert(const MultivariatePolynomial &o)
    {
        return o.rcp_from_this_cast<MultivariatePolynomial>();
    }
    static RCP<const MultivariatePolynomial> convert(const UExprPoly &o);
    RCP<const Basic> as_symbolic() const;
};

template <class T>
struct is_mpoly_expr : std::false_type {
};

template <>
struct is_mpoly_expr<UExprPoly> : std::true_type {
};

template <>
struct is_mpoly_expr<MultivariatePolynomial> : std::true_type {
};

template <typename T, typename U,
          typename
          = enable_if_t<is_mpoly_expr<T>::value and is_mpoly_expr<U>::value>>
RCP<const MultivariatePolynomial> add_mult_poly(const T &a, const U &b)
{
    return MultivariatePolynomial::convert(a)
        ->add(*MultivariatePolynomial::convert(b));
}

template <typename T, typename U,
          typename
          = enable_if_t<is_mpoly_expr<T>::value and is_mpoly_expr<U>::value>>
RCP<const MultivariatePolynomial> mul_mult_poly(const T &a, const U &b)
{
    return MultivariatePolynomial::convert(a)
        ->mul(*MultivariatePolynomial::convert(b));
}

template <typename T, typename U,
          typename
          = enable_if_t<is_mpoly_expr<T>::value and is_mpoly_expr<U>::value>>
RCP<const MultivariatePolynomial> sub_mult_poly(const T &a, const U &b)
{
    return MultivariatePolynomial::convert(a)
        ->sub(*MultivariatePolynomial::convert(b));
}

template <class T>
struct is_mpoly_int : std::false_type {
};

template <>
struct is_mpoly_int<UIntPoly> : std::true_type {
};

template <>
struct is_mpoly_int<MultivariateIntPolynomial> : std::true_type {
};

template <typename T, typename U,
          typename
          = enable_if_t<is_mpoly_int<T>::value and is_mpoly_int<U>::value>>
RCP<const MultivariateIntPolynomial> add_mult_poly(const T &a, const U &b)
{
    return MultivariateIntPolynomial::convert(a)
        ->add(*MultivariateIntPolynomial::convert(b));
}

template <typename T, typename U,
          typename
          = enable_if_t<is_mpoly_int<T>::value and is_mpoly_int<U>::value>>
RCP<const MultivariateIntPolynomial> mul_mult_poly(const T &a, const U &b)
{
    return MultivariateIntPolynomial::convert(a)
        ->mul(*MultivariateIntPolynomial::convert(b));
}

template <typename T, typename U,
          typename
          = enable_if_t<is_mpoly_int<T>::value and is_mpoly_int<U>::value>>
RCP<const MultivariateIntPolynomial> sub_mult_poly(const T &a, const U &b)
{
    return MultivariateIntPolynomial::convert(a)
        ->sub(*MultivariateIntPolynomial::convert(b));
}

} // SymEngine

#endif
