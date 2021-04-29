#include <symengine/parser.h>
#include <symengine/parser/sbml/sbml_parser.h>
#include <symengine/parser/sbml/sbml_parser.tab.hh>
#include <symengine/real_double.h>
#include <symengine/real_mpfr.h>
#include <symengine/functions.h>
#include <algorithm>
#include <cctype>

using SymEngine::ceiling;
using SymEngine::floor;
using SymEngine::gamma;
using SymEngine::logical_xor;
using SymEngine::Lt;
using SymEngine::piecewise;
using SymEngine::pow;
using SymEngine::zero;

namespace SymEngine
{

RCP<const Basic>
parse_sbml(const std::string &s, bool convert_xor,
           const std::map<const std::string, const RCP<const Basic>> &constants)
{
    SbmlParser p(constants);
    return p.parse(s, convert_xor);
}

RCP<const Basic> SbmlParser::parse(const std::string &input, bool convert_xor)
{
    inp = input;
    if (convert_xor) {
        std::replace(inp.begin(), inp.end(), '^', '@');
    }
    m_tokenizer.set_string(inp);
    if (sbmlparse(*this) == 0)
        return this->res;
    throw ParseError("Parsing Unsuccessful");
}

static std::string lowercase(const std::string &str)
{
    std::string lower = str;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
    return lower;
}

// reference :
// http://stackoverflow.com/questions/30393285/stdfunction-fails-to-distinguish-overloaded-functions
typedef RCP<const Basic> (*single_arg_func)(const RCP<const Basic> &);
typedef RCP<const Basic> (*double_arg_func)(const RCP<const Basic> &,
                                            const RCP<const Basic> &);
typedef RCP<const Boolean> (*single_arg_boolean_func)(const RCP<const Basic> &);
typedef RCP<const Boolean> (*double_arg_boolean_func)(const RCP<const Basic> &,
                                                      const RCP<const Basic> &);
typedef RCP<const Basic> (*vector_arg_func)(const vec_basic &);

// cast overloaded functions below to single_arg, double_arg before they can
// be used in the map
static single_arg_func single_casted_log = log;

static double_arg_boolean_func double_casted_Eq = Eq;

static vector_arg_func vector_casted_add = add;
static vector_arg_func vector_casted_mul = mul;

static RCP<const Basic> sqr(const RCP<const Basic> &x)
{
    return pow(x, integer(2));
}

static RCP<const Basic> sbml_log(const RCP<const Basic> &b,
                                 const RCP<const Basic> &x)
{
    return log(x, b);
}

static RCP<const Basic> log10(const RCP<const Basic> &x)
{
    return log(x, integer(10));
}

static RCP<const Basic> root(const RCP<const Basic> &b,
                             const RCP<const Basic> &x)
{
    return pow(x, div(integer(1), b));
}

static RCP<const Basic> fact(const RCP<const Basic> &x)
{
    return gamma(add(x, integer(1)));
}

static RCP<const Boolean> vec_ge(const vec_basic &x)
{
    set_boolean b;
    for (std::size_t i = 0; i + 1 < x.size(); ++i) {
        b.insert(Ge(x[i], x[i + 1]));
    }
    return logical_and(b);
}

static RCP<const Boolean> vec_gt(const vec_basic &x)
{
    set_boolean b;
    for (std::size_t i = 0; i + 1 < x.size(); ++i) {
        b.insert(Gt(x[i], x[i + 1]));
    }
    return logical_and(b);
}

static RCP<const Boolean> vec_le(const vec_basic &x)
{
    set_boolean b;
    for (std::size_t i = 0; i + 1 < x.size(); ++i) {
        b.insert(Le(x[i], x[i + 1]));
    }
    return logical_and(b);
}

static RCP<const Boolean> vec_lt(const vec_basic &x)
{
    set_boolean b;
    for (std::size_t i = 0; i + 1 < x.size(); ++i) {
        b.insert(Lt(x[i], x[i + 1]));
    }
    return logical_and(b);
}

RCP<const Basic> SbmlParser::modulo(const RCP<const Basic> &a,
                                    const RCP<const Basic> &b)
{
    return piecewise({{sub(a, mul(b, ceiling(div(a, b)))),
                       logical_xor({Lt(a, zero), Lt(b, zero)})},
                      {sub(a, mul(b, floor(div(a, b)))), boolTrue}});
}

RCP<const Basic> SbmlParser::functionify(const std::string &name)
{
    const static std::map<const std::string, const RCP<const Basic>>
        zero_arg_functions = {{"plus", integer(0)},
                              {"times", integer(1)},
                              {"and", boolTrue},
                              {"or", boolFalse},
                              {"xor", boolFalse}};
    std::string lname = lowercase(name);
    auto l = zero_arg_functions.find(lname);
    if (l != zero_arg_functions.end()) {
        return l->second;
    }
    throw ParseError("Parsing Unsuccessful: Function '" + name
                     + "' has no arguments");
}

RCP<const Basic> SbmlParser::functionify(const std::string &name,
                                         vec_basic &params)
{
    const static std::map<
        const std::string,
        const std::function<RCP<const Basic>(const RCP<const Basic> &,
                                             const RCP<const Basic> &)>>
        double_arg_functions = {
            {"minus", sub}, {"divide", div}, {"pow", pow},
            {"power", pow}, {"root", root},  {"log", sbml_log},
        };
    const static std::map<
        const std::string,
        const std::function<RCP<const Basic>(const RCP<const Basic> &)>>
        single_arg_functions
        = {{"sin", sin},         {"cos", cos},        {"tan", tan},
           {"cot", cot},         {"csc", csc},        {"sec", sec},

           {"asin", asin},       {"acos", acos},      {"atan", atan},
           {"asec", asec},       {"acsc", acsc},      {"acot", acot},

           {"arcsin", asin},     {"arccos", acos},    {"arctan", atan},
           {"arcsec", asec},     {"arccsc", acsc},    {"arccot", acot},

           {"sinh", sinh},       {"cosh", cosh},      {"tanh", tanh},
           {"coth", coth},       {"sech", sech},      {"csch", csch},

           {"asinh", asinh},     {"acosh", acosh},    {"atanh", atanh},
           {"asech", asech},     {"acoth", acoth},    {"acsch", acsch},

           {"arcsinh", asinh},   {"arccosh", acosh},  {"arctanh", atanh},
           {"arcsech", asech},   {"arccoth", acoth},  {"arccsch", acsch},

           {"sqr", sqr},         {"sqrt", sqrt},      {"root", sqrt},
           {"abs", abs},         {"exp", exp},        {"ln", single_casted_log},
           {"log", log10},       {"log10", log10},    {"ceil", ceiling},
           {"ceiling", ceiling}, {"factorial", fact}, {"floor", floor}};

    const static std::map<const std::string,
                          const std::function<RCP<const Basic>(vec_basic &)>>
        multi_arg_functions = {
            {"max", max},
            {"min", min},
            {"plus", vector_casted_add},
            {"times", vector_casted_mul},
        };

    const static std::map<
        const std::string,
        const std::function<RCP<const Boolean>(const RCP<const Boolean> &)>>
        single_arg_boolean_boolean_functions = {
            {"not", logical_not},
        };

    const static std::map<
        const std::string,
        const std::function<RCP<const Boolean>(const RCP<const Basic> &,
                                               const RCP<const Basic> &)>>
        double_arg_boolean_functions = {{"eq", double_casted_Eq}, {"neq", Ne}};

    const static std::map<const std::string,
                          const std::function<RCP<const Boolean>(vec_basic &)>>
        multi_arg_vec_basic_boolean_functions
        = {{"geq", vec_ge}, {"gt", vec_gt}, {"leq", vec_le}, {"lt", vec_lt}};

    const static std::map<
        const std::string,
        const std::function<RCP<const Boolean>(vec_boolean &)>>
        multi_arg_vec_boolean_functions = {{"xor", logical_xor}};

    const static std::map<
        const std::string,
        const std::function<RCP<const Boolean>(set_boolean &)>>
        multi_arg_set_boolean_functions = {
            {"and", logical_and},
            {"or", logical_or},
        };

    std::string lname = lowercase(name);
    if (params.size() == 1) {
        auto it1 = single_arg_functions.find(lname);
        if (it1 != single_arg_functions.end()) {
            return it1->second(params[0]);
        }
        auto it3 = single_arg_boolean_boolean_functions.find(lname);
        if (it3 != single_arg_boolean_boolean_functions.end()) {
            return it3->second(rcp_static_cast<const Boolean>(params[0]));
        }
    }

    if (params.size() == 2) {
        auto it1 = double_arg_functions.find(lname);
        if (it1 != double_arg_functions.end()) {
            return it1->second(params[0], params[1]);
        }
        auto it2 = double_arg_boolean_functions.find(lname);
        if (it2 != double_arg_boolean_functions.end()) {
            return it2->second(params[0], params[1]);
        }
    }

    auto it1 = multi_arg_functions.find(lname);
    if (it1 != multi_arg_functions.end()) {
        return it1->second(params);
    }

    if (lname == "piecewise") {
        PiecewiseVec v;
        for (std::size_t i = 0; i < params.size() / 2; ++i) {
            // piecewise has pairs of {value, bool condition}
            v.push_back({params[2 * i],
                         rcp_static_cast<const Boolean>(params[2 * i + 1])});
        }
        if (params.size() % 2 == 1) {
            // piecewise can also have a default value
            v.push_back({params.back(), boolTrue});
        }
        return piecewise(std::move(v));
    }

    auto it2 = multi_arg_vec_boolean_functions.find(lname);
    if (it2 != multi_arg_vec_boolean_functions.end()) {
        vec_boolean p;
        for (auto &v : params) {
            p.push_back(rcp_static_cast<const Boolean>(v));
        }
        return it2->second(p);
    }

    auto it3 = multi_arg_set_boolean_functions.find(lname);
    if (it3 != multi_arg_set_boolean_functions.end()) {
        set_boolean s;
        for (auto &v : params) {
            s.insert(rcp_static_cast<const Boolean>(v));
        }
        return it3->second(s);
    }

    auto it4 = multi_arg_vec_basic_boolean_functions.find(lname);
    if (it4 != multi_arg_vec_basic_boolean_functions.end()) {
        if (params.size() < 2) {
            throw ParseError("Parsing Unsuccessful: '" + it4->first
                             + "' must have at least 2 arguments");
        }
        return it4->second(params);
    }

    return function_symbol(name, params);
}

RCP<const Basic> SbmlParser::parse_identifier(const std::string &expr)
{
    const static std::map<const std::string, const RCP<const Basic>>
        parser_constants = {{"pi", pi},
                            {"exponentiale", E},
                            {"avogadro", symbol("avogadro")},
                            {"time", symbol("time")},
                            {"inf", Inf},
                            {"infinity", Inf},
                            {"nan", Nan},
                            {"notanumber", Nan},
                            {"true", boolTrue},
                            {"false", boolFalse}};

    auto it_l = local_parser_constants.find(expr);
    if (it_l != local_parser_constants.end()) {
        return it_l->second;
    }
    std::string lexpr = lowercase(expr);
    auto it_c = parser_constants.find(lexpr);
    if (it_c != parser_constants.end()) {
        return it_c->second;
    } else {
        return symbol(expr);
    }
}

RCP<const Basic> SbmlParser::parse_numeric(const std::string &expr)
{
    const char *startptr = expr.c_str();
    char *lendptr;
    // if the expr is numeric, it's either a float or an integer
    errno = 0;
    long l = std::strtol(startptr, &lendptr, 0);

    // Number is a long;
    if (expr.find_first_of('.') == std::string::npos
        && lendptr == startptr + expr.length()) {
        if (errno != ERANGE) {
            // No overflow in l
            return integer(l);
        } else {
            return integer(integer_class(expr));
        }
    } else {
#ifdef HAVE_SYMENGINE_MPFR
        unsigned digits = 0;
        for (size_t i = 0; i < expr.length(); ++i) {
            if (expr[i] == '.' or expr[i] == '-')
                continue;
            if (expr[i] == 'E' or expr[i] == 'e')
                break;
            if (digits != 0 or expr[i] != '0') {
                ++digits;
            }
        }
        if (digits <= 15) {
            char *endptr = 0;
            double d = std::strtod(startptr, &endptr);
            return real_double(d);
        } else {
            // mpmath.libmp.libmpf.dps_to_prec
            long prec = std::max(
                long(1), std::lround((digits + 1) * 3.3219280948873626));
            return real_mpfr(mpfr_class(expr, prec));
        }
#else
        char *endptr = 0;
        double d = std::strtod(startptr, &endptr);
        return real_double(d);
#endif
    }
}

SbmlParser::SbmlParser(
    const std::map<const std::string, const RCP<const Basic>> &parser_constants)
    : local_parser_constants(parser_constants)
{
}

} // namespace SymEngine
