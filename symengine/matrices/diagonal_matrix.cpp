#include <symengine/matrices/diagonal_matrix.h>

namespace SymEngine
{

hash_t DiagonalMatrix::__hash__() const
{
    hash_t seed = SYMENGINE_DIAGONALMATRIX;
    for (const auto &a : diag_)
        hash_combine<Basic>(seed, *a);
    return seed;
}

bool DiagonalMatrix::__eq__(const Basic &o) const
{
    if (is_a<DiagonalMatrix>(o)) {
        const DiagonalMatrix &other = down_cast<const DiagonalMatrix &>(o);
        return unified_eq(diag_, other.diag_);
    }
    return false;
}

int DiagonalMatrix::compare(const Basic &o) const
{
    SYMENGINE_ASSERT(is_a<DiagonalMatrix>(o));
    const DiagonalMatrix &other = down_cast<const DiagonalMatrix &>(o);
    return unified_compare(diag_, other.diag_);
}

bool DiagonalMatrix::is_canonical(const vec_basic &container) const
{
    if (container.size() == 0) {
        return false;
    }
    return true;
}

RCP<const MatrixExpr> diagonal_matrix(const vec_basic &container)
{
    return make_rcp<const DiagonalMatrix>(container);
}

} // namespace SymEngine
