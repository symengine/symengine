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
Vec translate(Vec original, vec_uint translator, unsigned int size)
{
    Vec changed;
    changed.resize(size, 0);
    for (unsigned int i = 0; i < original.size(); i++) {
        changed[translator[i]] = original[i];
    }
    return changed;
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

template <typename MPoly, typename Dict, typename Coeff, typename Vec>
class MPolyBase
{
public:
    // vars: set of variables for the polynomial
    // degrees: max degrees of the symbols
    // dict: dictionary for sparse represntation of polynomial, x**1 * y**2 + 3
    // * x**4 * y ** 5
    // is represented as {(1,2):1,(4,5):3}
    set_basic vars_;
    umap_basic_uint degrees_;
    Dict dict_;

protected:
    // Creates a MPoly in cannonical form based on
    // dictionary d.
    inline static MPoly from_dict(const set_basic &s, Dict &&d)
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

        umap_basic_uint degs;
        // Calculate the degrees of the polynomial
        int whichvar = 0;
        for (auto sym : s) {
            degs.insert(std::pair<RCP<const Basic>, unsigned int>(sym, 0));
            for (auto bucket : d) {
                if (bucket.first[whichvar] > 0
                    && static_cast<unsigned int>(bucket.first[whichvar])
                           > degs.find(sym)->second)
                    degs.find(sym)->second = bucket.first[whichvar];
            }
            whichvar++;
        }
        return MPoly(s, std::move(degs), std::move(d));
    }

public:
    MPolyBase() = default;
    // constructor from components
    MPolyBase(const set_basic &vars, umap_basic_uint &&degrees, Dict &&dict)
        : vars_{std::move(vars)}, degrees_{std::move(degrees)},
          dict_{std::move(dict)}
    {
        SYMENGINE_ASSERT(is_canonical(vars_, degrees_, dict_))
    }

    // Public function for creating MPoly:
    // vec_basic can contain symbols for the polynomial in any order, but the
    // symbols
    // will be sorted in the set_basic of the actual object.
    // The order of the exponenents in the vectors in the dictionary will also
    // be permuted accordingly.
    static MPoly create(const vec_basic &v, Dict &&d)
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
            dict.insert(std::pair<Vec, Coeff>(
                translate<Vec>(bucket.first, translator, s.size()),
                bucket.second));
        }
        return MPoly::from_dict(s, std::move(dict));
    }

    bool is_canonical(const set_basic &vars, const umap_basic_uint &degrees,
                      const Dict &dict)
    {
        for (auto bucket : dict) {
            if (bucket.second == 0)
                return false;
        }
        // checks that the maximum degree of any variable is correct according
        // to the dictionary keeps track of the index of the variable we are
        // checking
        unsigned int whichvar = 0;
        for (auto var : vars) {
            unsigned int maxdegree = 0;
            for (auto bucket : dict) {
                if (bucket.first[whichvar] > 0
                    && static_cast<unsigned int>(bucket.first[whichvar])
                           > degrees.find(var)->second)
                    return false;
                else if (bucket.first[whichvar] > 0
                         && maxdegree < static_cast<unsigned int>(
                                            bucket.first[whichvar]))
                    maxdegree = bucket.first[whichvar];
            }
            if (maxdegree != degrees.find(var)->second)
                return false;
            whichvar++;
        }
        return true;
    }

    bool __eq__(const MPoly &o_) const
    {
        // TODO : fix for when vars are different, but there is an intersection
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
            if (dict_.begin()->first == v1 && o_.dict_.begin()->first == v2)
                return true;
            return false;
        } else if (0 == dict_.size() && 0 == o_.dict_.size()) {
            return true;
        } else {
            return (unified_eq(vars_, o_.vars_) && unified_eq(dict_, o_.dict_));
        }
    }

    MPoly add(const MPoly &b) const
    {
        vec_uint v1;
        vec_uint v2;
        set_basic s;
        Dict dict;
        umap_basic_uint degs;
        unsigned int size = reconcile(v1, v2, s, vars_, b.vars_);
        for (auto bucket : dict_) {
            dict.insert(std::pair<Vec, Coeff>(
                translate<Vec>(bucket.first, v1, size), bucket.second));
        }
        for (auto bucket : b.dict_) {
            auto target = dict.find(translate<Vec>(bucket.first, v2, size));
            if (target != dict.end()) {
                target->second += bucket.second;
            } else {
                dict.insert(std::pair<Vec, Coeff>(
                    translate<Vec>(bucket.first, v2, size), bucket.second));
            }
        }
        return MPoly::from_dict(s, std::move(dict));
    }
    MPoly neg() const
    {
        Dict dict;
        set_basic s = vars_;
        for (auto bucket : dict_) {
            dict.insert(std::pair<Vec, Coeff>(bucket.first, -bucket.second));
        }
        return MPoly::from_dict(s, std::move(dict));
    }
    MPoly sub(const MPoly &b) const
    {
        return this->add(b.neg());
    }
    MPoly mul(const MPoly &b) const
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
        return MPoly::from_dict(s, std::move(dict));
    }
    static MPoly diff(const MPolyBase<MPoly, Dict, Coeff, Vec> &self,
                      const RCP<const Basic> &x)
    {
        Dict dict;
        if (self.vars_.find(x) != self.vars_.end()) {
            auto i = self.vars_.begin();
            unsigned int index = 0;
            while (!(*i)->__eq__(*x)) {
                i++;
                index++;
            } // find the index of the variable we are differentiating WRT.
            for (auto bucket : self.dict_) {
                if (bucket.first[index] != 0) {
                    Vec v = bucket.first;
                    v[index]--;
                    dict.insert(std::pair<Vec, Coeff>(
                        v, bucket.second * bucket.first[index]));
                }
            }
            return MPoly::from_dict(self.vars_, std::move(dict));
        } else {
            Dict d;
            return MPoly::from_dict(self.vars_, std::move(d));
        }
    }
};

// MultivariateIntPolynomialExpr
class MultivariateIntPolynomialExpr
    : public MPolyBase<MultivariateIntPolynomialExpr, umap_uvec_mpz,
                       integer_class, vec_uint>
{
public:
    MultivariateIntPolynomialExpr(const set_basic &vars,
                                  umap_basic_uint &&degrees,
                                  umap_uvec_mpz &&dict)
        : MPolyBase(std::move(vars), std::move(degrees), std::move(dict))
    {
    }
    // IMPLEMENT_TYPEID(MULTIVARIATEINTPOLYNOMIALEXPR);
    vec_basic get_args() const;
    std::size_t __hash__() const;
    int compare(const MultivariateIntPolynomialExpr &o) const;
    integer_class eval(
        std::map<RCP<const Basic>, integer_class, RCPBasicKeyLess> &vals) const;

    std::string toString() const;

    static inline const MultivariateIntPolynomialExpr &
    convert(const MultivariateIntPolynomialExpr &o)
    {
        return o;
    }

    static MultivariateIntPolynomialExpr convert(const UIntPoly &o);
};

// MultivariatePolynomialExpr
class MultivariatePolynomialExpr
    : public MPolyBase<MultivariatePolynomialExpr, umap_vec_expr, Expression,
                       vec_int>
{
public:
    MultivariatePolynomialExpr() = default;
    MultivariatePolynomialExpr(const set_basic &vars, umap_basic_uint &&degrees,
                               umap_vec_expr &&dict)
        : MPolyBase(std::move(vars), std::move(degrees), std::move(dict))
    {
    }
    explicit MultivariatePolynomialExpr(int i)
    {
        if (i != 0) {
            vec_int v;
            dict_.insert(std::pair<vec_int, Expression>(v, Expression(i)));
        }
    }
    explicit MultivariatePolynomialExpr(Expression i)
    {
        if (i != 0) {
            vec_int v;
            dict_.insert(std::pair<vec_int, Expression>(v, i));
        }
    }
    // IMPLEMENT_TYPEID(MULTIVARIATEPOLYNOMIALEXPR);
    vec_basic get_args() const;
    std::size_t __hash__() const;
    int compare(const MultivariatePolynomialExpr &o) const;
    Expression
    eval(std::map<RCP<const Basic>, Expression, RCPBasicKeyLess> &vals) const;

    std::string toString() const;

    static inline const MultivariatePolynomialExpr &
    convert(const MultivariatePolynomialExpr &o)
    {
        return o;
    }

    static MultivariatePolynomialExpr convert(const UExprPoly &o);

    static MultivariatePolynomialExpr convert(const int i);
    static MultivariatePolynomialExpr convert(const Expression &o);

    MultivariatePolynomialExpr truncate(map_basic_uint precs)
    {
        umap_vec_expr d;
        for (auto bucket : dict_) {
            bool accept = true;
            auto var = vars_.begin();
            auto prec = precs.begin();
            unsigned int whichvar = 0;
            while (var != vars_.end() && prec != precs.end()) {
                if ((*var)->__eq__(*prec->first)) {
                    if (bucket.first[whichvar] > 0
                        && static_cast<unsigned int>(bucket.first[whichvar])
                               > prec->second)
                        accept = false;
                    prec++;
                }
                var++;
                whichvar++;
            }
            if (accept)
                d.insert(std::pair<vec_int, Expression>(bucket.first,
                                                        bucket.second));
        }
        return MultivariatePolynomialExpr::from_dict(vars_, std::move(d));
    }

    RCP<const Basic> get_basic() const
    {
        return SymEngine::add(this->get_args());
    }

    MultivariatePolynomialExpr &operator=(const MultivariatePolynomialExpr &o)
        = default;

    MultivariatePolynomialExpr &operator=(const Expression &o)
    {
        return *this = convert(o);
    }

    friend MultivariatePolynomialExpr
    operator+(const MultivariatePolynomialExpr &a,
              const MultivariatePolynomialExpr &b)
    {
        return a.add(b);
    }

    friend MultivariatePolynomialExpr
    operator+(const MultivariatePolynomialExpr &a, const Expression &b)
    {
        return a.add(convert(b));
    }
    friend MultivariatePolynomialExpr
    operator+(const Expression &a, const MultivariatePolynomialExpr &b)
    {
        return convert(a).add(b);
    }

    MultivariatePolynomialExpr &operator+=(const MultivariatePolynomialExpr &a)
    {
        if (!unified_eq(vars_, a.vars_)) {
            return *this = this->add(a);
        } else {
            for (auto bucket : a.dict_) {
                auto target = dict_.find(bucket.first);
                if (target != dict_.end()) {
                    target->second += bucket.second;
                    if (target->second == 0) {
                        dict_.erase(target);
                    }
                } else {
                    dict_.insert(std::pair<vec_int, Expression>(bucket.first,
                                                                bucket.second));
                }
            }
            return *this;
        }
    }

    MultivariatePolynomialExpr &operator+=(const Expression &a)
    {
        vec_int v;
        v.resize(vars_.size(), 0);
        auto target = dict_.find(v);
        if (target != dict_.end()) {
            target->second += a;
            if (target->second == 0) {
                dict_.erase(target);
            }
        } else if (a != 0) {
            dict_.insert(std::pair<vec_int, Expression>(v, a));
        }
        return *this;
    }

    friend MultivariatePolynomialExpr
    operator-(const MultivariatePolynomialExpr &a,
              const MultivariatePolynomialExpr &b)
    {
        return a.sub(b);
    }

    friend MultivariatePolynomialExpr
    operator-(const MultivariatePolynomialExpr &a, const Expression &b)
    {
        return a.sub(convert(b));
    }

    friend MultivariatePolynomialExpr
    operator-(const Expression &a, const MultivariatePolynomialExpr &b)
    {
        return convert(a).sub(b);
    }

    MultivariatePolynomialExpr &operator-=(const MultivariatePolynomialExpr &a)
    {
        if (!unified_eq(vars_, a.vars_)) {
            return *this = this->sub(a);
        } else {
            for (auto bucket : a.dict_) {
                auto target = dict_.find(bucket.first);
                if (target != dict_.end()) {
                    target->second -= bucket.second;
                    if (target->second == 0) {
                        dict_.erase(target);
                    }
                } else {
                    dict_.insert(std::pair<vec_int, Expression>(
                        bucket.first, -bucket.second));
                }
            }
            return *this;
        }
    }

    MultivariatePolynomialExpr &operator-=(const Expression &a)
    {
        vec_int v;
        v.resize(vars_.size(), 0);
        auto target = dict_.find(v);
        if (target != dict_.end()) {
            target->second -= a;
            if (target->second == 0) {
                dict_.erase(target);
            }
        } else if (a != 0) {
            dict_.insert(std::pair<vec_int, Expression>(v, -a));
        }
        return *this;
    }

    MultivariatePolynomialExpr operator-() const
    {
        return this->neg();
    }

    friend MultivariatePolynomialExpr
    operator*(const MultivariatePolynomialExpr &a,
              const MultivariatePolynomialExpr &b)
    {
        return a.mul(b);
    }

    friend MultivariatePolynomialExpr
    operator*(const MultivariatePolynomialExpr &a, const Expression &b)
    {
        return a.mul(convert(b));
    }

    friend MultivariatePolynomialExpr
    operator*(const Expression &a, const MultivariatePolynomialExpr &b)
    {
        return convert(a).mul(b);
    }

    MultivariatePolynomialExpr &operator*=(const MultivariatePolynomialExpr &b)
    {
        if (b.dict_.size() == 0) {
            dict_.clear();
        } else if (b.dict_.size() == 1) {
            vec_int v;
            v.resize(b.vars_.size(), 0);
            if (b.dict_.begin()->first == v) {
                *this *= b.dict_.begin()->second;
            } else {
                *this = this->mul(b);
            }
        } else {
            *this = this->mul(b);
        }
        return *this;
    }

    MultivariatePolynomialExpr &operator*=(const Expression &a)
    {
        if (a == 0) {
            dict_.clear();
        } else {
            for (auto &bucket : dict_) {
                bucket.second *= a;
            }
        }
        return *this;
    }

    friend MultivariatePolynomialExpr
    operator/(const MultivariatePolynomialExpr &a, const Expression &b)
    {
        return a * (1 / b);
    }

    MultivariatePolynomialExpr &operator/=(const Expression &b)
    {
        for (auto &bucket : dict_)
            bucket.second /= b;
        return *this;
    }

    bool operator==(const MultivariatePolynomialExpr &o) const
    {
        return this->__eq__(o);
    }

    bool operator==(const Expression &o) const
    {
        return this->__eq__(convert(o));
    }

    bool operator!=(const MultivariatePolynomialExpr &o) const
    {
        return !(this->__eq__(o));
    }

    bool operator!=(const Expression &o) const
    {
        return !(this->__eq__(convert(o)));
    }
};

template <class T>
struct is_mpoly_expr : std::false_type {
};

template <>
struct is_mpoly_expr<UExprPoly> : std::true_type {
};

template <>
struct is_mpoly_expr<MultivariatePolynomialExpr> : std::true_type {
};

template <typename T, typename U,
          typename
          = enable_if_t<is_mpoly_expr<T>::value and is_mpoly_expr<U>::value>>
MultivariatePolynomialExpr add_mult_poly(const T &a, const U &b)
{
    return MultivariatePolynomialExpr::convert(a)
        .add(MultivariatePolynomialExpr::convert(b));
}

template <typename T, typename U,
          typename
          = enable_if_t<is_mpoly_expr<T>::value and is_mpoly_expr<U>::value>>
MultivariatePolynomialExpr mul_mult_poly(const T &a, const U &b)
{
    return MultivariatePolynomialExpr::convert(a)
        .mul(MultivariatePolynomialExpr::convert(b));
}

template <typename T, typename U,
          typename
          = enable_if_t<is_mpoly_expr<T>::value and is_mpoly_expr<U>::value>>
MultivariatePolynomialExpr sub_mult_poly(const T &a, const U &b)
{
    return MultivariatePolynomialExpr::convert(a)
        .sub(MultivariatePolynomialExpr::convert(b));
}

template <class T>
struct is_mpoly_int : std::false_type {
};

template <>
struct is_mpoly_int<UIntPoly> : std::true_type {
};

template <>
struct is_mpoly_int<MultivariateIntPolynomialExpr> : std::true_type {
};

template <typename T, typename U,
          typename
          = enable_if_t<is_mpoly_int<T>::value and is_mpoly_int<U>::value>>
MultivariateIntPolynomialExpr add_mult_poly(const T &a, const U &b)
{
    return MultivariateIntPolynomialExpr::convert(a)
        .add(MultivariateIntPolynomialExpr::convert(b));
}

template <typename T, typename U,
          typename
          = enable_if_t<is_mpoly_int<T>::value and is_mpoly_int<U>::value>>
MultivariateIntPolynomialExpr mul_mult_poly(const T &a, const U &b)
{
    return MultivariateIntPolynomialExpr::convert(a)
        .mul(MultivariateIntPolynomialExpr::convert(b));
}

template <typename T, typename U,
          typename
          = enable_if_t<is_mpoly_int<T>::value and is_mpoly_int<U>::value>>
MultivariateIntPolynomialExpr sub_mult_poly(const T &a, const U &b)
{
    return MultivariateIntPolynomialExpr::convert(a)
        .sub(MultivariateIntPolynomialExpr::convert(b));
}

} // SymEngine

#endif
