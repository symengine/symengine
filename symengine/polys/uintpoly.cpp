#include <symengine/polys/uintpoly.h>
#include <symengine/fields.h>

namespace SymEngine
{
void UIntDict::itrunc(const integer_class &mod)
{
    for (auto it = dict_.begin(); it != dict_.end();) {
        mp_fdiv_r(it->second, it->second, mod);
        if (it->second > mod / 2)
            it->second -= mod;
        if (it->second == 0_z) {
            it = dict_.erase(it);
        } else {
            ++it;
        }
    }
}

void UIntDict::zz_hensel_step(const integer_class &m, const UIntDict &f,
                              const UIntDict &g, const UIntDict &h,
                              const UIntDict &s, const UIntDict &t,
                              const Ptr<UIntDict> &G, const Ptr<UIntDict> &H,
                              const Ptr<UIntDict> &S, const Ptr<UIntDict> &T)
{
    UIntDict q, r;
    integer_class M;
    mp_pow_ui(M, m, 2);

    auto e = f - g * h;
    e.itrunc(M);

    zz_divide(h, s * e, outArg(q), outArg(r));

    q.itrunc(M);
    r.itrunc(M);

    UIntDict u = t * e + q * g;
    *G = (g + u);
    (*G).itrunc(M);
    *H = (h + r);
    (*H).itrunc(M);

    u = s * (*G) + t * (*H);
    auto b = (u - 1);
    b.itrunc(M);

    zz_divide(*H, s * b, outArg(q), outArg(r));
    q.itrunc(M);
    r.itrunc(M);

    u = t * b + q * (*G);
    *S = (s - r);
    (*S).itrunc(M);
    *T = (t - u);
    (*T).itrunc(M);
}

integer_class UIntDict::l1_norm() const
{
    integer_class out(0_z);
    for (auto &a : dict_) {
        out += mp_abs(a.second);
    }
    return out;
}

UIntDict UIntDict::primitive() const
{
    integer_class gcd(0_z);
    UIntDict out(dict_);
    for (auto &a : dict_) {
        mp_gcd(gcd, gcd, mp_abs(a.second));
    }
    for (auto &a : out.dict_) {
        a.second /= gcd;
    }
    return out;
}

std::vector<UIntDict>
UIntDict::zz_hensel_lift(const integer_class &p,
                         const std::vector<UIntDict> &f_list,
                         unsigned int l) const
{
    std::vector<UIntDict> res;
    size_t r = f_list.size();
    auto lc = get_lc();

    if (r == 1) {
        integer_class g, r, s, pl;
        mp_pow_ui(pl, p, l);
        mp_gcdext(g, r, s, lc, pl);
        UIntDict F((*this) * r);
        F.itrunc(pl);
        res.push_back(F);
        return res;
    }

    std::vector<UIntDict> sub1, sub2;
    integer_class m(p);
    size_t k(r / 2);
    auto d = std::ceil(std::log2(l)); // TODO
    auto g = GaloisFieldDict::from_vec({lc}, p);

    for (unsigned i = 0; i < k; ++i) {
        g *= GaloisFieldDict(f_list[i].dict_, p);
        sub1.push_back(f_list[i]);
    }

    sub2.push_back(f_list[k]);
    auto h = GaloisFieldDict(f_list[k].dict_, p);
    for (unsigned i = k + 1; i < f_list.size(); ++i) {
        h *= GaloisFieldDict(f_list[i].dict_, p);
        sub2.push_back(f_list[i]);
    }

    GaloisFieldDict temp2, s, t;
    GaloisFieldDict::gf_gcdex(g, h, outArg(s), outArg(t), outArg(temp2));

    UIntDict dg = UIntDict::from_vec(g.dict_);
    dg.itrunc(p);
    UIntDict dh = UIntDict::from_vec(h.dict_);
    dh.itrunc(p);
    UIntDict ds = UIntDict::from_vec(s.dict_);
    ds.itrunc(p);
    UIntDict dt = UIntDict::from_vec(t.dict_);
    dt.itrunc(p);

    for (unsigned int i = 1; i <= d; ++i) {
        UIntDict::zz_hensel_step(m, (*this), dg, dh, ds, dt, outArg(dg),
                                 outArg(dh), outArg(ds), outArg(dt));
        mp_pow_ui(m, m, 2);
    }

    res = dg.zz_hensel_lift(p, sub1, l);
    auto temp = dh.zz_hensel_lift(p, sub2, l);
    res.insert(res.end(), temp.begin(), temp.end());
    return res;
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
    auto bound = integer_class(int(2 * gamma * std::log(gamma)));
    std::vector<std::pair<integer_class,
                          std::set<GaloisFieldDict, GaloisFieldDict::DictLess>>>
        a;
    for (integer_class i = 3_z; i <= bound; mp_nextprime(i, i)) {
        if (b % i == 0)
            continue;
        auto F = GaloisFieldDict(this->poly_.dict_, i);
        if (not F.gf_is_sqf())
            continue;
        integer_class temp;
        F.gf_monic(temp, outArg(F));
        auto fsqfx = F.gf_zassenhaus();
        a.push_back({i, fsqfx});
        if (fsqfx.size() < 15 or a.size() > 4)
            break;
    }
    auto fsqf = *(std::min_element(
        a.begin(), a.end(),
        [](const std::pair<integer_class,
                           std::set<GaloisFieldDict, GaloisFieldDict::DictLess>>
               &lhs,
           const std::pair<integer_class,
                           std::set<GaloisFieldDict, GaloisFieldDict::DictLess>>
               &rhs) { return lhs.second.size() < rhs.second.size(); }));
    auto l = std::ceil(std::log(2 * B + 1) / std::log(mp_get_ui(fsqf.first)));
    std::vector<UIntDict> modular;
    for (auto &f : fsqf.second) {
        UIntDict temp = UIntDict::from_vec(f.dict_);
        temp.itrunc(fsqf.first);
        modular.push_back(temp);
    }
    std::vector<UIntDict> g
        = this->get_poly().zz_hensel_lift(fsqf.first, modular, l);
    std::vector<unsigned int> T(g.size());
    std::iota(std::begin(T), std::end(T), 0);
    unsigned int s = 1;
    integer_class pl;
    mp_pow_ui(pl, fsqf.first, l);

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
                G = G.primitive();
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
                G = G.primitive();
                f = H.primitive();
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
