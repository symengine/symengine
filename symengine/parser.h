#ifndef SYMENGINE_PARSER_H
#define SYMENGINE_PARSER_H

#include <symengine/basic.h>

namespace SymEngine
{

SYMENGINE_EXPORT
RCP<const Basic>
parse(const std::string &s, bool convert_xor = true,
      const std::map<const std::string, const RCP<const Basic>> &constants
      = {});

SYMENGINE_EXPORT RCP<const Basic> parse_old(const std::string &s,
                                            bool convert_xor = true);

SYMENGINE_EXPORT
RCP<const Basic>
parse_sbml(const std::string &s,
           const std::map<const std::string, const RCP<const Basic>> &constants
           = {});
} // namespace SymEngine

#endif
