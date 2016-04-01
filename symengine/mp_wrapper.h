#ifndef SYMENGINE_MP_WRAPPER_H
#define SYMENGINE_MP_WRAPPER_H

#include <symengine/symengine_rcp.h>
#include <gmp.h>

#if SYMENGINE_INTEGER_CLASS == SYMENGINE_FLINT
#include <flint/fmpz.h>
#include <flint/fmpq.h>
#endif

#define SYMENGINE_UI(f) f##_ui
#define SYMENGINE_SI(f) f##_si

#define SYMENGINE_MPZ_WRAPPER_IMPLEMENT_RELATIONAL(op, func, val, rev_op)      \
    template <typename T,                                                      \
              typename std::enable_if<std::is_integral<T>::value               \
                                          && std::is_unsigned<T>::value,       \
                                      int>::type                               \
              = 0>                                                             \
    inline friend bool operator op(const mpz_wrapper &a, const T b)            \
    {                                                                          \
        return SYMENGINE_UI(func)(a.get_mpz_t(), b) op val;                    \
    }                                                                          \
    template <typename T,                                                      \
              typename std::enable_if<std::is_integral<T>::value, int>::type   \
              = 0>                                                             \
    inline friend bool operator op(const T a, const mpz_wrapper &b)            \
    {                                                                          \
        return b rev_op a;                                                     \
    }                                                                          \
    template <typename T,                                                      \
              typename std::enable_if<std::is_integral<T>::value               \
                                          && std::is_signed<T>::value,         \
                                      int>::type                               \
              = 0>                                                             \
    inline friend bool operator op(const mpz_wrapper &a, const T b)            \
    {                                                                          \
        return SYMENGINE_SI(func)(a.get_mpz_t(), b) op val;                    \
    }                                                                          \
    inline friend bool operator op(const mpz_wrapper &a, const mpz_wrapper &b) \
    {                                                                          \
        return func(a.get_mpz_t(), b.get_mpz_t()) op val;                      \
    }

#define SYMENGINE_MPZ_WRAPPER_IMPLEMENT_IN_PLACE(op, func)                     \
    inline mpz_wrapper operator op(const mpz_wrapper &a)                       \
    {                                                                          \
        func(get_mpz_t(), get_mpz_t(), a.get_mpz_t());                         \
        return *this;                                                          \
    }                                                                          \
    template <typename T,                                                      \
              typename std::enable_if<std::is_integral<T>::value               \
                                          && std::is_unsigned<T>::value,       \
                                      int>::type                               \
              = 0>                                                             \
    inline mpz_wrapper operator op(const T a)                                  \
    {                                                                          \
        SYMENGINE_UI(func)(get_mpz_t(), get_mpz_t(), a);                       \
        return *this;                                                          \
    }

#define SYMENGINE_MPZ_WRAPPER_IMPLEMENT_NON_COMMUTATIVE(op, func, op_eq)       \
    template <typename T,                                                      \
              typename std::enable_if<std::is_integral<T>::value               \
                                          && std::is_unsigned<T>::value,       \
                                      int>::type                               \
              = 0>                                                             \
    inline friend mpz_wrapper operator op(const mpz_wrapper &a, const T b)     \
    {                                                                          \
        mpz_wrapper res;                                                       \
        SYMENGINE_UI(func)(res.get_mpz_t(), a.get_mpz_t(), b);                 \
        return res;                                                            \
    }                                                                          \
    inline friend mpz_wrapper operator op(const mpz_wrapper &a,                \
                                          const mpz_wrapper &b)                \
    {                                                                          \
        mpz_wrapper res;                                                       \
        func(res.get_mpz_t(), a.get_mpz_t(), b.get_mpz_t());                   \
        return res;                                                            \
    }                                                                          \
    SYMENGINE_MPZ_WRAPPER_IMPLEMENT_IN_PLACE(op_eq, func)

#define SYMENGINE_MPZ_WRAPPER_IMPLEMENT_COMMUTATIVE(op, func, op_eq)           \
    SYMENGINE_MPZ_WRAPPER_IMPLEMENT_NON_COMMUTATIVE(op, func, op_eq)           \
    template <typename T,                                                      \
              typename std::enable_if<std::is_integral<T>::value, int>::type   \
              = 0>                                                             \
    inline friend mpz_wrapper operator op(const T a, mpz_wrapper &b)           \
    {                                                                          \
        return b op a;                                                         \
    }

namespace SymEngine
{

#if SYMENGINE_INTEGER_CLASS == SYMENGINE_FLINT

class fmpz_wrapper
{
private:
    fmpz_t mp;

public:
    template <typename T,
              typename std::enable_if<std::is_integral<T>::value
                                          && std::is_unsigned<T>::value,
                                      int>::type
              = 0>
    inline fmpz_wrapper(const T i)
    {
        fmpz_init(mp);
        fmpz_set_ui(mp, i);
    }
    template <typename T,
              typename std::enable_if<std::is_integral<T>::value
                                          && std::is_signed<T>::value,
                                      int>::type
              = 0>
    inline fmpz_wrapper(const T i)
    {
        fmpz_init(mp);
        fmpz_set_si(mp, i);
    }
    inline fmpz_wrapper()
    {
        fmpz_init(mp);
    }
    inline fmpz_wrapper(const mpz_t m)
    {
        fmpz_init(mp);
        fmpz_set_mpz(mp, m);
    }
    inline fmpz_wrapper(const fmpz_t m)
    {
        fmpz_init(mp);
        fmpz_set(mp, m);
    }
    inline fmpz_wrapper(const std::string &s, unsigned base = 10)
    {
        fmpz_init(mp);
        fmpz_set_str(mp, s.c_str(), base);
    }
    inline fmpz_wrapper(const fmpz_wrapper &other)
    {
        fmpz_init(mp);
        fmpz_set(mp, other.get_fmpz_t());
    }
    inline fmpz_wrapper(fmpz_wrapper &&other)
    {
        fmpz_init(mp);
        fmpz_swap(mp, other.get_fmpz_t());
    }
    inline fmpz_wrapper &operator=(const fmpz_wrapper &other)
    {
        fmpz_set(mp, other.get_fmpz_t());
        return *this;
    }
    inline fmpz_wrapper &operator=(fmpz_wrapper &&other)
    {
        fmpz_swap(mp, other.get_fmpz_t());
        return *this;
    }
    inline ~fmpz_wrapper()
    {
        fmpz_clear(mp);
    }
    inline fmpz *get_fmpz_t()
    {
        return mp;
    }
    inline const fmpz *get_fmpz_t() const
    {
        return mp;
    }
    inline friend fmpz_wrapper operator+(const fmpz_wrapper &a,
                                         const fmpz_wrapper &b)
    {
        fmpz_wrapper res;
        fmpz_add(res.get_fmpz_t(), a.get_fmpz_t(), b.get_fmpz_t());
        return res;
    }
    inline fmpz_wrapper operator+=(const fmpz_wrapper &a)
    {
        fmpz_add(mp, mp, a.get_fmpz_t());
        return *this;
    }
    inline friend fmpz_wrapper operator-(const fmpz_wrapper &a,
                                         const fmpz_wrapper &b)
    {
        fmpz_wrapper res;
        fmpz_sub(res.get_fmpz_t(), a.get_fmpz_t(), b.get_fmpz_t());
        return res;
    }
    inline fmpz_wrapper operator-=(const fmpz_wrapper &a)
    {
        fmpz_sub(mp, mp, a.get_fmpz_t());
        return *this;
    }
    inline fmpz_wrapper operator-() const
    {
        fmpz_wrapper res;
        fmpz_neg(res.get_fmpz_t(), mp);
        return res;
    }
    inline friend fmpz_wrapper operator*(const fmpz_wrapper &a,
                                         const fmpz_wrapper &b)
    {
        fmpz_wrapper res;
        fmpz_mul(res.get_fmpz_t(), a.get_fmpz_t(), b.get_fmpz_t());
        return res;
    }
    inline fmpz_wrapper operator*=(const fmpz_wrapper &a)
    {
        fmpz_mul(mp, mp, a.get_fmpz_t());
        return *this;
    }
    inline friend fmpz_wrapper operator/(const fmpz_wrapper &a,
                                         const fmpz_wrapper &b)
    {
        fmpz_wrapper res;
        fmpz_tdiv_q(res.get_fmpz_t(), a.get_fmpz_t(), b.get_fmpz_t());
        return res;
    }
    inline fmpz_wrapper operator/=(const fmpz_wrapper &a)
    {
        fmpz_tdiv_q(mp, mp, a.get_fmpz_t());
        return *this;
    }
    inline friend fmpz_wrapper operator%(const fmpz_wrapper &a,
                                         const fmpz_wrapper &b)
    {
        fmpz_wrapper res, tmp;
        fmpz_tdiv_qr(tmp.get_fmpz_t(), res.get_fmpz_t(), a.get_fmpz_t(),
                     b.get_fmpz_t());
        return res;
    }
    inline fmpz_wrapper operator%=(const fmpz_wrapper &a)
    {
        fmpz_wrapper tmp;
        fmpz_tdiv_qr(tmp.get_fmpz_t(), mp, mp, a.get_fmpz_t());
        return *this;
    }
    inline fmpz_wrapper operator++()
    {
        fmpz_add_ui(mp, mp, 1);
        return *this;
    }
    inline fmpz_wrapper operator++(int)
    {
        fmpz_wrapper orig = *this;
        ++(*this);
        return orig;
    }
    inline fmpz_wrapper operator--()
    {
        fmpz_sub_ui(mp, mp, 1);
        return *this;
    }
    inline fmpz_wrapper operator--(int)
    {
        fmpz_wrapper orig = *this;
        --(*this);
        return orig;
    }
    inline friend bool operator==(const fmpz_wrapper &a, const fmpz_wrapper &b)
    {
        return fmpz_equal(a.get_fmpz_t(), b.get_fmpz_t()) == 1;
    }
    inline friend bool operator!=(const fmpz_wrapper &a, const fmpz_wrapper &b)
    {
        return fmpz_equal(a.get_fmpz_t(), b.get_fmpz_t()) != 1;
    }
    inline friend bool operator<(const fmpz_wrapper &a, const fmpz_wrapper &b)
    {
        return fmpz_cmp(a.get_fmpz_t(), b.get_fmpz_t()) < 0;
    }
    inline friend bool operator<=(const fmpz_wrapper &a, const fmpz_wrapper &b)
    {
        return fmpz_cmp(a.get_fmpz_t(), b.get_fmpz_t()) <= 0;
    }
    inline friend bool operator>(const fmpz_wrapper &a, const fmpz_wrapper &b)
    {
        return fmpz_cmp(a.get_fmpz_t(), b.get_fmpz_t()) > 0;
    }
    inline friend bool operator>=(const fmpz_wrapper &a, const fmpz_wrapper &b)
    {
        return fmpz_cmp(a.get_fmpz_t(), b.get_fmpz_t()) >= 0;
    }
    inline fmpz_wrapper operator<<=(unsigned long u)
    {
        fmpz_mul_2exp(mp, mp, u);
        return *this;
    }
    inline fmpz_wrapper operator<<(unsigned long u) const
    {
        fmpz_wrapper res;
        fmpz_mul_2exp(res.get_fmpz_t(), mp, u);
        return res;
    }
    inline fmpz_wrapper operator>>=(unsigned long u)
    {
        fmpz_tdiv_q_2exp(mp, mp, u);
        return *this;
    }
    inline fmpz_wrapper operator>>(unsigned long u) const
    {
        fmpz_wrapper res;
        fmpz_tdiv_q_2exp(res.get_fmpz_t(), mp, u);
        return res;
    }
};

class mpz_view_flint
{

public:
    mpz_view_flint(const fmpz_wrapper &i)
    {
        if (!COEFF_IS_MPZ(*i.get_fmpz_t())) {
            mpz_init_set_si(m, *i.get_fmpz_t());
        } else {
            ptr = COEFF_TO_PTR(*i.get_fmpz_t());
        }
    }
    operator mpz_srcptr() const
    {
        if (ptr == nullptr)
            return m;
        return ptr;
    }
    ~mpz_view_flint()
    {
        if (ptr == nullptr)
            mpz_clear(m);
    }

private:
    mpz_srcptr ptr = nullptr;
    mpz_t m;
};

class fmpq_wrapper
{
private:
    fmpq_t mp;

public:
    fmpq *get_fmpq_t()
    {
        return mp;
    }
    const fmpq *get_fmpq_t() const
    {
        return mp;
    }
    fmpq_wrapper()
    {
        fmpq_init(mp);
    }
    fmpq_wrapper(const mpz_t m)
    {
        fmpq_init(mp);
        fmpz_set_mpz(fmpq_numref(mp), m);
    }
    fmpq_wrapper(const fmpz_t m)
    {
        fmpq_init(mp);
        fmpz_set(fmpq_numref(mp), m);
    }
    fmpq_wrapper(const mpq_t m)
    {
        fmpq_init(mp);
        fmpq_set_mpq(mp, m);
    }
    fmpq_wrapper(const fmpq_t m)
    {
        fmpq_init(mp);
        fmpq_set(mp, m);
    }
    template <typename T,
              typename std::enable_if<std::is_integral<T>::value
                                          && std::is_unsigned<T>::value,
                                      int>::type
              = 0>
    fmpq_wrapper(const T i)
    {
        fmpq_init(mp);
        fmpz_set_ui(fmpq_numref(mp), i);
    }
    template <typename T,
              typename std::enable_if<std::is_integral<T>::value
                                          && std::is_signed<T>::value,
                                      int>::type
              = 0>
    fmpq_wrapper(const T i)
    {
        fmpq_init(mp);
        fmpz_set_si(fmpq_numref(mp), i);
    }
    fmpq_wrapper(const fmpz_wrapper &n, const fmpz_wrapper &d = 1)
    {
        fmpq_init(mp);
        fmpz_set(fmpq_numref(mp), n.get_fmpz_t());
        fmpz_set(fmpq_denref(mp), d.get_fmpz_t());
    }
    fmpq_wrapper(const fmpq_wrapper &other)
    {
        fmpq_init(mp);
        fmpq_set(mp, other.get_fmpq_t());
    }
    fmpq_wrapper(fmpq_wrapper &&other)
    {
        fmpq_init(mp);
        fmpq_swap(mp, other.get_fmpq_t());
    }
    fmpq_wrapper &operator=(const fmpq_wrapper &other)
    {
        fmpq_set(mp, other.get_fmpq_t());
        return *this;
    }
    fmpq_wrapper &operator=(fmpq_wrapper &&other)
    {
        fmpq_swap(mp, other.get_fmpq_t());
        return *this;
    }
    ~fmpq_wrapper()
    {
        fmpq_clear(mp);
    }
    const fmpz_wrapper &get_den() const
    {
        return reinterpret_cast<const fmpz_wrapper &>(*fmpq_denref(mp));
    }
    const fmpz_wrapper &get_num() const
    {
        return reinterpret_cast<const fmpz_wrapper &>(*fmpq_numref(mp));
    }
    fmpz_wrapper &get_den()
    {
        return reinterpret_cast<fmpz_wrapper &>(*fmpq_denref(mp));
    }
    fmpz_wrapper &get_num()
    {
        return reinterpret_cast<fmpz_wrapper &>(*fmpq_numref(mp));
    }
    friend fmpq_wrapper operator+(const fmpq_wrapper &a, const fmpq_wrapper &b)
    {
        fmpq_wrapper res;
        fmpq_add(res.get_fmpq_t(), a.get_fmpq_t(), b.get_fmpq_t());
        return res;
    }
    fmpq_wrapper operator+=(const fmpq_wrapper &a)
    {
        fmpq_add(mp, mp, a.get_fmpq_t());
        return *this;
    }
    friend fmpq_wrapper operator-(const fmpq_wrapper &a, const fmpq_wrapper &b)
    {
        fmpq_wrapper res;
        fmpq_sub(res.get_fmpq_t(), a.get_fmpq_t(), b.get_fmpq_t());
        return res;
    }
    fmpq_wrapper operator-=(const fmpq_wrapper &a)
    {
        fmpq_sub(mp, mp, a.get_fmpq_t());
        return *this;
    }
    fmpq_wrapper operator-() const
    {
        fmpq_wrapper res;
        fmpq_neg(res.get_fmpq_t(), mp);
        return res;
    }
    friend fmpq_wrapper operator*(const fmpq_wrapper &a, const fmpq_wrapper &b)
    {
        fmpq_wrapper res;
        fmpq_mul(res.get_fmpq_t(), a.get_fmpq_t(), b.get_fmpq_t());
        return res;
    }
    fmpq_wrapper operator*=(const fmpq_wrapper &a)
    {
        fmpq_mul(mp, mp, a.get_fmpq_t());
        return *this;
    }
    friend fmpq_wrapper operator/(const fmpq_wrapper &a, const fmpq_wrapper &b)
    {
        fmpq_wrapper res;
        fmpq_div(res.get_fmpq_t(), a.get_fmpq_t(), b.get_fmpq_t());
        return res;
    }
    fmpq_wrapper operator/=(const fmpq_wrapper &a)
    {
        fmpq_div(mp, mp, a.get_fmpq_t());
        return *this;
    }
    bool operator==(const fmpq_wrapper &other) const
    {
        return fmpq_equal(mp, other.get_fmpq_t());
    }
    bool operator!=(const fmpq_wrapper &other) const
    {
        return not(*this == other);
    }
    bool operator<(const fmpq_wrapper &other) const
    {
        return fmpq_cmp(mp, other.get_fmpq_t()) < 0;
    }
    bool operator<=(const fmpq_wrapper &other) const
    {
        return fmpq_cmp(mp, other.get_fmpq_t()) <= 0;
    }
    bool operator>(const fmpq_wrapper &other) const
    {
        return fmpq_cmp(mp, other.get_fmpq_t()) > 0;
    }
    bool operator>=(const fmpq_wrapper &other) const
    {
        return fmpq_cmp(mp, other.get_fmpq_t()) >= 0;
    }
};

class mpq_view_flint
{

public:
    mpq_view_flint(const fmpq_wrapper &i)
    {
        mpq_init(m);
        fmpq_get_mpq(m, i.get_fmpq_t());
    }
    operator mpq_srcptr() const
    {
        return m;
    }
    ~mpq_view_flint()
    {
        mpq_clear(m);
    }

private:
    mpq_t m;
};
#elif SYMENGINE_INTEGER_CLASS == SYMENGINE_GMP

class mpz_wrapper
{
private:
    mpz_t mp;

public:
    template <typename T,
              typename std::enable_if<std::is_integral<T>::value
                                          && std::is_unsigned<T>::value,
                                      int>::type
              = 0>
    mpz_wrapper(const T i)
    {
        mpz_init_set_ui(mp, i);
    }
    template <typename T,
              typename std::enable_if<std::is_integral<T>::value
                                          && std::is_signed<T>::value,
                                      int>::type
              = 0>
    mpz_wrapper(const T i)
    {
        mpz_init_set_si(mp, i);
    }
    inline mpz_wrapper()
    {
        mpz_init(mp);
    }
    inline mpz_wrapper(const mpz_t m)
    {
        mpz_init_set(mp, m);
    }
    inline mpz_wrapper(const std::string &s, unsigned base = 10)
    {
        mpz_init_set_str(mp, s.c_str(), base);
    }
    inline mpz_wrapper(const mpz_wrapper &other)
    {
        mpz_init_set(mp, other.get_mpz_t());
    }
    inline mpz_wrapper(mpz_wrapper &&other)
    {
        mp->_mp_d = nullptr;
        mpz_swap(mp, other.get_mpz_t());
    }
    inline mpz_wrapper &operator=(const mpz_wrapper &other)
    {
        if (mp->_mp_d == nullptr) {
            mpz_init_set(mp, other.get_mpz_t());
        } else {
            mpz_set(mp, other.get_mpz_t());
        }
        return *this;
    }
    inline mpz_wrapper &operator=(mpz_wrapper &&other)
    {
        mpz_swap(mp, other.get_mpz_t());
        return *this;
    }
    template <typename T,
              typename std::enable_if<std::is_integral<T>::value
                                          && std::is_unsigned<T>::value,
                                      int>::type
              = 0>
    inline mpz_wrapper &operator=(T other)
    {
        if (mp->_mp_d == nullptr) {
            mpz_init_set_ui(mp, other);
        } else {
            mpz_set_ui(mp, other);
        }
        return *this;
    }
    template <typename T,
              typename std::enable_if<std::is_integral<T>::value
                                          && std::is_signed<T>::value,
                                      int>::type
              = 0>
    inline mpz_wrapper &operator=(T other)
    {
        if (mp->_mp_d == nullptr) {
            mpz_init_set_si(mp, other);
        } else {
            mpz_set_si(mp, other);
        }
        return *this;
    }
    inline ~mpz_wrapper()
    {
        if (mp->_mp_d != nullptr) {
            mpz_clear(mp);
        }
    }
    inline mpz_ptr get_mpz_t()
    {
        return mp;
    }
    inline mpz_srcptr get_mpz_t() const
    {
        return mp;
    }

    //! + operator
    SYMENGINE_MPZ_WRAPPER_IMPLEMENT_COMMUTATIVE(+, mpz_add, += )
    //! * operator
    SYMENGINE_MPZ_WRAPPER_IMPLEMENT_COMMUTATIVE(*, mpz_mul, *= )
    //! - operator
    SYMENGINE_MPZ_WRAPPER_IMPLEMENT_NON_COMMUTATIVE(-, mpz_sub, -= )

    template <typename T,
              typename std::enable_if<std::is_integral<T>::value
                                          && std::is_unsigned<T>::value,
                                      int>::type
              = 0>
    inline friend mpz_wrapper operator-(const T b, const mpz_wrapper &a)
    {
        mpz_wrapper res;
        mpz_ui_sub(res.get_mpz_t(), b, a.get_mpz_t());
        return res;
    }
    //! / operator
    SYMENGINE_MPZ_WRAPPER_IMPLEMENT_NON_COMMUTATIVE(/, mpz_tdiv_q, /= )
    //! % operator
    SYMENGINE_MPZ_WRAPPER_IMPLEMENT_NON_COMMUTATIVE(%, mpz_tdiv_r, %= )

    inline mpz_wrapper operator-() const
    {
        mpz_wrapper res;
        mpz_neg(res.get_mpz_t(), mp);
        return res;
    }

    inline mpz_wrapper operator++()
    {
        mpz_add_ui(mp, mp, 1);
        return *this;
    }
    inline mpz_wrapper operator++(int)
    {
        mpz_wrapper orig = *this;
        ++(*this);
        return orig;
    }
    inline mpz_wrapper operator--()
    {
        mpz_sub_ui(mp, mp, 1);
        return *this;
    }
    inline mpz_wrapper operator--(int)
    {
        mpz_wrapper orig = *this;
        --(*this);
        return orig;
    }

    //! < operator
    SYMENGINE_MPZ_WRAPPER_IMPLEMENT_RELATIONAL(<, mpz_cmp, 0, >)
    //! <= operator
    SYMENGINE_MPZ_WRAPPER_IMPLEMENT_RELATIONAL(<=, mpz_cmp, 0, >= )
    //! > operator
    SYMENGINE_MPZ_WRAPPER_IMPLEMENT_RELATIONAL(>, mpz_cmp, 0, < )
    //! >= operator
    SYMENGINE_MPZ_WRAPPER_IMPLEMENT_RELATIONAL(>=, mpz_cmp, 0, <= )
    //! == operator
    SYMENGINE_MPZ_WRAPPER_IMPLEMENT_RELATIONAL(==, mpz_cmp, 0, == )
    //! != operator
    SYMENGINE_MPZ_WRAPPER_IMPLEMENT_RELATIONAL(!=, mpz_cmp, 0, != )

    inline mpz_wrapper operator<<=(unsigned long u)
    {
        mpz_mul_2exp(mp, mp, u);
        return *this;
    }
    inline mpz_wrapper operator<<(unsigned long u) const
    {
        mpz_wrapper res;
        mpz_mul_2exp(res.get_mpz_t(), mp, u);
        return res;
    }
    inline mpz_wrapper operator>>=(unsigned long u)
    {
        mpz_tdiv_q_2exp(mp, mp, u);
        return *this;
    }
    inline mpz_wrapper operator>>(unsigned long u) const
    {
        mpz_wrapper res;
        mpz_tdiv_q_2exp(res.get_mpz_t(), mp, u);
        return res;
    }
    inline unsigned long get_ui() const
    {
        return mpz_get_ui(mp);
    }
    inline signed long get_si() const
    {
        return mpz_get_si(mp);
    }
    inline double long get_d() const
    {
        return mpz_get_d(mp);
    }
    inline int fits_ulong_p() const
    {
        return mpz_fits_ulong_p(mp);
    }
    inline int fits_slong_p() const
    {
        return mpz_fits_slong_p(mp);
    }
};

class mpq_wrapper
{
private:
    mpq_t mp;

public:
    mpq_ptr get_mpq_t()
    {
        return mp;
    }
    mpq_srcptr get_mpq_t() const
    {
        return mp;
    }
    mpq_wrapper()
    {
        mpq_init(mp);
    }
    mpq_wrapper(const mpz_t m)
    {
        mpq_init(mp);
        mpz_set(mpq_numref(mp), m);
    }
    mpq_wrapper(const mpq_t m)
    {
        mpq_init(mp);
        mpq_set(mp, m);
    }
    template <typename T,
              typename std::enable_if<std::is_integral<T>::value
                                          && std::is_unsigned<T>::value,
                                      int>::type
              = 0>
    mpq_wrapper(const T i)
    {
        mpq_init(mp);
        mpz_set_ui(mpq_numref(mp), i);
    }
    template <typename T,
              typename std::enable_if<std::is_integral<T>::value
                                          && std::is_signed<T>::value,
                                      int>::type
              = 0>
    mpq_wrapper(const T i)
    {
        mpq_init(mp);
        mpz_set_si(mpq_numref(mp), i);
    }
    mpq_wrapper(const mpz_wrapper &n, const mpz_wrapper &d = 1)
    {
        mpq_init(mp);
        mpz_set(mpq_numref(mp), n.get_mpz_t());
        mpz_set(mpq_denref(mp), d.get_mpz_t());
    }
    mpq_wrapper(const mpq_wrapper &other)
    {
        mpq_init(mp);
        mpq_set(mp, other.get_mpq_t());
    }
    mpq_wrapper(mpq_wrapper &&other)
    {
        mpq_init(mp);
        mpq_swap(mp, other.get_mpq_t());
    }
    mpq_wrapper &operator=(const mpq_wrapper &other)
    {
        mpq_set(mp, other.get_mpq_t());
        return *this;
    }
    mpq_wrapper &operator=(mpq_wrapper &&other)
    {
        mpq_swap(mp, other.get_mpq_t());
        return *this;
    }
    ~mpq_wrapper()
    {
        mpq_clear(mp);
    }
    const mpz_wrapper &get_den() const
    {
        return reinterpret_cast<const mpz_wrapper &>(*mpq_denref(mp));
    }
    const mpz_wrapper &get_num() const
    {
        return reinterpret_cast<const mpz_wrapper &>(*mpq_numref(mp));
    }
    mpz_wrapper &get_den()
    {
        return reinterpret_cast<mpz_wrapper &>(*mpq_denref(mp));
    }
    mpz_wrapper &get_num()
    {
        return reinterpret_cast<mpz_wrapper &>(*mpq_numref(mp));
    }
    friend mpq_wrapper operator+(const mpq_wrapper &a, const mpq_wrapper &b)
    {
        mpq_wrapper res;
        mpq_add(res.get_mpq_t(), a.get_mpq_t(), b.get_mpq_t());
        return res;
    }
    mpq_wrapper operator+=(const mpq_wrapper &a)
    {
        mpq_add(mp, mp, a.get_mpq_t());
        return *this;
    }
    friend mpq_wrapper operator-(const mpq_wrapper &a, const mpq_wrapper &b)
    {
        mpq_wrapper res;
        mpq_sub(res.get_mpq_t(), a.get_mpq_t(), b.get_mpq_t());
        return res;
    }
    mpq_wrapper operator-=(const mpq_wrapper &a)
    {
        mpq_sub(mp, mp, a.get_mpq_t());
        return *this;
    }
    mpq_wrapper operator-() const
    {
        mpq_wrapper res;
        mpq_neg(res.get_mpq_t(), mp);
        return res;
    }
    friend mpq_wrapper operator*(const mpq_wrapper &a, const mpq_wrapper &b)
    {
        mpq_wrapper res;
        mpq_mul(res.get_mpq_t(), a.get_mpq_t(), b.get_mpq_t());
        return res;
    }
    mpq_wrapper operator*=(const mpq_wrapper &a)
    {
        mpq_mul(mp, mp, a.get_mpq_t());
        return *this;
    }
    friend mpq_wrapper operator/(const mpq_wrapper &a, const mpq_wrapper &b)
    {
        mpq_wrapper res;
        mpq_div(res.get_mpq_t(), a.get_mpq_t(), b.get_mpq_t());
        return res;
    }
    mpq_wrapper operator/=(const mpq_wrapper &a)
    {
        mpq_div(mp, mp, a.get_mpq_t());
        return *this;
    }
    bool operator==(const mpq_wrapper &other) const
    {
        return mpq_cmp(mp, other.get_mpq_t()) == 0;
    }
    bool operator!=(const mpq_wrapper &other) const
    {
        return not(*this == other);
    }
    bool operator<(const mpq_wrapper &other) const
    {
        return mpq_cmp(mp, other.get_mpq_t()) < 0;
    }
    bool operator<=(const mpq_wrapper &other) const
    {
        return mpq_cmp(mp, other.get_mpq_t()) <= 0;
    }
    bool operator>(const mpq_wrapper &other) const
    {
        return mpq_cmp(mp, other.get_mpq_t()) > 0;
    }
    bool operator>=(const mpq_wrapper &other) const
    {
        return mpq_cmp(mp, other.get_mpq_t()) >= 0;
    }
    double get_d() const
    {
        return mpq_get_d(mp);
    }
    void canonicalize()
    {
        mpq_canonicalize(mp);
    }
};

#endif

#if SYMENGINE_INTEGER_CLASS == SYMENGINE_FLINT
std::ostream &operator<<(std::ostream &os, const SymEngine::fmpq_wrapper &f);
std::ostream &operator<<(std::ostream &os, const SymEngine::fmpz_wrapper &f);
#elif SYMENGINE_INTEGER_CLASS == SYMENGINE_GMP
std::ostream &operator<<(std::ostream &os, const SymEngine::mpq_wrapper &f);
std::ostream &operator<<(std::ostream &os, const SymEngine::mpz_wrapper &f);
#endif

} // SymEngine

#endif // SYMENGINE_MP_WRAPPER_H
