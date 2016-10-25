#include <symengine/add.h>
#include <symengine/pow.h>
#include <symengine/complex.h>
#include <symengine/visitor_double.h>

namespace SymEngine
{

Add::Add(const RCP<const Number> &coef, umap_basic_num &&dict)
    : coef_{coef}, dict_{std::move(dict)}
{
    SYMENGINE_ASSERT(is_canonical(coef, dict_))
}

bool Add::is_canonical(const RCP<const Number> &coef,
                       const umap_basic_num &dict) const
{
    if (coef == null)
        return false;
    if (dict.size() == 0)
        return false;
    if (dict.size() == 1) {
        // e.g. 0 + x, 0 + 2x
        if (coef->is_exact_zero())
            return false;
    }
    // Check that each term in 'dict' is in canonical form
    for (const auto &p : dict) {
        if (p.first == null)
            return false;
        if (p.second == null)
            return false;
        // e.g. 2*3
        if (is_a_Number(*p.first))
            return false;
        // e.g. 1*x (={1:x}), this should rather be just x (={x:1})
        if (is_a<Integer>(*p.first)
            and rcp_static_cast<const Integer>(p.first)->is_one())
            return false;
        // e.g. x*0
        if (is_a_Number(*p.second)
            and rcp_static_cast<const Number>(p.second)->is_zero())
            return false;

        // e.g. {3x: 2}, this should rather be just {x: 6}
        if (is_a<Mul>(*p.first)
            and not(rcp_static_cast<const Mul>(p.first)->coef_->is_one()))
            return false;
    }
    return true;
}

hash_t Add::__hash__() const
{
    hash_t seed = ADD, temp;
    hash_combine<Basic>(seed, *coef_);
    for (const auto &p : dict_) {
        temp = p.first->hash();
        hash_combine<Basic>(temp, *(p.second));
        seed ^= temp;
    }
    return seed;
}

bool Add::__eq__(const Basic &o) const
{
    if (is_a<Add>(o) and eq(*coef_, *(static_cast<const Add &>(o).coef_))
        and unified_eq(dict_, static_cast<const Add &>(o).dict_))
        return true;

    return false;
}

int Add::compare(const Basic &o) const
{
    SYMENGINE_ASSERT(is_a<Add>(o))
    const Add &s = static_cast<const Add &>(o);
    // # of elements
    if (dict_.size() != s.dict_.size())
        return (dict_.size() < s.dict_.size()) ? -1 : 1;

    // coef
    int cmp = coef_->__cmp__(*s.coef_);
    if (cmp != 0)
        return cmp;

    // Compare dictionaries:
    // NOTE: This is slow. Add should cache this map_basic_num representation
    // once it is computed.
    map_basic_num adict(dict_.begin(), dict_.end());
    map_basic_num bdict(s.dict_.begin(), s.dict_.end());
    return unified_compare(adict, bdict);
}

// Very quickly (!) creates the appropriate instance (i.e. Add, Symbol,
// Integer, Mul) depending on the size of the dictionary 'd'.
// If d.size() > 1 then it just returns Add. This means that the dictionary
// must be in canonical form already. For d.size == 1, it returns Mul, Pow,
// Symbol or Integer, depending on the expression.
RCP<const Basic> Add::from_dict(const RCP<const Number> &coef,
                                umap_basic_num &&d)
{
    if (d.size() == 0) {
        return coef;
    } else if (d.size() == 1 and coef->is_exact_zero()) {
        auto p = d.begin();
        if (is_a<Integer>(*(p->second))) {
            if (rcp_static_cast<const Integer>(p->second)->is_zero()) {
                return p->second;
            }
            if (rcp_static_cast<const Integer>(p->second)->is_one()) {
                return p->first;
            }
            if (is_a<Mul>(*(p->first))) {
#if !defined(WITH_SYMENGINE_THREAD_SAFE) and defined(WITH_SYMENGINE_RCP)
                if (rcp_static_cast<const Mul>(p->first)->use_count() == 1) {
                    // We can steal the dictionary:
                    // Cast away const'ness, so that we can move 'dict_', since
                    // 'p->first' will be destroyed when 'd' is at the end of
                    // this function, so we "steal" its dict_ to avoid an
                    // unnecessary copy. We know the refcount_ is one, so
                    // nobody else is using the Mul except us.
                    const map_basic_basic &d2
                        = rcp_static_cast<const Mul>(p->first)->dict_;
                    map_basic_basic &d3 = const_cast<map_basic_basic &>(d2);
                    return Mul::from_dict(p->second, std::move(d3));
                } else {
#else
                {
#endif
                    // We need to copy the dictionary:
                    map_basic_basic d2
                        = rcp_static_cast<const Mul>(p->first)->dict_;
                    return Mul::from_dict(p->second, std::move(d2));
                }
            }
            map_basic_basic m;
            if (is_a<Pow>(*(p->first))) {
                insert(m, rcp_static_cast<const Pow>(p->first)->get_base(),
                       rcp_static_cast<const Pow>(p->first)->get_exp());
            } else {
                insert(m, p->first, one);
            }
            return make_rcp<const Mul>(p->second, std::move(m));
        }
        map_basic_basic m;
        if (is_a_Number(*p->second)) {
            if (is_a<Mul>(*(p->first))) {
#if !defined(WITH_SYMENGINE_THREAD_SAFE) and defined(WITH_SYMENGINE_RCP)
                if (rcp_static_cast<const Mul>(p->first)->use_count() == 1) {
                    // We can steal the dictionary:
                    // Cast away const'ness, so that we can move 'dict_', since
                    // 'p->first' will be destroyed when 'd' is at the end of
                    // this function, so we "steal" its dict_ to avoid an
                    // unnecessary copy. We know the refcount_ is one, so
                    // nobody else is using the Mul except us.
                    const map_basic_basic &d2
                        = rcp_static_cast<const Mul>(p->first)->dict_;
                    map_basic_basic &d3 = const_cast<map_basic_basic &>(d2);
                    return Mul::from_dict(p->second, std::move(d3));
                } else {
#else
                {
#endif
                    // We need to copy the dictionary:
                    map_basic_basic d2
                        = rcp_static_cast<const Mul>(p->first)->dict_;
                    return Mul::from_dict(p->second, std::move(d2));
                }
            }
            if (is_a<Pow>(*p->first)) {
                insert(m, rcp_static_cast<const Pow>(p->first)->get_base(),
                       rcp_static_cast<const Pow>(p->first)->get_exp());
            } else {
                insert(m, p->first, one);
            }
            return make_rcp<const Mul>(p->second, std::move(m));
        } else {
            insert(m, p->first, one);
            insert(m, p->second, one);
            return make_rcp<const Mul>(one, std::move(m));
        }
    } else {
        return make_rcp<const Add>(coef, std::move(d));
    }
}

// Adds (coef*t) to the dict "d"
// Assumption: "t" does not have any numerical coefficients, those are in "coef"
void Add::dict_add_term(umap_basic_num &d, const RCP<const Number> &coef,
                        const RCP<const Basic> &t)
{
    auto it = d.find(t);
    if (it == d.end()) {
        // Not found, add it in if it is nonzero:
        if (not(coef->is_zero()))
            insert(d, t, coef);
    } else {
        iaddnum(outArg(it->second), coef);
        if (it->second->is_zero())
            d.erase(it);
    }
}

void Add::coef_dict_add_term(const Ptr<RCP<const Number>> &coef,
                             umap_basic_num &d, const RCP<const Number> &c,
                             const RCP<const Basic> &term)
{
    if (is_a_Number(*term)) {
        iaddnum(coef, mulnum(c, rcp_static_cast<const Number>(term)));
    } else if (is_a<Add>(*term)) {
        if (c->is_one()) {
            for (const auto &q : (rcp_static_cast<const Add>(term))->dict_)
                Add::dict_add_term(d, q.second, q.first);
            iaddnum(coef, rcp_static_cast<const Add>(term)->coef_);
        } else {
            Add::dict_add_term(d, c, term);
        }
    } else {
        RCP<const Number> coef2;
        RCP<const Basic> t;
        Add::as_coef_term(term, outArg(coef2), outArg(t));
        Add::dict_add_term(d, mulnum(c, coef2), t);
    }
}

void Add::as_coef_term(const RCP<const Basic> &self,
                       const Ptr<RCP<const Number>> &coef,
                       const Ptr<RCP<const Basic>> &term)
{
    if (is_a<Mul>(*self)) {
        if (neq(*(rcp_static_cast<const Mul>(self)->coef_), *one)) {
            *coef = (rcp_static_cast<const Mul>(self))->coef_;
            // We need to copy our 'dict_' here, as 'term' has to have its own.
            map_basic_basic d2 = (rcp_static_cast<const Mul>(self))->dict_;
            *term = Mul::from_dict(one, std::move(d2));
        } else {
            *coef = one;
            *term = self;
        }
    } else if (is_a_Number(*self)) {
        *coef = rcp_static_cast<const Number>(self);
        *term = one;
    } else {
        SYMENGINE_ASSERT(not is_a<Add>(*self));
        *coef = one;
        *term = self;
    }
}

RCP<const Basic> add(const vec_basic &a)
{
    SymEngine::umap_basic_num d;
    RCP<const Number> coef = zero;
    RCP<const Basic> others = zero;
    for (const auto &i : a) {
        if (is_a_Symbolic(*i)) {
            Add::coef_dict_add_term(outArg(coef), d, one, i);
        } else {
            others = add(others, i);
        }
    }
    if (eq(*others, *zero)) {
        return Add::from_dict(coef, std::move(d));
    } else {
        return add(others, Add::from_dict(coef, std::move(d)));
    }
}

RCP<const Basic> sub(const RCP<const Basic> &a, const RCP<const Basic> &b)
{
    return add(a, mul(minus_one, b));
}

void Add::as_two_terms(const Ptr<RCP<const Basic>> &a,
                       const Ptr<RCP<const Basic>> &b) const
{
    auto p = dict_.begin();
    *a = mul(p->first, p->second);
    umap_basic_num d = dict_;
    d.erase(p->first);
    *b = Add::from_dict(coef_, std::move(d));
}

vec_basic Add::get_args() const
{
    vec_basic args;
    if (not coef_->is_exact_zero())
        args.push_back(coef_);
    for (const auto &p : dict_) {
        args.push_back(Add::from_dict(zero, {{p.first, p.second}}));
    }
    return args;
}

class AddVisitor : DoubleDispatchVisitor<AddVisitor>
{
public:
    using DoubleDispatchVisitor::apply;
    using DoubleDispatchVisitor::dispatch;
    inline AddVisitor(const RCP<const Basic> &a) : DoubleDispatchVisitor(a){};

    static inline RCP<const Basic> dispatch(const Number &a, const Number &b)
    {
        return a.add(b);
    }

    template <typename P>
    static inline RCP<const Basic> dispatch(const P &a, const Add &b)
    {
        return dispatch(b, a);
    }

    static inline RCP<const Basic> dispatch(const Add &a, const Add &b)
    {
        umap_basic_num d = a.dict_;
        RCP<const Number> coef = a.coef_;
        for (const auto &p : b.dict_)
            Add::dict_add_term(d, p.second, p.first);
        iaddnum(outArg(coef), b.coef_);
        return Add::from_dict(coef, std::move(d));
    }

    static inline RCP<const Basic> dispatch(const Add &a, const Number &b)
    {
        umap_basic_num d = a.dict_;
        return Add::from_dict(a.coef_->add(b), std::move(d));
    }

    static inline RCP<const Basic> dispatch(const Add &a, const Mul &b)
    {
        umap_basic_num d = a.dict_;
        map_basic_basic mul_d = b.dict_;
        Add::dict_add_term(d, b.coef_, Mul::from_dict(one, std::move(mul_d)));
        return Add::from_dict(a.coef_, std::move(d));
    }

    static inline RCP<const Basic> dispatch(const Add &a, const Symbolic &b)
    {
        umap_basic_num d = a.dict_;
        Add::dict_add_term(d, one, b.rcp_from_this());
        return Add::from_dict(a.coef_, std::move(d));
    }

    static inline RCP<const Basic> dispatch(const Symbolic &a,
                                            const Symbolic &b)
    {
        umap_basic_num d;
        RCP<const Number> coef;
        RCP<const Basic> t;
        Add::as_coef_term(a.rcp_from_this(), outArg(coef), outArg(t));
        Add::dict_add_term(d, coef, t);
        Add::as_coef_term(b.rcp_from_this(), outArg(coef), outArg(t));
        Add::dict_add_term(d, coef, t);
        auto it = d.find(one);
        if (it == d.end()) {
            coef = zero;
        } else {
            coef = it->second;
            d.erase(it);
        }
        return Add::from_dict(coef, std::move(d));
    }

    template <typename P,
              typename
              = enable_if_t<std::is_base_of<SeriesCoeffInterface, P>::value>>
    static inline RCP<const Basic> dispatch(const P &a, const P &b)
    {
        return a.add(b);
    }

    template <typename P, typename Q,
              typename = enable_if_t<std::is_base_of<Set, P>::value
                                     or std::is_base_of<Set, Q>::value
                                     or std::is_base_of<Boolean, P>::value
                                     or std::is_base_of<Boolean, Q>::value
                                     or std::is_base_of<MPoly, P>::value
                                     or std::is_base_of<MPoly, Q>::value
                                     or std::is_base_of<GaloisField, P>::value
                                     or std::is_base_of<GaloisField, Q>::value>>
    static inline RCP<const Basic> dispatch(const P &a, const Q &b)
    {
        throw std::runtime_error("Addition is not supported");
    }

    template <typename Container, typename Poly>
    static inline RCP<const Poly> dispatch(const UPolyBase<Container, Poly> &a,
                                           const UPolyBase<Container, Poly> &b)
    {
        if (!(a.get_var()->__eq__(*b.get_var())))
            throw std::runtime_error("Error: variables must agree.");
        auto dict = a.get_poly();
        dict += b.get_poly();
        return Poly::from_container(a.get_var(), std::move(dict));
    }
};

RCP<const Basic> add(const RCP<const Basic> &a, const RCP<const Basic> &b)
{
    AddVisitor addvisitor(a);
    return addvisitor.apply(b);
}

} // SymEngine
