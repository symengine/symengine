#include <stdio.h>
#include "cwrapper.h"

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

int main(int argc, char* argv[])
{
    test_cwrapper();
    return 0;
}
