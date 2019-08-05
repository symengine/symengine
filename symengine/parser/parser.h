#ifndef SYMENGINE_PARSER_PARSER_H
#define SYMENGINE_PARSER_PARSER_H

#include <fstream>
#include <algorithm>
#include <memory>

#include <symengine/parser/tokenizer.h>
#include <symengine/add.h>
#include <symengine/pow.h>
#include <symengine/logic.h>

namespace SymEngine
{

/*
   To Parse (default) constructor is expensive as it creates all the maps and
   tables. If just one expression needs to be parsed, then calling
   SymEngine::parse() does the job. But if multiple expressions are to be
   parsed, then first initialize SymEngine::Parser and after that call
   SymEngine::Parser::parse() repeatedly.

   Example:

   Parser p;
   auto r = p.parse("x**2");

*/

// reference :
// http://stackoverflow.com/questions/30393285/stdfunction-fails-to-distinguish-overloaded-functions
typedef RCP<const Basic> (*single_arg_func)(const RCP<const Basic> &);
typedef RCP<const Basic> (*double_arg_func)(const RCP<const Basic> &,
                                            const RCP<const Basic> &);
typedef RCP<const Boolean> (*single_arg_boolean_func)(const RCP<const Basic> &);
typedef RCP<const Boolean> (*double_arg_boolean_func)(const RCP<const Basic> &,
                                                      const RCP<const Basic> &);

class Parser
{
    std::string inp;

public:
    Tokenizer m_tokenizer;
    RCP<const Basic> res;

    RCP<const Basic> parse(const std::string &input, bool convert_xor = true);

    const static std::map<const std::string, const RCP<const Basic>> constants;

    // maps string to corresponding single argument function
    const static std::map<const std::string,
                          const std::function<RCP<const Basic>(
                              const RCP<const Basic> &)>>
        single_arg_functions;

    // maps string to corresponding double argument function
    const static std::map<const std::string,
                          const std::function<RCP<const Basic>(
                              const RCP<const Basic> &,
                              const RCP<const Basic> &)>>
        double_arg_functions;

    // maps string to corresponding multi argument function
    const static std::map<const std::string,
                          const std::function<RCP<const Basic>(vec_basic &)>>
        multi_arg_functions;

    // maps string to corresponding single argument boolean function
    const static std::map<const std::string,
                          const std::function<RCP<const Boolean>(
                              const RCP<const Basic> &)>>
        single_arg_boolean_functions;

    // maps string to corresponding single argument boolean function (accepting
    // Boolean objects)
    const static std::map<const std::string,
                          const std::function<RCP<const Boolean>(
                              const RCP<const Boolean> &)>>
        single_arg_boolean_boolean_functions;

    // maps string to corresponding double argument boolean function
    const static std::map<const std::string,
                          const std::function<RCP<const Boolean>(
                              const RCP<const Basic> &,
                              const RCP<const Basic> &)>>
        double_arg_boolean_functions;
    // maps string to corresponding multi argument vec_boolean function
    const static std::map<const std::string,
                          const std::function<RCP<const Boolean>(
                              vec_boolean &)>>
        multi_arg_vec_boolean_functions;

    // maps string to corresponding multi argument set_boolean function
    const static std::map<const std::string,
                          const std::function<RCP<const Boolean>(
                              set_boolean &)>>
        multi_arg_set_boolean_functions;

    int parse();

    RCP<const Basic> functionify(const std::string &name, vec_basic &params);
    RCP<const Basic> parse_numeric(const std::string &expr);
    RCP<const Basic> parse_identifier(const std::string &expr);
    std::tuple<RCP<const Basic>, RCP<const Basic>>
    parse_implicit_mul(const std::string &expr);

private:
};

} // namespace SymEngine

#endif
