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

RCP<const MatrixExpr> diagonal_matrix(const vec_basic &container)
{
    return make_rcp<const DiagonalMatrix>(container);
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

class MatrixTraceVisitor : public BaseVisitor<MatrixTraceVisitor>
{
private:
    RCP<const Basic> trace_;

public:
    MatrixTraceVisitor() {}

    void bvisit(const Basic &x){};

    void bvisit(const IdentityMatrix &x)
    {
        trace_ = x.size();
    };

    void bvisit(const ZeroMatrix &x)
    {
        tribool sq = is_square(x);
        if (is_true(sq)) {
            trace_ = zero;
        } else if (is_false(sq)) {
            throw DomainError("Trace is only valid for square matrices");
        } else {
            auto arg = rcp_static_cast<const MatrixExpr>(x.rcp_from_this());
            trace_ = make_rcp<const Trace>(arg);
        }
    };

    void bvisit(const DiagonalMatrix &x)
    {
        trace_ = add(x.get_container());
    };

    RCP<const Basic> apply(const MatrixExpr &s)
    {
        s.accept(*this);
        return trace_;
    };
};

RCP<const Basic> trace(const RCP<const MatrixExpr> &arg)
{
    MatrixTraceVisitor visitor;
    return visitor.apply(*arg);
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

    void bvisit(const DiagonalMatrix &x)
    {
        tribool current = tribool::tritrue;
        for (auto &e : x.get_container()) {
            tribool next = is_zero(*e);
            if (is_false(next)) {
                is_zero_ = next;
                return;
            }
            current = andwk_tribool(current, next);
        }
        is_zero_ = current;
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

    void bvisit(const DiagonalMatrix &x)
    {
        tribool current = tribool::tritrue;
        for (auto &e : x.get_container()) {
            tribool next = is_real(*e);
            if (is_false(next)) {
                is_real_ = next;
                return;
            }
            current = andwk_tribool(current, next);
        }
        is_real_ = current;
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

    void bvisit(const DiagonalMatrix &x)
    {
        is_symmetric_ = tribool::tritrue;
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

    void bvisit(const DiagonalMatrix &x)
    {
        is_square_ = tribool::tritrue;
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

    void bvisit(const DiagonalMatrix &x)
    {
        is_diagonal_ = tribool::tritrue;
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
