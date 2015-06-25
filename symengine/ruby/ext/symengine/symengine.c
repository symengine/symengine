#include "ruby_basic.h"
#include "ruby_symbol.h"
///////////////////
// Ruby Bindings //
///////////////////

//variable name for a module starts with m
static VALUE m_symengine;

//variable names for classes begin with c
static VALUE c_basic;
static VALUE c_symbol;

void Init_symengine() {
    m_symengine = rb_define_module("SymEngine");

    //Basic class
    c_basic = rb_define_class_under(m_symengine, "Basic", rb_cObject);
    rb_define_alloc_func(c_basic, cbasic_alloc);
    rb_define_method(c_basic, "initialize", cbasic_init, 0);
    rb_define_method(c_basic, "+", cbasic_add, 1);
    rb_define_method(c_basic, "-", cbasic_sub, 1);
    rb_define_method(c_basic, "*", cbasic_mul, 1);
    rb_define_method(c_basic, "/", cbasic_div, 1);
    rb_define_method(c_basic, "**", cbasic_pow, 1);
    rb_define_method(c_basic, "==", cbasic_eq, 1);
    rb_define_method(c_basic, "-@", cbasic_neg, 0);
    rb_define_method(c_basic, "to_s", cbasic_to_str, 0);
    rb_define_method(c_basic, "expand", cbasic_expand, 0);

    //Symbol class
    c_symbol = rb_define_class_under(m_symengine, "Symbol", c_basic);
    rb_define_alloc_func(c_symbol, csymbol_alloc);
    rb_define_method(c_symbol, "initialize", csymbol_init, 1);
}
