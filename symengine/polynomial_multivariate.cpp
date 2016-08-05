#include <symengine/polynomial_multivariate.h>

namespace SymEngine
{

RCP<const Basic> MIntPoly::as_symbolic() const
{
    vec_basic args;
    for (const auto &p : poly_.dict_) {
        RCP<const Basic> res = integer(p.second);
        int whichvar = 0;
        for (auto sym : vars_) {
            if (0 != p.first[whichvar])
                res = SymEngine::mul(res, pow(sym, integer(p.first[whichvar])));
            whichvar++;
        }
        args.push_back(res);
    }
    return SymEngine::add(args);
}

std::size_t MIntPoly::__hash__() const
{
    std::size_t seed = MINTPOLY;
    for (auto var : vars_)
        hash_combine<std::string>(seed, var->__str__());

    for (auto &p : poly_.dict_) {
        std::size_t t = vec_hash<vec_uint>()(p.first);
        hash_combine<std::size_t>(t, mp_get_si(p.second));
        seed ^= t;
    }
    return seed;
}

integer_class MIntPoly::eval(
    std::map<RCP<const Basic>, integer_class, RCPBasicKeyLess> &vals) const
{
    // TODO : handle missing values
    integer_class ans(0), temp, term;
    for (auto bucket : poly_.dict_) {
        term = bucket.second;
        unsigned int whichvar = 0;
        for (auto sym : vars_) {
            mp_pow_ui(temp, vals.find(sym)->second, bucket.first[whichvar]);
            term *= temp;
            whichvar++;
        }
        ans += term;
    }
    return ans;
}

RCP<const Basic> MExprPoly::as_symbolic() const
{
    vec_basic args;
    for (const auto &p : poly_.dict_) {
        RCP<const Basic> res = (p.second.get_basic());
        int whichvar = 0;
        for (auto sym : vars_) {
            if (0 != p.first[whichvar])
                res = SymEngine::mul(res, pow(sym, integer(p.first[whichvar])));
            whichvar++;
        }
        args.push_back(res);
    }
    return SymEngine::add(args);
}

std::size_t MExprPoly::__hash__() const
{
    std::size_t seed = MEXPRPOLY;
    for (auto var : vars_)
        hash_combine<std::string>(seed, var->__str__());

    for (auto &p : poly_.dict_) {
        std::size_t t = vec_hash<vec_int>()(p.first);
        hash_combine<Basic>(t, *(p.second.get_basic()));
        seed ^= t;
    }
    return seed;
}

Expression MExprPoly::eval(
    std::map<RCP<const Basic>, Expression, RCPBasicKeyLess> &vals) const
{
    // TODO : handle missing values
    Expression ans(0);
    for (auto bucket : poly_.dict_) {
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

unsigned int reconcile(vec_uint &v1, vec_uint &v2, set_basic &s,
                       const set_basic &s1, const set_basic &s2)
{
    auto i = s1.begin();
    auto j = s2.begin();
    unsigned int pos = 0;

    // Performs a merge of s1 and s2, and builds up v1 and v2 as translators
    // v1[i] and v2[i] is the position of the ith symbol in the new set

    // set union
    s = s1;
    s.insert(s2.begin(), s2.end());

    for (auto &it : s) {
        if (i != s1.end() and eq(*it, **i)) {
            v1.push_back(pos);
            i++;
        }
        if (j != s2.end() and eq(*it, **j)) {
            v2.push_back(pos);
            j++;
        }
        pos++;
    }
    return pos; // return size of the new symbol set
}

RCP<const Basic> MultivariatePolynomial::as_symbolic() const
{
    vec_basic args;
    for (const auto &p : dict_) {
        RCP<const Basic> res = (p.second.get_basic());
        int whichvar = 0;
        for (auto sym : vars_) {
            if (0 != p.first[whichvar])
                res = SymEngine::mul(res, pow(sym, integer(p.first[whichvar])));
            whichvar++;
        }
        args.push_back(res);
    }
    return SymEngine::add(args);
}

std::size_t MultivariatePolynomial::__hash__() const
{
    std::size_t seed = MULTIVARIATEPOLYNOMIAL;
    for (auto var : vars_)
        hash_combine<std::string>(seed, var->__str__());

    for (auto &p : dict_) {
        std::size_t t = vec_hash<vec_int>()(p.first);
        hash_combine<Basic>(t, *(p.second.get_basic()));
        seed ^= t;
    }
    return seed;
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
MultivariatePolynomial::convert(const UExprPoly &o)
{
    vec_basic s;
    s.push_back(o.get_var());

    umap_vec_expr d;
    for (auto &p : o.get_dict()) {
        vec_int v;
        v.push_back(p.first);
        d[v] = p.second;
    }
    return MultivariatePolynomial::create(s, std::move(d));
}

} // SymEngine
