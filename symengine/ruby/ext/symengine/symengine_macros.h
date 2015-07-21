#ifndef SYMENGINE_MACROS_H_
#define SYMENGINE_MACROS_H_

#include "ruby.h"
#include "symengine/cwrapper.h"

VALUE Klass_of_Basic(const basic_struct *basic_ptr);

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
    }

#endif //SYMENGINE_MACROS_H_
