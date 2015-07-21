#ifndef RUBY_BASIC_H_
#define RUBY_BASIC_H_

#include <ruby.h>
#include <symengine/cwrapper.h>

#include "symengine_macros.h"

void cbasic_free(void *ptr);

void cbasic_free_heap(void *ptr);

VALUE cbasic_alloc(VALUE klass);

VALUE cbasic_binary_op(VALUE self, VALUE operand2, void (*cwfunc_ptr)(basic_struct*, const basic_struct*, const basic_struct*));

VALUE cbasic_unary_op(VALUE self, void (*cwfunc_ptr)(basic_struct*, const basic_struct*));

VALUE cbasic_add(VALUE self, VALUE operand2);

VALUE cbasic_sub(VALUE self, VALUE operand2);

VALUE cbasic_mul(VALUE self, VALUE operand2);

VALUE cbasic_div(VALUE self, VALUE operand2);

VALUE cbasic_pow(VALUE self, VALUE operand2);

VALUE cbasic_eq(VALUE self, VALUE operand2);

VALUE cbasic_neq(VALUE self, VALUE operand2);

VALUE cbasic_neg(VALUE self);

VALUE cbasic_get_args(VALUE self);

VALUE cbasic_free_symbols(VALUE self);

VALUE cbasic_to_str(VALUE self);

VALUE cbasic_expand(VALUE self);

VALUE cbasic_hash(VALUE self);

#endif //RUBY_BASIC_H_
