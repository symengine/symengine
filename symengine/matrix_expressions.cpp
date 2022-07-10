#include <symengine/matrix_expressions.h>
#include <symengine/visitor.h>

namespace SymEngine
{

hash_t IdentityMatrix::__hash__() const
{
    return n_->__hash__();
}

bool IdentityMatrix::__eq__(const Basic &o) const
{
    return (is_a<IdentityMatrix>(o)
            && n_->__eq__(*down_cast<const IdentityMatrix &>(o).n_));
}

int IdentityMatrix::compare(const Basic &o) const
{
    SYMENGINE_ASSERT(is_a<IdentityMatrix>(o));

    return n_->compare(*down_cast<const IdentityMatrix &>(o).n_);
}

vec_basic IdentityMatrix::get_args() const
{
    return {n_};
}

RCP<const MatrixExpr> identity_matrix(const RCP<const Basic> &n)
{
    if (is_a_Number(*n)) {
        if (is_a<Integer>(*n)) {
            if (down_cast<const Integer &>(*n).is_negative()) {
                throw DomainError(
                    "Dimension of IdentityMatrix must be nonnegative");
            }
        } else {
            throw DomainError(
                "Dimension of IdentityMatrix must be a nonnegative integer");
        }
    }
    return make_rcp<const IdentityMatrix>(n);
}

hash_t ZeroMatrix::__hash__() const
{
    hash_t seed = SYMENGINE_ZEROMATRIX;
    hash_combine<Basic>(seed, *m_);
    hash_combine<Basic>(seed, *n_);
    return seed;
}

bool ZeroMatrix::__eq__(const Basic &o) const
{
    return (is_a<ZeroMatrix>(o)
            && m_->__eq__(*down_cast<const ZeroMatrix &>(o).m_)
            && n_->__eq__(*down_cast<const ZeroMatrix &>(o).n_));
}

int ZeroMatrix::compare(const Basic &o) const
{
    SYMENGINE_ASSERT(is_a<ZeroMatrix>(o));

    const ZeroMatrix &other = down_cast<const ZeroMatrix &>(o);
    auto temp = m_->compare(*(other.m_));
    if (temp != 0) {
        return temp;
    } else {
        return n_->compare(*(other.n_));
    }
}

vec_basic ZeroMatrix::get_args() const
{
    return {m_, n_};
}

RCP<const MatrixExpr> zero_matrix(const RCP<const Basic> &m,
                                  const RCP<const Basic> &n)
{
    if (is_a_Number(*m)) {
        if (is_a<Integer>(*m)) {
            if (down_cast<const Integer &>(*m).is_negative()) {
                throw DomainError(
                    "Dimension of ZeroMatrix must be nonnegative");
            }
        } else {
            throw DomainError(
                "Dimension of ZeroMatrix must be a nonnegative integer");
        }
    }
    if (is_a_Number(*n)) {
        if (is_a<Integer>(*n)) {
            if (down_cast<const Integer &>(*n).is_negative()) {
                throw DomainError(
                    "Dimension of ZeroMatrix must be nonnegative");
            }
        } else {
            throw DomainError(
                "Dimension of ZeroMatrix must be a nonnegative integer");
        }
    }

    return make_rcp<const ZeroMatrix>(m, n);
}

class MatrixRealVisitor : public BaseVisitor<MatrixRealVisitor>
{
private:
    tribool is_real_;

public:
    MatrixRealVisitor() {}

    void bvisit(const Basic &x){};

    void bvisit(const IdentityMatrix &x)
    {
        is_real_ = tribool::tritrue;
    };

    void bvisit(const ZeroMatrix &x)
    {
        is_real_ = tribool::tritrue;
    };

    tribool apply(const MatrixExpr &s)
    {
        s.accept(*this);
        return is_real_;
    };
};

tribool is_real(const MatrixExpr &m)
{
    MatrixRealVisitor visitor;
    return visitor.apply(m);
}

class MatrixSymmetricVisitor : public BaseVisitor<MatrixSymmetricVisitor>
{
private:
    tribool is_symmetric_;

public:
    MatrixSymmetricVisitor() {}

    void bvisit(const Basic &x){};

    void bvisit(const IdentityMatrix &x)
    {
        is_symmetric_ = tribool::tritrue;
    };

    void bvisit(const ZeroMatrix &x)
    {
        auto diff = sub(x.nrows(), x.ncols());
        is_symmetric_ = is_zero(*diff);
    };

    tribool apply(const MatrixExpr &s)
    {
        s.accept(*this);
        return is_symmetric_;
    };
};

tribool is_symmetric(const MatrixExpr &m)
{
    MatrixSymmetricVisitor visitor;
    return visitor.apply(m);
}

} // namespace SymEngine
