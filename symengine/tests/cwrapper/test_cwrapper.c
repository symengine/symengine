#include <stdio.h>
#include <assert.h>
#include <symengine/cwrapper.h>

void test_cwrapper() {
    char* s;
    basic x, y, z;
    basic_init(x);
    basic_init(y);
    basic_init(z);
    symbol_set(x, "x");
    symbol_set(y, "y");
    symbol_set(z, "z");

    s = basic_str(x);
    printf("Symbol : %s\n", s);
    basic_str_free(s);
    basic e;
    basic_init(e);

    integer_set_ui(e, 123);
    s = basic_str(e);
    printf("Integer : %s\n", s);
    basic_str_free(s);

    integer_set_ui(e, 456);
    basic_add(e, e, x);
    basic_mul(e, e, y);
    basic_div(e, e, z);
    s = basic_str(e);
    printf("Basic : %s\n", s);
    basic_str_free(s);

    basic_diff(e, e, z);
    s = basic_str(e);
    printf("Basic : %s\n", s);
    basic_str_free(s);

    rational_set_ui(e, 100, 47);
    s = basic_str(e);

    printf("Rational : %s\n", s);
    printf("Is_a_Symbol %s: %d\n", s, is_a_Symbol(e));
    printf("Is_a_Rational %s: %d\n", s, is_a_Rational(e));
    printf("Is_a_Integer %s: %d\n", s, is_a_Integer(e));

    integer_set_ui(e, 123);
    printf("integer_get_ui 123: %lu\n", integer_get_ui(e));

    integer_set_si(e, -123);
    printf("integer_get_si -123: %ld\n", integer_get_si(e));

    mpz_t test;
    mpz_init(test);

    integer_get_mpz(test, e);
    printf("integer_get_mpz(e): %ld\n", mpz_get_ui(test));

    mpz_clear(test);
    basic_free(e);
    basic_free(x);
    basic_free(y);
    basic_free(z);
    basic_str_free(s);
}

void test_CVectorInt1()
{
    // Allocate on heap
    CVectorInt *vec = vectorint_new();
    vectorint_push_back(vec, 5);
    assert(vectorint_get(vec, 0) == 5);
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
    vec = (CVectorInt*)data1;
    assert(vectorint_placement_new(vec, sizeof(data1)) == 2);

    struct X data2[1];  // Aligned properly but small
    vec = (CVectorInt*)data2;
    assert(vectorint_placement_new(vec, sizeof(data2)) == 1);

    char data3[50]; // Aligned properly and enough size to fit std::vector<int>
    vec = (CVectorInt*)data3;
    assert(vectorint_placement_new(vec, 1) == 1);
    assert(vectorint_placement_new(vec, 2) == 1);
    assert(vectorint_placement_new(vec, sizeof(data3)) == 0);
    vectorint_push_back(vec, 5);
    assert(vectorint_get(vec, 0) == 5);
}

void test_CVecBasic()
{
    CVecBasic *vec = vecbasic_new();

    basic x;
    basic_init(x);
    symbol_set(x, "x");
    vecbasic_push_back(vec, x);

    basic y;
    basic_init(y);
    vecbasic_get(vec, 0, y);

    // TODO: enable this once basic_eq() is implemented
    // assert(basic_eq(x, y));

    vecbasic_free(vec);
}

void test_get_args()
{
    basic x, y, z, e;
    basic_init(x);
    basic_init(y);
    basic_init(z);
    basic_init(e);
    symbol_set(x, "x");
    symbol_set(y, "y");
    symbol_set(z, "z");

    integer_set_ui(e, 123);
    basic_add(e, e, x);
    basic_mul(e, e, y);
    basic_div(e, e, z);

    CVecBasic *args = vecbasic_new();
    basic_get_args(e, args);
    assert(vecbasic_size(args) == 3);
    vecbasic_free(args);

    basic_free(e);
    basic_free(x);
    basic_free(y);
    basic_free(z);
}

int main(int argc, char* argv[])
{
    test_cwrapper();
    test_CVectorInt1();
    test_CVectorInt2();
    test_CVecBasic();
    test_get_args();
    return 0;
}
