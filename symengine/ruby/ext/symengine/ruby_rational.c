#include "ruby_rational.h"

VALUE crational_init(VALUE self, VALUE rat_value) {
    basic_struct *this, *num_basic, *den_basic;
    num_basic = basic_new_heap();
    den_basic = basic_new_heap();

    Data_Get_Struct(self, basic_struct, this);

    VALUE num, den;
    num = rb_funcallv_public(rat_value, rb_intern("numerator"), 0, NULL);
    den = rb_funcallv_public(rat_value, rb_intern("denominator"), 0, NULL);

    GET_SYMINTFROMVAL(num, num_basic);
    GET_SYMINTFROMVAL(den, den_basic);

    rational_set(this, num_basic, den_basic);
    basic_free_heap(num_basic);
    basic_free_heap(den_basic);
    return self;
}
