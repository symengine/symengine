#include <stdio.h>
#include "cwrapper.h"

void test_cwrapper() {
    char* x_ = "x";
    char* s;
    basic x = basic_new(), y = basic_new(), z = basic_new();
    basic_symbol(x, "x");
    basic_symbol(y, "y");
    basic_symbol(z, "z");
    
    s = basic_str(x);
    printf("Symbol : %s\n", s);
    basic e = basic_new();

    basic_integer_ui(e, 123);
    s = basic_str(e);
    printf("Integer : %s\n", s);

    basic_integer_ui(e, 456);
    basic_add(e, e, x);
    basic_mul(e, e, y);
    basic_div(e, e, z);
    printf("Basic : %s\n", basic_str(e));
    basic_diff(e, e, z);
    printf("Basic : %s\n", basic_str(e));

    basic_rational_ui(e, 100, 47);
    s = basic_str(e);

    printf("Rational : %s\n", s);
    printf("Is_a_Symbol %s: %d\n", s, is_a_Symbol(e));
    printf("Is_a_Rational %s: %d\n", s, is_a_Rational(e));
    printf("Is_a_Integer %s: %d\n", s, is_a_Integer(e));
}

int main(int argc, char* argv[])
{
    test_cwrapper();
    return 0;
}
