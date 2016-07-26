#include <symengine/polynomial_multivariate.h>
#include <symengine/printer.h>

namespace SymEngine
{

vec_basic MultivariateIntPolynomialExpr::get_args() const
{
    vec_basic args;
    umap_uvec_mpz d;
    // To change the ordering in which the terms appear in the vector, use
    // a different comparator for order_umap
    auto v = sorted_keys(dict_);
    for (const auto &p : v) {
        map_basic_basic b;
        int whichvar = 0;
        for (auto sym : vars_) {
            if (integer_class(0) != p[whichvar])
                insert(b, sym, integer(p[whichvar]));
            whichvar++;
        }
        args.push_back(
            Mul::from_dict(integer(dict_.find(p)->second), std::move(b)));
    }
    return args;
}

std::size_t MultivariateIntPolynomialExpr::__hash__() const
{
    std::size_t seed = 0; // MULTIVARIATEINTPOLYNOMIALEXPR;
    for (auto var : vars_)
        hash_combine<std::string>(seed, var->__str__());

    for (auto &p : dict_) {
        std::size_t t = vec_uint_hash()(p.first);
        hash_combine<std::size_t>(t, mp_get_si(p.second));
        seed ^= t;
    }
    return seed;
}

int MultivariateIntPolynomialExpr::compare(
    const MultivariateIntPolynomialExpr &s) const
{
    // copied from UnivariateIntPolynomial::compare and then modified.
    if (vars_.size() != s.vars_.size())
        return vars_.size() < s.vars_.size() ? -1 : 1;
    if (dict_.size() != s.dict_.size())
        return dict_.size() < s.dict_.size() ? -1 : 1;

    int cmp = unified_compare(vars_, s.vars_);
    if (cmp != 0)
        return cmp;

    return unified_compare(dict_, s.dict_);
}

integer_class MultivariateIntPolynomialExpr::eval(
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

std::string MultivariateIntPolynomialExpr::toString() const
{
    std::ostringstream s;
    bool first = true; // is this the first term being printed out?

    std::map<RCP<const Basic>, unsigned int, RCPBasicStrCmp> m;
    auto ptr = vars_.begin();
    for (unsigned int i = 0; i < vars_.size(); i++) {
        m.insert(std::pair<RCP<const Basic>, unsigned int>(*ptr, i));
        ptr++;
        // i is the position in the original set;
    }

    vec_uint translator;
    translator.resize(vars_.size());
    unsigned int j = 0;
    for (auto bucket : m) {
        translator[bucket.second] = j;
        // ptr->second is the position in the old exponent
        // j is the position in the new ordering
        j++;
    }

    std::map<vec_uint, integer_class> d;

    for (auto bucket : dict_) {
        d.insert(std::pair<vec_uint, integer_class>(
            translate<vec_uint>(bucket.first, translator, vars_.size()),
            bucket.second));
    }

    for (auto bucket : d) {
        integer_class c = bucket.second;
        if (!first) {
            if (c < 0) {
                s << " - ";
            } else {
                s << " + ";
            }
        } else if (c < 0) {
            s << "-";
        }

        unsigned int i = 0;
        std::ostringstream expr;
        bool first_var = true;
        for (auto var_bucket : m) {
            if (bucket.first[i] != 0) {
                if (!first_var) {
                    expr << "*";
                }
                expr << var_bucket.first->__str__();
                if (bucket.first[i] > 1)
                    expr << "**" << bucket.first[i];
                first_var = false;
            }
            i++;
        }
        if (mp_abs(c) != 1) {
            s << mp_abs(c);
            if (!expr.str().empty()) {
                s << "*";
            }
        } else if (expr.str().empty()) {
            s << "1";
        }
        s << expr.str();
        first = false;
    }

    if (s.str().empty())
        s << "0";
    return s.str();
}

MultivariateIntPolynomialExpr
MultivariateIntPolynomialExpr::convert(const UIntPoly &o)
{
    vec_basic s;
    s.push_back(o.get_var());

    umap_uvec_mpz d;
    for (auto &p : o.get_dict()) {
        vec_uint v;
        v.push_back(p.first);
        d[v] = p.second;
    }
    return MultivariateIntPolynomialExpr::create(s, std::move(d));
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
        if ((*a1)->__eq__(**a2) && (a1 != s1.end() && a2 != s2.end())) {
            v1.insert(v1.end(), poscount);
            v2.insert(v2.end(), poscount);
            s.insert(*a1);
            a1++;
            a2++;
        } else if (RCPBasicKeyLess()(*a1, *a2)) {
            v1.insert(v1.end(), poscount);
            s.insert(*a1);
            a1++;
        } else {
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

vec_basic MultivariatePolynomialExpr::get_args() const
{
    vec_basic args;
    umap_uvec_expr d;
    // To change the ordering in which the terms appear in the vector, use
    // a different comparator for order_umap
    std::vector<vec_int> v = sorted_keys(dict_);
    for (const auto &p : v) {
        RCP<const Basic> res = ((dict_.find(p)->second).get_basic());
        int whichvar = 0;
        for (auto sym : vars_) {
            if (0 != p[whichvar])
                res = SymEngine::mul(res, pow(sym, integer(p[whichvar])));
            whichvar++;
        }
        args.push_back(res);
    }
    return args;
}

std::size_t MultivariatePolynomialExpr::__hash__() const
{
    std::size_t seed = 0; // MULTIVARIATEPOLYNOMIALEXPR;
    for (auto var : vars_)
        hash_combine<std::string>(seed, var->__str__());

    for (auto &p : dict_) {
        std::size_t t = vec_int_hash()(p.first);
        hash_combine<Basic>(t, *(p.second.get_basic()));
        seed ^= t;
    }
    return seed;
}

int MultivariatePolynomialExpr::compare(
    const MultivariatePolynomialExpr &s) const
{
    // copied from UnivariateIntPolynomial::compare and then modified.
    if (vars_.size() != s.vars_.size())
        return vars_.size() < s.vars_.size() ? -1 : 1;
    if (dict_.size() != s.dict_.size())
        return dict_.size() < s.dict_.size() ? -1 : 1;

    int cmp = unified_compare(vars_, s.vars_);
    if (cmp != 0)
        return cmp;

    return unified_compare(dict_, s.dict_);
}

Expression MultivariatePolynomialExpr::eval(
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

std::string MultivariatePolynomialExpr::toString() const
{
    std::ostringstream s;
    StrPrinter s_;
    bool first = true; // is this the first term being printed out?

    std::map<RCP<const Basic>, unsigned int, RCPBasicStrCmp> m;

    auto ptr = vars_.begin();
    for (unsigned int i = 0; i < vars_.size(); i++) {
        m.insert(std::pair<RCP<const Basic>, unsigned int>(*ptr, i));
        ptr++;
        // i is the position in the original set;
    }

    vec_uint translator;
    translator.resize(vars_.size());
    unsigned int j = 0;
    for (auto bucket : m) {
        translator[bucket.second] = j;
        // ptr->second is the position in the old exponent
        // j is the position in the new ordering
        j++;
    }

    std::map<vec_int, Expression> d;

    for (auto bucket : dict_) {
        d.insert(std::pair<vec_int, Expression>(
            translate<vec_int>(bucket.first, translator, vars_.size()),
            bucket.second));
    }

    for (auto bucket : d) {
        Expression c = bucket.second;
        std::string t = s_.parenthesizeLT(c.get_basic(), PrecedenceEnum::Mul);
        if ('-' == t[0] && !first) {
            s << " - ";
            t = t.substr(1);
        } else if (!first) {
            s << " + ";
        }
        std::ostringstream expr;
        bool first_var = true;
        unsigned int i = 0;
        for (auto var_bucket : m) {
            if (bucket.first[i] != 0) {
                if (!first_var) {
                    expr << "*";
                }
                expr << var_bucket.first->__str__();
                if (bucket.first[i] > 1 || bucket.first[i] < 0)
                    expr << "**" << bucket.first[i];
                first_var = false;
            }
            i++;
        }
        if (c != 1 && c != -1) {
            s << t;
            if (!expr.str().empty()) {
                s << "*";
            }
        } else if (expr.str().empty()) {
            s << "1";
        }
        s << expr.str();
        first = false;
    }

    if (s.str().empty())
        s << "0";
    return s.str();
}

MultivariatePolynomialExpr
MultivariatePolynomialExpr::convert(const UExprPoly &o)
{
    vec_basic s;
    s.push_back(o.get_var());

    umap_vec_expr d;
    for (auto &p : o.get_dict()) {
        vec_int v;
        v.push_back(p.first);
        d[v] = p.second;
    }
    return MultivariatePolynomialExpr::create(s, std::move(d));
}

MultivariatePolynomialExpr MultivariatePolynomialExpr::convert(const int i)
{
    set_basic s;
    vec_int v;
    return MultivariatePolynomialExpr::from_dict(s, {{v, Expression(i)}});
}

MultivariatePolynomialExpr
MultivariatePolynomialExpr::convert(const Expression &o)
{
    set_basic s;
    vec_int v;
    return MultivariatePolynomialExpr::from_dict(s, {{v, o}});
}

} // SymEngine
