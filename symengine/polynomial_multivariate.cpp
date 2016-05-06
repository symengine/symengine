#include <symengine/add.h>
#include <symengine/constants.h>
#include <symengine/mul.h>
#include <symengine/polynomial.h>
#include <symengine/polynomial_multivariate.h>
#include <symengine/pow.h>

namespace SymEngine
{

MultivariateIntPolynomial::MultivariateIntPolynomial(const set_sym &vars,
                                                     umap_sym_uint &degrees,
                                                     umap_uvec_mpz &dict)
    : vars_{std::move(vars)}, degrees_{std::move(degrees)},
      dict_{std::move(dict)}
{
    SYMENGINE_ASSERT(is_canonical(vars_, degrees_, dict_))
}

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
    return (
        set_eq<set_sym>(vars_,
                        static_cast<const MultivariateIntPolynomial &>(o).vars_)
        && umap_uvec_mpz_eq(
               dict_, static_cast<const MultivariateIntPolynomial &>(o).dict_));
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

std::string MultivariateIntPolynomial::toString() const
{
    std::ostringstream s;
    bool first = true; // is this the first term being printed out?
    // To change the ordering in which the terms will print out, change
    // vec_uint_compare in dict.h
    std::vector<vec_uint> v
        = order_umap<vec_uint, umap_uvec_mpz, vec_uint_compare>(dict_);

    for (vec_uint exps : v) {
        integer_class c = dict_.find(exps)->second;
        if (c != 0) {
            if (c > 0 && !first) {
                s << "+ ";
            } else if (c < 0) {
                s << "- ";
            }
            unsigned int i = 0;
            std::ostringstream expr;
            for (auto it : vars_) {
                if (dict_.find(exps)->first[i] != 0) {
                    expr << it->get_name();
                    if (dict_.find(exps)->first[i] > 1)
                        expr << "**" << dict_.find(exps)->first[i];
                    expr << " ";
                }
                i++;
            }
            if (mp_abs(c) != 1 || expr.str().empty())
                s << mp_abs(c) << "*";
            s << expr.str();
            first = false;
        }
    }

    if (s.str().empty())
        s << "0 ";
    std::string final(s.str());
    final.pop_back();
    return final;
}

unsigned int reconcile(vec_uint &v1, vec_uint &v2, set_sym &s,
                       const set_sym &s1, const set_sym &s2)
{
    auto a1 = s1.begin();
    auto a2 = s2.begin();
    unsigned int poscount = 0;
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

} // SymEngine