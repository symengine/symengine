#include <symengine/parser/parser.h>
namespace SymEngine
{

std::map<const std::string, const RCP<const Basic>>
initialize_parser_constants()
{
    return {{"e", E},
            {"E", E},
            {"EulerGamma", EulerGamma},
            {"Catalan", Catalan},
            {"GoldenRatio", GoldenRatio},
            {"pi", pi},
            {"I", I},
            {"oo", Inf},
            {"inf", Inf},
            {"zoo", ComplexInf},
            {"nan", Nan}};
}

// cast overloaded functions below to single_arg, double_arg before they can
// be used in the map
single_arg_func single_casted_log = log;
single_arg_func single_casted_zeta = zeta;

double_arg_func double_casted_log = log;
double_arg_func double_casted_zeta = zeta;

single_arg_boolean_func single_casted_Eq = Eq;
double_arg_boolean_func double_casted_Eq = Eq;

std::map<const std::string,
         const std::function<RCP<const Basic>(const RCP<const Basic> &)>>
initialize_parser_single_arg_functions()
{
    return {
        {"sin", sin},
        {"cos", cos},
        {"tan", tan},
        {"cot", cot},
        {"csc", csc},
        {"sec", sec},

        {"asin", asin},
        {"acos", acos},
        {"atan", atan},
        {"asec", asec},
        {"acsc", acsc},
        {"acot", acot},

        {"sinh", sinh},
        {"cosh", cosh},
        {"tanh", tanh},
        {"coth", coth},
        {"sech", sech},
        {"csch", csch},

        {"asinh", asinh},
        {"acosh", acosh},
        {"atanh", atanh},
        {"asech", asech},
        {"acoth", acoth},
        {"acsch", acsch},

        {"gamma", gamma},
        {"sqrt", sqrt},
        {"abs", abs},
        {"exp", exp},
        {"erf", erf},
        {"erfc", erfc},
        {"loggamma", loggamma},
        {"lambertw", lambertw},
        {"dirichlet_eta", dirichlet_eta},
        {"ln", single_casted_log},
        {"log", single_casted_log},
        {"zeta", single_casted_zeta},
    };
}
std::map<const std::string,
         const std::function<RCP<const Basic>(const RCP<const Basic> &,
                                              const RCP<const Basic> &)>>
initialize_parser_double_arg_functions()
{
    return {
        {"pow", (double_arg_func)pow}, {"beta", beta},
        {"log", double_casted_log},    {"zeta", double_casted_zeta},
        {"lowergamma", lowergamma},    {"uppergamma", uppergamma},
        {"polygamma", polygamma},      {"kronecker_delta", kronecker_delta},
    };
}
std::map<const std::string, const std::function<RCP<const Basic>(vec_basic &)>>
initialize_parser_multi_arg_functions()
{
    return {
        {"max", max}, {"min", min}, {"levi_civita", levi_civita},
    };
}

std::map<const std::string,
         const std::function<RCP<const Boolean>(const RCP<const Basic> &)>>
initialize_parser_single_arg_boolean_functions()
{
    return {
        {"Eq", single_casted_Eq},
    };
}

std::map<const std::string,
         const std::function<RCP<const Boolean>(const RCP<const Boolean> &)>>
initialize_parser_single_arg_boolean_boolean_functions()
{
    return {
        {"Not", logical_not},
    };
}

std::map<const std::string,
         const std::function<RCP<const Boolean>(const RCP<const Basic> &,
                                                const RCP<const Basic> &)>>
initialize_parser_double_arg_boolean_functions()
{
    return {
        {"Eq", double_casted_Eq},
        {"Ne", Ne},
        {"Ge", Ge},
        {"Gt", Gt},
        {"Le", Le},
        {"Lt", Lt},
    };
}
std::map<const std::string,
         const std::function<RCP<const Boolean>(vec_boolean &)>>
initialize_parser_multi_arg_vec_boolean_functions()
{
    return {
        {"Xor", logical_xor}, {"Xnor", logical_xnor},
    };
}

std::map<const std::string,
         const std::function<RCP<const Boolean>(set_boolean &)>>
initialize_parser_multi_arg_set_boolean_functions()
{
    return {
        {"And", logical_and},
        {"Or", logical_or},
        {"Nand", logical_nand},
        {"Nor", logical_nor},
    };
}

const std::map<const std::string, const RCP<const Basic>> Parser::constants
    = initialize_parser_constants();
const std::map<const std::string,
               const std::function<RCP<const Basic>(const RCP<const Basic> &)>>
    Parser::single_arg_functions = initialize_parser_single_arg_functions();
const std::map<const std::string,
               const std::function<RCP<const Basic>(const RCP<const Basic> &,
                                                    const RCP<const Basic> &)>>
    Parser::double_arg_functions = initialize_parser_double_arg_functions();
const std::map<const std::string,
               const std::function<RCP<const Basic>(vec_basic &)>>
    Parser::multi_arg_functions = initialize_parser_multi_arg_functions();

const std::map<const std::string, const std::function<RCP<const Boolean>(
                                      const RCP<const Basic> &)>>
    Parser::single_arg_boolean_functions
    = initialize_parser_single_arg_boolean_functions();

const std::map<const std::string, const std::function<RCP<const Boolean>(
                                      const RCP<const Boolean> &)>>
    Parser::single_arg_boolean_boolean_functions
    = initialize_parser_single_arg_boolean_boolean_functions();

const std::map<const std::string,
               const std::function<RCP<const Boolean>(
                   const RCP<const Basic> &, const RCP<const Basic> &)>>
    Parser::double_arg_boolean_functions
    = initialize_parser_double_arg_boolean_functions();

const std::map<const std::string,
               const std::function<RCP<const Boolean>(vec_boolean &)>>
    Parser::multi_arg_vec_boolean_functions
    = initialize_parser_multi_arg_vec_boolean_functions();

const std::map<const std::string,
               const std::function<RCP<const Boolean>(set_boolean &)>>
    Parser::multi_arg_set_boolean_functions
    = initialize_parser_multi_arg_set_boolean_functions();
}
