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
        if (q == 0_z)
            break;
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
    UIntDict f(get_poly());
    if (n == 1) {
        factors.insert(rcp_from_this_cast<const UIntPoly>());
        return factors;
    }
    integer_class fc(get_poly().dict_.begin()->second);
    auto A = get_poly().max_abs_coef();
    auto b = get_lc();
    integer_class temp;
    temp = 1 << n;
    integer_class B = integer_class(int(std::sqrt(n + 1))) * temp * A * b;
    B = mp_abs(B);

    auto log2_d_A = std::log2(mp_get_d(A));
    unsigned int gamma
        = std::ceil(2 * (2 * n * std::log2(n + 1) + (2 * n - 1) * log2_d_A));
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
        auto F = GaloisFieldDict(get_poly().dict_, i_c);
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
    unsigned l;
    if (mp_fits_slong_p(B)) {
        temp = 2 * B + 1;
        l = std::ceil(std::log(mp_get_d(temp))
                      / std::log(mp_get_ui(fsqf.first)));
    } else {
        auto b_d = mp_get_d(mp_abs(b));
        l = std::ceil(
            (1 + std::log2(n + 1) / 2.0 + n + log2_d_A + std::log2(b_d))
            / (std::log2(mp_get_ui(fsqf.first))));
    }
    integer_class pl;
    mp_pow_ui(pl, fsqf.first, l);
    GaloisFieldDict gf(get_poly().dict_, pl);
    std::vector<UIntDict> g
        = GaloisFieldDict::zz_hensel_lift(gf, fsqf.first, fsqf.second, l);

    std::vector<unsigned int> T(g.size());
    std::iota(std::begin(T), std::end(T), 0);
    unsigned int s = 1;

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
        bool broken = false;
        std::string bitmask(s, 1);
        bitmask.resize(T.size(), 0);
        do {
            std::vector<unsigned int> S;
            for (unsigned int i = 0; i < T.size(); ++i) {
                if (bitmask[i]) {
                    S.push_back(T[i]);
                }
            }
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
            std::vector<unsigned int> T_S;
            for (unsigned int i = 0; i < T.size(); ++i) {
                if (not bitmask[i]) {
                    T_S.push_back(T[i]);
                }
            }
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
            if (G_norm * H_norm <= B) {
                T = T_S;
                G.primitive(outArg(G));
                H.primitive(outArg(f));
                factors.insert(
                    UIntPoly::from_dict(get_var(), std::move(G.dict_)));
                b = f.dict_.rbegin()->second;
                broken = true;
                break;
            }
        } while (std::prev_permutation(bitmask.begin(), bitmask.end()));
        if (not std::next_permutation(bitmask.begin(), bitmask.end())) {
            if (not broken)
                s += 1;
        }
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
    UIntDict this_dict = get_poly();
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
    SYMENGINE_ASSIGN_TYPEID()
    SYMENGINE_ASSERT(is_canonical(get_poly()))
}

hash_t UIntPoly::__hash__() const
{
    hash_t seed = UINTPOLY;

    seed += get_var()->hash();
    for (const auto &it : get_poly().dict_) {
        hash_t temp = UINTPOLY;
        hash_combine<unsigned int>(temp, it.first);
        hash_combine<long long int>(temp, mp_get_si(it.second));
        seed += temp;
    }
    return seed;
}

} // SymEngine
