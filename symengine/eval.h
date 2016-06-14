/**
 *  \file eval.h
 *
 **/
#ifndef SYMENGINE_EVAL_H
#define SYMENGINE_EVAL_H

#include <symengine/basic.h>
#include <symengine/dict.h>
#include <symengine/symengine_rcp.h>

namespace SymEngine
{

/*
 * Evaluates basic b, according to the number of significant bits, and the
 * complex / real nature of the number
 */

RCP<const Number> eval(const Basic &b, unsigned long bits = 53,
                       bool real = false);

} // SymEngine

#endif // SYMENGINE_EVAL_H
