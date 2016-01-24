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
#include <vector>
#include <stack>
#include <map>
#include <set>
#include <string>
#include <functional>
namespace SymEngine {

class ExpressionParser
{
    std::set<char> OPERATORS = {'-', '+', '*', '/', '^', '(', ')', ','};
    std::map<char, int> op_precedence = {
        {')', 0}, {',', 0}, {'-', 1},  {'+', 1},
        {'*', 3}, {'/', 4}, {'^', 5}
    };
    std::map<std::string, RCP<const Basic> > constants = {
        {"e", E}, {"EulerGamma", EulerGamma}, {"pi", pi}, {"i", I}
    };

    // reference : http://stackoverflow.com/questions/30393285/stdfunction-fails-to-distinguish-overloaded-functions
    typedef RCP<const Basic> (*single_arg_func)(const RCP<const Basic>&);
    typedef RCP<const Basic> (*double_arg_func)(const RCP<const Basic>&, const RCP<const Basic>&);

    single_arg_func single_casted_log = log;       // as they are overloaded
    single_arg_func single_casted_zeta = zeta;

    std::map<   std::string,
                std::function<RCP<const Basic>(const RCP<const Basic>&)>
            >   single_arg_functions = {

        {"sin", sin}, {"cos", cos}, {"tan", tan},
        {"cot", cot}, {"csc", csc}, {"sec", sec},

        {"arcsin", asin}, {"arccos", acos}, {"arctan", atan},
        {"arcsec", asec}, {"arccsc", acsc}, {"arccot", acot},

        {"sinh", sinh}, {"cosh", cosh}, {"tanh", tanh},
        {"coth", coth}, // implement sech, csch

        {"arcsinh", asinh}, {"arccosh", acosh}, {"arctanh", atanh},
        {"arcsech", asech}, {"arccoth", acoth}, // implement acsch

        {"gamma", gamma}, {"sqrt", sqrt}, {"abs", abs}, {"exp", exp},
        {"lambertw", lambertw}, {"dirichlet_eta", dirichlet_eta},
        {"ln", single_casted_log}, {"log", single_casted_log}, {"zeta", single_casted_zeta}
    };

    double_arg_func double_casted_log = log;       // as they are overloaded
    double_arg_func double_casted_zeta = zeta;

    std::map<   std::string,
                std::function<RCP<const Basic>(const RCP<const Basic>&, const RCP<const Basic>&)>
            >   double_arg_functions = {

        {"log", double_casted_log}, {"zeta", double_casted_zeta},
        {"pow", pow}, {"beta", beta}, {"uppergamma", uppergamma},
        {"lowergamma", lowergamma}, {"kronecker_delta", kronecker_delta},
        {"polygamma", polygamma}
    };

    std::vector<int> operator_end;
    std::string s;
    unsigned int s_len;

    RCP<const Basic> parse_string(unsigned int l, unsigned int h)
    {
        RCP<const Basic> result;
        bool result_set = false;
        bool is_not_numeric = false;
        std::string expr = "";

        for (unsigned int iter = l; iter < h; ++iter)
        {
            if (is_operator(iter))
            {
                if (s[iter] != '(')
                    if (!result_set)
                        result = set_result(expr, is_not_numeric);

                switch(s[iter])
                {
                    case '+':
                        result = add(result, parse_string(iter+1, operator_end[iter]));
                        iter = operator_end[iter]-1;
                        break;
                    case '*':
                        result = mul(result, parse_string(iter+1, operator_end[iter]));
                        iter = operator_end[iter]-1;
                        break;
                    case '-':
                        result = sub(result, parse_string(iter+1, operator_end[iter]));
                        iter = operator_end[iter]-1;
                        break;
                    case '/':
                        result = div(result, parse_string(iter+1, operator_end[iter]));
                        iter = operator_end[iter]-1;
                        break;
                    case '^':
                        result = pow(result, parse_string(iter+1, operator_end[iter]));
                        iter = operator_end[iter]-1;
                        break;
                    case '(':
                        result = functionify(iter, expr);
                        break;
                    case ')':
                        continue;
                    case ',':
                        continue;
                }

                result_set = true;
                is_not_numeric = false;
            }
            else
            {
                expr += s[iter];

                int ascii = s[iter] - '0';
                if (ascii < 0 or ascii > 9)
                    is_not_numeric = true;

                if (iter == h-1)
                    result = set_result(expr, is_not_numeric);
            }
        }

        return result;
    }

    bool is_operator(int iter)
    {
        if (iter >= 0 and iter < (int)s_len)
            if (OPERATORS.find(s[iter]) != OPERATORS.end())
                return true;
        return false;
    }

    RCP<const Basic> functionify(unsigned int& iter, const std::string& expr)
    {
        RCP<const Basic> param1 = parse_string(iter+1, operator_end[iter]);
        iter = operator_end[iter] - 1;

        if(expr == "") return param1;

        if (single_arg_functions.find(expr) != single_arg_functions.end())
            if (s[iter+1] != ',')
                return single_arg_functions[expr](param1);

        iter++;
        RCP<const Basic> param2 = parse_string(iter+1, operator_end[iter]);
        iter = operator_end[iter] - 1;

        if (double_arg_functions.find(expr) != double_arg_functions.end())
            return double_arg_functions[expr](param1, param2);

        throw std::runtime_error("Unknown function " + expr);
        // remaining : levi_civita
    }

    RCP<const Basic> set_result(const std::string &expr, const bool& is_not_numeric)
    {
        if (expr == "") return zero;

        if (is_not_numeric)
        {
            if (constants.find(expr) != constants.end())
                return constants[expr];
            return symbol(expr);
        }
        else
            return integer(std::atoi(expr.c_str()));
    }

public:

    RCP<const Basic> parse(const std::string &in)
    {
        std::stack<unsigned int> right_bracket;
        std::stack<std::pair<int, unsigned int> > op_stack;
        s = "";

        for (unsigned int i = 0; i < in.length(); ++i)
        {
            if (in[i] == ' ')
                continue;
            s += in[i];
        }

        s_len = s.length();
        operator_end.clear();
        operator_end.resize(s_len);
        op_stack.push(std::make_pair(-1, s_len));

        for (int i = s_len-1; i >= 0; i--)
        {
            if (is_operator(i))
            {
                char x = s[i];
                if(x == '(')
                {
                    while(op_stack.top().second != right_bracket.top())
                        op_stack.pop();
                    operator_end[i] = right_bracket.top();
                    right_bracket.pop();
                    op_stack.pop();
                }
                else if(x == ')' or x == ',')
                {
                    if (x == ',')
                    {
                        operator_end[i] = right_bracket.top();
                        right_bracket.pop();
                    }
                    op_stack.push(std::make_pair(op_precedence[x], i));
                    right_bracket.push(i);
                }
                else
                {
                    while(op_precedence[x] < op_stack.top().first)
                        op_stack.pop();

                    operator_end[i] = op_stack.top().second;
                    op_stack.push(std::make_pair(op_precedence[x], i));
                }
            }
        }
        return parse_string(0, s_len);
    }
};

} // SymEngine
