#include "ruby_basic.h"
///////////////////
// Ruby Bindings //
///////////////////

//variable name for a variable starts with m
static VALUE m_symengine;

//variable names for classes begin with c
static VALUE c_basic;

void Init_symengine() {
    m_symengine = rb_define_module("SymEngine");

    //Basic class
    c_basic = rb_define_class_under(m_symengine, "Basic", rb_cObject);
    rb_define_alloc_func(c_basic,cbasic_alloc);
    rb_define_method(c_basic, "initialize", cbasic_init, 0);
    rb_define_method(c_basic, "+", cbasic_add, 1);
    rb_define_method(c_basic, "-", cbasic_sub, 1);
    rb_define_method(c_basic, "*", cbasic_mul, 1);
    rb_define_method(c_basic, "/", cbasic_div, 1);
    rb_define_method(c_basic, "**", cbasic_pow, 1);
    rb_define_method(c_basic, "-@", cbasic_neg, 0);
}
