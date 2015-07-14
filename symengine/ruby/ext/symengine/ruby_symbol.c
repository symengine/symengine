#include "ruby_symbol.h"

void csymbol_free(void *ptr) {
    cbasic_free(ptr);
}

void csymbol_free_heap(void *ptr) {
    cbasic_free_heap(ptr);
}

VALUE csymbol_alloc(VALUE klass) {
    return alloc_func(klass, csymbol_free_heap);
}

VALUE csymbol_init(VALUE self, VALUE name) {
    Check_Type(name, T_STRING);
    basic_struct *this;
    char *str_ptr = StringValueCStr(name);

    Data_Get_Struct(self, basic_struct, this);

    symbol_set(this, str_ptr);

    return self;
}
