/**
 *  \file eval.h
 *
 **/
#ifndef SYMENGINE_EVAL_H
#define SYMENGINE_EVAL_H

#include <symengine/basic.h>
#include <symengine/dict.h>
#include <symengine/symengine_rcp.h>

#ifdef SYMENGINE_HAVE_MPFR
#include <mpfr.h>
#endif // SYMENGINE_HAVE_MPFR

#ifdef SYMENGINE_HAVE_MPC
#include <mpc.h>
#endif // SYMENGINE_HAVE_MPC

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
