#include <symengine/series_flint.h>
#include <symengine/series_visitor.h>

#ifdef HAVE_SYMENGINE_FLINT

#include <csetjmp>
#include <csignal>
#include <cstdlib>
#include <iostream>

jmp_buf env;

void on_sigabrt (int signum)
{
    longjmp (env, 1);
    throw std::runtime_error("Flint abort caught in series_flint.cpp");
}

void try_and_catch_abort (std::function< void ()> func)
{
    if (setjmp (env) == 0) {
        signal(SIGABRT, &on_sigabrt);
        func();
    }
    else {
        std::cout << "aborted\n";
    }
}

namespace SymEngine {

URatPSeriesFlint::URatPSeriesFlint(fp_t p, const std::string varname, const unsigned degree)
        : SeriesBase(std::move(p), varname, degree) {

}
RCP<const URatPSeriesFlint> URatPSeriesFlint::series(const RCP<const Basic> &t, const std::string &x, unsigned int prec) {
    fp_t p("2  0 1");
    SeriesVisitor<fp_t, flint::fmpqxx, URatPSeriesFlint> visitor(p, x, prec);
    RCP<const URatPSeriesFlint> ret;

    auto vseries = [&]() -> void
    {
        ret = visitor.series(t);
    };

    try_and_catch_abort(vseries);
    return ret;
}

std::size_t URatPSeriesFlint::__hash__() const {
    std::hash<std::string> str_hash;
    std::size_t seed = URATPSERIESFLINT;
    hash_combine(seed, var_);
    hash_combine(seed, degree_);
    hash_combine(seed, str_hash(p_.to_string()));
    return seed;
}

RCP<const Basic> URatPSeriesFlint::as_basic() const
{
    RCP<const Symbol> x = symbol(var_);
    RCP<const Number> zcoef;
    umap_basic_num dict_;
    mpq_class gc;
    for (int n=0; n<degree_; n++) {
        const flint::fmpqxx fc(p_.get_coeff(n));
        if (not fc.is_zero()) {
            fmpq_get_mpq(gc.get_mpq_t(), fc._data().inner);
            gc.canonicalize();
            RCP<const Number> basic;
            if (gc.get_den() == 1)
                basic = integer(gc.get_num());
            else
                basic = Rational::from_mpq(gc);
            auto term = SymEngine::mul(SymEngine::pow(x, SymEngine::integer(n)), basic);
            if (n==0)
                zcoef = basic;
            Add::coef_dict_add_term(outArg(basic), dict_, one, term);
        }
        else
            if (n==0)
                zcoef = integer(0);
    }
    return std::move(Add::from_dict(zcoef, std::move(dict_)));
}

umap_int_basic URatPSeriesFlint::as_dict() const
{
    umap_int_basic map;
    mpq_class gc;
    for (int n=0; n<degree_; n++) {
        const flint::fmpqxx fc(p_.get_coeff(n));
        if (not fc.is_zero()) {
            fmpq_get_mpq(gc.get_mpq_t(), fc._data().inner);
            gc.canonicalize();
            RCP<const Number> basic;
            if (gc.get_den() == 1)
                basic = integer(gc.get_num());
            else
                basic = Rational::from_mpq(gc);
            map[n] = basic;
        }
    }
    return map;
}

RCP<const Basic> URatPSeriesFlint::get_coeff(int n) const
{
    const flint::fmpqxx fc(p_.get_coeff(n));
    mpq_class gc;
    fmpq_get_mpq(gc.get_mpq_t(), fc._data().inner);
    gc.canonicalize();
    if (gc.get_den() == 1)
        return integer(gc.get_num());
    else
        return Rational::from_mpq(gc);
}

int URatPSeriesFlint::compare(const Basic &o) const {
    SYMENGINE_ASSERT(is_a<URatPSeriesFlint>(o))
    const URatPSeriesFlint &s = static_cast<const URatPSeriesFlint &>(o);
    if (var_ != s.var_)
        return (var_ < s.var_) ? -1 : 1;
    if (degree_ != s.degree_)
        return (degree_ < s.degree_) ? -1 : 1;
    if (p_ == s.p_)
        return 0;
    return (p_ < s.p_) ? -1 : 1;
}

flint::fmpzxx URatPSeriesFlint::convert(const Integer &x) {
    flint::fmpzxx r;
    fmpz_set_mpz(r._data().inner, x.as_mpz().get_mpz_t());
    return r;
}

flint::fmpqxx URatPSeriesFlint::convert(const mpq_class &x) {
    flint::fmpqxx r;
    flint::fmpzxx i1;
    fmpz_set_mpz(i1._data().inner, x.get_num_mpz_t());
    flint::fmpzxx i2;
    fmpz_set_mpz(i2._data().inner, x.get_den_mpz_t());
    r.num() = i1;
    r.den() = i2;
    return r;
}

fp_t URatPSeriesFlint::var(const std::string &s) {
    fp_t r("2  0 1");
    return r;
}

flint::fmpqxx URatPSeriesFlint::convert(const Rational &x) {
    return convert(x.as_mpq());
}

flint::fmpqxx URatPSeriesFlint::convert(const Number &x) {
    throw std::runtime_error("Not Implemented");
}

fp_t URatPSeriesFlint::pow(const fp_t &s, int n, unsigned prec) {
    if (n > 0)
        return fp_t(s.pow(unsigned(n)));
    else if (n < 0)
        return fp_t(s.inv_series(prec).pow(unsigned(-n)));
    return fp_t(1);
}

unsigned URatPSeriesFlint::ldegree(const fp_t &s) {
    long i = 0;
    while (i <= s.degree())
        if (not s.get_coeff(i++).is_zero())
            return i-1;
    return 0;
}

flint::fmpqxx URatPSeriesFlint::root(flint::fmpqxx &c, unsigned n) {
    flint::fmpqxx cl_rat = c, cl_root;
    cl_rat.canonicalise();
    cl_root.num() = cl_rat.num().root(n);
    if (cl_rat.den() == 1)
        cl_root.den() = 1;
    else
        cl_root.den() = cl_rat.den().root(n);
    return cl_root;
}

fp_t URatPSeriesFlint::diff(const fp_t &s, const fp_t &var) {
    return fp_t(s.derivative());
}

fp_t URatPSeriesFlint::integrate(const fp_t &s, const fp_t &var) {
    return fp_t(s.integral());
}

fp_t URatPSeriesFlint::subs(const fp_t &s, const fp_t &var, const fp_t &r, unsigned prec) {
    return fp_t(s(r));
}

}
#endif // HAVE_SYMENGINE_FLINT

