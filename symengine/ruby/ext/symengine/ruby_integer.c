#include "ruby_integer.h"

VALUE cinteger_init(VALUE self, VALUE num_value) {
    basic_struct *this;
    Data_Get_Struct(self, basic_struct, this);
    GET_SYMINTFROMVAL(num_value, this);
    return self;
}
