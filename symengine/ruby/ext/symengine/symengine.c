#include "ruby_basic.h"
#include "ruby_symbol.h"
#include "ruby_integer.h"
#include "ruby_rational.h"
#include "symengine.h"

///////////////////
// Ruby Bindings //
///////////////////

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
    rb_define_method(c_basic, "diff", cbasic_diff, 1);
    rb_define_method(c_basic, "==", cbasic_eq, 1);
    rb_define_method(c_basic, "eql?", cbasic_eq, 1);
    rb_define_method(c_basic, "!=", cbasic_neq, 1);
    rb_define_method(c_basic, "-@", cbasic_neg, 0);
    rb_define_method(c_basic, "to_s", cbasic_to_str, 0);
    rb_define_method(c_basic, "expand", cbasic_expand, 0);
    rb_define_method(c_basic, "args", cbasic_get_args, 0);
    rb_define_protected_method(c_basic, "pr_free_symbols", cbasic_free_symbols, 0);
    rb_define_method(c_basic, "hash", cbasic_hash, 0);
    rb_define_method(c_basic, "subs", cbasic_subs, -1);
    rb_define_method(c_basic, "coerce", cbasic_coerce, 1);

    //Symbol class
    c_symbol = rb_define_class_under(m_symengine, "Symbol", c_basic);
    rb_define_alloc_func(c_symbol, cbasic_alloc);
    rb_define_method(c_symbol, "initialize", csymbol_init, 1);

    //Integer class
    c_integer = rb_define_class_under(m_symengine, "Integer", c_basic);
    rb_define_alloc_func(c_integer, cbasic_alloc);
    rb_define_method(c_integer, "initialize", cinteger_init, 1);

    //Rational class
    c_rational = rb_define_class_under(m_symengine, "Rational", c_basic);
    rb_define_alloc_func(c_rational, cbasic_alloc);
    rb_define_method(c_rational, "initialize", crational_init, 1);

    //Function class
    c_function = rb_define_class_under(m_symengine, "Function", c_basic);
    rb_define_alloc_func(c_function, cbasic_alloc);

    //Trigfuntion class
    c_trigfunction = rb_define_class_under(m_symengine, "Trigfuntion", c_function);
    rb_define_alloc_func(c_trigfunction, cbasic_alloc);

    //Sin class
    c_sin = rb_define_class_under(m_symengine, "Sin", c_trigfunction);
    rb_define_alloc_func(c_sin, cbasic_alloc);
    rb_define_method(c_sin, "initialize", csin_init, 1);

    //Cos class
    c_cos = rb_define_class_under(m_symengine, "Cos", c_trigfunction);
    rb_define_alloc_func(c_cos, cbasic_alloc);
    rb_define_method(c_cos, "initialize", ccos_init, 1);

    //Tan class
    c_tan = rb_define_class_under(m_symengine, "Tan", c_trigfunction);
    rb_define_alloc_func(c_tan, cbasic_alloc);
    rb_define_method(c_tan, "initialize", ctan_init, 1);

    //Cot class
    c_cot = rb_define_class_under(m_symengine, "Cot", c_trigfunction);
    rb_define_alloc_func(c_cot, cbasic_alloc);
    rb_define_method(c_cot, "initialize", ccot_init, 1);

    //Csc class
    c_csc = rb_define_class_under(m_symengine, "Csc", c_trigfunction);
    rb_define_alloc_func(c_csc, cbasic_alloc);
    rb_define_method(c_csc, "initialize", ccsc_init, 1);

    //Sec class
    c_sec = rb_define_class_under(m_symengine, "Sec", c_trigfunction);
    rb_define_alloc_func(c_sec, cbasic_alloc);
    rb_define_method(c_sec, "initialize", csec_init, 1);

}
