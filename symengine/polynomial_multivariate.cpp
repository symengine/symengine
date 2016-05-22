#include <symengine/add.h>
#include <symengine/constants.h>
#include <symengine/mul.h>
#include <symengine/polynomial.h>
#include <symengine/polynomial_multivariate.h>
#include <symengine/pow.h>

namespace SymEngine
{

RCP<const MultivariateIntPolynomial>
MultivariateIntPolynomial::from_dict(const set_sym &s, umap_uvec_mpz &&d)
{
    umap_sym_uint degs;
    // Remove entries in d corresponding to terms with coefficient 0
    auto iter = d.begin();
    while (iter != d.end()) {
        if (integer_class(0) == iter->second) {
            auto toErase = iter;
            iter++;
            d.erase(toErase);
        } else {
            iter++;
        }
    }

    // Calculate the degrees of the polynomial
    int whichvar = 0;
    for (auto sym : s) {
        degs.insert(std::pair<RCP<const Symbol>, unsigned int>(sym, 0));
        for (auto bucket : d) {
            if (bucket.first[whichvar] > degs.find(sym)->second)
                degs.find(sym)->second = bucket.first[whichvar];
        }
        whichvar++;
    }
    return make_rcp<const MultivariateIntPolynomial>(s, degs, d);
}

MultivariateIntPolynomial::MultivariateIntPolynomial(const set_sym &vars,
                                                     umap_sym_uint &degrees,
                                                     umap_uvec_mpz &dict)
    : vars_{std::move(vars)}, degrees_{std::move(degrees)},
      dict_{std::move(dict)}
{
    SYMENGINE_ASSERT(is_canonical(vars_, degrees_, dict_))
}

RCP<const MultivariateIntPolynomial>
MultivariateIntPolynomial::multivariate_int_polynomial(const vec_sym &v,
                                                       umap_uvec_mpz &&d)
{
    set_sym s;
    // Symbols in the vector are sorted by placeing them in an std::map.
    // The image of the symbols in the map is their original location in the
    // vector.
    std::map<RCP<const Symbol>, unsigned int, RCPSymbolCompare> m;
    for (unsigned int i = 0; i < v.size(); i++) {
        s.insert(v[i]);
        m.insert(std::pair<RCP<const Symbol>, unsigned int>(v[i], i));
    }

    // vec_uint translator represents the permutation of the exponents
    vec_uint translator;
    translator.resize(s.size());
    auto mptr = m.begin();
    for (unsigned int i = 0; i < s.size(); i++) {
        translator[mptr->second] = i;
        mptr++;
    }

    umap_uvec_mpz dict;

    // Permute the exponents
    for (auto &bucket : d) {
        dict.insert(std::pair<vec_uint, integer_class>(
            translate(bucket.first, translator, s.size()), bucket.second));
    }

    return MultivariateIntPolynomial::from_dict(s, std::move(dict));
}

vec_basic MultivariateIntPolynomial::get_args() const
{
    vec_basic args;
    umap_uvec_mpz d;
    // To change the ordering in which the terms appear in the vector, change
    // vec_uint_compare in dict.h
    std::vector<vec_uint> v
        = order_umap<vec_uint, umap_uvec_mpz, vec_uint_compare>(dict_);
    for (const auto &p : v) {
        map_basic_basic b;
        int whichvar = 0;
        for (auto sym : vars_) {
            if (integer_class(0) != p[whichvar])
                b.insert(std::pair<RCP<const Basic>, RCP<const Basic>>(
                    sym, make_rcp<Integer>(integer_class(p[whichvar]))));
            whichvar++;
        }
        args.push_back(Mul::from_dict(
            make_rcp<const Integer>(dict_.find(p)->second), std::move(b)));
    }
    return args;
}

bool MultivariateIntPolynomial::is_canonical(const set_sym &vars,
                                             const umap_sym_uint &degrees,
                                             const umap_uvec_mpz &dict)
{
    // checks that the dictionary does not contain terms with coefficient 0
    for (auto bucket : dict) {
        if (integer_class(0) == bucket.second)
            return false;
    }

    // checks that the maximum degree of any variable is correct according to
    // the dictionary
    unsigned int whichvar
        = 0; // keeps track of the index of the variable we are checking
    for (auto var : vars) {
        unsigned int maxdegree = 0;
        for (auto bucket : dict) {
            if (bucket.first[whichvar] > degrees.find(var)->second)
                return false;
            else if (maxdegree < bucket.first[whichvar])
                maxdegree = bucket.first[whichvar];
        }
        if (maxdegree != degrees.find(var)->second)
            return false;
        whichvar++;
    }
    return true;
}

std::size_t MultivariateIntPolynomial::__hash__() const
{
    std::hash<std::string> hash_string;
    std::size_t seed = MULTIVARIATEINTPOLYNOMIAL;
    for (auto var : vars_)
        // boost's method for combining hashes
        seed ^= hash_string(var->get_name()) + 0x9e3779b + (seed << 6)
                + (seed >> 2);

    std::vector<vec_uint> v
        = order_umap<vec_uint, umap_uvec_mpz, vec_uint_compare>(dict_);

    for (auto vec : v) {
        seed ^= vec_uint_hash()(dict_.find(vec)->first) + 0x9e3779b
                + (seed << 6) + (seed >> 2);
        seed ^= mpz_hash(dict_.find(vec)->second) + 0x9e3779b + (seed << 6)
                + (seed >> 2);
    }
    return seed;
}

bool MultivariateIntPolynomial::__eq__(const Basic &o) const
{
    // compare constants without regards to vars
    if (1 == dict_.size()
        && 1
               == static_cast<const MultivariateIntPolynomial &>(o)
                      .dict_.size()) {
        if (dict_.begin()->second
            != static_cast<const MultivariateIntPolynomial &>(o)
                   .dict_.begin()
                   ->second)
            return false;
        vec_uint v1;
        v1.resize(vars_.size(), 0);
        vec_uint v2;
        v2.resize(
            static_cast<const MultivariateIntPolynomial &>(o).vars_.size(), 0);
        if (dict_.begin()->first == v1
            || static_cast<const MultivariateIntPolynomial &>(o)
                       .dict_.begin()
                       ->first
                   == v2)
            return true;
        return false;
    } else if (0 == dict_.size()
               && 0
                      == static_cast<const MultivariateIntPolynomial &>(o)
                             .dict_.size()) {
        return true;
    } else {
        return (
            set_eq<set_sym>(
                vars_, static_cast<const MultivariateIntPolynomial &>(o).vars_)
            && umap_uvec_mpz_eq(
                   dict_,
                   static_cast<const MultivariateIntPolynomial &>(o).dict_));
    }
}

int MultivariateIntPolynomial::compare(const Basic &o) const
{
    // copied from UnivariateIntPolynomial::compare and then modified.
    const MultivariateIntPolynomial &s
        = static_cast<const MultivariateIntPolynomial &>(o);

    int cmp = set_compare<set_sym>(vars_, s.vars_);
    if (cmp != 0)
        return cmp;

    return umap_uvec_mpz_compare(dict_, s.dict_);
}

integer_class MultivariateIntPolynomial::eval(
    std::map<RCP<const Symbol>, integer_class, RCPSymbolCompare> &vals) const
{
    integer_class ans(0);
    for (auto bucket : dict_) {
        integer_class term = bucket.second;
        unsigned int whichvar = 0;
        for (auto sym : vars_) {
            integer_class temp;
            mp_pow_ui(temp, vals.find(sym)->second, bucket.first[whichvar]);
            term *= temp;
            whichvar++;
        }
        ans += term;
    }
    return ans;
}

unsigned int reconcile(vec_uint &v1, vec_uint &v2, set_sym &s,
                       const set_sym &s1, const set_sym &s2)
{
    auto a1 = s1.begin();
    auto a2 = s2.begin();
    unsigned int poscount = 0;
    // Performs a merge sort of s1 and s2, and builds up v1 and v2 as
    // translators:
    // v[i] is the position of the ith symbol in the new set.
    while (a1 != s1.end() && a2 != s2.end()) {
        if (0 == (*a1)->compare(**a2) && (a1 != s1.end() && a2 != s2.end())) {
            v1.insert(v1.end(), poscount);
            v2.insert(v2.end(), poscount);
            s.insert(*a1);
            a1++;
            a2++;
        } else if (-1 == (*a1)->compare(**a2)) {
            v1.insert(v1.end(), poscount);
            s.insert(*a1);
            a1++;
        } else if (1 == (*a1)->compare(**a2)) {
            v2.insert(v2.end(), poscount);
            s.insert(*a2);
            a2++;
        }
        poscount++;
    }
    if (a1 == s1.end()) {
        while (a2 != s2.end()) {
            v2.insert(v2.end(), poscount);
            s.insert(*a2);
            a2++;
            poscount++;
        }
    } else if (a2 == s2.end()) {
        while (a1 != s1.end()) {
            v1.insert(v1.end(), poscount);
            s.insert(*a1);
            a1++;
            poscount++;
        }
    }
    return poscount; // return size of the new vectors
}

unsigned int reconcile(vec_uint &v1, unsigned int &v2, set_sym &s,
                       const set_sym &s1, const RCP<const Symbol> s2)
{
    auto a1 = s1.begin();
    unsigned int poscount = 0;
    bool inserted = false;
    while (a1 != s1.end() && !inserted) {
        if (0 == (*a1)->compare(*s2)) {
            v1.insert(v1.end(), poscount);
            v2 = poscount;
            s.insert(*a1);
            a1++;
            inserted = true;
        } else if (-1 == (*a1)->compare(*s2)) {
            v1.insert(v1.end(), poscount);
            s.insert(*a1);
            a1++;
        } else if (1 == (*a1)->compare(*s2)) {
            v2 = poscount;
            s.insert(s2);
            inserted = true;
        }
        poscount++;
    }
    if (a1 == s1.end() && !inserted) {
        v2 = poscount;
        s.insert(s2);
        poscount++;
    } else if (inserted) {
        while (a1 != s1.end()) {
            v1.insert(v1.end(), poscount);
            s.insert(*a1);
            a1++;
            poscount++;
        }
    }
    return poscount;
}

vec_uint translate(vec_uint original, vec_uint translator, unsigned int size)
{
    vec_uint changed;
    changed.resize(size, 0);
    for (unsigned int i = 0; i < original.size(); i++) {
        changed[translator[i]] = original[i];
    }
    return changed;
}

vec_uint translate(unsigned int original, unsigned int translator,
                   unsigned int size)
{
    vec_uint changed;
    changed.resize(size, 0);
    changed[translator] = original;
    return changed;
}

RCP<const MultivariateIntPolynomial>
add_mult_poly(const MultivariateIntPolynomial &a,
              const MultivariateIntPolynomial &b)
{
    vec_uint v1;
    vec_uint v2;
    set_sym s;
    umap_uvec_mpz dict;
    umap_sym_uint degs;
    unsigned int size = reconcile(v1, v2, s, a.vars_, b.vars_);
    for (auto bucket : a.dict_) {
        dict.insert(std::pair<vec_uint, integer_class>(
            translate(bucket.first, v1, size), bucket.second));
    }
    for (auto bucket : b.dict_) {
        auto target = dict.find(translate(bucket.first, v2, size));
        if (target != dict.end()) {
            target->second += bucket.second;
        } else {
            dict.insert(std::pair<vec_uint, integer_class>(
                translate(bucket.first, v2, size), bucket.second));
        }
    }
    return MultivariateIntPolynomial::from_dict(s, std::move(dict));
}

// translates two vec_uints to the desired format and adds them together
// componentwise
vec_uint uint_vec_translate_and_add(const vec_uint &v1, const vec_uint &v2,
                                    const vec_uint &translator1,
                                    const vec_uint &translator2,
                                    const unsigned int size)
{
    vec_uint result;
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

vec_uint uint_vec_translate_and_add(const vec_uint &v1, const unsigned int v2,
                                    const vec_uint &translator1,
                                    const unsigned int &translator2,
                                    const unsigned int size)
{
    vec_uint result;
    for (unsigned int i = 0; i < size; i++) {
        result.insert(result.end(), 0);
    }
    for (unsigned int i = 0; i < translator1.size(); i++) {
        result[translator1[i]] += v1[i];
    }
    result[translator2] += v2;
    return result;
}

RCP<const MultivariateIntPolynomial>
neg_mult_poly(const MultivariateIntPolynomial &a)
{
    umap_uvec_mpz dict;
    set_sym s = a.vars_;
    for (auto bucket : a.dict_) {
        dict.insert(
            std::pair<vec_uint, integer_class>(bucket.first, -bucket.second));
    }
    return MultivariateIntPolynomial::from_dict(s, std::move(dict));
}

RCP<const MultivariateIntPolynomial>
sub_mult_poly(const MultivariateIntPolynomial &a,
              const MultivariateIntPolynomial &b)
{
    return (add_mult_poly(a, *neg_mult_poly(b)));
}

RCP<const MultivariateIntPolynomial>
mul_mult_poly(const MultivariateIntPolynomial &a,
              const MultivariateIntPolynomial &b)
{
    // Naive algorithm
    vec_uint v1;
    vec_uint v2;
    set_sym s;
    umap_uvec_mpz dict;
    unsigned int size = reconcile(v1, v2, s, a.vars_, b.vars_);
    for (auto a_bucket : a.dict_) {
        for (auto b_bucket : b.dict_) {
            vec_uint target = uint_vec_translate_and_add(
                a_bucket.first, b_bucket.first, v1, v2, size);
            if (dict.find(target) == dict.end()) {
                dict.insert(std::pair<vec_uint, integer_class>(
                    target, a_bucket.second * b_bucket.second));
            } else {
                dict.find(target)->second += a_bucket.second * b_bucket.second;
            }
        }
    }
    return MultivariateIntPolynomial::from_dict(s, std::move(dict));
}

RCP<const MultivariateIntPolynomial>
add_mult_poly(const MultivariateIntPolynomial &a,
              const UnivariateIntPolynomial &b)
{
    vec_uint v1;
    unsigned int v2;
    set_sym s;
    umap_uvec_mpz dict;
    unsigned int size = reconcile(v1, v2, s, a.vars_, b.get_var());
    for (auto bucket : a.dict_) {
        dict.insert(std::pair<vec_uint, integer_class>(
            translate(bucket.first, v1, size), bucket.second));
    }
    for (auto bucket : b.get_dict()) {
        auto target = dict.find(translate(bucket.first, v2, size));
        if (target != dict.end()) {
            target->second += bucket.second;
        } else {
            dict.insert(std::pair<vec_uint, integer_class>(
                translate(bucket.first, v2, size), bucket.second));
        }
    }
    return MultivariateIntPolynomial::from_dict(s, std::move(dict));
}

RCP<const MultivariateIntPolynomial>
add_mult_poly(const UnivariateIntPolynomial &a,
              const MultivariateIntPolynomial &b)
{
    return add_mult_poly(b, a);
}

RCP<const MultivariateIntPolynomial>
sub_mult_poly(const MultivariateIntPolynomial &a,
              const UnivariateIntPolynomial &b)
{
    return add_mult_poly(a, *neg_poly(b));
}

RCP<const MultivariateIntPolynomial>
sub_mult_poly(const UnivariateIntPolynomial &a,
              const MultivariateIntPolynomial &b)
{
    return add_mult_poly(*neg_mult_poly(b), a);
}

RCP<const MultivariateIntPolynomial>
mul_mult_poly(const MultivariateIntPolynomial &a,
              const UnivariateIntPolynomial &b)
{
    // Naive algorithm
    vec_uint v1;
    unsigned int v2;
    set_sym s;
    umap_uvec_mpz dict;
    unsigned int size = reconcile(v1, v2, s, a.vars_, b.get_var());
    for (auto a_bucket : a.dict_) {
        for (auto b_bucket : b.get_dict()) {
            vec_uint target = uint_vec_translate_and_add(
                a_bucket.first, b_bucket.first, v1, v2, size);
            if (dict.find(target) == dict.end()) {
                dict.insert(std::pair<vec_uint, integer_class>(
                    target, a_bucket.second * b_bucket.second));
            } else {
                dict.find(target)->second += a_bucket.second * b_bucket.second;
            }
        }
    }
    return MultivariateIntPolynomial::from_dict(s, std::move(dict));
}

RCP<const MultivariateIntPolynomial>
mul_mult_poly(const UnivariateIntPolynomial &a,
              const MultivariateIntPolynomial &b)
{
    return mul_mult_poly(b, a);
}

RCP<const MultivariateIntPolynomial>
add_mult_poly(const UnivariateIntPolynomial &a,
              const UnivariateIntPolynomial &b)
{
    unsigned int v1;
    unsigned int v2;
    set_sym s;
    umap_uvec_mpz dict;
    bool same = false; // are the variables of a and b the same?
    // Here we preform the same sort of thing done in the reconcile functions.
    if (0 == a.get_var()->compare(*b.get_var())) {
        v1 = 0;
        v2 = 0;
        s.insert(a.get_var());
        same = true;
    } else {
        if (-1 == a.get_var()->compare(*b.get_var())) {
            v1 = 0;
            v2 = 1;
        } else {
            v2 = 0;
            v1 = 1;
        }
        s.insert(a.get_var());
        s.insert(b.get_var());
    }

    for (auto bucket : a.get_dict()) {
        vec_uint v;
        v.insert(v.end(), 0);
        if (!same)
            v.insert(v.end(), 0);
        v[v1] = bucket.first;
        dict.insert(std::pair<vec_uint, integer_class>(v, bucket.second));
    }
    for (auto bucket : b.get_dict()) {
        vec_uint v;
        v.insert(v.end(), 0);
        if (!same)
            v.insert(v.end(), 0);
        v[v2] = bucket.first;
        auto target = dict.find(v);
        if (target != dict.end()) {
            target->second += bucket.second;
        } else {
            dict.insert(std::pair<vec_uint, integer_class>(v, bucket.second));
        }
    }
    return MultivariateIntPolynomial::from_dict(s, std::move(dict));
}

RCP<const MultivariateIntPolynomial>
sub_mult_poly(const UnivariateIntPolynomial &a,
              const UnivariateIntPolynomial &b)
{
    return add_mult_poly(a, *neg_poly(b));
}

RCP<const MultivariateIntPolynomial>
mul_mult_poly(const UnivariateIntPolynomial &a,
              const UnivariateIntPolynomial &b)
{
    unsigned int v1;
    unsigned int v2;
    set_sym s;
    umap_uvec_mpz dict;
    bool same = false; // are the variables of a and b the same?
    if (0 == a.get_var()->compare(*b.get_var())) {
        v1 = 0;
        v2 = 0;
        s.insert(a.get_var());
        same = true;
    } else {
        if (-1 == a.get_var()->compare(*b.get_var())) {
            v1 = 0;
            v2 = 1;
        } else {
            v2 = 0;
            v1 = 1;
        }
        s.insert(a.get_var());
        s.insert(b.get_var());
    }
    for (auto a_bucket : a.get_dict()) {
        for (auto b_bucket : b.get_dict()) {
            vec_uint target;
            target.insert(target.end(), 0);
            if (!same)
                target.insert(target.end(), 0);
            target[v1] += a_bucket.first;
            target[v2] += b_bucket.first;
            if (dict.find(target) == dict.end()) {
                dict.insert(std::pair<vec_uint, integer_class>(
                    target, a_bucket.second * b_bucket.second));
            } else {
                dict.find(target)->second += a_bucket.second * b_bucket.second;
            }
        }
    }
    return MultivariateIntPolynomial::from_dict(s, std::move(dict));
}

// MultivariatePolynomial
MultivariatePolynomial::MultivariatePolynomial(const set_sym &vars,
                                               umap_sym_uint &degrees,
                                               umap_uvec_expr &dict)
    : vars_{std::move(vars)}, degrees_{std::move(degrees)},
      dict_{std::move(dict)}
{
    SYMENGINE_ASSERT(is_canonical(vars_, degrees_, dict_))
}

RCP<const MultivariatePolynomial>
MultivariatePolynomial::from_dict(const set_sym &s, umap_uvec_expr &&d)
{
    umap_sym_uint degs;
    auto iter = d.begin();
    while (iter != d.end()) {
        if (Expression(0) == iter->second) {
            auto toErase = iter;
            iter++;
            d.erase(toErase);
        } else {
            iter++;
        }
    }

    int whichvar = 0;
    for (auto sym : s) {
        degs.insert(std::pair<RCP<const Symbol>, unsigned int>(sym, 0));
        for (auto bucket : d) {
            if (bucket.first[whichvar] > degs.find(sym)->second)
                degs.find(sym)->second = bucket.first[whichvar];
        }
        whichvar++;
    }
    return make_rcp<const MultivariatePolynomial>(s, degs, d);
}

RCP<const MultivariatePolynomial>
MultivariatePolynomial::multivariate_polynomial(const vec_sym &v,
                                                umap_uvec_expr &&d)
{
    set_sym s;
    // Symbols in the vector are sorted by placeing them in an std::map.
    // The image of the symbols in the map is their original location in the
    // vector.
    std::map<RCP<const Symbol>, unsigned int, RCPSymbolCompare> m;
    for (unsigned int i = 0; i < v.size(); i++) {
        s.insert(v[i]);
        m.insert(std::pair<RCP<const Symbol>, unsigned int>(v[i], i));
    }

    // vec_uint translator represents the permutation of the exponents
    vec_uint translator;
    translator.resize(s.size());
    auto mptr = m.begin();
    for (unsigned int i = 0; i < s.size(); i++) {
        translator[mptr->second] = i;
        mptr++;
    }

    umap_uvec_expr dict;

    // Permute the exponents
    for (auto &bucket : d) {
        dict.insert(std::pair<vec_uint, Expression>(
            translate(bucket.first, translator, s.size()), bucket.second));
    }

    return MultivariatePolynomial::from_dict(s, std::move(dict));
}

vec_basic MultivariatePolynomial::get_args() const
{
    vec_basic args;
    umap_uvec_expr d;
    // To change the ordering in which the terms appear in the vector, change
    // vec_uint_compare in dict.h
    std::vector<vec_uint> v
        = order_umap<vec_uint, umap_uvec_expr, vec_uint_compare>(dict_);
    for (const auto &p : v) {
        RCP<const Basic> res = ((dict_.find(p)->second).get_basic());
        int whichvar = 0;
        for (auto sym : vars_) {
            if (0 != p[whichvar])
                res = mul(res, pow(sym, make_rcp<Integer>(
                                            integer_class(p[whichvar]))));
            whichvar++;
        }
        args.push_back(res);
    }
    return args;
}

bool MultivariatePolynomial::is_canonical(const set_sym &vars,
                                          const umap_sym_uint &degrees,
                                          const umap_uvec_expr &dict)
{
    // checks that the dictionary does not contain terms with coefficient 0
    for (auto bucket : dict) {
        if (Expression(0) == bucket.second)
            return false;
    }

    // checks that the maximum degree of any variable is correct according to
    // the dictionary
    unsigned int whichvar
        = 0; // keeps track of the index of the variable we are checking
    for (auto var : vars) {
        unsigned int maxdegree = 0;
        for (auto bucket : dict) {
            if (bucket.first[whichvar] > degrees.find(var)->second)
                return false;
            else if (maxdegree < bucket.first[whichvar])
                maxdegree = bucket.first[whichvar];
        }
        if (maxdegree != degrees.find(var)->second)
            return false;
        whichvar++;
    }
    return true;
}

std::size_t MultivariatePolynomial::__hash__() const
{
    std::hash<std::string> hash_string;
    std::size_t seed = MULTIVARIATEPOLYNOMIAL;
    for (auto var : vars_)
        // boost's method for combining hashes
        seed ^= hash_string(var->get_name()) + 0x9e3779b + (seed << 6)
                + (seed >> 2);

    std::vector<vec_uint> v
        = order_umap<vec_uint, umap_uvec_expr, vec_uint_compare>(dict_);

    for (auto vec : v) {
        seed ^= vec_uint_hash()(dict_.find(vec)->first) + 0x9e3779b
                + (seed << 6) + (seed >> 2);
        seed ^= (dict_.find(vec)->second).get_basic()->__hash__() + 0x9e3779b
                + (seed << 6) + (seed >> 2);
    }
    return seed;
}

bool MultivariatePolynomial::__eq__(const Basic &o) const
{
    // compare constants without regard to vars
    if (1 == dict_.size()
        && 1 == static_cast<const MultivariatePolynomial &>(o).dict_.size()) {
        if (dict_.begin()->second
            != static_cast<const MultivariatePolynomial &>(o)
                   .dict_.begin()
                   ->second)
            return false;
        vec_uint v1;
        v1.resize(vars_.size(), 0);
        vec_uint v2;
        v2.resize(static_cast<const MultivariatePolynomial &>(o).vars_.size(),
                  0);
        if (dict_.begin()->first == v1
            && static_cast<const MultivariatePolynomial &>(o)
                       .dict_.begin()
                       ->first
                   == v2)
            return true;
        return false;
    } else if (dict_.size() == 0
               && static_cast<const MultivariatePolynomial &>(o).dict_.size()
                      == 0) {
        return true;
    } else {
        return (set_eq<set_sym>(
                    vars_, static_cast<const MultivariatePolynomial &>(o).vars_)
                && umap_uvec_expr_eq(
                       dict_,
                       static_cast<const MultivariatePolynomial &>(o).dict_));
    }
}

int MultivariatePolynomial::compare(const Basic &o) const
{
    // copied from UnivariateIntPolynomial::compare and then modified.
    const MultivariatePolynomial &s
        = static_cast<const MultivariatePolynomial &>(o);

    int cmp = set_compare<set_sym>(vars_, s.vars_);
    if (cmp != 0)
        return cmp;

    return umap_uvec_expr_compare(dict_, s.dict_);
}

Expression MultivariatePolynomial::eval(
    std::map<RCP<const Symbol>, Expression, RCPSymbolCompare> &vals) const
{
    Expression ans(0);
    for (auto bucket : dict_) {
        Expression term = bucket.second;
        unsigned int whichvar = 0;
        for (auto sym : vars_) {
            term *= pow_ex(vals.find(sym)->second, bucket.first[whichvar]);
            whichvar++;
        }
        ans += term;
    }
    return ans;
}

RCP<const MultivariatePolynomial> add_mult_poly(const MultivariatePolynomial &a,
                                                const MultivariatePolynomial &b)
{
    vec_uint v1;
    vec_uint v2;
    set_sym s;
    umap_uvec_expr dict;
    umap_sym_uint degs;
    unsigned int size = reconcile(v1, v2, s, a.vars_, b.vars_);
    for (auto bucket : a.dict_) {
        dict.insert(std::pair<vec_uint, Expression>(
            translate(bucket.first, v1, size), bucket.second));
    }
    for (auto bucket : b.dict_) {
        auto target = dict.find(translate(bucket.first, v2, size));
        if (target != dict.end()) {
            target->second += bucket.second;
        } else {
            dict.insert(std::pair<vec_uint, Expression>(
                translate(bucket.first, v2, size), bucket.second));
        }
    }
    return MultivariatePolynomial::from_dict(s, std::move(dict));
}

RCP<const MultivariatePolynomial> neg_mult_poly(const MultivariatePolynomial &a)
{
    umap_uvec_expr dict;
    set_sym s = a.vars_;
    for (auto bucket : a.dict_) {
        dict.insert(
            std::pair<vec_uint, Expression>(bucket.first, -bucket.second));
    }
    return MultivariatePolynomial::from_dict(s, std::move(dict));
}

RCP<const MultivariatePolynomial> sub_mult_poly(const MultivariatePolynomial &a,
                                                const MultivariatePolynomial &b)
{
    return (add_mult_poly(a, *neg_mult_poly(b)));
}

RCP<const MultivariatePolynomial> mul_mult_poly(const MultivariatePolynomial &a,
                                                const MultivariatePolynomial &b)
{
    vec_uint v1;
    vec_uint v2;
    set_sym s;
    umap_uvec_expr dict;
    unsigned int size = reconcile(v1, v2, s, a.vars_, b.vars_);
    for (auto a_bucket : a.dict_) {
        for (auto b_bucket : b.dict_) {
            vec_uint target = uint_vec_translate_and_add(
                a_bucket.first, b_bucket.first, v1, v2, size);
            if (dict.find(target) == dict.end()) {
                dict.insert(std::pair<vec_uint, Expression>(
                    target, a_bucket.second * b_bucket.second));
            } else {
                dict.find(target)->second += a_bucket.second * b_bucket.second;
            }
        }
    }
    return MultivariatePolynomial::from_dict(s, std::move(dict));
}

RCP<const MultivariatePolynomial> add_mult_poly(const MultivariatePolynomial &a,
                                                const UnivariatePolynomial &b)
{
    vec_uint v1;
    unsigned int v2;
    set_sym s;
    umap_uvec_expr dict;
    unsigned int size = reconcile(v1, v2, s, a.vars_, b.get_var());
    for (auto bucket : a.dict_) {
        dict.insert(std::pair<vec_uint, Expression>(
            translate(bucket.first, v1, size), bucket.second));
    }
    for (auto bucket : b.get_dict()) {
        auto target = dict.find(translate(bucket.first, v2, size));
        if (target != dict.end()) {
            target->second += bucket.second;
        } else {
            dict.insert(std::pair<vec_uint, Expression>(
                translate(bucket.first, v2, size), bucket.second));
        }
    }
    return MultivariatePolynomial::from_dict(s, std::move(dict));
}

RCP<const MultivariatePolynomial> add_mult_poly(const UnivariatePolynomial &a,
                                                const MultivariatePolynomial &b)
{
    return add_mult_poly(b, a);
}

RCP<const MultivariatePolynomial> sub_mult_poly(const MultivariatePolynomial &a,
                                                const UnivariatePolynomial &b)
{
    return add_mult_poly(a, *neg_uni_poly(b));
}

RCP<const MultivariatePolynomial> sub_mult_poly(const UnivariatePolynomial &a,
                                                const MultivariatePolynomial &b)
{
    return add_mult_poly(*neg_mult_poly(b), a);
}

RCP<const MultivariatePolynomial> mul_mult_poly(const MultivariatePolynomial &a,
                                                const UnivariatePolynomial &b)
{
    vec_uint v1;
    unsigned int v2;
    set_sym s;
    umap_uvec_expr dict;
    unsigned int size = reconcile(v1, v2, s, a.vars_, b.get_var());
    for (auto a_bucket : a.dict_) {
        for (auto b_bucket : b.get_dict()) {
            vec_uint target = uint_vec_translate_and_add(
                a_bucket.first, b_bucket.first, v1, v2, size);
            if (dict.find(target) == dict.end()) {
                dict.insert(std::pair<vec_uint, Expression>(
                    target, a_bucket.second * b_bucket.second));
            } else {
                dict.find(target)->second += a_bucket.second * b_bucket.second;
            }
        }
    }
    return MultivariatePolynomial::from_dict(s, std::move(dict));
}

RCP<const MultivariatePolynomial> mul_mult_poly(const UnivariatePolynomial &a,
                                                const MultivariatePolynomial &b)
{
    return mul_mult_poly(b, a);
}

RCP<const MultivariatePolynomial> add_mult_poly(const UnivariatePolynomial &a,
                                                const UnivariatePolynomial &b)
{
    unsigned int v1;
    unsigned int v2;
    set_sym s;
    umap_uvec_expr dict;
    bool same = false; // are the variables of a and b the same?
    if (0 == a.get_var()->compare(*b.get_var())) {
        v1 = 0;
        v2 = 0;
        s.insert(a.get_var());
        same = true;
    } else {
        if (-1 == a.get_var()->compare(*b.get_var())) {
            v1 = 0;
            v2 = 1;
        } else {
            v2 = 0;
            v1 = 1;
        }
        s.insert(a.get_var());
        s.insert(b.get_var());
    }

    for (auto bucket : a.get_dict()) {
        vec_uint v;
        v.insert(v.end(), 0);
        if (!same)
            v.insert(v.end(), 0);
        v[v1] = bucket.first;
        dict.insert(std::pair<vec_uint, Expression>(v, bucket.second));
    }
    for (auto bucket : b.get_dict()) {
        vec_uint v;
        v.insert(v.end(), 0);
        if (!same)
            v.insert(v.end(), 0);
        v[v2] = bucket.first;
        auto target = dict.find(v);
        if (target != dict.end()) {
            target->second += bucket.second;
        } else {
            dict.insert(std::pair<vec_uint, Expression>(v, bucket.second));
        }
    }
    return MultivariatePolynomial::from_dict(s, std::move(dict));
}

RCP<const MultivariatePolynomial> sub_mult_poly(const UnivariatePolynomial &a,
                                                const UnivariatePolynomial &b)
{
    return add_mult_poly(a, *neg_uni_poly(b));
}

RCP<const MultivariatePolynomial> mul_mult_poly(const UnivariatePolynomial &a,
                                                const UnivariatePolynomial &b)
{
    unsigned int v1;
    unsigned int v2;
    set_sym s;
    umap_uvec_expr dict;
    bool same = false; // are the variables of a and b the same?
    if (0 == a.get_var()->compare(*b.get_var())) {
        v1 = 0;
        v2 = 0;
        s.insert(a.get_var());
        same = true;
    } else {
        if (-1 == a.get_var()->compare(*b.get_var())) {
            v1 = 0;
            v2 = 1;
        } else {
            v2 = 0;
            v1 = 1;
        }
        s.insert(a.get_var());
        s.insert(b.get_var());
    }
    for (auto a_bucket : a.get_dict()) {
        for (auto b_bucket : b.get_dict()) {
            vec_uint target;
            target.insert(target.end(), 0);
            if (!same)
                target.insert(target.end(), 0);
            target[v1] += a_bucket.first;
            target[v2] += b_bucket.first;
            if (dict.find(target) == dict.end()) {
                dict.insert(std::pair<vec_uint, Expression>(
                    target, a_bucket.second * b_bucket.second));
            } else {
                dict.find(target)->second += a_bucket.second * b_bucket.second;
            }
        }
    }
    return MultivariatePolynomial::from_dict(s, std::move(dict));
}

} // SymEngine
