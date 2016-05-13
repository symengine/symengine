#include <symengine/univariate_int_generic.h>
#include <symengine/symbol.h>
#include <symengine/add.h>
#include <symengine/constants.h>
#include <symengine/mul.h>
#include <symengine/pow.h>

namespace SymEngine
{

UnivariateInt::UnivariateInt(const RCP<const Symbol> &var,
                             const unsigned int &degree, map_uint_mpz &&dict)
    : UnivariateIntBase(var, degree, std::move(dict))
{
    SYMENGINE_ASSERT(is_canonical(degree_, container_))
}

UnivariateInt::UnivariateInt(const RCP<const Symbol> &var,
                             const std::vector<integer_class> &v)
    : UnivariateIntBase(var, std::move(v))
{
    container_ = {};
    for (unsigned int i = 0; i < v.size(); i++) {
        if (v[i] != 0) {
            container_[i] = v[i];
        }
    }
    if (container_.empty())
        degree_ = 0;
    else
        degree_ = (--container_.end())->first;
}

bool UnivariateInt::is_canonical(const unsigned int &degree_,
                                 const map_uint_mpz &dict) const
{
    if (var_->get_name() == "")
        if (!(dict.empty() or (dict.size() == 1 and dict.begin()->first == 0)))
            return false;

    if (dict.size() != 0) {
        unsigned int actual_degree = (--dict.end())->first;
        if (actual_degree != degree_) {
            return false;
        }
    } else if (degree_ != 0)
        return false;

    // Check if dictionary contains terms with coeffienct 0
    for (auto iter : dict)
        if (iter.second == 0)
            return false;
    return true;
}

std::size_t UnivariateInt::__hash__() const
{
    std::hash<std::string> hash_string;
    std::size_t seed = UNIVARIATEINT;
    seed += hash_string(this->var_->get_name());
    for (const auto &it : this->container_) {
        std::size_t temp = UNIVARIATEINT;
        hash_combine<unsigned int>(temp, it.first);
        hash_combine<long long int>(temp, mp_get_si(it.second));
        seed += temp;
    }
    return seed;
}

int UnivariateInt::compare(const Basic &o) const
{
    const UnivariateInt &s = static_cast<const UnivariateInt &>(o);

    if (container_.size() != s.container_.size())
        return (container_.size() < s.container_.size()) ? -1 : 1;

    int cmp = var_->compare(*s.var_);
    if (cmp != 0)
        return cmp;

    return map_uint_mpz_compare(container_, s.container_);
}

vec_basic UnivariateInt::get_args() const
{
    vec_basic args;
    for (const auto &p : container_) {
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
    if (container_.empty())
        args.push_back(zero);
    return args;
}

bool UnivariateInt::is_zero() const
{
    return container_.empty();
}

bool UnivariateInt::is_one() const
{
    return container_.size() == 1 and container_.begin()->second == 1
           and container_.begin()->first == 0;
}

bool UnivariateInt::is_minus_one() const
{
    return container_.size() == 1 and container_.begin()->second == -1
           and container_.begin()->first == 0;
}

bool UnivariateInt::is_integer() const
{
    if (container_.empty())
        return true;
    if (container_.size() == 1 and container_.begin()->first == 0)
        return true;
    return false;
}

bool UnivariateInt::is_symbol() const
{
    if (container_.size() == 1 and container_.begin()->first == 1
        and container_.begin()->second == 1)
        return true;
    return false;
}

bool UnivariateInt::is_mul() const
{
    if (container_.size() == 1 and container_.begin()->first != 0
        and container_.begin()->second != 1 and container_.begin()->second != 0)
        return true;
    return false;
}

bool UnivariateInt::is_pow() const
{
    if (container_.size() == 1 and container_.begin()->second == 1
        and container_.begin()->first != 1 and container_.begin()->first != 0)
        return true;
    return false;
}

unsigned int UnivariateInt::get_length() const
{
    return container_.size();
}

integer_class UnivariateInt::eval(const integer_class &x) const
{
    unsigned int last_deg = container_.rbegin()->first;
    integer_class result(0), x_pow;

    for (auto it = container_.rbegin(); it != container_.rend(); ++it) {

        mp_pow_ui(x_pow, x, last_deg - (*it).first);
        last_deg = (*it).first;
        result = (*it).second + x_pow * result;
    }
    mp_pow_ui(x_pow, x, last_deg);
    result *= x_pow;

    return result;
}

integer_class UnivariateInt::eval_bit(const unsigned int &x) const
{
    unsigned int last_deg = container_.rbegin()->first;
    integer_class result(0);

    for (auto it = container_.rbegin(); it != container_.rend(); ++it) {

        result = (*it).second + (result << x * (last_deg - (*it).first));
        last_deg = (*it).first;
    }
    result = result << x * last_deg;

    return result;
}

} // SymEngine
