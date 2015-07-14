#include "ruby_basic.h"

void cbasic_free(void *ptr){
    basic_struct *basic_ptr = ptr;
    basic_free(basic_ptr);
}

void cbasic_free_heap(void *ptr) {
    basic_struct *basic_ptr = ptr;
    basic_free_heap(basic_ptr);
}

VALUE alloc_func(VALUE klass, void(*free_func_ptr)(void *)) {
    VALUE obj;
    basic_struct *struct_ptr = basic_new_heap();

    obj = Data_Wrap_Struct(klass, NULL, free_func_ptr, struct_ptr);

    return obj;
}

VALUE cbasic_alloc(VALUE klass){
    return alloc_func(klass, cbasic_free_heap);
}

VALUE cbasic_init(VALUE self){
    return self;
}

VALUE cbasic_binary_op(VALUE self, VALUE operand2, void (*cwfunc_ptr)(basic_struct*, const basic_struct*, const basic_struct*)){
    basic_struct *this, *cbasic_operand2, *cresult;
    VALUE result;

    Data_Get_Struct(self, basic_struct, this);
    Data_Get_Struct(operand2, basic_struct, cbasic_operand2);

    cresult = basic_new_heap();
    cwfunc_ptr(cresult, this, cbasic_operand2);
    result = Data_Wrap_Struct(rb_obj_class(self), NULL , cbasic_free_heap, cresult);

    return result;
}

VALUE cbasic_unary_op(VALUE self, void (*cwfunc_ptr)(basic_struct*, const basic_struct*)){
    basic_struct *this, *cresult;
    VALUE result;

    Data_Get_Struct(self, basic_struct, this);

    cresult = basic_new_heap();
    cwfunc_ptr(cresult, this);
    result = Data_Wrap_Struct(rb_obj_class(self), NULL , cbasic_free_heap, cresult);

    return result;
}

VALUE cbasic_add(VALUE self, VALUE operand2){
    return cbasic_binary_op(self, operand2, basic_add);
}

VALUE cbasic_sub(VALUE self, VALUE operand2){
    return cbasic_binary_op(self, operand2, basic_sub);
}

VALUE cbasic_mul(VALUE self, VALUE operand2){
    return cbasic_binary_op(self, operand2, basic_mul);
}

VALUE cbasic_div(VALUE self, VALUE operand2){
    return cbasic_binary_op(self, operand2, basic_div);
}

VALUE cbasic_pow(VALUE self, VALUE operand2){
    return cbasic_binary_op(self, operand2, basic_pow);
}

VALUE cbasic_eq(VALUE self, VALUE operand2) {
    basic_struct *this, *cbasic_operand2;

    Data_Get_Struct(self, basic_struct, this);
    Data_Get_Struct(operand2, basic_struct, cbasic_operand2);

    return basic_eq(this, cbasic_operand2) ? Qtrue : Qfalse;
}

VALUE cbasic_neq(VALUE self, VALUE operand2) {
    basic_struct *this, *cbasic_operand2;

    Data_Get_Struct(self, basic_struct, this);
    Data_Get_Struct(operand2, basic_struct, cbasic_operand2);

    return basic_neq(this, cbasic_operand2) ? Qtrue : Qfalse;
}

VALUE cbasic_neg(VALUE self){
    return cbasic_unary_op(self, basic_neg);
}

VALUE cbasic_get_args(VALUE self) {
    basic_struct *this, *iterator_basic;
    CVecBasic *args = vecbasic_new();
    int size = 0;

    Data_Get_Struct(self, basic_struct, this);
    basic_get_args(this, args);

    size = vecbasic_size(args);
    VALUE ruby_array = rb_ary_new2(size);
    int i = 0;
    VALUE temp = NULL;
    for(i = 0; i < size; i++) {
        basic_struct *temp_basic = basic_new_heap();
        vecbasic_get(args, i, temp_basic);
        temp = Data_Wrap_Struct(rb_obj_class(self), NULL , cbasic_free_heap, temp_basic);
        rb_ary_push(ruby_array, temp);
    }
    vecbasic_free(args);
    return ruby_array;
}

VALUE cbasic_free_symbols(VALUE self) {
    basic_struct *this, *iterator_basic;
    CSetBasic *symbols = setbasic_new();
    int size = 0;

    Data_Get_Struct(self, basic_struct, this);
    basic_free_symbols(this, symbols);

    size = setbasic_size(symbols);
    VALUE ruby_array = rb_ary_new2(size);
    int i = 0;
    VALUE temp = NULL;
    for(i = 0; i < size; i++) {
        basic_struct *temp_basic = basic_new_heap();
        setbasic_get(symbols, i, temp_basic);
        temp = Data_Wrap_Struct(rb_obj_class(self), NULL , cbasic_free_heap, temp_basic);
        rb_ary_push(ruby_array, temp);
    }
    setbasic_free(symbols);
    return ruby_array;
}

VALUE cbasic_to_str(VALUE self){
    basic_struct *this;
    char *str_ptr;
    VALUE result;

    Data_Get_Struct(self, basic_struct, this);

    str_ptr = basic_str(this);
    result = rb_str_new_cstr(str_ptr);
    basic_str_free(str_ptr);

    return result;
}

VALUE cbasic_expand(VALUE self){
    return cbasic_unary_op(self, basic_expand);
}
