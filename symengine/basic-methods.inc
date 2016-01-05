class EvalRealDoubleVisitorFinal;
#define SYMENGINE_INCLUDE_METHODS(SUFFIX) \
virtual void accept(Visitor &v) const SUFFIX; \
virtual void accept(EvalRealDoubleVisitorFinal &v) const SUFFIX; \
virtual RCP<const Basic> diff(const RCP<const Symbol> &x) const SUFFIX;
