#include <symengine/symengine_config.h>

#if defined(HAVE_C_FUNCTION_NOT_FUNC)
#define __func__ __FUNCTION__
#endif

#include <symengine/cwrapper.h>
#include <string.h>

void test_cwrapper() {
    char* s;
    basic x, y, z;
    basic_new_stack(x);
    basic_new_stack(y);
    basic_new_stack(z);
    symbol_set(x, "x");
    symbol_set(y, "y");
    symbol_set(z, "z");

    s = basic_str(x);
    SYMENGINE_C_ASSERT(strcmp(s, "x") == 0);
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

    integer_set_ui(e, 123);
    SYMENGINE_C_ASSERT(integer_get_ui(e) == 123);

    integer_set_si(e, -123);
    SYMENGINE_C_ASSERT(integer_get_si(e) == -123);

    mpz_t test;
    mpz_init(test);

    integer_get_mpz(test, e);
    SYMENGINE_C_ASSERT(mpz_get_ui(test) == 123);

    mpz_clear(test);
    basic_free_stack(e);
    basic_free_stack(x);
    basic_free_stack(y);
    basic_free_stack(z);
    basic_str_free(s);
}

void test_basic() {
    basic x;
    basic_new_stack(x);
    symbol_set(x, "x");

    basic_struct *y = basic_new_heap();
    symbol_set(y, "x");

    SYMENGINE_C_ASSERT(basic_eq(x, y))

    basic_free_heap(y);
    basic_free_stack(x);
}

void test_complex() {
    basic e;
    basic f;
    char* s;
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

    basic_free_stack(e);
    basic_free_stack(f);
}

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

    char data1[1];  // Not aligned properly
    SYMENGINE_C_ASSERT(vectorint_placement_new_check(data1, sizeof(data1)) ==1);

    struct X data2[1];  // Aligned properly but small
    SYMENGINE_C_ASSERT(vectorint_placement_new_check(data2, sizeof(data2)) ==1);

    struct X data3[50]; // Aligned properly and enough size to fit std::vector<int>
    SYMENGINE_C_ASSERT(vectorint_placement_new_check(data3, 1) == 1);
    SYMENGINE_C_ASSERT(vectorint_placement_new_check(data3, 2) == 1);
    SYMENGINE_C_ASSERT(vectorint_placement_new_check(data3, sizeof(data3)) == 0);
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

void test_get_type() {
    basic x, y;
    basic_new_stack(x);
    basic_new_stack(y);
    symbol_set(x, "x");
    integer_set_ui(y, 123);

    SYMENGINE_C_ASSERT(basic_get_type(x) == SYMENGINE_SYMBOL);
    SYMENGINE_C_ASSERT(basic_get_type(y) == SYMENGINE_INTEGER);

    basic_free_stack(x);
    basic_free_stack(y);
}

void test_hash() {
    basic x1, x2, y;
    basic_new_stack(x1);
    basic_new_stack(x2);
    basic_new_stack(y);
    symbol_set(x1, "x");
    symbol_set(x2, "x");
    symbol_set(y, "y");

    SYMENGINE_C_ASSERT(basic_hash(x1) == basic_hash(x2));
    if (basic_hash(x1) != basic_hash(y)) SYMENGINE_C_ASSERT(basic_neq(x1,y));

    basic_free_stack(x1);
    basic_free_stack(x2);
    basic_free_stack(y);
}

void test_subs2() {
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
    //e should be x*y*z

    basic_subs2(s, e, y, x);
    basic_subs2(s, s, z, x);
    //s should be x**3

    integer_set_si(z, 3);
    basic_pow(e, x, z);
    //e should be x**3

    SYMENGINE_C_ASSERT(basic_eq(s, e));

    basic_free_stack(s);
    basic_free_stack(e);
    basic_free_stack(x);
    basic_free_stack(y);
    basic_free_stack(z);
}

void test_subs() {
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
    //e should be x*y*z

    CMapBasicBasic *map = mapbasicbasic_new();
    mapbasicbasic_insert(map, y, x);
    mapbasicbasic_insert(map, z, x);
    basic_subs(s, e, map);
    //s should be x**3

    integer_set_si(z, 3);
    basic_pow(e, x, z);
    //e should be x**3

    SYMENGINE_C_ASSERT(basic_eq(s, e));

    mapbasicbasic_free(map);
    basic_free_stack(s);
    basic_free_stack(e);
    basic_free_stack(x);
    basic_free_stack(y);
    basic_free_stack(z);
}

void test_constants() {
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

    char* s;
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

int main(int argc, char* argv[])
{
    test_cwrapper();
    test_complex();
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
    return 0;
}
