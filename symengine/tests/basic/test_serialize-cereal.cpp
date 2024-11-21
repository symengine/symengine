#include "catch.hpp"

#include <symengine/basic.h>
#include <symengine/parser.h>
#include <symengine/serialize-cereal.h>
#include <cereal/archives/binary.hpp>

using std::string;

using SymEngine::Basic;
using SymEngine::complex_double;
using SymEngine::Integer;
using SymEngine::is_a;
using SymEngine::Number;
using SymEngine::RCP;
using SymEngine::Symbol;
#ifdef HAVE_SYMENGINE_MPFR
using SymEngine::mpfr_class;
using SymEngine::RealMPFR;
#endif

namespace se = SymEngine;

template <typename T>
string dumps(RCP<const T> obj)
{
    std::ostringstream oss;
    cereal::BinaryOutputArchive{oss}(obj);
    return oss.str();
}

template <typename T>
RCP<const T> loads(string sobj)
{
    RCP<const T> obj;
    std::istringstream iss(sobj);
    cereal::BinaryInputArchive{iss}(obj);
    return obj;
}

template <typename T>
void check_string_serialization_roundtrip(RCP<const T> basic1)
{
    RCP<const Basic> basic2 = loads<T>(dumps<T>(basic1));
    REQUIRE(eq(*basic1, *basic2));

    RCP<const Basic> basic3 = loads<Basic>(dumps<Basic>(basic1));
    REQUIRE(eq(*basic1, *basic3));
}

TEST_CASE("Test serialization using cereal", "[serialize-cereal]")
{
    RCP<const Symbol> symb_x_ori = se::symbol("x");
    string s_symb_x = dumps<Symbol>(symb_x_ori);
    RCP<const Symbol> symb_x_des = loads<Symbol>(s_symb_x);
    REQUIRE(symb_x_ori->__eq__(*symb_x_des));
    RCP<const Basic> basic_x_des = loads<Basic>(s_symb_x);
    REQUIRE(is_a<Symbol>(*basic_x_des));
    REQUIRE(!is_a<Integer>(*basic_x_des));

    // Symbol
    check_string_serialization_roundtrip(se::symbol("y"));
    // Add
    check_string_serialization_roundtrip(
        se::add(se::symbol("y"), se::integer(3)));
    check_string_serialization_roundtrip(
        se::add(se::symbol("y"), se::integer(-3)));
    // Pow
    check_string_serialization_roundtrip(
        se::pow(se::symbol("y"), se::integer(2)));
    check_string_serialization_roundtrip(se::reals());
    check_string_serialization_roundtrip(
        complex_double(std::complex<double>(4, 5)));
#ifdef HAVE_SYMENGINE_MPFR
    check_string_serialization_roundtrip(
        real_mpfr(mpfr_class("0.35", 100, 10)));
#endif
}

TEST_CASE("Test serialization exception", "[serialize-cereal]")
{
    RCP<const Basic> expr = se::parse("x + y");
    std::string orig_data = expr->dumps();
    // These positions were chosen because they do not try to create an object
    // that fails asserts.
    std::vector<int> positions = {4, 8, 9, 15, 16};

    for (auto &pos : positions) {
        std::string data = orig_data;
        data[pos] = char(9);
        CHECK_THROWS_AS(Basic::loads(data), se::SerializationError);
    }
}
