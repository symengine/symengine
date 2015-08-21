#include "ruby_basic.h"

void cbasic_free(void *ptr){
    basic_struct *basic_ptr = ptr;
    basic_free_stack(basic_ptr);
}

void cbasic_free_heap(void *ptr) {
    basic_struct *basic_ptr = ptr;
    basic_free_heap(basic_ptr);
}

VALUE cbasic_alloc(VALUE klass){
    basic_struct *struct_ptr = basic_new_heap();
    return Data_Wrap_Struct(klass, NULL, cbasic_free_heap, struct_ptr);
}

VALUE cbasic_binary_op(VALUE self, VALUE operand2, void (*cwfunc_ptr)(basic_struct*, const basic_struct*, const basic_struct*)){
    basic_struct *this, *cresult;
    VALUE result;

    basic cbasic_operand2;
    basic_new_stack(cbasic_operand2);

    Data_Get_Struct(self, basic_struct, this);
    sympify(operand2, cbasic_operand2);

    cresult = basic_new_heap();
    cwfunc_ptr(cresult, this, cbasic_operand2);
    result = Data_Wrap_Struct(Klass_of_Basic(cresult), NULL , cbasic_free_heap, cresult);
    basic_free_stack(cbasic_operand2);

    return result;
}

VALUE cbasic_unary_op(VALUE self, void (*cwfunc_ptr)(basic_struct*, const basic_struct*)){
    basic_struct *this, *cresult;
    VALUE result;

    Data_Get_Struct(self, basic_struct, this);

    cresult = basic_new_heap();
    cwfunc_ptr(cresult, this);
    result = Data_Wrap_Struct(Klass_of_Basic(cresult), NULL , cbasic_free_heap, cresult);

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

VALUE cbasic_diff(VALUE self, VALUE operand2) {
    basic_struct *this, *cresult;
    VALUE result;

    basic cbasic_operand2;
    basic_new_stack(cbasic_operand2);

    Data_Get_Struct(self, basic_struct, this);
    sympify(operand2, cbasic_operand2);

    cresult = basic_new_heap();
    int status = basic_diff(cresult, this, cbasic_operand2);
    if (status == 0) {
        basic_free_stack(cbasic_operand2);
        basic_free_heap(cresult);
        return Qnil;
    }
    result = Data_Wrap_Struct(Klass_of_Basic(cresult), NULL , cbasic_free_heap, cresult);
    basic_free_stack(cbasic_operand2);

    return result;
}

VALUE cbasic_eq(VALUE self, VALUE operand2) {
    basic_struct *this;

    basic cbasic_operand2;
    basic_new_stack(cbasic_operand2);
    Data_Get_Struct(self, basic_struct, this);
    sympify(operand2, cbasic_operand2);

    VALUE ret_val = basic_eq(this, cbasic_operand2) ? Qtrue : Qfalse;
    basic_free_stack(cbasic_operand2);

    return ret_val;
}

VALUE cbasic_neq(VALUE self, VALUE operand2) {
    basic_struct *this;

    basic cbasic_operand2;
    basic_new_stack(cbasic_operand2);
    Data_Get_Struct(self, basic_struct, this);
    sympify(operand2, cbasic_operand2);

    VALUE ret_val =  basic_neq(this, cbasic_operand2) ? Qtrue : Qfalse;
    basic_free_stack(cbasic_operand2);

    return ret_val;
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

VALUE cbasic_hash(VALUE self){
    basic_struct *this;
    Data_Get_Struct(self, basic_struct, this);
    // All ruby objects return FIXNUM when `hash` method is called.
    // Though this function returns BIGNUM it won't be a problem, since
    // we need proper comparison only among objects in SymEngine.
    // The objects that should have the same hash will always match
    // and when comparing to the FIXNUM from hash of other ruby objects,
    // it will return false as it is supposed to.
    // However, an alternate implementation is given below
    // long lhash = basic_hash(this) % FIX2LONG(FIXNUM_MAX);
    // return LONG2FIX(lhash);
    return SIZET2NUM(basic_hash(this));
}

int insert_entries(VALUE key, VALUE val, VALUE input) {
    CMapBasicBasic *cmapbb;
    Data_Get_Struct(input, CMapBasicBasic, cmapbb);

    basic ckey, cval;
    basic_new_stack(ckey);
    basic_new_stack(cval);
    sympify(key, ckey);
    sympify(val, cval);

    mapbasicbasic_insert(cmapbb, ckey, cval);

    basic_free_stack(ckey);
    basic_free_stack(cval);
}

VALUE cbasic_subs(int argc, VALUE *argv, VALUE self) {
    basic_struct *this, *cresult;
    cresult = basic_new_heap();

    VALUE val_a, val_b;
    Data_Get_Struct(self, basic_struct, this);

    rb_scan_args(argc, argv, "11", &val_a, &val_b); // 1 mandatory and 1 optional parameter
    if (argc == 1) {
        Check_Type(val_a, T_HASH);
        CMapBasicBasic *cmapbb = mapbasicbasic_new();
        VALUE mapbb = Data_Wrap_Struct(rb_cObject, NULL, mapbasicbasic_free, cmapbb);

        rb_hash_foreach(val_a, insert_entries, mapbb);
        basic_subs(cresult, this, cmapbb);
    } else {
        basic a, b;
        basic_new_stack(a);
        basic_new_stack(b);

        sympify(val_a, a);
        sympify(val_b, b);
        basic_subs2(cresult, this, a, b);

        basic_free_stack(a);
        basic_free_stack(b);
    }

    return Data_Wrap_Struct(Klass_of_Basic(cresult), NULL, cbasic_free_heap, cresult);
}

VALUE cbasic_coerce(VALUE self, VALUE other){
    basic_struct *cbasic_operand2;
    cbasic_operand2 = basic_new_heap();
    sympify(other, cbasic_operand2);
    VALUE new_other = Data_Wrap_Struct(Klass_of_Basic(cbasic_operand2), NULL , cbasic_free_heap, cbasic_operand2);
    return rb_assoc_new(new_other, self);
}
