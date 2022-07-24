#ifndef SYMENGINE_MATRICES_MATRIX_MUL_H
#define SYMENGINE_MATRICES_MATRIX_MUL_H

#include <symengine/basic.h>
#include <symengine/matrices/matrix_expr.h>
#include <symengine/matrices/size.h>

namespace SymEngine
{

class MatrixMul : public MatrixExpr
{
private:
    vec_basic factors_;

public:
    MatrixMul(const vec_basic &factors) : factors_(factors)
    {
        SYMENGINE_ASSIGN_TYPEID();
        SYMENGINE_ASSERT(is_canonical(factors));
    }

    IMPLEMENT_TYPEID(SYMENGINE_MATRIXMUL)
    hash_t __hash__() const override;
    bool __eq__(const Basic &o) const override;
    int compare(const Basic &o) const override;
    bool is_canonical(const vec_basic &factods) const;
    vec_basic get_args() const override
    {
        return vec_basic(factors_.begin(), factors_.end());
    }
    inline const vec_basic &get_factors() const
    {
        return factors_;
    }
};

RCP<const MatrixExpr> matrix_mul(const vec_basic &factors);

} // namespace SymEngine

#endif
