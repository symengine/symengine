#include <symengine/basic.h>
#include <symengine/symbol.h>
#include <symengine/add.h>
#include <symengine/integer.h>
#include <symengine/rational.h>
#include <symengine/complex.h>
#include <symengine/mul.h>
#include <symengine/pow.h>
#include <symengine/functions.h>
#include <symengine/constants.h>
#include <symengine/visitor.h>
#include <symengine/parser.h>
#include <vector>
#include <stack>
#include <map>
#include <set>
#include <string>
#include <functional>

namespace SymEngine
{

class ExpressionParser
{

    // OPERATORS and op_precedence used internally, for parsing
    std::set<char> OPERATORS = {'-', '+', '*', '/', '^', '(', ')', ','};
    std::map<char, int> op_precedence = {{')', 0}, {',', 0}, {'-', 1}, {'+', 1},
                                         {'*', 3}, {'/', 4}, {'^', 5}};
    // symengine supported constants
    std::map<const std::string, const RCP<const Basic>> constants = {

        {"e", E}, {"E", E}, {"EulerGamma", EulerGamma}, {"pi", pi}, {"I", I}};

    // reference :
    // http://stackoverflow.com/questions/30393285/stdfunction-fails-to-distinguish-overloaded-functions
    typedef RCP<const Basic> (*single_arg_func)(const RCP<const Basic> &);
    typedef RCP<const Basic> (*double_arg_func)(const RCP<const Basic> &,
                                                const RCP<const Basic> &);

    // cast overlaoded functions below to single_arg, double_arg before they can
    // be used in the map
    single_arg_func single_casted_log = log;
    single_arg_func single_casted_zeta = zeta;

    double_arg_func double_casted_log = log;
    double_arg_func double_casted_zeta = zeta;

    // maps string to corresponding single argument function
    std::map<std::string,
             std::function<RCP<const Basic>(const RCP<const Basic> &)>>
        single_arg_functions = {

            {"", [](const RCP<const Basic> &x) {return x; }},

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
            {"loggamma", loggamma},
            {"lambertw", lambertw},
            {"dirichlet_eta", dirichlet_eta},
            {"ln", single_casted_log},
            {"log", single_casted_log},
            {"zeta", single_casted_zeta}};

    // maps string to corresponding double argument function
    std::map<std::string,
             std::function<RCP<const Basic>(const RCP<const Basic> &,
                                            const RCP<const Basic> &)>>
        double_arg_functions = {

            {"pow", pow},
            {"beta", beta},
            {"log", double_casted_log},
            {"zeta", double_casted_zeta},
            {"lowergamma", lowergamma},
            {"uppergamma", uppergamma},
            {"polygamma", polygamma},
            {"kronecker_delta", kronecker_delta}};

    // maps string to corresponding multi argument function
    std::map<std::string, std::function<RCP<const Basic>(vec_basic &)>>
        multi_arg_functions = {

            {"max", max}, {"min", min}, {"levi_civita", levi_civita}};

    // vector which stores where parsing 'ends' for a particular index
    // eg. for a '(', it stores where the next ',' or ')' occurs, so as to know
    // what part of the string is to be parsed 'together'
    std::vector<int> operator_end;

    // the string to be parsed, obtained after removing all spaces from input
    // string
    std::string s;
    // it's length
    unsigned int s_len;

    // parses a string from [l, r)
    RCP<const Basic> parse_string(unsigned int l, unsigned int h)
    {
        // the result of a particular parse from l->h
        RCP<const Basic> result;
        // the current expr being processed
        std::string expr = "";
        // has the result been set even once?
        bool result_set = false;
        // is the current expr being parsed numeric?
        bool is_not_numeric = false;
        // number of '.' in the expression
        int num_dots = 0;

        // a parse_string is called empty in scenarios like "x+"
        if (l == h)
            throw std::runtime_error("Expected token!");

        for (unsigned int iter = l; iter < h; ++iter) {
            if (is_operator(iter)) {
                // if an operator is encountered, which is not '(' a result must
                // be evaluated (if not already)!
                if (!result_set)
                    if (s[iter] != '(')
                        result = set_result(expr, is_not_numeric, num_dots);

                // continue the parsing after operator_end[iter], as we have
                // already parsed till there
                // using the recursive call to parse_string
                if (s[iter] == '+') {
                    result = add(result,
                                 parse_string(iter + 1, operator_end[iter]));
                    iter = operator_end[iter] - 1;

                } else if (s[iter] == '*') {
                    result = mul(result,
                                 parse_string(iter + 1, operator_end[iter]));
                    iter = operator_end[iter] - 1;

                } else if (s[iter] == '-') {
                    result = sub(result,
                                 parse_string(iter + 1, operator_end[iter]));
                    iter = operator_end[iter] - 1;

                } else if (s[iter] == '/') {
                    result = div(result,
                                 parse_string(iter + 1, operator_end[iter]));
                    iter = operator_end[iter] - 1;

                } else if (s[iter] == '^') {
                    result = pow(result,
                                 parse_string(iter + 1, operator_end[iter]));
                    iter = operator_end[iter] - 1;

                } else if (s[iter] == '(') {
                    result = functionify(iter, expr);

                } else {
                    continue;
                }
                // result has definitely been set
                result_set = true;

            } else {
                // if not an operator, we append it to the current expr
                expr += s[iter];
                // check wether it's numeric or not
                if (s[iter] == '.') {
                    num_dots++;

                } else {
                    int ascii = s[iter] - '0';
                    if (ascii < 0 or ascii > 9)
                        is_not_numeric = true;
                }
                // if the parsing was to finish after this, result must be set
                // occurs when no operator present eg. "3"
                if (iter == h - 1)
                    result = set_result(expr, is_not_numeric, num_dots);
            }
        }

        return result;
    }

    // returns true if the s[iter] is an operator
    bool is_operator(int iter)
    {
        if (iter >= 0 and iter < (int)s_len)
            if (OPERATORS.find(s[iter]) != OPERATORS.end())
                return true;
        return false;
    }

    // is called on detecting a "func(", thus "func" must be a type of
    // function and everything inside arguments of the function
    RCP<const Basic> functionify(unsigned int &iter, const std::string &expr)
    {
        vec_basic params;

        while (s[iter] != ')') {
            params.push_back(parse_string(iter + 1, operator_end[iter]));
            iter = operator_end[iter];
        }

        if (params.size() == 1)
            if (single_arg_functions.find(expr) != single_arg_functions.end())
                return single_arg_functions[expr](params[0]);

        if (params.size() == 2)
            if (double_arg_functions.find(expr) != double_arg_functions.end())
                return double_arg_functions[expr](params[0], params[1]);

        if (multi_arg_functions.find(expr) != multi_arg_functions.end())
            return multi_arg_functions[expr](params);

        return function_symbol(expr, params);
    }

    // return a <Basic> by parsing the 'expr' passed from parse_string
    RCP<const Basic> set_result(const std::string &expr,
                                const bool &is_not_numeric, const int &num_dots)
    {
        // for handling cases like "-2"
        // expr will be "" in this case, but we must return 0
        if (expr == "")
            return zero;

        // if the expr wasn't numeric, it's either a constant, or a user
        // declared symbol
        // otherwise it's an integer (yet to add float/double support)
        if (is_not_numeric) {
            if (constants.find(expr) != constants.end())
                return constants[expr];
            return symbol(expr);

        } else {
            if (num_dots > 1) {
                throw std::runtime_error("Invalid symbol or number!");

            } else if (num_dots == 1) {
                return real_double(std::atof(expr.c_str()));

            } else {
                return integer(std::atoi(expr.c_str()));
            }
        }
    }

    bool operator_error(char prev, char current)
    {
        if (prev == '(') {
            if (current == ')')
                return true;

        } else if (prev == '-') {
            if (current != '(' and current != ',' and current != ')')
                return true;

        } else {
            if (current != ')')
                return true;
        }
        return false;
    }

public:
    // does all the preprocessing related to parsing
    RCP<const Basic> parse_expr(const std::string &in)
    {
        // stack to maintain right brackets, to match with corresponding left
        // brackets
        std::stack<unsigned int> right_bracket;
        // stack to maintain operators, in order of their precedence
        // (essentially how BODMAS was implemented)
        std::stack<std::pair<int, unsigned int>> op_stack;

        bool last_char_was_op = false;
        char last_char = 'x';
        s = "";

        // removing spaces from the string
        for (unsigned int i = 0; i < in.length(); ++i) {
            if (in[i] == ' ') {
                continue;

            } else if (in[i] == '*' and i + 1 < in.length()
                       and in[i + 1] == '*') {
                s += '^';
                i++;

            } else {
                s += in[i];
            }
        }
        s_len = s.length();
        operator_end.clear();
        operator_end.resize(s_len, -1);
        // the 'defacto' end of any operator
        // won't ever be pushed out of the stack
        op_stack.push(std::make_pair(-1, s_len));
        right_bracket.push(s_len);

        for (int i = s_len - 1; i >= 0; i--) {
            if (is_operator(i)) {
                char x = s[i];

                if (x == '(') {
                    // find the matching right bracket in op_stack
                    while (op_stack.top().second != right_bracket.top())
                        op_stack.pop();
                    // it's end is the index of the ')' (maybe pseudo created by
                    // a ',')
                    operator_end[i] = right_bracket.top();

                    // this should never happen, every '(' should have a
                    // matcihng ')' in the bracket stack
                    if (operator_end[i] == (int)s_len)
                        throw std::runtime_error("Mismatching parantheses!");
                    right_bracket.pop();
                    op_stack.pop();

                } else if (x == ')' or x == ',') {
                    // ',' acts as a pseudo ')', for the intended '('
                    if (x == ',') {
                        // it's end is the actual ')'
                        operator_end[i] = right_bracket.top();
                        right_bracket.pop();
                    }
                    op_stack.push(std::make_pair(op_precedence[x], i));
                    right_bracket.push(i);

                } else {
                    // if it's a normal operator, remove operators with higher
                    // precedence
                    while (op_precedence[x] < op_stack.top().first)
                        op_stack.pop();
                    // whatever is on the top now, it's the 'end'
                    operator_end[i] = op_stack.top().second;
                    op_stack.push(std::make_pair(op_precedence[x], i));
                }

                if (last_char_was_op and operator_error(last_char, x))
                    throw std::runtime_error("Operator inconsistency!");
                last_char_was_op = true;

            } else {
                last_char_was_op = false;
            }

            last_char = s[i];
        }
        // extra right_brackets in the string
        if (right_bracket.top() != s_len)
            throw std::runtime_error("Mismatching parantheses!");

        // final answer is parse_string from [0, len)
        return parse_string(0, s_len);
    }
};

RCP<const Basic> parse(const std::string &s)
{
    ExpressionParser p;
    return p.parse_expr(s);
}

} // SymEngine
