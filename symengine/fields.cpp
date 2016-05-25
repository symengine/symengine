#include <symengine/fields.h>

namespace SymEngine
{

GaloisField::GaloisField(std::map<unsigned, int> &dict, unsigned modulo)
    : modulo_(modulo)
{
    unsigned temp;
    for (auto iter : dict) {
        temp = mod(iter.second, modulo_);
        if (temp != 0)
            dict_[iter.first] = temp;
    }
}

GaloisField::GaloisField(std::vector<int> &dict, unsigned modulo)
    : modulo_(modulo)
{
    unsigned temp;
    for (unsigned i = 0; i < dict.size(); i++) {
        temp = mod(dict[i], modulo_);
        if (temp != 0)
            dict_[i] = temp;
    }
}

std::size_t GaloisField::__hash__() const
{
    std::size_t seed = GALOISFIELD;
    for (const auto &it : this->dict_) {
        std::size_t temp = GALOISFIELD;
        hash_combine<unsigned int>(temp, it.first);
        hash_combine<int>(temp, it.second);
        seed += temp;
    }
    return seed;
}

bool GaloisField::__eq__(const Basic &o) const
{
    if (is_a<GaloisField>(o)) {
        const GaloisField &s = static_cast<const GaloisField &>(o);
        return this->dict_ == s.dict_;
    }
    return false;
}

int GaloisField::compare(const Basic &o) const
{
    SYMENGINE_ASSERT(is_a<GaloisField>(o))
    // const GaloisField &s = static_cast<const GaloisField &>(o);
    return 0; // TODO
}

int GaloisField::mod(int a, int b) const
{
    if (a >= 0)
        return a % b;
    else
        return (a / b) * b + b + a;
}

RCP<const GaloisField> GaloisField::gf_neg() const
{
    return gf_mul_ground(-1);
}

RCP<const GaloisField> GaloisField::gf_add_ground(int a) const
{
    auto dict = dict_;
    dict[0] = dict[0] + a;
    return gf(dict, modulo_);
}

RCP<const GaloisField> GaloisField::gf_sub_ground(int a) const
{
    return gf_add_ground(-1 * a);
}

RCP<const GaloisField> GaloisField::gf_mul_ground(int a) const
{
    std::map<unsigned, int> dict;
    for (auto iter : dict_) {
        dict[iter.first] = a * dict[iter.second];
    }
    return gf(dict, modulo_);
}
// TODO invert and gf_quo_ground and gcdex
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
    std::map<unsigned, int> dict_out;
    for (auto iter : o->dict_) {
        for (auto it : dict_) {
            dict_out[iter.first + it.first]
                = mod(dict_out[iter.first + it.first] + iter.second * it.second,
                      modulo_);
        }
    }
    return gf(dict_out, modulo_);
}
}
