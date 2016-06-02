#include <symengine/fields.h>
#include <symengine/functions.h>

namespace SymEngine
{

GaloisField::GaloisField(const map_uint_mpz &dict, const integer_class modulo)
    : modulo_(modulo)
{
    integer_class temp;
    for (auto iter : dict) {
        temp = iter.second % modulo_;
        if (temp != 0) {
            if (temp < 0)
                temp += modulo_;
            dict_[iter.first] = temp;
        }
    }
}

GaloisField::GaloisField(const std::vector<integer_class> &v,
                         const integer_class modulo)
    : modulo_(modulo)
{
    integer_class temp;
    for (unsigned i = 0; i < v.size(); i++) {
        temp = v[i] % modulo_;
        if (temp != 0) {
            if (temp < 0)
                temp += modulo_;
            dict_[i] = temp;
        }
    }
}

std::size_t GaloisField::__hash__() const
{
    std::size_t seed = GALOISFIELD;
    for (const auto &it : this->dict_) {
        std::size_t temp = GALOISFIELD;
        hash_combine<unsigned>(temp, it.first);
        hash_combine<long long int>(temp, mp_get_si(it.second));
        seed += temp;
    }
    hash_combine<long long int>(seed, mp_get_si(modulo_));
    return seed;
}

bool GaloisField::__eq__(const Basic &o) const
{
    if (is_a<GaloisField>(o)) {
        const GaloisField &s = static_cast<const GaloisField &>(o);
        return this->dict_ == s.dict_ and this->modulo_ == s.modulo_;
    }
    return false;
}

int GaloisField::compare(const Basic &o) const
{
    SYMENGINE_ASSERT(is_a<GaloisField>(o))
    // const GaloisField &s = static_cast<const GaloisField &>(o);
    return 0; // TODO
}

RCP<const GaloisField> GaloisField::gf_neg() const
{
    return gf_mul_ground(integer_class(-1));
}

RCP<const GaloisField> GaloisField::gf_add_ground(const integer_class a) const
{
    auto dict = dict_;
    dict[0] = dict[0] + a;
    return gf(dict, modulo_);
}

RCP<const GaloisField> GaloisField::gf_sub_ground(const integer_class a) const
{
    return gf_add_ground(-1 * a);
}

RCP<const GaloisField> GaloisField::gf_mul_ground(const integer_class a) const
{
    map_uint_mpz dict;
    for (auto iter : dict_) {
        dict[iter.first] = a * iter.second;
    }
    return gf(dict, modulo_);
}

RCP<const GaloisField> GaloisField::gf_quo_ground(const integer_class a) const
{
    map_uint_mpz dict;
    integer_class inv;
    mp_invert(inv, a, modulo_);
    for (auto iter : dict_) {
        dict[iter.first] = inv * iter.second;
    }
    return gf(dict, modulo_);
}

RCP<const GaloisField>
GaloisField::gf_add(const RCP<const GaloisField> &o) const
{
    SYMENGINE_ASSERT(modulo_ == o->modulo_);
    auto dict_out = dict_;
    for (auto iter : o->dict_) {
        dict_out[iter.first] = dict_out[iter.first] + iter.second;
    }
    return gf(dict_out, modulo_);
}

RCP<const GaloisField>
GaloisField::gf_sub(const RCP<const GaloisField> &o) const
{
    SYMENGINE_ASSERT(modulo_ == o->modulo_);
    auto dict_out = dict_;
    for (auto iter : o->dict_) {
        dict_out[iter.first] = dict_out[iter.first] - iter.second;
    }
    return gf(dict_out, modulo_);
}

RCP<const GaloisField>
GaloisField::gf_mul(const RCP<const GaloisField> &o) const
{
    SYMENGINE_ASSERT(modulo_ == o->modulo_);
    map_uint_mpz dict_out;
    for (auto iter : o->dict_) {
        for (auto it : dict_) {
            dict_out[iter.first + it.first]
                = (dict_out[iter.first + it.first] + iter.second * it.second)
                  % modulo_;
        }
    }
    return gf(dict_out, modulo_);
}

void GaloisField::gf_div(const RCP<const GaloisField> &o,
                         const Ptr<RCP<const GaloisField>> &quo,
                         const Ptr<RCP<const GaloisField>> &rem) const
{
    SYMENGINE_ASSERT(modulo_ == o->modulo_);
    if (o->dict_.empty())
        throw std::runtime_error("ZeroDivisionError");
    map_uint_mpz dict_divisor = o->dict_;
    map_uint_mpz dict_out;
    long deg_dividend = (--dict_.end())->first;
    long deg_divisor = (--dict_divisor.end())->first;
    if (deg_dividend < deg_divisor) {
        *quo = gf(dict_out, modulo_);
        *rem = gf(dict_, modulo_);
        return;
    }
    dict_out = this->dict_;
    integer_class inv;
    mp_invert(inv, (--dict_divisor.end())->second, modulo_);
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
    *quo = gf(dict_quo, modulo_);
    *rem = gf(dict_rem, modulo_);
}

RCP<const GaloisField>
GaloisField::gf_quo(const RCP<const GaloisField> &o) const
{
    SYMENGINE_ASSERT(modulo_ == o->modulo_);
    if (o->dict_.empty())
        throw std::runtime_error("ZeroDivisionError");
    map_uint_mpz dict_divisor = o->dict_;
    map_uint_mpz dict_out;
    long deg_dividend = (--dict_.end())->first;
    long deg_divisor = (--dict_divisor.end())->first;
    if (deg_dividend < deg_divisor) {
        return gf(dict_out, modulo_);
    }
    dict_out = this->dict_;
    integer_class inv;
    mp_invert(inv, (--dict_divisor.end())->second, modulo_);
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
    return gf(dict_quo, modulo_);
}

RCP<const GaloisField> GaloisField::gf_lshift(const integer_class n) const
{
    map_uint_mpz dict_out;
    for (auto iter : dict_) {
        dict_out[mp_get_si(iter.first + n)] = iter.second;
    }
    return gf(dict_out, modulo_);
}

void GaloisField::gf_rshift(const integer_class n,
                            const Ptr<RCP<const GaloisField>> &quo,
                            const Ptr<RCP<const GaloisField>> &rem) const
{
    map_uint_mpz dict_quo, dict_rem;
    for (auto iter : dict_) {
        integer_class a = iter.first - n;
        if (a >= 0)
            dict_quo[mp_get_si(a)] = iter.second;
        else
            dict_rem[iter.first] = iter.second;
    }
    *quo = gf(dict_quo, modulo_);
    *rem = gf(dict_rem, modulo_);
}
}
