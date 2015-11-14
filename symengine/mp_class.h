#ifndef SYMENGINE_INTEGER_CLASS_H
#define SYMENGINE_INTEGER_CLASS_H

#if SYMENGINE_INTEGER_CLASS == SYMENGINE_PIRANHA
#include <piranha/mp_integer.hpp>
#include <piranha/mp_rational.hpp>

#elif SYMENGINE_INTEGER_CLASS == SYMENGINE_FLINT
#include <flint/fmpzxx.h>
#include <flint/fmpqxx.h>
#endif

#define __GMPXX_USE_CXX11 1
#include <gmpxx.h>

namespace SymEngine {

#if SYMENGINE_INTEGER_CLASS == SYMENGINE_PIRANHA
typedef piranha::integer integer_class;
typedef piranha::rational rational_class;
#elif SYMENGINE_INTEGER_CLASS == SYMENGINE_FLINT
typedef flint::fmpzxx integer_class;
typedef flint::fmpqxx rational_class;
#else
typedef mpz_class integer_class;
typedef mpq_class rational_class;
#endif

inline namespace literals
{
    inline integer_class operator "" _z(const char* str)
    {
        return integer_class(str);
    }
}

// Helper functions for mpz_class
inline double get_d(const mpz_class &i) {
    return i.get_d();
}

inline bool fits_ulong_p(const mpz_class &i) {
    return i.fits_ulong_p();
}

inline bool fits_slong_p(const mpz_class &i) {
    return i.fits_slong_p();
}

inline unsigned long get_ui(const mpz_class &i) {
    return i.get_ui();
}

inline long get_si(const mpz_class &i) {
    return i.get_si();
}

inline int sign(const mpz_class &i) {
    return sgn(i);
}

inline mpz_srcptr get_mpz_t(const mpz_class &i) {
    return i.get_mpz_t();
}

inline mpz_ptr get_mpz_t(mpz_class &i) {
    return i.get_mpz_t();
}

// Helper functions for mpq_class
inline const mpz_class& get_den(const mpq_class &i) {
    return i.get_den();
}

inline const mpz_class& get_num(const mpq_class &i) {
    return i.get_num();
}

inline mpq_srcptr get_mpq_t(const mpq_class &i) {
    return i.get_mpq_t();
}

inline void canonicalize(mpq_class &i) {
    i.canonicalize();
}

inline double get_d(const mpq_class &i) {
    return i.get_d();
}

inline int sign(const mpq_class &i) {
    return sgn(i);
}

#if SYMENGINE_INTEGER_CLASS == SYMENGINE_PIRANHA
// Helper functions for piranha::integer
inline piranha::integer abs(const piranha::integer &i) {
    return i.abs();
}

inline piranha::integer sqrt(const piranha::integer &i) {
    return i.sqrt();
}

inline mpz_ptr get_mpz_t(piranha::integer &i) {
    return i.get_mpz_ptr();
}

inline auto get_mpz_t(const piranha::integer &i) -> decltype(i.get_mpz_view())  {
    return i.get_mpz_view();
}

inline int sign(const piranha::integer &i) {
    return i.sign();
}

inline long get_si(const piranha::integer &i) {
    return mpz_get_si(i.get_mpz_view());
}

inline unsigned long get_ui(const piranha::integer &i) {
    return mpz_get_ui(i.get_mpz_view());
}

inline double get_d(const piranha::integer &i) {
    return mpz_get_d(i.get_mpz_view());
}

inline bool fits_ulong_p(const piranha::integer &i) {
    return mpz_fits_ulong_p(i.get_mpz_view()) != 0;
}

inline bool fits_slong_p(const piranha::integer &i) {
    return mpz_fits_slong_p(i.get_mpz_view()) != 0;
}

// Helper functions for piranha::rational

inline piranha::rational abs(const piranha::rational &i) {
    return i.abs();
}

inline const piranha::integer& get_den(const piranha::rational &i) {
    return i.den();
}

inline const piranha::integer& get_num(const piranha::rational &i) {
    return i.num();
}

inline void canonicalize(piranha::rational &i) {
    i.canonicalise();
}

inline double get_d(const piranha::rational &i) {
    return mpq_get_d(i.get_mpq_view().get());
}

inline auto get_mpq_t(const piranha::rational &i) -> decltype(i.get_mpq_view()) {
    return i.get_mpq_view();
}

inline int sign(const piranha::rational &i) {
    return i.num().sign();
}
#elif SYMENGINE_INTEGER_CLASS == SYMENGINE_FLINT

class mpz_view_flint {

public:
    mpz_view_flint(const flint::fmpzxx &i) {
        if (!COEFF_IS_MPZ(*i._fmpz()))
        {
            mpz_init_set_si(m, *i._fmpz());
        }
        else
        {
            ptr = COEFF_TO_PTR(*i._fmpz());
        }
    }
    operator mpz_srcptr() const
	{
        if (ptr == nullptr) return m;
        return ptr;
	}
    ~mpz_view_flint() {
        if (ptr == nullptr) mpz_clear(m);
    }

private:
    mpz_srcptr ptr = nullptr;
    mpz_t m;
};

inline mpz_view_flint get_mpz_t(const flint::fmpzxx &i) {
    return mpz_view_flint(i);
}

inline mpz_ptr get_mpz_t(flint::fmpzxx &i) {
    return _fmpz_promote(i._fmpz());
}

inline int sign(const flint::fmpzxx &i) {
    return i.sgn();
}

inline long get_si(const flint::fmpzxx &i) {
    return i.to<long>();
}

inline unsigned long get_ui(const flint::fmpzxx &i) {
    return i.to<unsigned long>();
}


inline bool fits_slong_p(const flint::fmpzxx &i) {
    return i.fits_si();
}


inline bool fits_ulong_p(const flint::fmpzxx &i) {
    return i.abs_fits_ui();
}


inline double get_d(const flint::fmpzxx &i) {
    return i.to<double>();
}


inline const flint::fmpzxx_srcref get_den(const flint::fmpqxx &i) {
    return i.den();
}

inline const flint::fmpzxx_srcref get_num(const flint::fmpqxx &i) {
    return i.num();
}


inline mpq_srcptr get_mpq_t(const flint::fmpqxx &i) {
    return nullptr;
}

inline void canonicalize(flint::fmpqxx &i) {
    i.canonicalise();
}

inline double get_d(const flint::fmpqxx &i) {
    return i.to<double>();
}

inline int sign(const flint::fmpqxx &i) {
    return i.sgn();
}

#endif

} // SymEngine namespace
#endif //SYMENGINE_INTEGER_CLASS_H
