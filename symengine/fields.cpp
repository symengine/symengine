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
    if (dict.modulo_ <= integer_class(0))
        return false;
    // for (auto iter : dict.dict_)
    //     if (iter.second == 0)
    //         return false;
    return true;
}

std::size_t GaloisField::__hash__() const
{
    std::hash<std::string> hash_string;
    std::size_t seed = GALOISFIELD;

    seed += hash_string(this->var_->get_name());
    for (const auto &it : poly_.dict_) {
        std::size_t temp = GALOISFIELD;
        hash_combine<long long int>(temp, mp_get_si(it));
        seed += temp;
    }
    return seed;
}

int GaloisField::compare(const Basic &o) const
{
    const GaloisField &s = static_cast<const GaloisField &>(o);

    if (poly_.size() != s.poly_.size())
        return (poly_.size() < s.poly_.size()) ? -1 : 1;

    int cmp = unified_compare(var_, s.var_);
    if (cmp != 0)
        return cmp;

    cmp = unified_compare(poly_.modulo_, s.poly_.modulo_);
    if (cmp != 0)
        return cmp;

    return unified_compare(poly_.dict_, s.poly_.dict_);
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
    // for (const auto &p : poly_.dict_) {
    //     if (p.first == 0) {
    //         args.push_back(integer(p.second));
    //     } else if (p.first == 1) {
    //         if (p.second == 1) {
    //             args.push_back(var_);
    //         } else {
    //             args.push_back(
    //                 Mul::from_dict(integer(p.second), {{var_, one}}));
    //         }
    //     } else {
    //         if (p.second == 1) {
    //             args.push_back(pow(var_, integer(p.first)));
    //         } else {
    //             args.push_back(Mul::from_dict(integer(p.second),
    //                                           {{var_, integer(p.first)}}));
    //         }
    //     }
    // }
    // if (poly_.dict_.empty())
    //     args.push_back(zero);
    return args;
}

void GaloisFieldDict::gf_div(const GaloisFieldDict &o,
                         const Ptr<GaloisFieldDict> &quo,
                         const Ptr<GaloisFieldDict> &rem) const
{
    if (modulo_ != o.modulo_)
        throw std::runtime_error("Error: field must be same.");
    if (o.dict_.empty())
        throw std::runtime_error("ZeroDivisionError");
    std::vector<integer_class> dict_out;
    if (dict_.empty()) {
        *quo = GaloisFieldDict::from_vec(dict_out, modulo_);
        *rem = GaloisFieldDict::from_vec(dict_, modulo_);
        return;
    }
    auto dict_divisor = o.dict_;
    unsigned int deg_dividend = this->degree();
    unsigned int deg_divisor = o.degree();
    if (deg_dividend < deg_divisor) {
        *quo = GaloisFieldDict::from_vec(dict_out, modulo_);
        *rem = GaloisFieldDict::from_vec(dict_, modulo_);
    } else {
        dict_out = dict_;
        integer_class inv;
        mp_invert(inv, *(dict_divisor.rbegin()), modulo_);
        integer_class coeff;
        for (auto it = deg_dividend + 1; it-- != 0; ) {
            coeff = dict_out[it];
            if (coeff == integer_class(0) && it >= deg_divisor)
                continue;
            auto lb = deg_divisor + it > deg_dividend ? 
                      deg_divisor + it - deg_dividend : 0;
            auto ub = std::min(it + 1, deg_divisor);
            for (long j = lb; j < ub; ++j) {
                mp_addmul(coeff, dict_out[it - j + deg_divisor], -dict_divisor[j]);
            }
            if (it >= deg_divisor)
                coeff *= inv;
            dict_out[it] = coeff % modulo_;
        }
        std::vector<integer_class> dict_rem, dict_quo;
        dict_rem.resize(deg_divisor);
        dict_quo.resize(deg_dividend - deg_divisor + 1);
        for (unsigned it = 0; it < dict_out.size(); it++) {
            if (it < deg_divisor)
                dict_rem[it] = dict_out[it];
            else
                dict_quo[it - deg_divisor] = dict_out[it];
        }
        *quo = GaloisFieldDict::from_vec(dict_quo, modulo_);
        *rem = GaloisFieldDict::from_vec(dict_rem, modulo_);
    }
}

GaloisFieldDict GaloisFieldDict::gf_lshift(const integer_class n) const
{
    std::vector<integer_class> dict_out;
    auto to_ret = GaloisFieldDict::from_vec(dict_out, modulo_);
    if (!dict_.empty()) {
        unsigned n_val = mp_get_si(n);
        to_ret.dict_.resize(n_val, integer_class(0));
        to_ret.dict_.insert(to_ret.dict_.end(), dict_.begin(), dict_.end());
    }
    return to_ret;
}

void GaloisFieldDict::gf_rshift(const integer_class n,
                            const Ptr<GaloisFieldDict> &quo,
                            const Ptr<GaloisFieldDict> &rem) const
{
    std::vector<integer_class> dict_quo;
    *quo = GaloisFieldDict::from_vec(dict_quo, modulo_);
    unsigned n_val = mp_get_si(n);
    if (n_val < dict_.size()) {
        quo->dict_.insert(quo->dict_.end(), dict_.begin() + n_val, dict_.end());
        std::vector<integer_class> dict_rem(dict_.begin(), dict_.begin() + n_val);
        *rem = GaloisFieldDict::from_vec(dict_rem, modulo_);
    } else {
        *rem = static_cast<GaloisFieldDict>(*this);
    }
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
            to_ret *= to_sq;
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
    *monic = static_cast<GaloisFieldDict>(*this);
    if (dict_.empty()) {
        res = integer_class(0);
    } else {
        res = *dict_.rbegin();
        if (res != integer_class(1)) {
            integer_class inv, temp;
            mp_invert(inv, res, modulo_);
            for (auto &iter : monic->dict_) {
                temp = inv;
                temp *= iter;
                mp_fdiv_r(iter, temp, modulo_);
            }
        }
    }
}

GaloisFieldDict GaloisFieldDict::gf_gcd(const GaloisFieldDict &o) const
{
    SYMENGINE_ASSERT(modulo_ == o.modulo_);
    GaloisFieldDict f = static_cast<GaloisFieldDict>(*this);
    GaloisFieldDict g = o;
    GaloisFieldDict temp_out;
    while (not g.dict_.empty()) {
        f.gf_div(g, outArg(temp_out), outArg(f)); // f, g = f % g, g
        f.dict_.swap(g.dict_);
    }
    integer_class temp_LC;
    f.gf_monic(temp_LC, outArg(f));
    return f;
}

GaloisFieldDict GaloisFieldDict::gf_lcm(const GaloisFieldDict &o) const
{
    SYMENGINE_ASSERT(modulo_ == o.modulo_);
    if (dict_.empty())
        return static_cast<GaloisFieldDict>(*this);
    if (o.dict_.empty())
        return o;
    GaloisFieldDict out, temp_out;
    out = o * (*this);
    out /= gf_gcd(o);
    integer_class temp_LC;
    out.gf_monic(temp_LC, outArg(out));
    return out;
}
}
