#include <symengine/visitor.h>
#include <symengine/parser.h>
#include <stack>
#include <symengine/symengine_exception.h>
#include <cctype>
#include <cerrno>

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

        {"e", E},
        {"E", E},
        {"EulerGamma", EulerGamma},
        {"Catalan", Catalan},
        {"GoldenRatio", GoldenRatio},
        {"pi", pi},
        {"I", I}};

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
            {"erfc", erfc},
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

    std::string get_string(int start, int end)
    {
        if (start == -1 or start >= end) {
            return "";
        } else {
            while (s[end - 1] == ' ') {
                --end;
            }
            return s.substr(start, end - start);
        }
    }

    // parses a string from [l, r)
    RCP<const Basic> parse_string(unsigned int l, unsigned int h)
    {
        // the result of a particular parse from l->h
        RCP<const Basic> result;
        // start of the current expr being processed
        int expr_start = -1;
        // has the result been set even once?
        bool result_set = false;

        // a parse_string is called empty in scenarios like "x+"
        if (l == h)
            throw ParseError("Expected token!");

        for (unsigned int iter = l; iter < h; ++iter) {
            if (is_operator(iter)) {

                if (s[iter] == '+' or s[iter] == '-') {
                    if (iter > l + 1 and iter < h - 1 and s[iter - 1] == 'e'
                        and s[iter - 2] >= '0' and s[iter - 2] <= '9'
                        and s[iter + 1] >= '0' and s[iter + 1] <= '9') {
                        continue;
                    }
                }

                // if an operator is encountered, which is not '(' a result must
                // be evaluated (if not already)!
                if (!result_set) {
                    if (s[iter] != '(') {
                        result = set_result(get_string(expr_start, iter));
                        expr_start = -1;
                    }
                }

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
                    result = functionify(iter, get_string(expr_start, iter));
                    expr_start = -1;
                } else {
                    continue;
                }
                // result has definitely been set
                result_set = true;

            } else {
                if (expr_start == -1 && s[iter] != ' ')
                    expr_start = iter;
                // if the parsing was to finish after this, result must be set
                // occurs when no operator present eg. "3"
                if (!result_set && iter == h - 1) {
                    result = set_result(get_string(expr_start, iter + 1));
                    expr_start = -1;
                }
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

        if (params.size() == 1) {
            if (single_arg_functions.find(expr) != single_arg_functions.end())
                return single_arg_functions[expr](params[0]);
        }

        if (params.size() == 2) {
            if (double_arg_functions.find(expr) != double_arg_functions.end())
                return double_arg_functions[expr](params[0], params[1]);
        }

        if (multi_arg_functions.find(expr) != multi_arg_functions.end()) {
            return multi_arg_functions[expr](params);
        }

        return function_symbol(expr, params);
    }

    // return a <Basic> by parsing the 'expr' passed from parse_string
    RCP<const Basic> set_result(const std::string &expr)
    {
        // for handling cases like "-2"
        // expr will be "" in this case, but we must return 0
        if (expr == "")
            return zero;

        const char *startptr = expr.c_str();
        char *endptr = 0;
        double d = std::strtod(startptr, &endptr);

        RCP<const Basic> num = one, sym;

        // Numerical part of the result of e.g. "100x";
        size_t length = endptr - startptr;
        std::string lexpr = std::string(startptr, length);
        bool has_numeric_part = endptr != startptr;

        // Check if there is a numeric part;
        if (has_numeric_part) {
            char *lendptr;
            // if the expr is numeric, it's either a float or an integer
            errno = 0;
            long l = std::strtol(startptr, &lendptr, 0);

            // Number is a long;
            if (lendptr == endptr) {
                if (errno != ERANGE) {
                    // No overflow in l
                    num = integer(l);
                } else {
                    num = integer(integer_class(lexpr));
                }
            } else {
#ifdef HAVE_SYMENGINE_MPFR
                unsigned digits = 0;
                for (size_t i = 0; i < length; ++i) {
                    if (expr[i] == '.' or expr[i] == '-')
                        continue;
                    if (expr[i] == 'E' or expr[i] == 'e')
                        break;
                    if (digits != 0 or expr[i] != '0') {
                        ++digits;
                    }
                }
                if (digits <= 15) {
                    num = real_double(d);
                } else {
                    // mpmath.libmp.libmpf.dps_to_prec
                    long prec
                        = std::max(long(1), std::lround((digits + 1)
                                                        * 3.3219280948873626));
                    num = real_mpfr(mpfr_class(lexpr, prec));
                }
#else
                num = real_double(d);
#endif
            }
            // Expression is numeric
            if (*endptr == '\0') {
                return num;
            }
        }

        // get the rest of the string
        lexpr = std::string(endptr, expr.length() - length);
        // if the expr is not numeric, it's either a constant, or a user
        // declared symbol
        auto c = constants.find(lexpr);
        if (c != constants.end()) {
            sym = c->second;
        } else {
            for (unsigned i = 0; i < lexpr.length(); ++i) {
                if (not std::isalnum(lexpr[i]) and lexpr[i] != '_'
                    and lexpr[i] >= 0) {
                    throw ParseError(lexpr + " is not a symbol or numeric");
                }
            }
            sym = symbol(lexpr);
        }
        if (has_numeric_part) {
            return mul(num, sym);
        } else {
            return sym;
        }
    }

    bool operator_error(char prev, char current)
    {
        if (prev == '(') {
            if (current == ')')
                return true;

        } else if (prev == '-' or prev == '+') {
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
        s.clear();
        s.reserve(in.length());

        // Replacing ** with ^
        for (unsigned int i = 0; i < in.length(); ++i) {
            if (in[i] == '*' and i + 1 < in.length() and in[i + 1] == '*') {
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
                        throw ParseError("Mismatching parantheses!");
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
                    if (x == '+' or x == '-') {
                        if (i > 1 and i < (int)s_len - 1 and s[i - 1] == 'e'
                            and s[i - 2] >= '0' and s[i - 2] <= '9'
                            and s[i + 1] >= '0' and s[i + 1] <= '9') {
                            // numeric like 1e-10
                            last_char_was_op = false;
                            continue;
                        }
                    }
                    if (last_char_was_op
                        and (last_char == '-' or last_char == '+'))
                        op_stack.pop();
                    // if it's a normal operator, remove operators with higher
                    // precedence
                    while (op_precedence[x] < op_stack.top().first)
                        op_stack.pop();
                    // whatever is on the top now, it's the 'end'
                    operator_end[i] = op_stack.top().second;
                    op_stack.push(std::make_pair(op_precedence[x], i));
                }
                if (last_char_was_op and operator_error(last_char, x))
                    throw ParseError("Operator inconsistency!");
                if (last_char_was_op and operator_error(last_char, x)) {
                    throw SymEngineException("Operator inconsistency!");
                }
                last_char_was_op = true;

            } else if (s[i] == ' ') {
                continue;
            } else {
                last_char_was_op = false;
            }

            last_char = s[i];
        }
        // extra right_brackets in the string
        if (right_bracket.top() != s_len)
            throw ParseError("Mismatching parantheses!");

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
