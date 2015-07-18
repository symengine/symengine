#include "ruby_basic.h"
#include "ruby_symbol.h"
#include "ruby_integer.h"
#include "ruby_rational.h"
///////////////////
// Ruby Bindings //
///////////////////

//variable name for a module starts with m
static VALUE m_symengine;

//variable names for classes begin with c
static VALUE c_basic;
static VALUE c_symbol;
static VALUE c_integer;
static VALUE c_rational;

void Init_symengine() {
    m_symengine = rb_define_module("SymEngine");

    //Basic class
    c_basic = rb_define_class_under(m_symengine, "Basic", rb_cObject);
    rb_define_alloc_func(c_basic, cbasic_alloc);
    rb_define_method(c_basic, "+", cbasic_add, 1);
    rb_define_method(c_basic, "-", cbasic_sub, 1);
    rb_define_method(c_basic, "*", cbasic_mul, 1);
    rb_define_method(c_basic, "/", cbasic_div, 1);
    rb_define_method(c_basic, "**", cbasic_pow, 1);
    rb_define_method(c_basic, "==", cbasic_eq, 1);
    rb_define_method(c_basic, "eql?", cbasic_eq, 1);
    rb_define_method(c_basic, "!=", cbasic_neq, 1);
    rb_define_method(c_basic, "-@", cbasic_neg, 0);
    rb_define_method(c_basic, "to_s", cbasic_to_str, 0);
    rb_define_method(c_basic, "expand", cbasic_expand, 0);
    rb_define_method(c_basic, "args", cbasic_get_args, 0);
    rb_define_protected_method(c_basic, "pr_free_symbols", cbasic_free_symbols, 0);
    rb_define_method(c_basic, "hash", cbasic_hash, 0);

    //Symbol class
    c_symbol = rb_define_class_under(m_symengine, "Symbol", c_basic);
    rb_define_alloc_func(c_symbol, cbasic_alloc);
    rb_define_method(c_symbol, "initialize", csymbol_init, 1);

    //Integer class
    c_integer = rb_define_class_under(m_symengine, "Integer", c_basic);
    rb_define_alloc_func(c_integer, cbasic_alloc);

    //Rational class
    c_rational = rb_define_class_under(m_symengine, "Rational", c_basic);
    rb_define_alloc_func(c_rational, cbasic_alloc);
}
