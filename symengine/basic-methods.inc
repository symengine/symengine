class EvalRealDoubleVisitorFinal;
#define _SYMENGINE_INCLUDE_METHODS(PREFIX, SUFFIX) \
PREFIX void accept(Visitor &v) const SUFFIX \
PREFIX void accept(EvalRealDoubleVisitorFinal &v) const SUFFIX

#define SYMENGINE_INCLUDE_METHODS_BASE() _SYMENGINE_INCLUDE_METHODS(virtual, = 0;)

#define SYMENGINE_INCLUDE_METHODS_DERIVED() _SYMENGINE_INCLUDE_METHODS(, override;)
