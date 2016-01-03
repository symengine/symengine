#define SYMENGINE_INCLUDE_METHODS \
virtual void accept(Visitor &v) const; \
virtual RCP<const Basic> diff(const RCP<const Symbol> &x) const; \
virtual RCP<const Basic> diff2(const RCP<const Symbol> &x) const;
