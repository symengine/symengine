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

bool IdentityMatrix::is_canonical(const RCP<const Basic> &n) const
{
    if (is_a_Number(*n)) {
        if (is_a<Integer>(*n)) {
            if (down_cast<const Integer &>(*n).is_negative()) {
                return false;
            }
        } else {
            return false;
        }
    }
    return true;
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

bool ZeroMatrix::is_canonical(const RCP<const Basic> &m,
                              const RCP<const Basic> &n) const
{
    if (is_a_Number(*m)) {
        if (is_a<Integer>(*m)) {
            if (down_cast<const Integer &>(*m).is_negative()) {
                return false;
            }
        } else {
            return false;
        }
    }
    if (is_a_Number(*n)) {
        if (is_a<Integer>(*n)) {
            if (down_cast<const Integer &>(*n).is_negative()) {
                return false;
            }
        } else {
            return false;
        }
    }
    return true;
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

hash_t MatrixAdd::__hash__() const
{
    hash_t seed = SYMENGINE_MATRIXADD;
    for (const auto &a : terms_) {
        hash_combine<Basic>(seed, *a);
    }
    return seed;
}

bool MatrixAdd::__eq__(const Basic &o) const
{
    if (is_a<MatrixAdd>(o)) {
        const MatrixAdd &other = down_cast<const MatrixAdd &>(o);
        return unified_eq(terms_, other.terms_);
    }
    return false;
}

int MatrixAdd::compare(const Basic &o) const
{
    SYMENGINE_ASSERT(is_a<MatrixAdd>(o));
    const MatrixAdd &other = down_cast<const MatrixAdd &>(o);
    return unified_compare(terms_, other.terms_);
}

bool MatrixAdd::is_canonical(const vec_basic terms) const
{
    if (terms.size() < 2) {
        return false;
    }
    size_t num_diag = 0;
    for (auto term : terms) {
        if (is_a<ZeroMatrix>(*term) || is_a<MatrixAdd>(*term)) {
            return false;
        } else if (is_a<DiagonalMatrix>(*term)) {
            num_diag++;
        }
    }
    if (num_diag > 1) {
        return false;
    }
    return true;
}

RCP<const MatrixExpr> matrix_add(const vec_basic &terms)
{
    if (terms.size() == 0) {
        throw DomainError("Empty sum of matrices");
    }
    if (terms.size() == 1) {
        return rcp_static_cast<const MatrixExpr>(terms[0]);
    }
    // extract nested MatrixAdd
    vec_basic expanded;
    for (auto &term : terms) {
        if (is_a<const MatrixAdd>(*term)) {
            auto container = down_cast<const MatrixAdd &>(*term).get_terms();
            expanded.insert(expanded.end(), container.begin(), container.end());
        } else {
            expanded.push_back(term);
        }
    }
    // Check sizes
    auto sz = size(down_cast<const MatrixExpr &>(*expanded[0]));
    for (auto it = expanded.begin() + 1; it != expanded.end(); ++it) {
        auto cursize = size(down_cast<const MatrixExpr &>(**it));
        auto rowdiff = sub(sz.first, cursize.first);
        tribool rowmatch = is_zero(*rowdiff);
        if (is_false(rowmatch)) {
            throw DomainError("Matrix dimension mismatch");
        }
        auto coldiff = sub(sz.second, cursize.second);
        tribool colmatch = is_zero(*coldiff);
        if (is_false(colmatch)) {
            throw DomainError("Matrix dimension mismatch");
        }
    }
    vec_basic keep;
    RCP<const DiagonalMatrix> diag;
    RCP<const ZeroMatrix> zero;
    for (auto &term : expanded) {
        if (is_a<ZeroMatrix>(*term)) {
            zero = rcp_static_cast<const ZeroMatrix>(term);
        } else if (is_a<DiagonalMatrix>(*term)) {
            if (diag.is_null()) {
                diag = rcp_static_cast<const DiagonalMatrix>(term);
            } else {
                vec_basic container;
                for (size_t i = 0; i < diag->get_container().size(); i++) {
                    container.push_back(
                        add(diag->get_container()[i],
                            down_cast<const DiagonalMatrix &>(*term)
                                .get_container()[i]));
                }
                diag = make_rcp<const DiagonalMatrix>(container);
            }
        } else {
            keep.push_back(term);
        }
    }
    if (!diag.is_null()) {
        keep.push_back(diag);
    }
    if (keep.size() == 1) {
        return rcp_static_cast<const MatrixExpr>(keep[0]);
    }
    if (keep.size() == 0 && !zero.is_null()) {
        return zero;
    }
    return make_rcp<const MatrixAdd>(keep);
}

class MatrixZeroVisitor : public BaseVisitor<MatrixZeroVisitor>
{
private:
    tribool is_zero_;
    const Assumptions *assumptions_;

public:
    MatrixZeroVisitor(const Assumptions *assumptions)
        : assumptions_(assumptions)
    {
    }

    void bvisit(const Basic &x){};
    void bvisit(const MatrixExpr &x)
    {
        is_zero_ = tribool::indeterminate;
    };

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
            tribool next = is_zero(*e, assumptions_);
            if (is_false(next)) {
                is_zero_ = next;
                return;
            }
            current = andwk_tribool(current, next);
        }
        is_zero_ = current;
    };

    void bvisit(const MatrixAdd &x)
    {
        is_zero_ = tribool::indeterminate;
    }

    tribool apply(const MatrixExpr &s)
    {
        s.accept(*this);
        return is_zero_;
    };
};

tribool is_zero(const MatrixExpr &m, const Assumptions *assumptions)
{
    MatrixZeroVisitor visitor(assumptions);
    return visitor.apply(m);
}

class MatrixRealVisitor : public BaseVisitor<MatrixRealVisitor>
{
private:
    tribool is_real_;
    const Assumptions *assumptions_;

public:
    MatrixRealVisitor(const Assumptions *assumptions)
        : assumptions_(assumptions)
    {
    }

    void bvisit(const Basic &x){};
    void bvisit(const MatrixExpr &x)
    {
        is_real_ = tribool::indeterminate;
    };

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
            tribool next = is_real(*e, assumptions_);
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

tribool is_real(const MatrixExpr &m, const Assumptions *assumptions)
{
    MatrixRealVisitor visitor(assumptions);
    return visitor.apply(m);
}

class MatrixSymmetricVisitor : public BaseVisitor<MatrixSymmetricVisitor>
{
private:
    tribool is_symmetric_;
    const Assumptions *assumptions_;

public:
    MatrixSymmetricVisitor(const Assumptions *assumptions)
        : assumptions_(assumptions)
    {
    }

    void bvisit(const Basic &x){};
    void bvisit(const MatrixExpr &x)
    {
        is_symmetric_ = tribool::indeterminate;
    };

    void bvisit(const IdentityMatrix &x)
    {
        is_symmetric_ = tribool::tritrue;
    };

    void bvisit(const ZeroMatrix &x)
    {
        is_symmetric_ = is_square(x, assumptions_);
    };

    void bvisit(const DiagonalMatrix &x)
    {
        is_symmetric_ = tribool::tritrue;
    };

    void bvisit(const MatrixAdd &x)
    {
        // MatrixAdd can currently only be diagonal+identity
        is_symmetric_ = tribool::tritrue;
    };

    tribool apply(const MatrixExpr &s)
    {
        s.accept(*this);
        return is_symmetric_;
    };
};

tribool is_symmetric(const MatrixExpr &m, const Assumptions *assumptions)
{
    MatrixSymmetricVisitor visitor(assumptions);
    return visitor.apply(m);
}

class MatrixSquareVisitor : public BaseVisitor<MatrixSquareVisitor>
{
private:
    tribool is_square_;
    const Assumptions *assumptions_;

public:
    MatrixSquareVisitor(const Assumptions *assumptions)
        : assumptions_(assumptions)
    {
    }

    void bvisit(const Basic &x){};
    void bvisit(const MatrixExpr &x)
    {
        is_square_ = tribool::indeterminate;
    };

    void bvisit(const IdentityMatrix &x)
    {
        is_square_ = tribool::tritrue;
    };

    void bvisit(const ZeroMatrix &x)
    {
        auto diff = sub(x.nrows(), x.ncols());
        is_square_ = is_zero(*diff, assumptions_);
    };

    void bvisit(const DiagonalMatrix &x)
    {
        is_square_ = tribool::tritrue;
    };

    void bvisit(const MatrixAdd &x)
    {
        for (auto &elt : x.get_terms()) {
            elt->accept(*this);
            if (not is_indeterminate(is_square_)) {
                return;
            }
        }
    };

    tribool apply(const MatrixExpr &s)
    {
        s.accept(*this);
        return is_square_;
    };
};

tribool is_square(const MatrixExpr &m, const Assumptions *assumptions)
{
    MatrixSquareVisitor visitor(assumptions);
    return visitor.apply(m);
}

class MatrixDiagonalVisitor : public BaseVisitor<MatrixDiagonalVisitor>
{
private:
    tribool is_diagonal_;
    const Assumptions *assumptions_;

public:
    MatrixDiagonalVisitor(const Assumptions *assumptions)
        : assumptions_(assumptions)
    {
    }

    void bvisit(const Basic &x){};
    void bvisit(const MatrixExpr &x)
    {
        is_diagonal_ = tribool::indeterminate;
    };

    void bvisit(const IdentityMatrix &x)
    {
        is_diagonal_ = tribool::tritrue;
    };

    void bvisit(const ZeroMatrix &x)
    {
        is_diagonal_ = is_square(x, assumptions_);
    };

    void bvisit(const DiagonalMatrix &x)
    {
        is_diagonal_ = tribool::tritrue;
    };

    void bvisit(const MatrixAdd &x)
    {
        // MatrixAdd can currently only be diagonal+identity
        is_diagonal_ = tribool::tritrue;
    };

    tribool apply(const MatrixExpr &s)
    {
        s.accept(*this);
        return is_diagonal_;
    };
};

tribool is_diagonal(const MatrixExpr &m, const Assumptions *assumptions)
{
    MatrixDiagonalVisitor visitor(assumptions);
    return visitor.apply(m);
}

class MatrixLowerVisitor : public BaseVisitor<MatrixLowerVisitor>
{
private:
    tribool is_lower_;
    const Assumptions *assumptions_;

public:
    MatrixLowerVisitor(const Assumptions *assumptions)
        : assumptions_(assumptions)
    {
    }

    void bvisit(const Basic &x){};
    void bvisit(const MatrixExpr &x)
    {
        is_lower_ = tribool::indeterminate;
    };

    void bvisit(const IdentityMatrix &x)
    {
        is_lower_ = tribool::tritrue;
    };

    void bvisit(const ZeroMatrix &x)
    {
        is_lower_ = is_square(x, assumptions_);
    };

    void bvisit(const DiagonalMatrix &x)
    {
        is_lower_ = tribool::tritrue;
    };

    void bvisit(const MatrixAdd &x)
    {
        // MatrixAdd can currently only be diagonal+identity
        is_lower_ = tribool::tritrue;
    };

    tribool apply(const MatrixExpr &s)
    {
        s.accept(*this);
        return is_lower_;
    };
};

tribool is_lower(const MatrixExpr &m, const Assumptions *assumptions)
{
    MatrixLowerVisitor visitor(assumptions);
    return visitor.apply(m);
}

class MatrixUpperVisitor : public BaseVisitor<MatrixUpperVisitor>
{
private:
    tribool is_upper_;
    const Assumptions *assumptions_;

public:
    MatrixUpperVisitor(const Assumptions *assumptions)
        : assumptions_(assumptions)
    {
    }

    void bvisit(const Basic &x){};
    void bvisit(const MatrixExpr &x)
    {
        is_upper_ = tribool::indeterminate;
    };

    void bvisit(const IdentityMatrix &x)
    {
        is_upper_ = tribool::tritrue;
    };

    void bvisit(const ZeroMatrix &x)
    {
        is_upper_ = is_square(x, assumptions_);
    };

    void bvisit(const DiagonalMatrix &x)
    {
        is_upper_ = tribool::tritrue;
    };

    void bvisit(const MatrixAdd &x)
    {
        // MatrixAdd can currently only be diagonal+identity
        is_upper_ = tribool::tritrue;
    };

    tribool apply(const MatrixExpr &s)
    {
        s.accept(*this);
        return is_upper_;
    };
};

tribool is_upper(const MatrixExpr &m, const Assumptions *assumptions)
{
    MatrixUpperVisitor visitor(assumptions);
    return visitor.apply(m);
}

class MatrixToeplitzVisitor : public BaseVisitor<MatrixToeplitzVisitor>
{
private:
    tribool is_toeplitz_;
    const Assumptions *assumptions_;

public:
    MatrixToeplitzVisitor(const Assumptions *assumptions)
        : assumptions_(assumptions)
    {
    }

    void bvisit(const Basic &x){};
    void bvisit(const MatrixExpr &x)
    {
        is_toeplitz_ = tribool::indeterminate;
    };

    void bvisit(const IdentityMatrix &x)
    {
        is_toeplitz_ = tribool::tritrue;
    };

    void bvisit(const ZeroMatrix &x)
    {
        is_toeplitz_ = tribool::tritrue;
    };

    void bvisit(const DiagonalMatrix &x)
    {
        tribool current = tribool::tritrue;
        auto vec = x.get_container();
        if (vec.size() == 1) {
            is_toeplitz_ = tribool::tritrue;
            return;
        }
        auto first = vec[0];
        for (auto it = vec.begin() + 1; it != vec.end(); ++it) {
            auto diff = sub(first, *it);
            tribool next = is_zero(*diff, assumptions_);
            if (is_false(next)) {
                is_toeplitz_ = next;
                return;
            }
            current = andwk_tribool(current, next);
        }
        is_toeplitz_ = current;
    };

    tribool apply(const MatrixExpr &s)
    {
        s.accept(*this);
        return is_toeplitz_;
    };
};

tribool is_toeplitz(const MatrixExpr &m, const Assumptions *assumptions)
{
    MatrixToeplitzVisitor visitor(assumptions);
    return visitor.apply(m);
}

class MatrixSizeVisitor : public BaseVisitor<MatrixSizeVisitor>
{
private:
    RCP<const Basic> nrows_;
    RCP<const Basic> ncols_;

public:
    MatrixSizeVisitor() {}

    void bvisit(const Basic &x){};

    void bvisit(const IdentityMatrix &x)
    {
        nrows_ = x.size();
        ncols_ = x.size();
    };

    void bvisit(const ZeroMatrix &x)
    {
        nrows_ = x.nrows();
        ncols_ = x.ncols();
    };

    void bvisit(const DiagonalMatrix &x)
    {
        nrows_ = integer(x.get_container().size());
        ncols_ = nrows_;
    };

    std::pair<RCP<const Basic>, RCP<const Basic>> apply(const MatrixExpr &s)
    {
        s.accept(*this);
        return std::make_pair(nrows_, ncols_);
    };
};

std::pair<RCP<const Basic>, RCP<const Basic>> size(const MatrixExpr &m)
{
    MatrixSizeVisitor visitor;
    return visitor.apply(m);
}

} // namespace SymEngine
