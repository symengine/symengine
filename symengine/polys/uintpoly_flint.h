/**
 *  \file uintpoly_flint.h
 *  Class for Polynomial: UIntPolyFlint
 **/
#ifndef SYMENGINE_UINTPOLY_FLINT_H
#define SYMENGINE_UINTPOLY_FLINT_H

#include <symengine/polys/upolybase.h>

 #ifdef HAVE_SYMENGINE_FLINT

 #include <flint/flint.h>
 #include <flint/fmpz_polyxx.h>

namespace SymEngine
{

class UIntPolyFlint : public UPolyBase<flint::fmpz_polyxx, UIntPolyFlint>
{
public:
    IMPLEMENT_TYPEID(UINTPOLYFLINT)

}; // UIntPolyFLint

}

#endif // HAVE_SYMENGINE_FLINT

#endif // SYMENGINE_UINTPOLY_FLINT_H