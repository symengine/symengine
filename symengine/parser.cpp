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

/*
operator before any char (error)
handle negative (-3*(-4))
handle symbols? (how)
handle all types of errors
handle functions
handle constants
handle rationals/doubles entered
give "not simplify" option
*/

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
        std::string expr;

        for (uint iter = l; iter < h; ++iter)
        {
            if (OPERATORS.find(s[iter]) != OPERATORS.end())
            {
                if (s[iter] != '(')
                    if (!result_set)
                        result = integer(std::stoi(expr));
                
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
            }
            else
            {
                expr += s[iter];

                if (iter == h-1)
                    result = integer(std::stoi(expr));
            }
        }

        return result;
    }

    RCP<const Basic> parse(std::string &s)
    {
        std::string copy;
        char x;
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
            x = copy[i];
            if (OPERATORS.find(x) != OPERATORS.end())
            {   
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
};

} // SymEngine
