#include <symengine/add.h>
#include <symengine/constants.h>
#include <symengine/mul.h>
#include <symengine/polynomial.h>
#include <symengine/polynomial_multivariate.h>
#include <symengine/pow.h>

namespace SymEngine
{

vec_basic MultivariateIntPolynomial::get_args() const
{
    vec_basic args;
    umap_uvec_mpz d;
    // To change the ordering in which the terms appear in the vector, use
    // a different comparator for order_umap
    std::vector<vec_uint> v = order_umap<vec_uint, umap_uvec_mpz>(dict_);
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

std::size_t MultivariateIntPolynomial::__hash__() const
{
    std::hash<std::string> hash_string;
    std::size_t seed = MULTIVARIATEINTPOLYNOMIAL;
    for (auto var : vars_)
        // boost's method for combining hashes
        seed ^= hash_string(var->__str__()) + 0x9e3779b + (seed << 6)
                + (seed >> 2);

    std::vector<vec_uint> v = order_umap<vec_uint, umap_uvec_mpz>(dict_);

    for (auto vec : v) {
        seed ^= vec_uint_hash()(dict_.find(vec)->first) + 0x9e3779b
                + (seed << 6) + (seed >> 2);
        seed ^= mpz_hash(dict_.find(vec)->second) + 0x9e3779b + (seed << 6)
                + (seed >> 2);
    }
    return seed;
}

int MultivariateIntPolynomial::compare(const Basic &o) const
{
    // copied from UnivariateIntPolynomial::compare and then modified.
    const MultivariateIntPolynomial &s
        = static_cast<const MultivariateIntPolynomial &>(o);

    int cmp = set_compare(vars_, s.vars_);
    if (cmp != 0)
        return cmp;

    return umap_uvec_mpz_compare(dict_, s.dict_);
}

integer_class MultivariateIntPolynomial::eval(
    std::map<RCP<const Basic>, integer_class, RCPBasicKeyLess> &vals) const
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

RCP<const MultivariateIntPolynomial>
MultivariateIntPolynomial::convert(const UnivariateIntPolynomial &o)
{
    vec_basic s;
    s.push_back(o.get_var());

    umap_uvec_mpz d;
    for (auto &p : o.get_dict()) {
        vec_uint v;
        v.push_back(p.first);
        d[v] = p.second;
    }
    return MultivariateIntPolynomial::create(s, std::move(d));
}

unsigned int reconcile(vec_uint &v1, vec_uint &v2, set_basic &s,
                       const set_basic &s1, const set_basic &s2)
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

vec_uint translate(vec_uint original, vec_uint translator, unsigned int size)
{
    vec_uint changed;
    changed.resize(size, 0);
    for (unsigned int i = 0; i < original.size(); i++) {
        changed[translator[i]] = original[i];
    }
    return changed;
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

vec_basic MultivariatePolynomial::get_args() const
{
    vec_basic args;
    umap_uvec_expr d;
    // To change the ordering in which the terms appear in the vector, use
    // a different comparator for order_umap
    std::vector<vec_uint> v = order_umap<vec_uint, umap_uvec_expr>(dict_);
    for (const auto &p : v) {
        RCP<const Basic> res = ((dict_.find(p)->second).get_basic());
        int whichvar = 0;
        for (auto sym : vars_) {
            if (0 != p[whichvar])
                res = SymEngine::mul(
                    res,
                    pow(sym, make_rcp<Integer>(integer_class(p[whichvar]))));
            whichvar++;
        }
        args.push_back(res);
    }
    return args;
}

std::size_t MultivariatePolynomial::__hash__() const
{
    std::hash<std::string> hash_string;
    std::size_t seed = MULTIVARIATEPOLYNOMIAL;
    for (auto var : vars_)
        // boost's method for combining hashes
        seed ^= hash_string(var->__str__()) + 0x9e3779b + (seed << 6)
                + (seed >> 2);

    std::vector<vec_uint> v = order_umap<vec_uint, umap_uvec_expr>(dict_);

    for (auto vec : v) {
        seed ^= vec_uint_hash()(dict_.find(vec)->first) + 0x9e3779b
                + (seed << 6) + (seed >> 2);
        seed ^= (dict_.find(vec)->second).get_basic()->__hash__() + 0x9e3779b
                + (seed << 6) + (seed >> 2);
    }
    return seed;
}

int MultivariatePolynomial::compare(const Basic &o) const
{
    // copied from UnivariateIntPolynomial::compare and then modified.
    const MultivariatePolynomial &s
        = static_cast<const MultivariatePolynomial &>(o);

    int cmp = set_compare<set_basic>(vars_, s.vars_);
    if (cmp != 0)
        return cmp;

    return umap_uvec_expr_compare(dict_, s.dict_);
}

Expression MultivariatePolynomial::eval(
    std::map<RCP<const Basic>, Expression, RCPBasicKeyLess> &vals) const
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

RCP<const MultivariatePolynomial>
MultivariatePolynomial::convert(const UnivariatePolynomial &o)
{
    vec_basic s;
    s.push_back(o.get_var());

    umap_uvec_expr d;
    for (auto &p : o.get_dict()) {
        vec_uint v;
        v.push_back(p.first);
        d[v] = p.second;
    }
    return MultivariatePolynomial::create(s, std::move(d));
}

} // SymEngine
