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
#include <bits/stdc++.h>

namespace SymEngine {

class expressionParser
{
    std::set<char> OPERATORS = {'-', '+', '*', '/', '^', '(', ')'};
    std::map<char, int> opPrecedence =   {
                                    {')', 0}, {'-', 1},  {'+', 1},
                                    {'*', 3}, {'/', 4}, {'^', 5}
                                    };
    std::vector<int> operatorClose;

public:

    RCP<const Basic> parse_string(std::string &s, uint l, uint h)
    {
        RCP<const Basic> result;
        bool result_set = false;
        bool expr_is_symbol = false;
        std::string expr;

        for (uint iter = l; iter < h; ++iter)
        {
            if (is_operator(s, iter))
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
                        result = add(result, parse_string(s, iter+1, operatorClose[iter]));
                        iter = operatorClose[iter]-1;
                        break;
                    case '(':
                        result = parse_string(s, iter+1, operatorClose[iter]);
                        iter = operatorClose[iter]-1;
                        break;
                    case '*':
                        result = mul(result, parse_string(s, iter+1, operatorClose[iter]));
                        iter = operatorClose[iter]-1;
                        break;
                    case '-':
                        result = sub(result, parse_string(s, iter+1, operatorClose[iter]));
                        iter = operatorClose[iter]-1;
                        break;
                    case '/':
                        result = div(result, parse_string(s, iter+1, operatorClose[iter]));
                        iter = operatorClose[iter]-1;
                        break;
                    case '^':
                        result = pow(result, parse_string(s, iter+1, operatorClose[iter]));
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

    RCP<const Basic> parse(std::string &s)
    {
        std::string copy;
        std::stack<uint> rBracket;
        std::stack<std::pair<int, uint> > opStack;

        for (uint i = 0; i < s.length(); ++i)
        {
            if (s[i] == ' ')
                continue;
            copy += s[i];
        }

        uint newLength = copy.length();
        operatorClose.clear();
        operatorClose.resize(newLength);
        opStack.push(std::make_pair(-1, newLength));

        for (int i = newLength-1; i >= 0; i--)
        {
            if (is_operator(copy, i))
            {
                char x = copy[i];
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
        return parse_string(copy, 0, newLength);
    }

    bool is_operator(std::string& s, int iter)
    {
        if (iter >= 0 and iter < (int)s.length())
            if (OPERATORS.find(s[iter]) != OPERATORS.end())
                return true;
        return false;
    }
};

} // SymEngine
