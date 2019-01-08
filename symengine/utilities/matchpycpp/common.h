#ifndef COMMON_H
#define COMMON_H

#include <symengine/basic.h>
#include <map>
#include <string>

typedef std::map<std::string, SymEngine::RCP<const SymEngine::Basic>>
    Substitution2;

#endif
