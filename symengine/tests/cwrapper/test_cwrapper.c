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

void test_basic() {
    basic x;
    basic_init(x);
    symbol_set(x, "x");

    basic_struct *y = basic_new_heap();
    symbol_set(y, "x");

    // TODO: enable this once basic_eq() is implemented
    //SYMENGINE_C_ASSERT(basic_eq(x, y))

    basic_free(x);
    basic_free_heap(y);
}

void test_CVectorInt1()
{
    // Allocate on heap
    CVectorInt *vec = vectorint_new();
    vectorint_push_back(vec, 5);
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
    basic_init(x);
    symbol_set(x, "x");
    vecbasic_push_back(vec, x);

    SYMENGINE_C_ASSERT(vecbasic_size(vec) == 1);

    basic y;
    basic_init(y);
    vecbasic_get(vec, 0, y);

    SYMENGINE_C_ASSERT(basic_eq(x, y));

    vecbasic_free(vec);
    basic_free(x);
    basic_free(y);
}

void test_CSetBasic()
{
    CSetBasic *set = setbasic_new();
    SYMENGINE_C_ASSERT(setbasic_size(set) == 0);

    basic x;
    basic_init(x);
    symbol_set(x, "x");

    int has_insert;
    has_insert = setbasic_insert(set, x);
    SYMENGINE_C_ASSERT(has_insert == 1);
    SYMENGINE_C_ASSERT(setbasic_size(set) == 1);

    has_insert = setbasic_insert(set, x);
    SYMENGINE_C_ASSERT(has_insert == 0);

    basic y;
    basic_init(y);
    symbol_set(y, "y");

    int is_found;
    is_found = setbasic_find(set, x);
    SYMENGINE_C_ASSERT(is_found == 1);

    is_found = setbasic_find(set, y);
    SYMENGINE_C_ASSERT(is_found == 0);

    setbasic_get(set, 0, y);
    SYMENGINE_C_ASSERT(basic_eq(x, y));

    setbasic_free(set);
    basic_free(x);
    basic_free(y);
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
    SYMENGINE_C_ASSERT(vecbasic_size(args) == 3);
    vecbasic_free(args);

    basic_free(e);
    basic_free(x);
    basic_free(y);
    basic_free(z);
}

void test_free_symbols()
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
    basic_pow(e, e, y);
    basic_div(e, e, z);

    CSetBasic *symbols = setbasic_new();
    basic_free_symbols(e, symbols);
    SYMENGINE_C_ASSERT(setbasic_size(symbols) == 3);
    setbasic_free(symbols);

    basic_free(e);
    basic_free(x);
    basic_free(y);
    basic_free(z);
}

void test_get_type() {
    basic x, y;
    basic_init(x);
    basic_init(y);
    symbol_set(x, "x");
    integer_set_ui(y, 123);

    SYMENGINE_C_ASSERT(basic_get_type(x) == SYMENGINE_SYMBOL);
    SYMENGINE_C_ASSERT(basic_get_type(y) == SYMENGINE_INTEGER);

    basic_free(x);
    basic_free(y);
}

int main(int argc, char* argv[])
{
    test_cwrapper();
    test_basic();
    test_CVectorInt1();
    test_CVectorInt2();
    test_CVecBasic();
    test_CSetBasic();
    test_get_args();
    test_free_symbols();
    test_get_type();
    return 0;
}
