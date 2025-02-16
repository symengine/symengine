#include "catch.hpp"

#include <symengine/basic.h>
#include <symengine/parser.h>
#include <symengine/serialize-cereal.h>
#include <cereal/archives/binary.hpp>

using std::string;

using SymEngine::add;
using SymEngine::Basic;
using SymEngine::complex_double;
using SymEngine::cos;
using SymEngine::Integer;
using SymEngine::is_a;
using SymEngine::Number;
using SymEngine::RCP;
using SymEngine::RCPBasicAwareInputArchive;
using SymEngine::RCPBasicAwareOutputArchive;
using SymEngine::sin;
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
    RCPBasicAwareOutputArchive<cereal::BinaryOutputArchive>{oss}(obj);
    return oss.str();
}

template <typename T>
RCP<const T> loads(string sobj)
{
    RCP<const T> obj;
    std::istringstream iss(sobj);
    RCPBasicAwareInputArchive<cereal::BinaryInputArchive>{iss}(obj);
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
    // that throws std::bad_alloc
    std::vector<int> positions
        = {29, 30, 31, 32, 56, 57, 58, 59, 75, 76, 77, 78, 94, 95, 96, 97};
#if defined(_MSC_VER) && defined(_DEBUG)
    size_t end = 55;
#else
    size_t end = orig_data.size();
#endif

    for (size_t pos = 0; pos < end; pos++) {
        if (std::find(positions.begin(), positions.end(), pos)
            != positions.end()) {
            continue;
        }
        std::string data = orig_data;
        data[pos] = char(9);
        // Corrupted data should either give an expr
        // or throw an error
        try {
            Basic::loads(data);
        } catch (se::SerializationError &e) {
        }
    }
}

TEST_CASE("Test serialization shared pointer", "[serialize-cereal]")
{
    RCP<const Basic> b = se::symbol("x");
    RCP<const Basic> expr = add(sin(b), cos(b));

    RCP<const Basic> new_expr = Basic::loads(expr->dumps());
    REQUIRE(new_expr->get_args()[0]->get_args()[0].get()
            == new_expr->get_args()[1]->get_args()[0].get());
}
