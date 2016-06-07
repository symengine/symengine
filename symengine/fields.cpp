#include <symengine/fields.h>
#include <symengine/add.h>
#include <symengine/constants.h>
#include <symengine/mul.h>
#include <symengine/pow.h>

namespace SymEngine
{
GaloisField::GaloisField(const RCP<const Symbol> &var, GaloisFieldDict &&dict)
    : UPolyBase(var, std::move(dict))
{
    SYMENGINE_ASSERT(is_canonical(poly_))
}

bool GaloisField::is_canonical(const GaloisFieldDict &dict) const
{
    // Check if dictionary contains terms with coeffienct 0
    for (auto iter : dict.dict_)
        if (iter.second == 0)
            return false;
    return true;
}

std::size_t GaloisField::__hash__() const
{
    std::hash<std::string> hash_string;
    std::size_t seed = GALOISFIELD;

    seed += hash_string(this->var_->get_name());
    for (const auto &it : poly_.dict_) {
        std::size_t temp = GALOISFIELD;
        hash_combine<unsigned int>(temp, it.first);
        hash_combine<long long int>(temp, mp_get_si(it.second));
        seed += temp;
    }
    return seed;
}

int GaloisField::compare(const Basic &o) const
{
    const GaloisField &s = static_cast<const GaloisField &>(o);

    if (poly_.size() != s.poly_.size())
        return (poly_.size() < s.poly_.size()) ? -1 : 1;

    int cmp = var_->compare(*s.var_);
    if (cmp != 0)
        return cmp;

    return map_uint_mpz_compare(poly_.dict_, s.poly_.dict_);
}


RCP<const GaloisField> GaloisField::from_dict(const RCP<const Symbol> &var,
                                        GaloisFieldDict &&d)
{
    return make_rcp<const GaloisField>(var, std::move(d));
}

RCP<const GaloisField> GaloisField::from_vec(const RCP<const Symbol> &var,
                                       const std::vector<integer_class> &v,
                                       const integer_class &modulo)
{
    return make_rcp<const GaloisField>(var, GaloisFieldDict::from_vec(v, modulo));
}

vec_basic GaloisField::get_args() const
{
    vec_basic args;
    for (const auto &p : poly_.dict_) {
        if (p.first == 0) {
            args.push_back(integer(p.second));
        } else if (p.first == 1) {
            if (p.second == 1) {
                args.push_back(var_);
            } else {
                args.push_back(
                    Mul::from_dict(integer(p.second), {{var_, one}}));
            }
        } else {
            if (p.second == 1) {
                args.push_back(pow(var_, integer(p.first)));
            } else {
                args.push_back(Mul::from_dict(integer(p.second),
                                              {{var_, integer(p.first)}}));
            }
        }
    }
    if (poly_.dict_.empty())
        args.push_back(zero);
    return args;
}

GaloisFieldDict GaloisFieldDict::gf_neg() const
{
    return gf_mul_ground(integer_class(-1));
}

GaloisFieldDict GaloisFieldDict::gf_mul_ground(const integer_class a) const
{
    map_uint_mpz dict;
    for (auto iter : dict_) {
        dict[iter.first] = a * iter.second;
    }
    return GaloisFieldDict(dict, modulo_);
}

void GaloisFieldDict::gf_imul_ground(const integer_class a)
{
    integer_class temp;
    for (auto &iter : dict_) {
        temp = a * iter.second;
        dict_add_val(iter.first, temp);
    }
}

GaloisFieldDict GaloisFieldDict::gf_quo_ground(const integer_class a) const
{
    if (a == 0)
        throw std::runtime_error("ZeroDivisionError");
    map_uint_mpz dict;
    integer_class inv;
    mp_invert(inv, a, modulo_);
    for (auto iter : dict_) {
        dict[iter.first] = inv * iter.second;
    }
    return GaloisFieldDict(dict, modulo_);
}


void GaloisFieldDict::gf_div(const GaloisFieldDict &o,
                         const Ptr<GaloisFieldDict> &quo,
                         const Ptr<GaloisFieldDict> &rem) const
{
    SYMENGINE_ASSERT(modulo_ == o.modulo_);
    if (o.dict_.empty())
        throw std::runtime_error("ZeroDivisionError");
    map_uint_mpz dict_divisor = o.dict_;
    map_uint_mpz dict_out;
    long deg_dividend = dict_.empty() ? 0 : dict_.rbegin()->first;
    long deg_divisor = dict_divisor.rbegin()->first;
    if (deg_dividend < deg_divisor) {
        *quo = GaloisFieldDict(dict_out, modulo_);
        *rem = GaloisFieldDict(dict_, modulo_);
    } else {
        dict_out = dict_;
        integer_class inv;
        mp_invert(inv, dict_divisor.rbegin()->second, modulo_);
        integer_class coeff;
        for (long it = deg_dividend; it >= 0; it--) {
            coeff = dict_out[it];
            long lb = std::max(long(0), deg_divisor - deg_dividend + it);
            long ub = std::min(it, deg_divisor - 1) + 1;
            for (long j = lb; j < ub; j++) {
                coeff
                    -= dict_out[it - j + deg_divisor] * dict_divisor[j];
            }
            if (it >= deg_divisor)
                coeff *= inv;
            dict_out[it] = coeff % modulo_;
        }
        map_uint_mpz dict_rem, dict_quo;
        for (auto it : dict_out) {
            if (it.first < deg_divisor)
                dict_rem[it.first] = it.second;
            else
                dict_quo[it.first - deg_divisor] = it.second;
        }
        *quo = GaloisFieldDict(dict_quo, modulo_);
        *rem = GaloisFieldDict(dict_rem, modulo_);
    }
}

GaloisFieldDict
GaloisFieldDict::gf_quo(const GaloisFieldDict &o) const
{
    SYMENGINE_ASSERT(modulo_ == o.modulo_);
    if (o.dict_.empty())
        throw std::runtime_error("ZeroDivisionError");
    map_uint_mpz dict_divisor = o.dict_;
    map_uint_mpz dict_out;
    long deg_dividend = dict_.empty() ? 0 : dict_.rbegin()->first;
    long deg_divisor = dict_divisor.rbegin()->first;
    if (deg_dividend < deg_divisor) {
        return GaloisFieldDict(dict_out, modulo_);
    }
    dict_out = dict_;
    integer_class inv;
    mp_invert(inv, dict_divisor.rbegin()->second, modulo_);
    integer_class coeff;
    map_uint_mpz dict_quo;
    for (long it = deg_dividend; it >= deg_divisor; it--) {
        coeff = dict_out[it];
        long lb = std::max(long(0), deg_divisor - deg_dividend + it);
        long ub = std::min(it, deg_divisor - 1) + 1;
        for (long j = lb; j < ub; j++) {
            coeff
                -= dict_out[it - j + deg_divisor] * dict_divisor[j];
        }
        dict_out[it] = dict_quo[it - deg_divisor]
            = (coeff * inv) % modulo_;
    }
    return GaloisFieldDict(dict_quo, modulo_);
}

GaloisFieldDict GaloisFieldDict::gf_lshift(const integer_class n) const
{
    map_uint_mpz dict_out;
    unsigned n_val = mp_get_si(n);
    for (auto iter : dict_) {
        dict_out.insert(dict_out.end(), {iter.first + n_val, iter.second});
    }
    return GaloisFieldDict(dict_out, modulo_);
}

void GaloisFieldDict::gf_rshift(const integer_class n,
                            const Ptr<GaloisFieldDict> &quo,
                            const Ptr<GaloisFieldDict> &rem) const
{
    map_uint_mpz dict_quo, dict_rem;
    unsigned n_val = mp_get_si(n);
    for (auto iter : dict_) {
        if (iter.first >= n_val)
            dict_quo.insert(dict_quo.end(), {iter.first - n_val, iter.second});
        else
            dict_rem.insert(dict_rem.end(), iter);
    }
    *quo = GaloisFieldDict(dict_quo, modulo_);
    *rem = GaloisFieldDict(dict_rem, modulo_);
}

GaloisFieldDict GaloisFieldDict::gf_sqr() const
{
    return (*this * *this);
}

GaloisFieldDict GaloisFieldDict::gf_pow(const integer_class n) const
{
    if (n == 0) {
        return GaloisFieldDict({integer_class(1)}, modulo_);
    }
    if (n == 1)
        return static_cast<GaloisFieldDict>(*this);
    if (n == 2)
        return gf_sqr();
    long num = mp_get_si(n);
    GaloisFieldDict to_sq = static_cast<GaloisFieldDict>(*this);
    GaloisFieldDict to_ret = GaloisFieldDict({integer_class(1)}, modulo_);
    while (1) {
        if (num & 1) {
            to_ret = to_ret * to_sq;
            num -= 1;
        }
        num >>= 1;
        if (num == 0)
            return to_ret;
        to_sq = to_sq.gf_sqr();
    }
}

void GaloisFieldDict::gf_monic(integer_class &res,
                           const Ptr<GaloisFieldDict> &monic) const
{
    if (dict_.empty()) {
        res = integer_class(0);
        *monic = static_cast<GaloisFieldDict>(*this);
    } else {
        res = dict_.rbegin()->second;
        if (res == integer_class(1)) 
            *monic = static_cast<GaloisFieldDict>(*this);
        else
            *monic = gf_quo_ground(res);
    }
}

GaloisFieldDict GaloisFieldDict::gf_gcd(const GaloisFieldDict &o) const
{
    SYMENGINE_ASSERT(modulo_ == o.modulo_);
    GaloisFieldDict f = static_cast<GaloisFieldDict>(*this);
    GaloisFieldDict g = o;
    GaloisFieldDict temp_out, temp_ex;
    while (not g.dict_.empty()) {
        temp_ex = g;
        f.gf_div(temp_ex, outArg(temp_out), outArg(g)); // g = f % g
        f = temp_ex;
    }
    integer_class temp_LC;
    f.gf_monic(temp_LC, outArg(temp_out));
    return temp_out;
}

GaloisFieldDict GaloisFieldDict::gf_lcm(const GaloisFieldDict &o) const
{
    SYMENGINE_ASSERT(modulo_ == o.modulo_);
    map_uint_mpz dict_out;
    if (dict_.empty() or o.dict_.empty())
        return GaloisFieldDict(dict_out, modulo_);
    GaloisFieldDict out;
    out = o * gf_quo(gf_gcd(o));
    integer_class temp_LC;
    out.gf_monic(temp_LC, outArg(out));
    return out;
}
}