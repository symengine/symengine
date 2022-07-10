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

hash_t Trace::__hash__() const
{
    hash_t seed = SYMENGINE_TRACE;
    hash_combine<Basic>(seed, *arg_);
    return seed;
}

bool Trace::__eq__(const Basic &o) const
{
    return (is_a<Trace>(o) && arg_->__eq__(*down_cast<const Trace &>(o).arg_));
}

int Trace::compare(const Basic &o) const
{
    SYMENGINE_ASSERT(is_a<Trace>(o));

    return arg_->compare(*down_cast<const Trace &>(o).arg_);
}

vec_basic Trace::get_args() const
{
    return {arg_};
}

RCP<const Basic> trace(const RCP<const MatrixExpr> &arg)
{
    if (is_a<IdentityMatrix>(*arg)) {
        return down_cast<const IdentityMatrix &>(*arg).size();
    } else if (is_a<ZeroMatrix>(*arg)) {
        tribool sq = is_square(*arg);
        if (is_true(sq)) {
            return zero;
        } else if (is_false(sq)) {
            throw DomainError("Trace is only valid for square matrices");
        }
    }
    return make_rcp<const Trace>(arg);
}

class MatrixZeroVisitor : public BaseVisitor<MatrixZeroVisitor>
{
private:
    tribool is_zero_;

public:
    MatrixZeroVisitor() {}

    void bvisit(const Basic &x){};

    void bvisit(const IdentityMatrix &x)
    {
        is_zero_ = tribool::trifalse;
    };

    void bvisit(const ZeroMatrix &x)
    {
        is_zero_ = tribool::tritrue;
    };

    tribool apply(const MatrixExpr &s)
    {
        s.accept(*this);
        return is_zero_;
    };
};

tribool is_zero(const MatrixExpr &m)
{
    MatrixZeroVisitor visitor;
    return visitor.apply(m);
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
        is_symmetric_ = is_square(x);
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

class MatrixSquareVisitor : public BaseVisitor<MatrixSquareVisitor>
{
private:
    tribool is_square_;

public:
    MatrixSquareVisitor() {}

    void bvisit(const Basic &x){};

    void bvisit(const IdentityMatrix &x)
    {
        is_square_ = tribool::tritrue;
    };

    void bvisit(const ZeroMatrix &x)
    {
        auto diff = sub(x.nrows(), x.ncols());
        is_square_ = is_zero(*diff);
    };

    tribool apply(const MatrixExpr &s)
    {
        s.accept(*this);
        return is_square_;
    };
};

tribool is_square(const MatrixExpr &m)
{
    MatrixSquareVisitor visitor;
    return visitor.apply(m);
}

class MatrixDiagonalVisitor : public BaseVisitor<MatrixDiagonalVisitor>
{
private:
    tribool is_diagonal_;

public:
    MatrixDiagonalVisitor() {}

    void bvisit(const Basic &x){};

    void bvisit(const IdentityMatrix &x)
    {
        is_diagonal_ = tribool::tritrue;
    };

    void bvisit(const ZeroMatrix &x)
    {
        is_diagonal_ = is_square(x);
    };

    tribool apply(const MatrixExpr &s)
    {
        s.accept(*this);
        return is_diagonal_;
    };
};

tribool is_diagonal(const MatrixExpr &m)
{
    MatrixDiagonalVisitor visitor;
    return visitor.apply(m);
}

} // namespace SymEngine
