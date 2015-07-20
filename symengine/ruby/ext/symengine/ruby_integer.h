#ifndef RUBY_INTEGER_H_
#define RUBY_INTEGER_H_

#include "ruby_basic.h"

#define GET_SYMINTFROMVAL(num_value, this) \
    switch(TYPE(num_value)) { \
        case T_FIXNUM: \
            integer_set_si(this, FIX2LONG(num_value)); \
            break; \
        case T_BIGNUM: \
            integer_set_ui(this, NUM2ULONG(num_value)); \
            break; \
        default: \
            rb_raise(rb_eTypeError, "Invalid Type: Fixnum or Bignum required"); \
            break; \
    } \

VALUE cinteger_init(VALUE self, VALUE num_value);

#endif //RUBY_INTEGER_H_
