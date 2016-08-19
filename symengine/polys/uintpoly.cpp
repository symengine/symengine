#include <symengine/polys/uintpoly.h>
#include <symengine/fields.h>
#include <symengine/ntheory.h>
#include <numeric>

namespace SymEngine
{
void UIntDict::itrunc(const integer_class &mod)
{
    integer_class mod_2 = mod / 2;
    for (auto it = dict_.begin(); it != dict_.end();) {
        mp_fdiv_r(it->second, it->second, mod);
        if (it->second > mod_2)
            it->second -= mod;
        if (it->second == 0_z) {
            it = dict_.erase(it);
        } else {
            ++it;
        }
    }
}

integer_class UIntDict::l1_norm() const
{
    integer_class out(0_z);
    for (auto &a : dict_) {
        out += mp_abs(a.second);
    }
    return out;
}

integer_class UIntDict::primitive(const Ptr<UIntDict> &res) const
{
    integer_class gcd(0_z);
    *res = dict_;
    for (auto &a : dict_) {
        mp_gcd(gcd, gcd, a.second);
    }
    for (auto &a : (*res).dict_) {
        a.second /= gcd;
    }
    return gcd;
}

void UIntDict::zz_divide(const UIntDict &a, const UIntDict &b,
                         const Ptr<UIntDict> &quo, const Ptr<UIntDict> &rem)
{
    auto b_poly = b;
    if (a.size() == 0)
        throw std::runtime_error("Zero Division Error");
    map_uint_mpz res;
    UIntDict tmp;
    integer_class q, r;
    unsigned int a_deg, b_deg;
    while (b_poly.degree() >= a.degree()) {
        a_deg = a.degree();
        b_deg = b_poly.degree();
        q = b_poly.get_lc() / a.get_lc();
        res[b_deg - a_deg] = q;
        tmp = UIntDict(map_uint_mpz{{b_deg - a_deg, q}});
        b_poly -= (a * tmp);
    }
    *quo = UIntDict(res);
    *rem = b;
    *rem -= (a * (*quo));
}

std::set<RCP<const UIntPoly>, RCPBasicKeyLess> UIntPoly::zz_zassenhaus() const
{
    std::set<RCP<const UIntPoly>, RCPBasicKeyLess> factors;
    unsigned int n = get_degree();
    UIntDict f(this->poly_);
    if (n == 1) {
        factors.insert(rcp_from_this_cast<const UIntPoly>());
        return factors;
    }
    integer_class fc(this->poly_.dict_.begin()->second);
    auto A = mp_get_ui(this->poly_.max_abs_coef());
    auto b = get_lc();
    auto B
        = std::abs(int(std::sqrt(n + 1)) * std::pow(2, n) * A * mp_get_si(b));
    auto C = std::pow(n + 1, 2 * n) * std::pow(A, 2 * n - 1);
    auto gamma = std::ceil(2 * std::log2(C));
    unsigned int bound = 2 * gamma * std::log(gamma);
    std::pair<integer_class,
              std::set<GaloisFieldDict, GaloisFieldDict::DictLess>> fsqf;
    size_t counter = 0;
    Sieve::iterator pi(bound);
    pi.next_prime(); // To avoid 2
    unsigned i;
    while ((i = pi.next_prime()) <= bound) {
        if (b % i == 0)
            continue;
        integer_class i_c = integer_class(i);
        auto F = GaloisFieldDict(this->poly_.dict_, i_c);
        if (not F.gf_is_sqf())
            continue;
        F.gf_monic(outArg(F));
        auto fsqfx = F.gf_zassenhaus();
        if (fsqfx.size() < fsqf.second.size() or fsqf.second.empty()) {
            fsqf.first = i_c;
            fsqf.second = fsqfx;
            ++counter;
        }
        if (fsqfx.size() < 15 or counter > 4)
            break;
    }
    auto l = std::ceil(std::log(2 * B + 1) / std::log(mp_get_ui(fsqf.first)));

    integer_class pl;
    mp_pow_ui(pl, fsqf.first, l);
    GaloisFieldDict gf(this->poly_.dict_, pl);
    std::vector<UIntDict> g
        = GaloisFieldDict::zz_hensel_lift(gf, fsqf.first, fsqf.second, l);

    std::vector<unsigned int> T(g.size());
    std::iota(std::begin(T), std::end(T), 0);
    unsigned int s = 1;

    auto _subsets = [](std::vector<unsigned int> in_set, unsigned int size) {
        std::vector<std::vector<unsigned int>> out;
        std::string bitmask(size, 1);
        bitmask.resize(in_set.size(), 0);
        do {
            std::vector<unsigned int> temp;
            for (unsigned int i = 0; i < in_set.size(); ++i) {
                if (bitmask[i])
                    temp.push_back(in_set[i]);
            }
            if (not temp.empty())
                out.push_back(temp);
        } while (std::prev_permutation(bitmask.begin(), bitmask.end()));
        return out;
    };

    auto _test_pl = [](const integer_class &fc, const integer_class &q,
                       const integer_class &pl) {
        integer_class q_copy(q);
        if (q_copy > pl / 2_z)
            q_copy -= pl;
        if (q_copy == 0_z)
            return true;
        return (fc % q_copy == 0_z);
    };

    while (2 * s <= T.size()) {
        std::vector<std::vector<unsigned int>> subsets = _subsets(T, s);
        auto it = std::begin(subsets);
        for (; it != std::end(subsets); ++it) {
            auto S = *it;
            UIntDict G;

            if (b == 1_z) {
                integer_class q = 1_z;
                for (auto i : S) {
                    q *= g[i].dict_.begin()->second;
                }
                q %= pl;
                if (not _test_pl(fc, q, pl))
                    continue;
            } else {
                G = UIntDict(b);
                for (auto &i : S) {
                    G *= g[i];
                }
                G.itrunc(pl);
                G.primitive(outArg(G));
                auto q = G.dict_.begin()->second;
                if (q != 0 and fc % q != 0)
                    continue;
            }
            UIntDict H = UIntDict(b);
            std::vector<unsigned int> T_S(T.size());
            auto it = std::set_difference(T.begin(), T.end(), S.begin(),
                                          S.end(), T_S.begin());
            T_S.resize(it - T_S.begin());
            if (b == 1_z) {
                G = UIntDict(b);
                for (auto &i : S) {
                    G = G * g[i];
                }
                G.itrunc(pl);
            }
            for (auto &i : T_S) {
                H *= g[i];
            }
            H.itrunc(pl);
            integer_class G_norm = G.l1_norm();
            integer_class H_norm = H.l1_norm();
            if (double(mp_get_si(G_norm) * mp_get_si(H_norm)) <= B) {
                T = T_S;
                G.primitive(outArg(G));
                H.primitive(outArg(f));
                factors.insert(
                    UIntPoly::from_dict(get_var(), std::move(G.dict_)));
                b = f.dict_.rbegin()->second;
                break;
            }
        }
        if (it == std::end(subsets))
            s += 1;
    }
    if (not f.empty()) {
        factors.insert(UIntPoly::from_dict(get_var(), std::move(f.dict_)));
    }
    return factors;
}

std::pair<integer_class, std::set<RCP<const UIntPoly>, RCPBasicKeyLess>>
UIntPoly::zz_factor_sqf() const
{
    std::pair<integer_class, std::set<RCP<const UIntPoly>, RCPBasicKeyLess>>
        out;
    auto n = this->get_degree();
    UIntDict this_dict = this->poly_;
    integer_class cont = this_dict.primitive(outArg(this_dict));
    if (this_dict.get_lc() < 0_z) {
        cont *= -1;
        this_dict *= -1;
    }
    out.first = cont;
    if (n == 0) {
        return out;
    }
    auto g = UIntPoly::from_dict(get_var(), std::move(this_dict.dict_));
    if (n == 1) {
        out.second.insert(g);
        return out;
    }
    out.second = g->zz_zassenhaus();
    return out;
}

UIntPoly::UIntPoly(const RCP<const Basic> &var, UIntDict &&dict)
    : USymEnginePoly(var, std::move(dict))
{
    SYMENGINE_ASSERT(is_canonical(poly_))
}

hash_t UIntPoly::__hash__() const
{
    hash_t seed = UINTPOLY;

    seed += var_->hash();
    for (const auto &it : poly_.dict_) {
        hash_t temp = UINTPOLY;
        hash_combine<unsigned int>(temp, it.first);
        hash_combine<long long int>(temp, mp_get_si(it.second));
        seed += temp;
    }
    return seed;
}

bool divides_upoly(const UIntPoly &a, const UIntPoly &b,
                   const Ptr<RCP<const UIntPoly>> &out)
{
    if (!(a.get_var()->__eq__(*b.get_var())))
        throw SymEngineException("Error: variables must agree.");

    auto a_poly = a.get_poly();
    auto b_poly = b.get_poly();
    if (a_poly.size() == 0)
        return false;

    map_uint_mpz res;
    UIntDict tmp;
    integer_class q, r;
    unsigned int a_deg, b_deg;

    while (b_poly.size() >= a_poly.size()) {
        a_deg = a_poly.degree();
        b_deg = b_poly.degree();

        mp_tdiv_qr(q, r, b_poly.get_lc(), a_poly.get_lc());
        if (r != 0)
            return false;

        res[b_deg - a_deg] = q;
        UIntDict tmp = UIntDict({{b_deg - a_deg, q}});
        b_poly -= (a_poly * tmp);
    }

    if (b_poly.empty()) {
        *out = UIntPoly::from_dict(a.get_var(), std::move(res));
        return true;
    } else {
        return false;
    }
}

} // SymEngine
