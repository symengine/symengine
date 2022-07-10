#ifndef SYMENGINE_MATRIXEXPR_H
#define SYMENGINE_MATRIXEXPR_H

#include <symengine/basic.h>
#include <symengine/integer.h>

namespace SymEngine
{

class MatrixExpr : public Basic
{
};

class IdentityMatrix : public MatrixExpr
{
private:
    RCP<const Basic> n_; // n >= 0

public:
    IdentityMatrix(const RCP<const Basic> &n) : n_(n)
    {
        SYMENGINE_ASSIGN_TYPEID();
    }

    IMPLEMENT_TYPEID(SYMENGINE_IDENTITYMATRIX)
    hash_t __hash__() const override;
    bool __eq__(const Basic &o) const override;
    int compare(const Basic &o) const override;
    vec_basic get_args() const override;
};

RCP<const MatrixExpr> identity_matrix(const RCP<const Basic> &n);

tribool is_real(const MatrixExpr &m);
tribool is_symmetric(const MatrixExpr &m);

} // namespace SymEngine

#endif
