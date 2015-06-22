#include "ruby_basic.h"

void cbasic_free(void *ptr){
    basic_struct *basic_ptr = ptr;
    basic_free(basic_ptr);
}

VALUE cbasic_alloc(VALUE klass){
    VALUE obj;
    basic_struct *struct_ptr;

    obj = Data_Make_Struct(klass, basic_struct, NULL, cbasic_free, struct_ptr);

    struct_ptr->data = NULL;

    return obj;
}

VALUE cbasic_init(VALUE self){
    basic_struct *this;

    Data_Get_Struct(self, basic_struct, this);
    basic_init(this);

    return self;
}

VALUE cbasic_binary_op(VALUE self, VALUE operand2, void (*cwfunc_ptr)(basic_struct*, const basic_struct*, const basic_struct*)){
    basic_struct *this, *cbasic_operand2, *cresult;
    VALUE result;

    Data_Get_Struct(self, basic_struct, this);
    Data_Get_Struct(operand2, basic_struct, cbasic_operand2);
    result = Data_Make_Struct(rb_obj_class(self), basic_struct, NULL , cbasic_free, cresult);

    basic_init(cresult);
    cwfunc_ptr(cresult, this, cbasic_operand2);

    return result;
}

VALUE cbasic_unary_op(VALUE self, void (*cwfunc_ptr)(basic_struct*, const basic_struct*)){
    basic_struct *this, *cresult;
    VALUE result;

    Data_Get_Struct(self, basic_struct, this);
    result = Data_Make_Struct(rb_obj_class(self), basic_struct, NULL, cbasic_free, cresult);

    basic_init(cresult);
    cwfunc_ptr(cresult, this);

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

VALUE cbasic_neg(VALUE self){
    return cbasic_unary_op(self, basic_neg);
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
