#include "symengine_macros.h"

VALUE Klass_of_Basic(const basic_struct *basic_ptr) {
    switch(basic_get_type(basic_ptr)) {
        case SYMENGINE_ADD:
        case SYMENGINE_MUL:
        case SYMENGINE_POW:
            return rb_path2class("SymEngine::Symbol");
        case SYMENGINE_INTEGER:
            return rb_path2class("SymEngine::Integer");
        case SYMENGINE_RATIONAL:
            return rb_path2class("SymEngine::Rational");
    }
}
