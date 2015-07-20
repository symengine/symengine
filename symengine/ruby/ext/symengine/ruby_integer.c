#include "ruby_integer.h"

VALUE cinteger_init(VALUE self, VALUE num_value) {
    basic_struct *this;

    Data_Get_Struct(self, basic_struct, this);
    switch(TYPE(num_value)) {
        case T_FIXNUM:
            integer_set_si(this, FIX2LONG(num_value));
            break;
        case T_BIGNUM:
            integer_set_ui(this, NUM2ULONG(num_value));
            break;
        default:
            rb_raise(rb_eTypeError, "Invalid Type: Fixnum or Bignum required");
            break;
    }

    return self;
}
