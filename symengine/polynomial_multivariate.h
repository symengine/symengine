#ifndef SYMENGINE_POLYNOMIALS_MULTIVARIATE
#define SYMENGINE_POLYNOMIALS_MULTIVARIATE

#include <symengine/expression.h>
#include <symengine/monomials.h>
#include <symengine/polys/uintpoly.h>
#include <symengine/polys/uexprpoly.h>

namespace SymEngine
{

template <typename Vec, typename Value, typename Wrapper>
class UDictWrapper
{
public:
    using Dict = std::unordered_map<Vec, Value, vec_hash<Vec>>;
    Dict dict_;
    unsigned int vec_size;

    typedef Vec vec_type;
    typedef Value value_type;

    UDictWrapper(unsigned int s) SYMENGINE_NOEXCEPT
    {
        vec_size = s;
    }

    UDictWrapper() SYMENGINE_NOEXCEPT
    {
    }

    ~UDictWrapper() SYMENGINE_NOEXCEPT
    {
    }

    UDictWrapper(Dict &&p) : dict_{p}
    {
    }

    UDictWrapper(const Dict &p)
    {
        for (auto &iter : p) {
            if (iter.second != Value(0))
                dict_[iter.first] = iter.second;
        }
        if (not p.empty())
            vec_size = p.begin()->first.size();
        else
            vec_size = 0;
    }

    Wrapper &operator=(Wrapper &&other)
    {
        if (this != &other)
            dict_ = std::move(other.dict_);
        return static_cast<Wrapper &>(*this);
    }

    friend Wrapper operator+(const Wrapper &a, const Wrapper &b)
    {
        SYMENGINE_ASSERT(a.vec_size == b.vec_size)
        Wrapper c = a;
        c += b;
        return c;
    }

    // both wrappers must have "aligned" vectors, ie same size
    // and vector positions refer to the same generators
    Wrapper &operator+=(const Wrapper &other)
    {
        SYMENGINE_ASSERT(vec_size == other.vec_size)

        for (auto &iter : other.dict_) {
            auto t = dict_.find(iter.first);
            if (t != dict_.end()) {
                t->second += iter.second;
                if (t->second == 0)
                    dict_.erase(t);
            } else {
                dict_.insert(t, {iter.first, iter.second});
            }
        }
        return static_cast<Wrapper &>(*this);
    }

    friend Wrapper operator-(const Wrapper &a, const Wrapper &b)
    {
        SYMENGINE_ASSERT(a.vec_size == b.vec_size)

        Wrapper c = a;
        c -= b;
        return c;
    }

    Wrapper operator-() const
    {
        auto c = *this;
        for (auto &iter : c.dict_)
            iter.second *= -1;
        return static_cast<Wrapper &>(c);
    }

    // both wrappers must have "aligned" vectors, ie same size
    // and vector positions refer to the same generators
    Wrapper &operator-=(const Wrapper &other)
    {
        SYMENGINE_ASSERT(vec_size == other.vec_size)

        for (auto &iter : other.dict_) {
            auto t = dict_.find(iter.first);
            if (t != dict_.end()) {
                t->second -= iter.second;
                if (t->second == 0)
                    dict_.erase(t);
            } else {
                dict_.insert(t, {iter.first, -iter.second});
            }
        }
        return static_cast<Wrapper &>(*this);
    }

    static Wrapper mul(const Wrapper &a, const Wrapper &b)
    {
        if (a.get_dict().empty())
            return a;
        if (b.get_dict().empty())
            return b;

        Wrapper p;
        for (auto a_ : a.dict_) {
            for (auto b_ : b.dict_) {

                Vec target(a.vec_size, 0);
                for (unsigned int i = 0; i < a.vec_size; i++)
                    target[i] = a_.first[i] + b_.first[i];

                if (p.dict_.find(target) == p.dict_.end()) {
                    p.dict_.insert({target, a_.second * b_.second});
                } else {
                    p.dict_.find(target)->second += a_.second * b_.second;
                }
            }
        }

        for (auto it = p.dict_.begin(); it != p.dict_.end();) {
            if (it->second == 0) {
                p.dict_.erase(it++);
            } else {
                ++it;
            }
        }
        return p;
    }

    static Wrapper pow(const Wrapper &a, unsigned int p)
    {
        Wrapper tmp = a, res(a.vec_size);

        Vec zero_v(a.vec_size, 0);
        res.insert({zero_v, 1});

        while (p != 1) {
            if (p % 2 == 0) {
                tmp = tmp * tmp;
            } else {
                res = res * tmp;
                tmp = tmp * tmp;
            }
            p >>= 1;
        }

        return (res * tmp);
    }

    friend Wrapper operator*(const Wrapper &a, const Wrapper &b)
    {
        SYMENGINE_ASSERT(a.vec_size == b.vec_size)
        return Wrapper::mul(a, b);
    }

    Wrapper &operator*=(const Wrapper &other)
    {
        SYMENGINE_ASSERT(vec_size == other.vec_size)

        if (dict_.empty())
            return static_cast<Wrapper &>(*this);

        if (other.dict_.empty()) {
            dict_.clear();
            return static_cast<Wrapper &>(*this);
        }

        Vec zero_v(vec_size, 0);
        // ! other is a just constant term
        if (other.dict_.size() == 1
            and other.dict_.find(zero_v) != other.dict_.end()) {
            auto t = other.dict_.begin();
            for (auto &i1 : dict_)
                i1.second *= t->second;
            return static_cast<Wrapper &>(*this);
        }

        Wrapper res = Wrapper::mul(static_cast<Wrapper &>(*this), other);
        res.dict_.swap(this->dict_);
        return static_cast<Wrapper &>(*this);
    }

    bool operator==(const Wrapper &other) const
    {
        return dict_ == other.dict_;
    }

    bool operator!=(const Wrapper &other) const
    {
        return not(*this == other);
    }

    const Dict &get_dict() const
    {
        return dict_;
    }

    bool empty() const
    {
        return dict_.empty();
    }

    Value get_coeff(Vec &x) const
    {
        auto ite = dict_.find(x);
        if (ite != dict_.end())
            return ite->second;
        return Value(0);
    }

    Wrapper translate(const vec_uint &translator, unsigned int size) const
    {
        SYMENGINE_ASSERT(translator.size() == vec_size)
        SYMENGINE_ASSERT(size >= vec_size)

        Dict d;

        for (auto it : dict_) {
            Vec changed;
            changed.resize(size, 0);
            for (unsigned int i = 0; i < vec_size; i++)
                changed[translator[i]] = it.first[i];
            d.insert({changed, it.second});
        }

        return Wrapper(std::move(d));
    }
};

class MIntDict : public UDictWrapper<vec_uint, integer_class, MIntDict>
{
public:
    MIntDict(unsigned int s) SYMENGINE_NOEXCEPT : UDictWrapper(s)
    {
    }

    MIntDict() SYMENGINE_NOEXCEPT
    {
    }

    ~MIntDict() SYMENGINE_NOEXCEPT
    {
    }

    MIntDict(MIntDict &&other) SYMENGINE_NOEXCEPT
        : UDictWrapper(std::move(other))
    {
    }

    MIntDict(const umap_uvec_mpz &p) : UDictWrapper(p)
    {
    }

    MIntDict(const MIntDict &) = default;

    MIntDict &operator=(const MIntDict &) = default;
};

class MIntPoly : public Basic
{
public:
    MIntDict poly_;
    set_basic vars_;

    MIntPoly(const set_basic &vars, MIntDict &&dict)
    {
        vars_ = vars;
        poly_ = dict;
    }

    IMPLEMENT_TYPEID(MINTPOLY);
    std::size_t __hash__() const;
    RCP<const Basic> as_symbolic() const;

    static RCP<const MIntPoly> from_container(const set_basic &vars,
                                              MIntDict &&d)
    {
        return make_rcp<const MIntPoly>(vars, std::move(d));
    }

    int compare(const Basic &o) const
    {
        SYMENGINE_ASSERT(is_a<MIntPoly>(o))

        const MIntPoly &s = static_cast<const MIntPoly &>(o);

        if (vars_.size() != s.vars_.size())
            return vars_.size() < s.vars_.size() ? -1 : 1;
        if (poly_.dict_.size() != s.poly_.dict_.size())
            return poly_.dict_.size() < s.poly_.dict_.size() ? -1 : 1;

        int cmp = unified_compare(vars_, s.vars_);
        if (cmp != 0)
            return cmp;

        return unified_compare(poly_.dict_, s.poly_.dict_);
    }

    static RCP<const MIntPoly> from_dict(const vec_basic &v, umap_uvec_mpz &&d)
    {
        set_basic s;
        std::map<RCP<const Basic>, unsigned int, RCPBasicKeyLess> m;
        // Symbols in the vector are sorted by placeing them in an std::map.
        // The image of the symbols in the map is their original location in the
        // vector.

        for (unsigned int i = 0; i < v.size(); i++) {
            m.insert({v[i], i});
            s.insert(v[i]);
        }

        // vec_uint translator represents the permutation of the exponents
        vec_uint trans;
        trans.reserve(s.size());
        for (auto mptr = m.begin(); mptr != m.end(); mptr++)
            trans.push_back(mptr->second);

        MIntDict x(std::move(d));
        return MIntPoly::from_container(
            s, std::move(x.translate(trans, s.size())));
    }

    inline vec_basic get_args() const
    {
        return {};
    }

    inline const MIntDict &get_poly() const
    {
        return poly_;
    }

    bool __eq__(const Basic &o) const
    {
        // TODO : fix for when vars are different, but there is an intersection
        if (not is_a<MIntPoly>(o))
            return false;
        const MIntPoly &o_ = static_cast<const MIntPoly &>(o);
        // compare constants without regards to vars
        if (1 == poly_.dict_.size() && 1 == o_.poly_.dict_.size()) {
            if (poly_.dict_.begin()->second != o_.poly_.dict_.begin()->second)
                return false;
            if (poly_.dict_.begin()->first == o_.poly_.dict_.begin()->first
                && unified_eq(vars_, o_.vars_))
                return true;
            vec_uint v1, v2;
            v1.resize(vars_.size(), 0);
            v2.resize(o_.vars_.size(), 0);
            if (poly_.dict_.begin()->first == v1 || o_.poly_.dict_.begin()->first == v2)
                return true;
            return false;
        } else if (0 == poly_.dict_.size() && 0 == o_.poly_.dict_.size()) {
            return true;
        } else {
            return (unified_eq(vars_, o_.vars_) && unified_eq(poly_.dict_, o_.poly_.dict_));
        }
    }
};

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

inline set_basic get_translated_container(MIntDict &x, MIntDict &y,
                                          const MIntPoly &a, const MIntPoly &b)
{
    vec_uint v1, v2;
    set_basic s;

    unsigned int sz = reconcile(v1, v2, s, a.vars_, b.vars_);
    x = a.poly_.translate(v1, sz);
    y = b.poly_.translate(v2, sz);

    return s;
}

inline RCP<const MIntPoly> add_mpoly(const MIntPoly &a, const MIntPoly &b)
{
    MIntDict x, y;
    set_basic s = get_translated_container(x, y, a, b);
    x += y;
    return MIntPoly::from_container(s, std::move(x));
}

inline RCP<const MIntPoly> sub_mpoly(const MIntPoly &a, const MIntPoly &b)
{
    MIntDict x, y;
    set_basic s = get_translated_container(x, y, a, b);
    x -= y;
    return MIntPoly::from_container(s, std::move(x));
}

inline RCP<const MIntPoly> mul_mpoly(const MIntPoly &a, const MIntPoly &b)
{
    MIntDict x, y;
    set_basic s = get_translated_container(x, y, a, b);
    x *= y;
    return MIntPoly::from_container(s, std::move(x));
}

inline RCP<const MIntPoly> neg_mpoly(const MIntPoly &a)
{
    MIntDict x = a.poly_;
    return MIntPoly::from_container(a.vars_, std::move(-x));
}

// translates vectors from one polynomial into vectors for another.
template <typename Vec>
Vec translate(const Vec &original, vec_uint translator, unsigned int size)
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
    result.resize(size, 0);
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
    static RCP<const Poly> from_dict(const set_basic &s, Dict &&d)
    {
        // Remove entries in d corresponding to terms with coefficient 0
        auto iter = d.begin();
        while (iter != d.end()) {
            SYMENGINE_ASSERT(iter->first.size() == s.size())
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
        vec_uint translator(s.size());
        auto mptr = m.begin();
        for (unsigned int i = 0; i < s.size(); i++) {
            translator[mptr->second] = i;
            mptr++;
        }

        Dict dict;
        // Permute the exponents
        for (auto &bucket : d) {
            Vec changed = translate<Vec>(bucket.first, translator, s.size());
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
            Vec changed = translate<Vec>(bucket.first, v1, size);
            dict.insert(std::pair<Vec, Coeff>(changed, bucket.second));
        }
        for (auto bucket : b.dict_) {
            Vec changed = translate<Vec>(bucket.first, v2, size);
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
