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

namespace SymEngine {

class expressionParser
{
    std::set<char> OPERATORS = {'-', '+', '*', '/', '^', '(', ')'};
    std::map<char, int> opPrecedence =   {
                                    {')', 0}, {'-', 1},  {'+', 1},
                                    {'*', 3}, {'/', 4}, {'^', 5}
                                    };
    std::vector<int> operatorClose;
    std::string S;
    unsigned int Slen;

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
                if (S[iter] != '(')
                {
                    if (!result_set)
                    {
                        if (expr_is_symbol)
                            result = symbol(expr);
                        else
                            result = integer(std::stoi(expr));
                    }
                }

                switch(S[iter])
                {
                    case '+':
                        result = add(result, parse_string(iter+1, operatorClose[iter]));
                        iter = operatorClose[iter]-1;
                        break;
                    case '*':
                        result = mul(result, parse_string(iter+1, operatorClose[iter]));
                        iter = operatorClose[iter]-1;
                        break;
                    case '-':
                        result = sub(result, parse_string(iter+1, operatorClose[iter]));
                        iter = operatorClose[iter]-1;
                        break;
                    case '/':
                        result = div(result, parse_string(iter+1, operatorClose[iter]));
                        iter = operatorClose[iter]-1;
                        break;
                    case '^':
                        result = pow(result, parse_string(iter+1, operatorClose[iter]));
                        iter = operatorClose[iter]-1;
                        break;
                    case '(':
                        result = functionify(iter, expr);
                        iter = operatorClose[iter]-1;
                        break;
                    case ')':
                        continue;
                }

                result_set = true;
                expr_is_symbol = false;
            }
            else
            {
                expr += S[iter];

                int ascii = S[iter] - '0';
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

    RCP<const Basic> parse(std::string &s)
    {
        std::stack<unsigned int> rBracket;
        std::stack<std::pair<int, unsigned int> > opStack;
        S = "";

        for (unsigned int i = 0; i < s.length(); ++i)
        {
            if (s[i] == ' ')
                continue;
            S += s[i];
        }

        Slen = S.length();
        operatorClose.clear();
        operatorClose.resize(Slen);
        opStack.push(std::make_pair(-1, Slen));

        for (int i = Slen-1; i >= 0; i--)
        {
            if (is_operator(i))
            {
                char x = S[i];
                if(x == '(')
                {
                    while(opStack.top().second != rBracket.top())
                        opStack.pop();
                    operatorClose[i] = rBracket.top();
                    rBracket.pop();
                    opStack.pop();
                }
                else if(x == ')')
                {
                    opStack.push(std::make_pair(opPrecedence[x], i));
                    rBracket.push(i);
                }
                else
                {
                    while(opPrecedence[x] < opStack.top().first)
                        opStack.pop();

                    operatorClose[i] = opStack.top().second;
                    opStack.push(std::make_pair(opPrecedence[x], i));
                }
            }
        }
        return parse_string(0, Slen);
    }

    bool is_operator(int iter)
    {
        if (iter >= 0 and iter < (int)Slen)
            if (OPERATORS.find(S[iter]) != OPERATORS.end())
                return true;
        return false;
    }

    RCP<const Basic> functionify(unsigned int iter, std::string expr)
    {
        RCP<const Basic> inner = parse_string(iter+1, operatorClose[iter]);

        if(expr == "") return inner;
        if(expr == "sin") return sin(inner);

        throw std::runtime_error("Unknown function " + expr);
    }
};

} // SymEngine
