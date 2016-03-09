#include <symengine/basic.h>
#include <symengine/printer.h>

namespace SymEngine {

std::string poly_print(const Expression &x) {
    Precedence prec;
    if (prec.getPrecedence(x.get_basic()) == PrecedenceEnum::Add) {
        return "(" + x.get_basic()->__str__() + ")";
    }
    return x.get_basic()->__str__();
}

} // SymEngine

std::ostream &operator<<(std::ostream &os, const SymEngine::Expression &x) {
    os << x.get_basic()->__str__();
    return os;
}
