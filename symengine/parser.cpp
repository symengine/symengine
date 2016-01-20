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

namespace SymEngine {

class expressionParser
{
    std::set<char> OPERATORS = {'-', '+', '*', '/', '^', '(', ')', ','};
    std::map<char, int> op_precedence =   {
                                    {')', 0}, {',', 0}, {'-', 1},  {'+', 1},
                                    {'*', 3}, {'/', 4}, {'^', 5}
                                    };
    std::vector<int> operator_end;
    std::string s;
    unsigned int s_len;

public:

    RCP<const Basic> parse_string(unsigned int l, unsigned int h)
    {
        RCP<const Basic> result;
        bool result_set = false;
        bool expr_is_symbol = false;
        std::string expr;

        for (unsigned int iter = l; iter < h; ++iter)
        {
            if (is_operator(iter))
            {
                if (s[iter] != '(')
                {
                    if (!result_set)
                    {
                        if (expr_is_symbol)
                            result = symbol(expr);
                        else
                            result = integer(std::stoi(expr));
                    }
                }

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
                expr_is_symbol = false;
            }
            else
            {
                expr += s[iter];

                int ascii = s[iter] - '0';
                if (ascii < 0 or ascii > 9)
                    expr_is_symbol = true;

                if (iter == h-1)
                {
                    if (expr_is_symbol)
                        result = symbol(expr);
                    else
                        result = integer(std::stoi(expr));
                }
            }
        }

        return result;
    }

    RCP<const Basic> parse(std::string &in)
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

    bool is_operator(int iter)
    {
        if (iter >= 0 and iter < (int)s_len)
            if (OPERATORS.find(s[iter]) != OPERATORS.end())
                return true;
        return false;
    }

    RCP<const Basic> functionify(unsigned int& iter, std::string expr)
    {
        RCP<const Basic> inner = parse_string(iter+1, operator_end[iter]);

        iter = operator_end[iter]-1;
        if(expr == "") return inner;
        if(expr == "sin") return sin(inner);

        throw std::runtime_error("Unknown function " + expr);
    }
};

} // SymEngine
