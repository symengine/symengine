#ifndef RUBY_BASIC_H_
#define RUBY_BASIC_H_

#include <ruby.h>
#include <symengine/cwrapper.h>

void cbasic_free(void *ptr);

VALUE alloc_func(VALUE klass, void(*free_func_ptr)(void*));

VALUE cbasic_alloc(VALUE klass);

VALUE cbasic_init(VALUE self);

VALUE cbasic_add(VALUE self, VALUE operand2);

VALUE cbasic_sub(VALUE self, VALUE operand2);

VALUE cbasic_mul(VALUE self, VALUE operand2);

VALUE cbasic_div(VALUE self, VALUE operand2);

VALUE cbasic_pow(VALUE self, VALUE operand2);

VALUE cbasic_eq(VALUE self, VALUE operand2);

VALUE cbasic_neq(VALUE self, VALUE operand2);

VALUE cbasic_neg(VALUE self);

VALUE cbasic_to_str(VALUE self);

VALUE cbasic_expand(VALUE self);

#endif //RUBY_BASIC_H_
