#ifndef RUBY_SYMBOL_H_
#define RUBY_SYMBOL_H_

#include "ruby_basic.h"

void csymbol_free(void *ptr);

VALUE csymbol_alloc(VALUE klass);

VALUE csymbol_init(VALUE self, VALUE name);

#endif //RUBY_SYMBOL_H_
