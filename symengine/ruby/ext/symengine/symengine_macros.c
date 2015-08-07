#include "symengine_macros.h"
#include "symengine.h"

void sympify(VALUE operand2, basic_struct *cbasic_operand2) {
    basic_struct *num_basic, *den_basic, *temp;
    VALUE new_operand2, num, den;

    switch(TYPE(operand2)) {
        case T_FIXNUM:
        case T_BIGNUM:
            GET_SYMINTFROMVAL(operand2, cbasic_operand2);
            break;

        case T_RATIONAL:
            num = rb_funcall(operand2, rb_intern("numerator"), 0, NULL);
            den = rb_funcall(operand2, rb_intern("denominator"), 0, NULL);

            num_basic = basic_new_heap();
            den_basic = basic_new_heap();

            GET_SYMINTFROMVAL(num, num_basic);
            GET_SYMINTFROMVAL(den, den_basic);

            rational_set(cbasic_operand2, num_basic, den_basic);

            basic_free_heap(num_basic);
            basic_free_heap(den_basic);
            break;

        case T_DATA:
            Data_Get_Struct(operand2, basic_struct, temp);
            basic_assign(cbasic_operand2, temp);
            break;
    }
}

VALUE Klass_of_Basic(const basic_struct *basic_ptr) {
    switch(basic_get_type(basic_ptr)) {
        case SYMENGINE_ADD:
        case SYMENGINE_MUL:
        case SYMENGINE_POW:
        case SYMENGINE_SYMBOL:
            return c_symbol;
        case SYMENGINE_INTEGER:
            return c_integer;
        case SYMENGINE_RATIONAL:
            return c_rational;
        default:
            return c_basic;
    }
}
