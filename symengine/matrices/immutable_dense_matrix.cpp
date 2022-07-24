#include <symengine/matrices/immutable_dense_matrix.h>

namespace SymEngine
{

hash_t ImmutableDenseMatrix::__hash__() const
{
    hash_t seed = SYMENGINE_IMMUTABLEDENSEMATRIX;
    hash_combine(seed, m_);
    hash_combine(seed, n_);
    for (const auto &a : values_) {
        hash_combine<Basic>(seed, *a);
    }
    return seed;
}

bool ImmutableDenseMatrix::__eq__(const Basic &o) const
{
    if (is_a<ImmutableDenseMatrix>(o)) {
        const ImmutableDenseMatrix &other
            = down_cast<const ImmutableDenseMatrix &>(o);
        if (m_ != other.m_ || n_ != other.n_) {
            return false;
        }
        return unified_eq(values_, other.values_);
    }
    return false;
}

int ImmutableDenseMatrix::compare(const Basic &o) const
{
    SYMENGINE_ASSERT(is_a<ImmutableDenseMatrix>(o));
    const ImmutableDenseMatrix &other
        = down_cast<const ImmutableDenseMatrix &>(o);
    if (m_ < other.m_) {
        return -1;
    } else if (m_ > other.m_) {
        return 1;
    }
    if (n_ < other.n_) {
        return -1;
    } else if (n_ > other.n_) {
        return 1;
    }
    return unified_compare(values_, other.values_);
}

bool ImmutableDenseMatrix::is_canonical(size_t m, size_t n,
                                        const vec_basic &values) const
{
    if (m < 1 || n < 1 || values.size() == 0) {
        return false;
    }
    if (m * n != values.size()) {
        return false;
    }
    return true;
}

RCP<const MatrixExpr> immutable_dense_matrix(size_t m, size_t n,
                                             const vec_basic &container)
{
    return make_rcp<const ImmutableDenseMatrix>(m, n, container);
}

} // namespace SymEngine
