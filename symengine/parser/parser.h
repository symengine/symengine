#include "parserbase.h"
#include "scanner.h"
#include <fstream>
#include <algorithm>

namespace SymEngine
{
#undef Parser
class Parser : public ParserBase
{
    Scanner d_scanner;
    std::istringstream inp;

public:
    RCP<const Basic> res;

    inline Parser(const std::string &input, bool convert_xor_)
    {
        if (convert_xor_) {
            std::string s = input;
            std::replace(s.begin(), s.end(), '^', '@');
            inp.str(s);
        } else {
            inp.str(input);
        }
        d_scanner.switchStreams(inp, std::cout);
        d_scanner.dval = &d_val__;
    }

    std::map<const std::string, const RCP<const Basic>> constants = {

        {"e", E},
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

    // reference :
    // http://stackoverflow.com/questions/30393285/stdfunction-fails-to-distinguish-overloaded-functions
    typedef RCP<const Basic> (*single_arg_func)(const RCP<const Basic> &);
    typedef RCP<const Basic> (*double_arg_func)(const RCP<const Basic> &,
                                                const RCP<const Basic> &);
    typedef RCP<const Boolean> (*single_arg_boolean_func)(
        const RCP<const Basic> &);
    typedef RCP<const Boolean> (*double_arg_boolean_func)(
        const RCP<const Basic> &, const RCP<const Basic> &);

    // cast overloaded functions below to single_arg, double_arg before they can
    // be used in the map
    single_arg_func single_casted_log = log;
    single_arg_func single_casted_zeta = zeta;
    single_arg_boolean_func single_casted_Eq = Eq;

    double_arg_func double_casted_log = log;
    double_arg_func double_casted_zeta = zeta;
    double_arg_boolean_func double_casted_Eq = Eq;

    // maps string to corresponding single argument function
    std::map<std::string,
             std::function<RCP<const Basic>(const RCP<const Basic> &)>>
        single_arg_functions = {
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

    // maps string to corresponding double argument function
    std::map<std::string,
             std::function<RCP<const Basic>(const RCP<const Basic> &,
                                            const RCP<const Basic> &)>>
        double_arg_functions = {
            {"pow", (double_arg_func)pow}, {"beta", beta},
            {"log", double_casted_log},    {"zeta", double_casted_zeta},
            {"lowergamma", lowergamma},    {"uppergamma", uppergamma},
            {"polygamma", polygamma},      {"kronecker_delta", kronecker_delta},
        };

    // maps string to corresponding multi argument function
    std::map<std::string, std::function<RCP<const Basic>(vec_basic &)>>
        multi_arg_functions = {
            {"max", max}, {"min", min}, {"levi_civita", levi_civita},
        };

    // maps string to corresponding single argument boolean function
    std::map<std::string,
             std::function<RCP<const Boolean>(const RCP<const Basic> &)>>
        single_arg_boolean_functions = {
            {"Eq", single_casted_Eq},
        };

    // maps string to corresponding single argument boolean function (accepting
    // Boolean objects)
    std::map<std::string,
             std::function<RCP<const Boolean>(const RCP<const Boolean> &)>>
        single_arg_boolean_boolean_functions = {
            {"Not", logical_not},
        };

    // maps string to corresponding double argument boolean function
    std::map<std::string,
             std::function<RCP<const Boolean>(const RCP<const Basic> &,
                                              const RCP<const Basic> &)>>
        double_arg_boolean_functions = {
            {"Eq", double_casted_Eq},
            {"Ne", Ne},
            {"Ge", Ge},
            {"Gt", Gt},
            {"Le", Le},
            {"Lt", Lt},
        };

    // maps string to corresponding multi argument vec_boolean function
    std::map<std::string, std::function<RCP<const Boolean>(vec_boolean &)>>
        multi_arg_vec_boolean_functions = {
            {"Xor", logical_xor}, {"Xnor", logical_xnor},
        };

    // maps string to corresponding multi argument set_boolean function
    std::map<std::string, std::function<RCP<const Boolean>(set_boolean &)>>
        multi_arg_set_boolean_functions = {
            {"And", logical_and},
            {"Or", logical_or},
            {"Nand", logical_nand},
            {"Nor", logical_nor},
        };

    int parse();

    RCP<const Basic> functionify(const std::string &name, vec_basic &params);
    RCP<const Basic> parse_numeric(const std::string &expr);
    RCP<const Basic> parse_identifier(const std::string &expr);
    std::tuple<RCP<const Basic>, RCP<const Basic>>
    parse_implicit_mul(const std::string &expr);

private:
    void error(char const *msg)
    {
    }             // called on (syntax) errors
    int lex();    // returns the next token from the
                  // lexical scanner.
    void print(); // use, e.g., d_token, d_loc

    void executeAction(int ruleNr);
    void errorRecovery();
    int lookup(bool recovery);
    void nextToken();
    void print__();
    void exceptionHandler__(std::exception const &exc)
    {
        std::cout << exc.what() << '\n';
    }
};
}
