#include <symengine/symengine_config.h>

#if defined(HAVE_C_FUNCTION_NOT_FUNC)
#define __func__ __FUNCTION__
#endif

#include <symengine/cwrapper.h>
#include <string.h>
#include <math.h>

#ifdef HAVE_SYMENGINE_MPFR
#include <mpfr.h>
#endif // HAVE_SYMENGINE_MPFR

void test_cwrapper()
{
    char *s;
    basic x, y, z;
    basic f;
    basic_new_stack(x);
    basic_new_stack(y);
    basic_new_stack(z);
    symbol_set(x, "x");
    symbol_set(y, "y");
    symbol_set(z, "z");

    SYMENGINE_C_ASSERT(is_a_Number(x) == 0);
    SYMENGINE_C_ASSERT(is_a_Number(y) == 0);
    SYMENGINE_C_ASSERT(is_a_Number(z) == 0);

    s = basic_str(x);
    SYMENGINE_C_ASSERT(strcmp(s, "x") == 0);
    basic_str_free(s);

    basic_new_stack(f);
    CVecBasic *vec = vecbasic_new();
    vecbasic_push_back(vec, x);
    vecbasic_push_back(vec, y);
    vecbasic_push_back(vec, z);
    function_symbol_set(f, "f", vec);
    s = basic_str(f);
    SYMENGINE_C_ASSERT(strcmp(s, "f(x, y, z)") == 0);
    vecbasic_free(vec);
    basic_str_free(s);

    basic e;
    basic_new_stack(e);
    integer_set_ui(e, 123);
    s = basic_str(e);
    SYMENGINE_C_ASSERT(strcmp(s, "123") == 0);
    basic_str_free(s);

    integer_set_ui(e, 456);
    basic_add(e, e, x);
    basic_mul(e, e, y);
    basic_div(e, e, z);
    s = basic_str(e);
    SYMENGINE_C_ASSERT(strcmp(s, "y*(456 + x)/z") == 0);
    basic_str_free(s);

    basic_diff(e, e, z);
    s = basic_str(e);
    SYMENGINE_C_ASSERT(strcmp(s, "-y*(456 + x)/z**2") == 0);
    basic_str_free(s);

    rational_set_ui(e, 100, 47);
    s = basic_str(e);

    SYMENGINE_C_ASSERT(strcmp(s, "100/47") == 0);
    SYMENGINE_C_ASSERT(!is_a_Symbol(e));
    SYMENGINE_C_ASSERT(is_a_Rational(e));
    SYMENGINE_C_ASSERT(!is_a_Integer(e));

    integer_set_si(e, 0);
    SYMENGINE_C_ASSERT(integer_get_si(e) == 0);
    SYMENGINE_C_ASSERT(basic_number_sign(e) == 0);

    integer_set_ui(e, 123);
    SYMENGINE_C_ASSERT(integer_get_ui(e) == 123);
    SYMENGINE_C_ASSERT(basic_number_sign(e) == 1);

    integer_set_si(e, -123);
    SYMENGINE_C_ASSERT(integer_get_si(e) == -123);
    SYMENGINE_C_ASSERT(basic_number_sign(e) == -1);
    SYMENGINE_C_ASSERT(is_a_Number(e) == 1);

    mpz_t test;
    mpz_init(test);

    integer_get_mpz(test, e);
    SYMENGINE_C_ASSERT(mpz_get_ui(test) == 123);

    char *str = "123 + 321";
    basic p;
    basic_new_stack(p);
    basic_parse(p, str);
    SYMENGINE_C_ASSERT(is_a_Integer(p));
    SYMENGINE_C_ASSERT(integer_get_si(p) == 444);

    mpz_clear(test);
    basic_free_stack(f);
    basic_free_stack(e);
    basic_free_stack(x);
    basic_free_stack(y);
    basic_free_stack(z);
    basic_free_stack(p);
    basic_str_free(s);
}

void test_basic()
{
    basic x;
    basic_new_stack(x);
    symbol_set(x, "x");

    basic_struct *y = basic_new_heap();
    symbol_set(y, "x");

    SYMENGINE_C_ASSERT(basic_eq(x, y))

    basic_free_heap(y);
    basic_free_stack(x);
}

void test_complex()
{
    basic e;
    basic f;
    char *s;
    basic_new_stack(e);
    basic_new_stack(f);
    rational_set_ui(e, 100, 47);
    rational_set_ui(f, 76, 59);
    complex_set(e, e, f);
    s = basic_str(e);

    SYMENGINE_C_ASSERT(strcmp(s, "100/47 + 76/59*I") == 0);
    SYMENGINE_C_ASSERT(!is_a_Symbol(e));
    SYMENGINE_C_ASSERT(!is_a_Rational(e));
    SYMENGINE_C_ASSERT(!is_a_Integer(e));
    SYMENGINE_C_ASSERT(is_a_Complex(e));

    basic_str_free(s);

    complex_real_part(f, e);
    s = basic_str(f);

    SYMENGINE_C_ASSERT(strcmp(s, "100/47") == 0);
    SYMENGINE_C_ASSERT(!is_a_Symbol(f));
    SYMENGINE_C_ASSERT(is_a_Rational(f));
    SYMENGINE_C_ASSERT(!is_a_Integer(f));
    SYMENGINE_C_ASSERT(!is_a_Complex(f));

    basic_str_free(s);

    complex_imaginary_part(f, e);
    s = basic_str(f);

    SYMENGINE_C_ASSERT(strcmp(s, "76/59") == 0);
    SYMENGINE_C_ASSERT(!is_a_Symbol(f));
    SYMENGINE_C_ASSERT(is_a_Rational(f));
    SYMENGINE_C_ASSERT(!is_a_Integer(f));
    SYMENGINE_C_ASSERT(!is_a_Complex(f));

    basic_str_free(s);

    basic_free_stack(e);
    basic_free_stack(f);
}

void test_complex_double()
{
    basic e;
    basic f;
    char *s;
    basic_new_stack(e);
    basic_new_stack(f);

    basic_const_I(e);
    real_double_set_d(f, 76.59);
    basic_mul(f, f, e);
    real_double_set_d(e, 100.47);
    basic_add(e, e, f);
    s = basic_str(e);

    SYMENGINE_C_ASSERT(strcmp(s, "100.47 + 76.59*I") == 0);
    SYMENGINE_C_ASSERT(!is_a_Symbol(e));
    SYMENGINE_C_ASSERT(!is_a_Rational(e));
    SYMENGINE_C_ASSERT(!is_a_Integer(e));
    SYMENGINE_C_ASSERT(!is_a_Complex(e));
    SYMENGINE_C_ASSERT(is_a_ComplexDouble(e));

    basic_str_free(s);

    complex_double_real_part(f, e);
    s = basic_str(f);

    SYMENGINE_C_ASSERT(strcmp(s, "100.47") == 0);
    SYMENGINE_C_ASSERT(!is_a_Symbol(f));
    SYMENGINE_C_ASSERT(!is_a_Rational(f));
    SYMENGINE_C_ASSERT(!is_a_Integer(f));
    SYMENGINE_C_ASSERT(!is_a_Complex(f));
    SYMENGINE_C_ASSERT(is_a_RealDouble(f));

    basic_str_free(s);

    complex_double_imaginary_part(f, e);
    s = basic_str(f);

    SYMENGINE_C_ASSERT(strcmp(s, "76.59") == 0);
    SYMENGINE_C_ASSERT(!is_a_Symbol(f));
    SYMENGINE_C_ASSERT(!is_a_Rational(f));
    SYMENGINE_C_ASSERT(!is_a_Integer(f));
    SYMENGINE_C_ASSERT(!is_a_Complex(f));
    SYMENGINE_C_ASSERT(is_a_RealDouble(f));

    basic_str_free(s);

    basic_free_stack(e);
    basic_free_stack(f);
}

void test_real_double()
{
    basic d;
    basic_new_stack(d);
    real_double_set_d(d, 123.456);
    SYMENGINE_C_ASSERT(real_double_get_d(d) == 123.456);

    char *s2;
    s2 = basic_str(d);

    SYMENGINE_C_ASSERT(is_a_RealDouble(d));
    SYMENGINE_C_ASSERT(strcmp(s2, "123.456") == 0);
    basic_str_free(s2);

    basic_free_stack(d);
}

#ifdef HAVE_SYMENGINE_MPFR
void test_real_mpfr()
{
    basic d, e;
    basic_new_stack(e);
    basic_new_stack(d);

    real_mpfr_set_d(d, 123.456, 200);
    SYMENGINE_C_ASSERT(basic_get_type(d) == SYMENGINE_REAL_MPFR);
    SYMENGINE_C_ASSERT(real_mpfr_get_d(d) == 123.456);

    real_mpfr_set_str(e, "456.123", 200);
    SYMENGINE_C_ASSERT(basic_get_type(e) == SYMENGINE_REAL_MPFR);
    SYMENGINE_C_ASSERT(real_mpfr_get_d(e) == 456.123);
    SYMENGINE_C_ASSERT(real_mpfr_get_prec(e) == 200);

    mpfr_t mp;
    mpfr_init2(mp, 200);
    real_mpfr_get(mp, e);
    real_mpfr_set(d, mp);
    SYMENGINE_C_ASSERT(basic_get_type(d) == SYMENGINE_REAL_MPFR);
    SYMENGINE_C_ASSERT(real_mpfr_get_d(d) == 456.123);

    real_mpfr_set_d(d, 0, 200);
    SYMENGINE_C_ASSERT(real_mpfr_is_zero(d) == 1);

    real_mpfr_set_d(d, 0.000001, 200);
    SYMENGINE_C_ASSERT(real_mpfr_is_zero(d) == 0);

    mpfr_clear(mp);
    basic_free_stack(d);
    basic_free_stack(e);
}
#endif // HAVE_SYMENGINE_MPFR

#ifdef HAVE_SYMENGINE_MPC
void test_complex_mpc()
{
    basic d, d1, d2;
    basic_new_stack(d);
    basic_new_stack(d1);
    basic_new_stack(d2);

    basic_const_I(d2);

    real_mpfr_set_d(d, 0.000001, 200);
    real_mpfr_set_d(d1, 0.000001, 200);
    basic_mul(d2, d1, d2);
    basic_add(d2, d, d2);
    SYMENGINE_C_ASSERT(basic_get_type(d2) == SYMENGINE_COMPLEX_MPC);
    SYMENGINE_C_ASSERT(complex_mpc_is_zero(d2) == 0);

    basic r1;
    basic_new_stack(r1);

    complex_mpc_real_part(r1, d2);
    SYMENGINE_C_ASSERT(basic_eq(r1, d));

    complex_mpc_imaginary_part(r1, d2);
    SYMENGINE_C_ASSERT(basic_eq(r1, d1));

    basic_free_stack(d);
    basic_free_stack(d1);
    basic_free_stack(d2);
    basic_free_stack(r1);
}
#endif // HAVE_SYMENGINE_MPC

void test_CVectorInt1()
{
    // Allocate on heap
    CVectorInt *vec = vectorint_new();
    vectorint_push_back(vec, 5);
    ;
    SYMENGINE_C_ASSERT(vectorint_get(vec, 0) == 5);
    vectorint_free(vec);
}

struct X {
    void *x;
};

void test_CVectorInt2()
{
    // Allocate on stack
    CVectorInt *vec;

    char data1[1]; // Not aligned properly
    SYMENGINE_C_ASSERT(vectorint_placement_new_check(data1, sizeof(data1))
                       == 1);

    struct X data2[1]; // Aligned properly but small
    SYMENGINE_C_ASSERT(vectorint_placement_new_check(data2, sizeof(data2))
                       == 1);

    struct X
        data3[50]; // Aligned properly and enough size to fit std::vector<int>
    SYMENGINE_C_ASSERT(vectorint_placement_new_check(data3, 1) == 1);
    SYMENGINE_C_ASSERT(vectorint_placement_new_check(data3, 2) == 1);
    SYMENGINE_C_ASSERT(vectorint_placement_new_check(data3, sizeof(data3))
                       == 0);
    vec = vectorint_placement_new(data3);
    vectorint_push_back(vec, 5);
    SYMENGINE_C_ASSERT(vectorint_get(vec, 0) == 5);
    vectorint_placement_free(vec);
}

void test_CVecBasic()
{
    CVecBasic *vec = vecbasic_new();
    SYMENGINE_C_ASSERT(vecbasic_size(vec) == 0);

    basic x;
    basic_new_stack(x);
    symbol_set(x, "x");
    vecbasic_push_back(vec, x);

    SYMENGINE_C_ASSERT(vecbasic_size(vec) == 1);

    basic y;
    basic_new_stack(y);
    vecbasic_get(vec, 0, y);

    SYMENGINE_C_ASSERT(basic_eq(x, y));

    vecbasic_free(vec);
    basic_free_stack(x);
    basic_free_stack(y);
}

void test_CSetBasic()
{
    CSetBasic *set = setbasic_new();
    SYMENGINE_C_ASSERT(setbasic_size(set) == 0);

    basic x;
    basic_new_stack(x);
    symbol_set(x, "x");

    int has_insert;
    has_insert = setbasic_insert(set, x);
    SYMENGINE_C_ASSERT(has_insert == 1);
    SYMENGINE_C_ASSERT(setbasic_size(set) == 1);

    has_insert = setbasic_insert(set, x);
    SYMENGINE_C_ASSERT(has_insert == 0);

    basic y;
    basic_new_stack(y);
    symbol_set(y, "y");

    int is_found;
    is_found = setbasic_find(set, x);
    SYMENGINE_C_ASSERT(is_found == 1);

    is_found = setbasic_find(set, y);
    SYMENGINE_C_ASSERT(is_found == 0);

    setbasic_get(set, 0, y);
    SYMENGINE_C_ASSERT(basic_eq(x, y));

    setbasic_free(set);
    basic_free_stack(x);
    basic_free_stack(y);
}

void test_CMapBasicBasic()
{
    CMapBasicBasic *map = mapbasicbasic_new();
    SYMENGINE_C_ASSERT(mapbasicbasic_size(map) == 0);

    basic x, y;
    basic_new_stack(x);
    basic_new_stack(y);
    symbol_set(x, "x");
    symbol_set(y, "y");

    mapbasicbasic_insert(map, x, y);
    SYMENGINE_C_ASSERT(mapbasicbasic_size(map) == 1);

    basic z;
    basic_new_stack(z);
    symbol_set(z, "z");

    int is_found;
    is_found = mapbasicbasic_get(map, x, z);
    SYMENGINE_C_ASSERT(is_found == 1);
    SYMENGINE_C_ASSERT(basic_eq(y, z));

    is_found = mapbasicbasic_get(map, y, z);
    SYMENGINE_C_ASSERT(is_found == 0);

    mapbasicbasic_free(map);
    basic_free_stack(x);
    basic_free_stack(y);
    basic_free_stack(z);
}

void test_get_args()
{
    basic x, y, z, e;
    basic_new_stack(x);
    basic_new_stack(y);
    basic_new_stack(z);
    basic_new_stack(e);
    symbol_set(x, "x");
    symbol_set(y, "y");
    symbol_set(z, "z");

    integer_set_ui(e, 123);
    basic_add(e, e, x);
    basic_mul(e, e, y);
    basic_div(e, e, z);

    CVecBasic *args = vecbasic_new();
    basic_get_args(e, args);
    SYMENGINE_C_ASSERT(vecbasic_size(args) == 3);
    vecbasic_free(args);

    basic_free_stack(e);
    basic_free_stack(x);
    basic_free_stack(y);
    basic_free_stack(z);
}

void test_free_symbols()
{
    basic x, y, z, e;
    basic_new_stack(x);
    basic_new_stack(y);
    basic_new_stack(z);
    basic_new_stack(e);
    symbol_set(x, "x");
    symbol_set(y, "y");
    symbol_set(z, "z");

    integer_set_ui(e, 123);
    basic_add(e, e, x);
    basic_pow(e, e, y);
    basic_div(e, e, z);

    CSetBasic *symbols = setbasic_new();
    basic_free_symbols(e, symbols);
    SYMENGINE_C_ASSERT(setbasic_size(symbols) == 3);
    setbasic_free(symbols);

    basic_free_stack(e);
    basic_free_stack(x);
    basic_free_stack(y);
    basic_free_stack(z);
}

void test_get_type()
{
    basic x, y;
    basic_new_stack(x);
    basic_new_stack(y);
    symbol_set(x, "x");
    integer_set_ui(y, 123);

    SYMENGINE_C_ASSERT(basic_get_type(x) == SYMENGINE_SYMBOL);
    SYMENGINE_C_ASSERT(basic_get_type(y) == SYMENGINE_INTEGER);

    SYMENGINE_C_ASSERT(basic_get_class_id("Integer") == SYMENGINE_INTEGER);
    SYMENGINE_C_ASSERT(basic_get_class_id("Add") == SYMENGINE_ADD);

    char *s;
    s = basic_get_class_from_id(SYMENGINE_INTEGER);
    SYMENGINE_C_ASSERT(strcmp(s, "Integer") == 0);
    basic_str_free(s);

    basic_free_stack(x);
    basic_free_stack(y);
}

void test_hash()
{
    basic x1, x2, y;
    basic_new_stack(x1);
    basic_new_stack(x2);
    basic_new_stack(y);
    symbol_set(x1, "x");
    symbol_set(x2, "x");
    symbol_set(y, "y");

    SYMENGINE_C_ASSERT(basic_hash(x1) == basic_hash(x2));
    if (basic_hash(x1) != basic_hash(y))
        SYMENGINE_C_ASSERT(basic_neq(x1, y));

    basic_free_stack(x1);
    basic_free_stack(x2);
    basic_free_stack(y);
}

void test_subs2()
{
    basic s, e, x, y, z;
    basic_new_stack(s);
    basic_new_stack(e);
    basic_new_stack(x);
    basic_new_stack(y);
    basic_new_stack(z);

    symbol_set(x, "x");
    symbol_set(y, "y");
    symbol_set(z, "z");
    basic_mul(e, x, y);
    basic_mul(e, e, z);
    // e should be x*y*z

    basic_subs2(s, e, y, x);
    basic_subs2(s, s, z, x);
    // s should be x**3

    integer_set_si(z, 3);
    basic_pow(e, x, z);
    // e should be x**3

    SYMENGINE_C_ASSERT(basic_eq(s, e));

    basic_free_stack(s);
    basic_free_stack(e);
    basic_free_stack(x);
    basic_free_stack(y);
    basic_free_stack(z);
}

void test_subs()
{
    basic s, e, x, y, z;
    basic_new_stack(s);
    basic_new_stack(e);
    basic_new_stack(x);
    basic_new_stack(y);
    basic_new_stack(z);

    symbol_set(x, "x");
    symbol_set(y, "y");
    symbol_set(z, "z");
    basic_mul(e, x, y);
    basic_mul(e, e, z);
    // e should be x*y*z

    CMapBasicBasic *map = mapbasicbasic_new();
    mapbasicbasic_insert(map, y, x);
    mapbasicbasic_insert(map, z, x);
    basic_subs(s, e, map);
    // s should be x**3

    integer_set_si(z, 3);
    basic_pow(e, x, z);
    // e should be x**3

    SYMENGINE_C_ASSERT(basic_eq(s, e));

    mapbasicbasic_free(map);
    basic_free_stack(s);
    basic_free_stack(e);
    basic_free_stack(x);
    basic_free_stack(y);
    basic_free_stack(z);
}

void test_constants()
{
    basic z, o, mo, i;
    basic_new_stack(z);
    basic_new_stack(o);
    basic_new_stack(mo);
    basic_new_stack(i);

    integer_set_si(z, 0);
    integer_set_si(o, 1);
    integer_set_si(mo, -1);
    complex_set(i, z, o);

    basic zero, one, minus_one, iota;
    basic_new_stack(zero);
    basic_new_stack(one);
    basic_new_stack(minus_one);
    basic_new_stack(iota);

    basic_const_zero(zero);
    basic_const_one(one);
    basic_const_minus_one(minus_one);
    basic_const_I(iota);

    SYMENGINE_C_ASSERT(basic_eq(z, zero));
    SYMENGINE_C_ASSERT(basic_eq(o, one));
    SYMENGINE_C_ASSERT(basic_eq(mo, minus_one));
    SYMENGINE_C_ASSERT(basic_eq(i, iota));

    basic_free_stack(z);
    basic_free_stack(zero);
    basic_free_stack(o);
    basic_free_stack(one);
    basic_free_stack(mo);
    basic_free_stack(minus_one);
    basic_free_stack(i);
    basic_free_stack(iota);

    basic custom, pi, e, euler_gamma;
    basic_new_stack(custom);
    basic_new_stack(pi);
    basic_new_stack(e);
    basic_new_stack(euler_gamma);

    basic_const_set(custom, "custom");
    basic_const_pi(pi);
    basic_const_E(e);
    basic_const_EulerGamma(euler_gamma);

    char *s;
    s = basic_str(custom);
    SYMENGINE_C_ASSERT(strcmp(s, "custom") == 0);
    basic_str_free(s);
    s = basic_str(pi);
    SYMENGINE_C_ASSERT(strcmp(s, "pi") == 0);
    basic_str_free(s);
    s = basic_str(e);
    SYMENGINE_C_ASSERT(strcmp(s, "E") == 0);
    basic_str_free(s);
    s = basic_str(euler_gamma);
    SYMENGINE_C_ASSERT(strcmp(s, "EulerGamma") == 0);
    basic_str_free(s);

    basic_free_stack(custom);
    basic_free_stack(pi);
    basic_free_stack(e);
    basic_free_stack(euler_gamma);
}

void test_ascii_art()
{
    char *s = ascii_art_str();
    SYMENGINE_C_ASSERT(strlen(s) > 0);
    basic_str_free(s);
}

void test_functions()
{
    basic pi, e;
    basic minus_one, minus_half, zero, one, two, four;
    basic pi_div_two, pi_div_four;
    basic e_minus_one;
    basic ans;

    basic_new_stack(pi);
    basic_new_stack(e);
    basic_new_stack(ans);
    basic_new_stack(two);
    basic_new_stack(pi_div_two);
    basic_new_stack(four);
    basic_new_stack(pi_div_four);
    basic_new_stack(one);
    basic_new_stack(minus_one);
    basic_new_stack(zero);
    basic_new_stack(e_minus_one);
    basic_new_stack(minus_half);

    basic_const_pi(pi);
    basic_const_E(e);
    integer_set_si(two, 2);
    integer_set_si(four, 4);
    integer_set_si(one, 1);
    integer_set_si(minus_one, -1);
    integer_set_si(zero, 0);

    basic_div(pi_div_two, pi, two);
    basic_div(pi_div_four, pi, four);
    basic_pow(e_minus_one, e, minus_one);
    basic_mul(e_minus_one, e_minus_one, minus_one);
    basic_div(minus_half, minus_one, two);

    char *s;

    basic_sin(ans, pi);
    SYMENGINE_C_ASSERT(basic_eq(ans, zero));

    basic_cos(ans, pi);
    SYMENGINE_C_ASSERT(basic_eq(ans, minus_one));

    basic_tan(ans, pi);
    SYMENGINE_C_ASSERT(basic_eq(ans, zero));

    basic_csc(ans, pi_div_two);
    s = basic_str(ans);
    SYMENGINE_C_ASSERT(basic_eq(ans, one));

    basic_sec(ans, pi);
    SYMENGINE_C_ASSERT(basic_eq(ans, minus_one));

    basic_cot(ans, pi_div_four);
    SYMENGINE_C_ASSERT(basic_eq(ans, one));

    basic_asin(ans, one);
    SYMENGINE_C_ASSERT(basic_eq(ans, pi_div_two));

    basic_acos(ans, one);
    SYMENGINE_C_ASSERT(basic_eq(ans, zero));

    basic_atan(ans, one);
    SYMENGINE_C_ASSERT(basic_eq(ans, pi_div_four));

    basic_acot(ans, one);
    SYMENGINE_C_ASSERT(basic_eq(ans, pi_div_four));

    basic_acsc(ans, one);
    SYMENGINE_C_ASSERT(basic_eq(ans, pi_div_two));

    basic_asec(ans, one);
    SYMENGINE_C_ASSERT(basic_eq(ans, zero));

    basic_sinh(ans, zero);
    SYMENGINE_C_ASSERT(basic_eq(ans, zero));

    basic_cosh(ans, zero);
    SYMENGINE_C_ASSERT(basic_eq(ans, one));

    basic_tanh(ans, zero);
    SYMENGINE_C_ASSERT(basic_eq(ans, zero));

    basic_csch(ans, one);
    s = basic_str(ans);
    SYMENGINE_C_ASSERT(strcmp(s, "csch(1)") == 0);
    basic_str_free(s);

    basic_sech(ans, zero);
    SYMENGINE_C_ASSERT(basic_eq(ans, one));

    basic_coth(ans, one);
    s = basic_str(ans);
    SYMENGINE_C_ASSERT(strcmp(s, "coth(1)") == 0);
    basic_str_free(s);

    basic_asinh(ans, zero);
    SYMENGINE_C_ASSERT(basic_eq(ans, zero));

    basic_acosh(ans, one);
    SYMENGINE_C_ASSERT(basic_eq(ans, zero));

    basic_atanh(ans, zero);
    SYMENGINE_C_ASSERT(basic_eq(ans, zero));

    basic_acsch(ans, one);
    s = basic_str(ans);
    SYMENGINE_C_ASSERT(strcmp(s, "log(1 + sqrt(2))") == 0);
    basic_str_free(s);

    basic_asech(ans, one);
    SYMENGINE_C_ASSERT(basic_eq(ans, zero));

    basic_acoth(ans, one);
    s = basic_str(ans);
    SYMENGINE_C_ASSERT(strcmp(s, "acoth(1)") == 0);
    basic_str_free(s);

    basic_lambertw(ans, e_minus_one);
    SYMENGINE_C_ASSERT(basic_eq(ans, minus_one));

    basic_zeta(ans, zero);
    SYMENGINE_C_ASSERT(basic_eq(ans, minus_half));

    basic_dirichlet_eta(ans, one);
    s = basic_str(ans);
    SYMENGINE_C_ASSERT(strcmp(s, "log(2)") == 0);
    basic_str_free(s);

    basic_gamma(ans, one);
    SYMENGINE_C_ASSERT(basic_eq(ans, one));

    basic_free_stack(ans);
    basic_free_stack(pi);
    basic_free_stack(two);
    basic_free_stack(pi_div_two);
    basic_free_stack(four);
    basic_free_stack(pi_div_four);
    basic_free_stack(one);
    basic_free_stack(minus_one);
    basic_free_stack(zero);
    basic_free_stack(e);
    basic_free_stack(e_minus_one);
    basic_free_stack(minus_half);
}

void test_ntheory()
{
    basic x, i1, i2, i4, i5;
    basic_new_stack(x);
    basic_new_stack(i1);
    basic_new_stack(i2);
    basic_new_stack(i4);
    basic_new_stack(i5);

    integer_set_si(i1, 1);
    integer_set_si(i2, 2);
    integer_set_si(i4, 4);
    integer_set_si(i5, 5);

    ntheory_gcd(x, i2, i4);
    SYMENGINE_C_ASSERT(basic_eq(x, i2));

    ntheory_lcm(x, i2, i4);
    SYMENGINE_C_ASSERT(basic_eq(x, i4));

    ntheory_nextprime(x, i4);
    SYMENGINE_C_ASSERT(basic_eq(x, i5));

    ntheory_mod(x, i5, i4);
    SYMENGINE_C_ASSERT(basic_eq(x, i1));

    ntheory_quotient(x, i5, i2);
    SYMENGINE_C_ASSERT(basic_eq(x, i2));

    ntheory_fibonacci(x, 5);
    SYMENGINE_C_ASSERT(basic_eq(x, i5));

    ntheory_lucas(x, 1);
    SYMENGINE_C_ASSERT(basic_eq(x, i1));

    ntheory_binomial(x, i5, 1);
    SYMENGINE_C_ASSERT(basic_eq(x, i5));

    basic_free_stack(x);
    basic_free_stack(i1);
    basic_free_stack(i2);
    basic_free_stack(i4);
    basic_free_stack(i5);
}

void test_eval()
{
    basic sin2, eval;
    basic_new_stack(sin2);
    basic_new_stack(eval);

    integer_set_si(sin2, 2);
    basic_sin(sin2, sin2);
    basic_evalf(eval, sin2, 53, 1);
    SYMENGINE_C_ASSERT(basic_get_type(eval) == SYMENGINE_REAL_DOUBLE);
    double d = 0.909297;
    double d2 = real_double_get_d(eval);
    d = fabs(d - d2);
    d2 = 0.000001;

    SYMENGINE_C_ASSERT(d < d2);

    basic_free_stack(sin2);

#ifdef HAVE_SYMENGINE_MPFR
    basic s, t, r, eval2;
    basic_new_stack(s);
    basic_new_stack(t);
    basic_new_stack(r);
    basic_new_stack(eval2);

    basic_const_pi(s);
    integer_set_str(t, "1963319607");
    basic_mul(s, s, t);
    integer_set_str(t, "6167950454");
    basic_sub(r, s, t);
    // value of `r` is approximately 0.000000000149734291

    basic_evalf(eval2, r, 53, 1);
    SYMENGINE_C_ASSERT(basic_get_type(eval2) == SYMENGINE_REAL_DOUBLE);
    // With 53 bit precision, `s` and `t` have the same value.
    // Hence value of `r` was  rounded down to `0.000000000000000`
    SYMENGINE_C_ASSERT(real_double_get_d(eval2) == 0.0);

    basic_evalf(eval2, r, 100, 1);
    SYMENGINE_C_ASSERT(basic_get_type(eval2) == SYMENGINE_REAL_MPFR);
    // With 100 bit precision, `s` and `t` are not equal in value.
    // Value of `r` is a positive quantity with value 0.000000000149734291.....
    SYMENGINE_C_ASSERT(real_mpfr_is_zero(eval2) == 0);

    basic_free_stack(s);
    basic_free_stack(t);
    basic_free_stack(r);
    basic_free_stack(eval2);
#endif // HAVE_SYMENGINE_MPFR

    basic imag, n1, n2, temp;
    basic_new_stack(imag);
    basic_new_stack(n1);
    basic_new_stack(n2);
    basic_new_stack(temp);

    basic_const_I(imag);
    integer_set_ui(n1, 4);
    basic_sin(n1, n1);
    integer_set_ui(temp, 3);
    basic_sin(temp, temp);
    basic_mul(temp, temp, imag);
    basic_add(n1, n1, temp);
    // n1 = sin(4) + sin(3)i

    integer_set_ui(n2, 2);
    basic_sin(n2, n2);
    integer_set_ui(temp, 7);
    basic_sin(temp, temp);
    basic_mul(temp, temp, imag);
    basic_add(n2, n2, temp);
    // n2 = sin(2) + sin(7)i

    basic_mul(n1, n1, n2);
    // n1 = (sin(4) + sin(3)i) * (sin(2) + sin(7)i)

    basic_evalf(eval, n1, 53, 0);
    SYMENGINE_C_ASSERT(basic_get_type(eval) == SYMENGINE_COMPLEX_DOUBLE);
    d = -0.780872515;
    complex_double_real_part(temp, eval);
    d2 = real_double_get_d(temp);
    complex_double_imaginary_part(temp, eval);
    double d3 = real_double_get_d(temp);
    double d4 = -0.3688890370;
    d = fabs(d - d2);
    d4 = fabs(d4 - d3);

    d2 = 0.000001;

    SYMENGINE_C_ASSERT(d < d2 && d4 < d2);

    basic_free_stack(eval);
    basic_free_stack(n1);
    basic_free_stack(n2);

#ifdef HAVE_SYMENGINE_MPC
    basic s1, t1, r1, eval3, com1, com2;
    basic_new_stack(s1);
    basic_new_stack(t1);
    basic_new_stack(r1);
    basic_new_stack(eval3);
    basic_new_stack(com1);
    basic_new_stack(com2);

    basic_const_pi(s1);
    integer_set_str(t1, "1963319607");
    basic_mul(s1, s1, t1);
    basic_mul(com1, s1, imag);
    basic_add(com1, com1, s1);
    integer_set_str(t1, "6167950454");
    basic_mul(com2, t1, imag);
    basic_add(com2, com2, t1);

    basic_sub(r1, com1, com2);
    // value of `r1` is approximately 0.000000000149734291 +
    // 0.000000000149734291i

    basic_evalf(eval3, r1, 53, 0);
    SYMENGINE_C_ASSERT(basic_get_type(eval3) == SYMENGINE_COMPLEX_DOUBLE);

    // With 53 bit precision, `com1` and `com2` have the same value.
    // Hence value of `r1` was  rounded down to `0.000000000000000`
    complex_double_real_part(temp, eval3);
    SYMENGINE_C_ASSERT(real_double_get_d(temp) == 0.0);
    complex_double_imaginary_part(temp, eval3);
    SYMENGINE_C_ASSERT(real_double_get_d(temp) == 0.0);

    basic_evalf(eval3, r1, 100, 0);
    SYMENGINE_C_ASSERT(basic_get_type(eval3) == SYMENGINE_COMPLEX_MPC);
    // With 100 bit precision, `com1` and `com2` are not equal in value.
    // Value of `r1` is a positive quantity with value 0.000000000149734291.....

    SYMENGINE_C_ASSERT(complex_mpc_is_zero(eval3) == 0);

    basic_free_stack(s1);
    basic_free_stack(t1);
    basic_free_stack(r1);
    basic_free_stack(eval3);
    basic_free_stack(com1);
    basic_free_stack(com2);
#endif // HAVE_SYMENGINE_MPC

    basic_free_stack(temp);
    basic_free_stack(imag);
}

void test_matrix()
{
    CDenseMatrix *A = dense_matrix_new();
    SYMENGINE_C_ASSERT(is_a_DenseMatrix(A));
    dense_matrix_free(A);

    basic i1, i2, i3, i4;
    basic_new_stack(i1);
    basic_new_stack(i2);
    basic_new_stack(i3);
    basic_new_stack(i4);

    integer_set_ui(i1, 1);
    integer_set_ui(i2, 2);
    integer_set_ui(i3, 3);
    integer_set_ui(i4, 4);

    CVecBasic *vec = vecbasic_new();
    vecbasic_push_back(vec, i1);
    vecbasic_push_back(vec, i2);
    vecbasic_push_back(vec, i3);
    vecbasic_push_back(vec, i4);

    CDenseMatrix *B = dense_matrix_new_vec(2, 2, vec);
    SYMENGINE_C_ASSERT(is_a_DenseMatrix(B));
    vecbasic_free(vec);

    dense_matrix_get_basic(i4, B, 0, 0);
    SYMENGINE_C_ASSERT(is_a_Integer(i4));
    SYMENGINE_C_ASSERT(integer_get_ui(i4) == 1);

    dense_matrix_get_basic(i3, B, 0, 1);
    SYMENGINE_C_ASSERT(is_a_Integer(i3));
    SYMENGINE_C_ASSERT(integer_get_ui(i3) == 2);

    dense_matrix_get_basic(i2, B, 1, 0);
    SYMENGINE_C_ASSERT(is_a_Integer(i2));
    SYMENGINE_C_ASSERT(integer_get_ui(i2) == 3);

    dense_matrix_get_basic(i1, B, 1, 1);
    SYMENGINE_C_ASSERT(is_a_Integer(i1));
    SYMENGINE_C_ASSERT(integer_get_ui(i1) == 4);

    integer_set_ui(i1, 5);

    dense_matrix_set_basic(B, 0, 0, i1);

    dense_matrix_get_basic(i4, B, 0, 0);
    SYMENGINE_C_ASSERT(is_a_Integer(i4));
    SYMENGINE_C_ASSERT(integer_get_ui(i4) == 5);

    // Equality
    SYMENGINE_C_ASSERT(dense_matrix_eq(B, B) == 1);

    // Inverse

    vec = vecbasic_new();

    integer_set_ui(i4, 4);
    integer_set_ui(i3, 3);
    integer_set_ui(i2, 2);

    vecbasic_push_back(vec, i4);
    vecbasic_push_back(vec, i3);
    vecbasic_push_back(vec, i3);
    vecbasic_push_back(vec, i2);

    CDenseMatrix *C = dense_matrix_new_vec(2, 2, vec);
    vecbasic_free(vec);

    vec = vecbasic_new();

    integer_set_si(i4, -4);
    integer_set_si(i2, -2);

    vecbasic_push_back(vec, i2);
    vecbasic_push_back(vec, i3);
    vecbasic_push_back(vec, i3);
    vecbasic_push_back(vec, i4);

    CDenseMatrix *D = dense_matrix_new_vec(2, 2, vec);

    CDenseMatrix *E = dense_matrix_new();
    dense_matrix_inv(E, C);

    SYMENGINE_C_ASSERT(dense_matrix_eq(E, D) == 1);

    int r = dense_matrix_rows(E);
    int c = dense_matrix_cols(E);

    SYMENGINE_C_ASSERT(r == 2);
    SYMENGINE_C_ASSERT(c == 2);

    // matrix addition
    dense_matrix_add_matrix(C, E, D);

    char *result = dense_matrix_str(C);
    char *expected = "[-4, 6]\n[6, -8]\n";
    SYMENGINE_C_ASSERT(strcmp(result, expected) == 0);
    basic_str_free(result);

    dense_matrix_transpose(C, B);
    result = dense_matrix_str(C);
    expected = "[5, 3]\n[2, 4]\n";
    // Transpose of [[5, 2],[3, 4]]

    SYMENGINE_C_ASSERT(strcmp(result, expected) == 0);
    basic_str_free(result);

    integer_set_ui(i1, 4);
    integer_set_ui(i2, 3);
    integer_set_ui(i3, 6);

    dense_matrix_set_basic(B, 0, 0, i1);
    dense_matrix_set_basic(B, 0, 1, i2);
    dense_matrix_set_basic(B, 1, 0, i3);
    dense_matrix_set_basic(B, 1, 1, i2);

    // LU decomposition
    dense_matrix_LU(C, D, B);

    result = dense_matrix_str(C);
    expected = "[1, 0]\n[3/2, 1]\n";

    SYMENGINE_C_ASSERT(strcmp(result, expected) == 0);
    basic_str_free(result);

    result = dense_matrix_str(D);
    expected = "[4, 3]\n[0, -3/2]\n";

    SYMENGINE_C_ASSERT(strcmp(result, expected) == 0);
    basic_str_free(result);

    // matrix multiplication
    dense_matrix_mul_matrix(E, C, D);
    SYMENGINE_C_ASSERT(dense_matrix_eq(E, B) == 1);

    // scalar multiplication
    dense_matrix_mul_scalar(E, D, i2);
    //"[[4, 3],[0, -3/2]] * 3

    result = dense_matrix_str(E);
    expected = "[12, 9]\n[0, -9/2]\n";
    SYMENGINE_C_ASSERT(strcmp(result, expected) == 0);
    basic_str_free(result);

    // scalar addition
    dense_matrix_add_scalar(E, D, i2);
    //"[[4, 3],[0, -3/2]] + 3

    result = dense_matrix_str(E);
    expected = "[7, 6]\n[3, 3/2]\n";
    SYMENGINE_C_ASSERT(strcmp(result, expected) == 0);
    basic_str_free(result);

    // LDL
    integer_set_ui(i1, 4);
    integer_set_ui(i2, 3);
    integer_set_ui(i3, 2);

    dense_matrix_set_basic(B, 0, 0, i1);
    dense_matrix_set_basic(B, 0, 1, i2);
    dense_matrix_set_basic(B, 1, 0, i2);
    dense_matrix_set_basic(B, 1, 1, i3);
    // B = [[4, 3],[3, 2]]

    // LDL decomposition
    // [[4, 3],[3, 2]] = [[1, 0],[3/4, 1]] * [[4, 0],[0, -1/4]] * [[1, 3/4],[0,
    // 1]]
    dense_matrix_LDL(C, D, B);
    // L : C, U : D

    result = dense_matrix_str(C);
    expected = "[1, 0]\n[3/4, 1]\n";
    SYMENGINE_C_ASSERT(strcmp(result, expected) == 0);
    basic_str_free(result);

    result = dense_matrix_str(D);
    expected = "[4, 0]\n[0, -1/4]\n";
    SYMENGINE_C_ASSERT(strcmp(result, expected) == 0);
    basic_str_free(result);

    // set matrix
    dense_matrix_set(C, D);
    SYMENGINE_C_ASSERT(dense_matrix_eq(D, C) == 1);

    dense_matrix_LDL(D, E, B);

    // now C should be equal to E, but different to D
    SYMENGINE_C_ASSERT(dense_matrix_eq(C, E) == 1);
    SYMENGINE_C_ASSERT(dense_matrix_eq(D, C) == 0);

    // submatrix
    dense_matrix_submatrix(C, B, 0, 0, 1, 0, 1, 1);
    result = dense_matrix_str(C);
    expected = "[4]\n[3]\n";
    SYMENGINE_C_ASSERT(strcmp(result, expected) == 0);
    basic_str_free(result);

    // LU_solve
    dense_matrix_set_basic(C, 1, 0, i1);
    dense_matrix_LU_solve(D, B, C);
    result = dense_matrix_str(D);
    expected = "[4]\n[-4]\n";
    SYMENGINE_C_ASSERT(strcmp(result, expected) == 0);
    basic_str_free(result);

    // Fractionfree LU
    dense_matrix_FFLU(C, B);
    result = dense_matrix_str(C);
    expected = "[4, 3]\n[3, -1]\n";
    SYMENGINE_C_ASSERT(strcmp(result, expected) == 0);
    basic_str_free(result);

    // FractionFree LDU
    dense_matrix_FFLDU(C, D, E, B);
    result = dense_matrix_str(C);
    expected = "[4, 0]\n[3, 1]\n";
    SYMENGINE_C_ASSERT(strcmp(result, expected) == 0);
    basic_str_free(result);

    result = dense_matrix_str(D);
    expected = "[4, 0]\n[0, 4]\n";
    SYMENGINE_C_ASSERT(strcmp(result, expected) == 0);
    basic_str_free(result);

    result = dense_matrix_str(E);
    expected = "[4, 3]\n[0, -1]\n";
    SYMENGINE_C_ASSERT(strcmp(result, expected) == 0);
    basic_str_free(result);

    // Num-py like functions

    // Ones
    dense_matrix_ones(D, 2, 3);
    result = dense_matrix_str(D);
    expected = "[1, 1, 1]\n[1, 1, 1]\n";
    SYMENGINE_C_ASSERT(strcmp(result, expected) == 0);
    basic_str_free(result);

    // Zeros
    dense_matrix_zeros(D, 3, 2);
    result = dense_matrix_str(D);
    expected = "[0, 0]\n[0, 0]\n[0, 0]\n";
    SYMENGINE_C_ASSERT(strcmp(result, expected) == 0);
    basic_str_free(result);

    // Diag
    dense_matrix_diag(D, vec, 0);
    result = dense_matrix_str(D);
    expected = "[-2, 0, 0, 0]\n[0, 3, 0, 0]\n[0, 0, 3, 0]\n[0, 0, 0, -4]\n";
    SYMENGINE_C_ASSERT(strcmp(result, expected) == 0);
    basic_str_free(result);

    // det
    dense_matrix_det(i1, D);
    SYMENGINE_C_ASSERT(integer_get_ui(i1) == 72);

    // eye
    dense_matrix_eye(D, 3, 4, 1);
    result = dense_matrix_str(D);
    expected = "[0, 1, 0, 0]\n[0, 0, 1, 0]\n[0, 0, 0, 1]\n";
    SYMENGINE_C_ASSERT(strcmp(result, expected) == 0);
    basic_str_free(result);

    // Num-py like functions

    // Ones
    dense_matrix_ones(D, 2, 3);
    result = dense_matrix_str(D);
    expected = "[1, 1, 1]\n[1, 1, 1]\n";
    SYMENGINE_C_ASSERT(strcmp(result, expected) == 0);
    basic_str_free(result);

    // Zeros
    dense_matrix_zeros(D, 3, 2);
    result = dense_matrix_str(D);
    expected = "[0, 0]\n[0, 0]\n[0, 0]\n";
    SYMENGINE_C_ASSERT(strcmp(result, expected) == 0);
    basic_str_free(result);

    // Diag
    dense_matrix_diag(D, vec, 0);
    result = dense_matrix_str(D);
    expected = "[-2, 0, 0, 0]\n[0, 3, 0, 0]\n[0, 0, 3, 0]\n[0, 0, 0, -4]\n";
    SYMENGINE_C_ASSERT(strcmp(result, expected) == 0);
    basic_str_free(result);

    // det
    dense_matrix_det(i1, D);
    SYMENGINE_C_ASSERT(integer_get_ui(i1) == 72);

    // eye
    dense_matrix_eye(D, 3, 4, 1);
    result = dense_matrix_str(D);
    expected = "[0, 1, 0, 0]\n[0, 0, 1, 0]\n[0, 0, 0, 1]\n";
    SYMENGINE_C_ASSERT(strcmp(result, expected) == 0);
    basic_str_free(result);

    vecbasic_free(vec);

    dense_matrix_free(B);
    dense_matrix_free(C);
    dense_matrix_free(D);
    dense_matrix_free(E);

    basic_free_stack(i1);
    basic_free_stack(i2);
    basic_free_stack(i3);
    basic_free_stack(i4);
}

int main(int argc, char *argv[])
{
    test_cwrapper();
    test_complex();
    test_complex_double();
    test_basic();
    test_CVectorInt1();
    test_CVectorInt2();
    test_CVecBasic();
    test_CSetBasic();
    test_CMapBasicBasic();
    test_get_args();
    test_free_symbols();
    test_get_type();
    test_hash();
    test_subs();
    test_subs2();
    test_constants();
    test_ascii_art();
    test_functions();
    test_ntheory();
    test_real_double();
    test_eval();
#ifdef HAVE_SYMENGINE_MPFR
    test_real_mpfr();
#endif // HAVE_SYMENGINE_MPFR
#ifdef HAVE_SYMENGINE_MPC
    test_complex_mpc();
#endif // HAVE_SYMENGINE_MPC
    symengine_print_stack_on_segfault();
    test_matrix();
    return 0;
}
