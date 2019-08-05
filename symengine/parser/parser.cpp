#include <symengine/parser/parser.h>
#include <symengine/parser/parser.tab.hh>
#include <symengine/real_double.h>
#include <symengine/real_mpfr.h>

namespace SymEngine
{

RCP<const Basic> parse(const std::string &s, bool convert_xor)
{
    // This is expensive:
    Parser p;
    // If you need to parse multiple strings, initialize Parser first, then
    // call Parser::parse() repeatedly.
    return p.parse(s, convert_xor);
}

RCP<const Basic> Parser::parse(const std::string &input, bool convert_xor)
{
    inp = input;
    if (convert_xor) {
        std::replace(inp.begin(), inp.end(), '^', '@');
    }
    m_tokenizer.set_string(inp);
    if (yyparse(*this) == 0)
        return this->res;
    throw ParseError("Parsing Unsuccessful");
}

RCP<const Basic> Parser::functionify(const std::string &name, vec_basic &params)
{
    if (params.size() == 1) {
        if (single_arg_functions.find(name) != single_arg_functions.end()) {
            return single_arg_functions[name](params[0]);
        }
        if (single_arg_boolean_functions.find(name)
            != single_arg_boolean_functions.end()) {
            return single_arg_boolean_functions[name](params[0]);
        }
        if (single_arg_boolean_boolean_functions.find(name)
            != single_arg_boolean_boolean_functions.end()) {
            return single_arg_boolean_boolean_functions[name](
                rcp_static_cast<const Boolean>(params[0]));
        }
    }

    if (params.size() == 2) {
        if (double_arg_functions.find(name) != double_arg_functions.end()) {
            return double_arg_functions[name](params[0], params[1]);
        }
        if (double_arg_boolean_functions.find(name)
            != double_arg_boolean_functions.end()) {
            return double_arg_boolean_functions[name](params[0], params[1]);
        }
    }

    if (multi_arg_functions.find(name) != multi_arg_functions.end()) {
        return multi_arg_functions[name](params);
    }

    if (multi_arg_vec_boolean_functions.find(name)
        != multi_arg_vec_boolean_functions.end()) {
        vec_boolean p;
        for (auto &v : params) {
            p.push_back(rcp_static_cast<const Boolean>(v));
        }
        return multi_arg_vec_boolean_functions[name](p);
    }

    if (multi_arg_set_boolean_functions.find(name)
        != multi_arg_set_boolean_functions.end()) {
        set_boolean s;
        for (auto &v : params) {
            s.insert(rcp_static_cast<const Boolean>(v));
        }
        return multi_arg_set_boolean_functions[name](s);
    }

    return function_symbol(name, params);
}

RCP<const Basic> Parser::parse_identifier(const std::string &expr)
{
    auto c = constants.find(expr);
    if (c != constants.end()) {
        return c->second;
    } else {
        return symbol(expr);
    }
}

RCP<const Basic> Parser::parse_numeric(const std::string &expr)
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

std::tuple<RCP<const Basic>, RCP<const Basic>>
Parser::parse_implicit_mul(const std::string &expr)
{
    const char *startptr = expr.c_str();
    char *endptr = 0;
    std::strtod(startptr, &endptr);

    RCP<const Basic> num = one, sym;

    // Numerical part of the result of e.g. "100x";
    size_t length = endptr - startptr;
    std::string lexpr = std::string(startptr, length);
    num = parse_numeric(lexpr);

    // get the rest of the string
    lexpr = std::string(startptr + length, expr.length() - length);
    if (lexpr.length() == 0) {
        sym = one;
    } else {
        sym = parse_identifier(lexpr);
    }
    return std::make_tuple(num, sym);
}

} // namespace SymEngine
