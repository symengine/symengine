#include "catch.hpp"

#include <symengine/serialize.h>
#include <cereal/archives/binary.hpp>

using std::string;

using SymEngine::Basic;
using SymEngine::Symbol;
using SymEngine::symbol;
using SymEngine::RCP;

template <typename T>
string dumps(RCP<const T> obj) {
    std::ostringstream oss;
    cereal::BinaryOutputArchive{oss}(obj);
    return oss.str();
}

template <typename T>
RCP<const T> loads(string sobj) {
    RCP<const T> obj;
    std::istringstream iss;
    cereal::BinaryInputArchive{iss}(obj);
    return obj;
}

TEST_CASE("Test serialization using cereal", "[serialize]") {
    RCP<const Symbol> symb_x_ori = symbol("x");
    string s_symb_x = dumps(symb_x_ori);
    RCP<const Symbol> symb_x_des = loads<RCP<const Symbol>>(s_symb_x);
    REQUIRE( symb_x_ori->__eq__(*symb_x_des));
}
