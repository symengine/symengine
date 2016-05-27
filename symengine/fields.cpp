#include <symengine/fields.h>

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
    return gf_mul_ground(-1);
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
}
