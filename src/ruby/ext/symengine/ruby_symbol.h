#include "ruby.h"
#include "ruby_basic.h"
#include "cwrapper.h"

void csymbol_free(void *ptr);

VALUE csymbol_alloc(VALUE klass);

VALUE csymbol_init(VALUE self, VALUE name);
