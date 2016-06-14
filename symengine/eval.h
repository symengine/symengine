/**
 *  \file eval.h
 *
 **/
#ifndef SYMENGINE_EVAL_H
#define SYMENGINE_EVAL_H

#include <symengine/basic.h>
#include <symengine/dict.h>
#include <symengine/symengine_rcp.h>

#ifdef HAVE_SYMENGINE_MPFR
#include <mpfr.h>
#endif // HAVE_SYMENGINE_MPFR

#ifdef SYMENGINE_HAVE_MPC
#include <mpc.h>
#endif // HAVE_SYMENGINE_MPC

namespace SymEngine
{

/*
 * Evaluates basic b, according to the number of significant bits, and the
 * complex / real nature of the number
 */

RCP<const Number> eval2(const Basic &b, unsigned long bits, bool real);

} // SymEngine

#endif // SYMENGINE_EVAL_H