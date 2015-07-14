#include "ruby_symbol.h"

VALUE csymbol_init(VALUE self, VALUE name) {
    Check_Type(name, T_STRING);
    basic_struct *this;
    char *str_ptr = StringValueCStr(name);

    Data_Get_Struct(self, basic_struct, this);

    symbol_set(this, str_ptr);

    return self;
}
