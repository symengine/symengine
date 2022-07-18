#include <symengine/matrix_expressions.h>
#include <symengine/visitor.h>
#include <symengine/test_visitors.h>

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

hash_t MatrixSymbol::__hash__() const
{
    hash_t seed = SYMENGINE_MATRIXSYMBOL;
    hash_combine(seed, name_);
    return seed;
}

bool MatrixSymbol::__eq__(const Basic &o) const
{
    return (is_a<MatrixSymbol>(o)
            && name_ == down_cast<const MatrixSymbol &>(o).name_);
}

int MatrixSymbol::compare(const Basic &o) const
{
    SYMENGINE_ASSERT(is_a<MatrixSymbol>(o));

    const MatrixSymbol &s = down_cast<const MatrixSymbol &>(o);
    if (name_ == s.name_)
        return 0;
    return name_ < s.name_ ? -1 : 1;
}

RCP<const MatrixExpr> matrix_symbol(const std::string &name)
{
    return make_rcp<const MatrixSymbol>(name);
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
    return true;
}

RCP<const MatrixExpr> immutable_dense_matrix(size_t m, size_t n,
                                             const vec_basic &container)
{
    return make_rcp<const ImmutableDenseMatrix>(m, n, container);
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
    size_t num_dense = 0;
    for (auto term : terms) {
        if (is_a<ZeroMatrix>(*term) || is_a<MatrixAdd>(*term)) {
            return false;
        } else if (is_a<DiagonalMatrix>(*term)) {
            num_diag++;
        } else if (is_a<ImmutableDenseMatrix>(*term)) {
            num_dense++;
        }
    }
    if (num_diag > 1 || num_dense > 1) {
        return false;
    }
    return true;
}

void check_matching_sizes(const vec_basic &vec)
{
    auto sz = size(down_cast<const MatrixExpr &>(*vec[0]));
    for (auto it = vec.begin() + 1; it != vec.end(); ++it) {
        if (sz.first.is_null()) {
            continue;
        }
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
    check_matching_sizes(expanded);
    vec_basic keep;
    RCP<const DiagonalMatrix> diag;
    RCP<const ImmutableDenseMatrix> dense;
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
        } else if (is_a<ImmutableDenseMatrix>(*term)) {
            if (dense.is_null()) {
                dense = rcp_static_cast<const ImmutableDenseMatrix>(term);
            } else {
                const vec_basic &vec1
                    = down_cast<const ImmutableDenseMatrix &>(*term)
                          .get_values();
                const vec_basic &vec2 = dense->get_values();
                vec_basic sum(vec1.size());
                for (size_t i = 0; i < vec1.size(); i++) {
                    sum[i] = add(vec1[i], vec2[i]);
                }
                dense = make_rcp<const ImmutableDenseMatrix>(
                    dense->nrows(), dense->ncols(), sum);
            }
        } else {
            keep.push_back(term);
        }
    }
    if (!diag.is_null()) {
        keep.push_back(diag);
    }
    if (!dense.is_null()) {
        keep.push_back(dense);
    }
    if (keep.size() == 1) {
        return rcp_static_cast<const MatrixExpr>(keep[0]);
    }
    if (keep.size() == 0 && !zero.is_null()) {
        return zero;
    }
    return make_rcp<const MatrixAdd>(keep);
}

hash_t HadamardProduct::__hash__() const
{
    hash_t seed = SYMENGINE_HADAMARDPRODUCT;
    for (const auto &a : factors_) {
        hash_combine<Basic>(seed, *a);
    }
    return seed;
}

bool HadamardProduct::__eq__(const Basic &o) const
{
    if (is_a<HadamardProduct>(o)) {
        const HadamardProduct &other = down_cast<const HadamardProduct &>(o);
        return unified_eq(factors_, other.factors_);
    }
    return false;
}

int HadamardProduct::compare(const Basic &o) const
{
    SYMENGINE_ASSERT(is_a<HadamardProduct>(o));
    const HadamardProduct &other = down_cast<const HadamardProduct &>(o);
    return unified_compare(factors_, other.factors_);
}

bool HadamardProduct::is_canonical(const vec_basic factors) const
{
    if (factors.size() < 2) {
        return false;
    }
    size_t num_diag = 0;
    size_t num_dense = 0;
    size_t num_ident = 0;
    for (auto factor : factors) {
        if (is_a<ZeroMatrix>(*factor) || is_a<HadamardProduct>(*factor)) {
            return false;
        } else if (is_a<DiagonalMatrix>(*factor)) {
            num_diag++;
        } else if (is_a<ImmutableDenseMatrix>(*factor)) {
            num_dense++;
        } else if (is_a<IdentityMatrix>(*factor)) {
            num_ident++;
        }
    }
    if (num_diag > 1 || num_ident > 1 || num_dense > 1) {
        return false;
    }
    return true;
}

RCP<const MatrixExpr> hadamard_product(const vec_basic &factors)
{
    if (factors.size() == 0) {
        throw DomainError("Empty hadamard product");
    }
    if (factors.size() == 1) {
        return rcp_static_cast<const MatrixExpr>(factors[0]);
    }
    // extract nested HadamardProduct
    vec_basic expanded;
    for (auto &factor : factors) {
        if (is_a<const HadamardProduct>(*factor)) {
            auto container
                = down_cast<const HadamardProduct &>(*factor).get_factors();
            expanded.insert(expanded.end(), container.begin(), container.end());
        } else {
            expanded.push_back(factor);
        }
    }
    check_matching_sizes(expanded);
    vec_basic keep;
    RCP<const DiagonalMatrix> diag;
    RCP<const ImmutableDenseMatrix> dense;
    bool have_identity = false;
    for (auto &factor : expanded) {
        if (is_a<ZeroMatrix>(*factor)) {
            return rcp_static_cast<const MatrixExpr>(factor);
        } else if (is_a<IdentityMatrix>(*factor)) {
            if (!have_identity) {
                have_identity = true;
                keep.push_back(factor);
            }
        } else if (is_a<DiagonalMatrix>(*factor)) {
            if (diag.is_null()) {
                diag = rcp_static_cast<const DiagonalMatrix>(factor);
            } else {
                vec_basic container;
                for (size_t i = 0; i < diag->get_container().size(); i++) {
                    container.push_back(
                        mul(diag->get_container()[i],
                            down_cast<const DiagonalMatrix &>(*factor)
                                .get_container()[i]));
                }
                diag = make_rcp<const DiagonalMatrix>(container);
            }
        } else if (is_a<ImmutableDenseMatrix>(*factor)) {
            if (dense.is_null()) {
                dense = rcp_static_cast<const ImmutableDenseMatrix>(factor);
            } else {
                const vec_basic &vec1
                    = down_cast<const ImmutableDenseMatrix &>(*factor)
                          .get_values();
                const vec_basic &vec2 = dense->get_values();
                vec_basic product(vec1.size());
                for (size_t i = 0; i < vec1.size(); i++) {
                    product[i] = mul(vec1[i], vec2[i]);
                }
                dense = make_rcp<const ImmutableDenseMatrix>(
                    dense->nrows(), dense->ncols(), product);
            }
        } else {
            keep.push_back(factor);
        }
    }
    if (!diag.is_null()) {
        keep.push_back(diag);
    }
    if (!dense.is_null()) {
        keep.push_back(dense);
    }
    if (keep.size() == 1) {
        return rcp_static_cast<const MatrixExpr>(keep[0]);
    }
    return make_rcp<const HadamardProduct>(keep);
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

    void bvisit(const ImmutableDenseMatrix &x)
    {
        ZeroVisitor visitor(assumptions_);
        is_zero_ = tribool::tritrue;
        for (auto &e : x.get_values()) {
            is_zero_ = and_tribool(is_zero_, visitor.apply(*e));
            if (is_false(is_zero_)) {
                return;
            }
        }
    }

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

    void bvisit(const ImmutableDenseMatrix &x)
    {
        RealVisitor visitor(assumptions_);
        tribool cur = tribool::tritrue;
        for (auto &e : x.get_values()) {
            cur = and_tribool(cur, visitor.apply(*e));
            if (is_false(cur)) {
                is_real_ = cur;
                return;
            }
        }
        is_real_ = cur;
    }

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

    void check_vector(const vec_basic &vec)
    {
        bool found_nonsym = false;
        for (auto &elt : vec) {
            elt->accept(*this);
            if (is_indeterminate(is_symmetric_)) {
                return;
            } else if (is_false(is_symmetric_)) {
                if (found_nonsym) {
                    return;
                } else {
                    found_nonsym = true;
                }
            }
        }
        if (found_nonsym) {
            is_symmetric_ = tribool::trifalse;
        } else {
            is_symmetric_ = tribool::tritrue;
        }
    };

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

    void bvisit(const ImmutableDenseMatrix &x)
    {
        size_t nrows = x.nrows();
        size_t ncols = x.ncols();
        if (nrows != ncols) {
            is_symmetric_ = tribool::trifalse;
            return;
        }
        ZeroVisitor visitor(assumptions_);
        is_symmetric_ = tribool::tritrue;
        for (size_t i = 0; i < ncols; i++) {
            for (size_t j = 0; j <= i; j++) {
                if (j != i) {
                    auto e1 = x.get(i, j);
                    auto e2 = x.get(j, i);
                    is_symmetric_ = and_tribool(is_symmetric_,
                                                visitor.apply(*sub(e1, e2)));
                }
                if (is_false(is_symmetric_)) {
                    return;
                }
            }
        }
    };

    void bvisit(const MatrixAdd &x)
    {
        check_vector(x.get_terms());
    };

    void bvisit(const HadamardProduct &x)
    {
        check_vector(x.get_factors());
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

    void check_vector(const vec_basic &vec)
    {
        for (auto &elt : vec) {
            elt->accept(*this);
            if (not is_indeterminate(is_square_)) {
                return;
            }
        }
    }

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

    void bvisit(const ImmutableDenseMatrix &x)
    {
        if (x.nrows() == x.ncols()) {
            is_square_ = tribool::tritrue;
        } else {
            is_square_ = tribool::trifalse;
        }
    };

    void bvisit(const MatrixAdd &x)
    {
        check_vector(x.get_terms());
    };

    void bvisit(const HadamardProduct &x)
    {
        check_vector(x.get_factors());
    }

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

    void bvisit(const ImmutableDenseMatrix &x)
    {
        if (x.nrows() != x.ncols()) {
            is_diagonal_ = tribool::trifalse;
            return;
        }
        size_t ncols = x.ncols();
        size_t offset;
        ZeroVisitor visitor(assumptions_);
        is_diagonal_ = tribool::tritrue;
        for (size_t i = 0; i < ncols; i++) {
            offset = i * ncols;
            for (size_t j = 0; j < ncols; j++) {
                if (j != i) {
                    auto &e = x.get_values()[offset];
                    is_diagonal_ = and_tribool(is_diagonal_, visitor.apply(*e));
                    if (is_false(is_diagonal_)) {
                        return;
                    }
                }
                offset++;
            }
        }
    };

    void bvisit(const MatrixAdd &x)
    {
        bool found_nondiag = false;
        for (auto &elt : x.get_terms()) {
            elt->accept(*this);
            if (is_indeterminate(is_diagonal_)) {
                return;
            } else if (is_false(is_diagonal_)) {
                if (found_nondiag) {
                    return;
                } else {
                    found_nondiag = true;
                }
            }
        }
        if (found_nondiag) {
            is_diagonal_ = tribool::trifalse;
        } else {
            is_diagonal_ = tribool::tritrue;
        }
    };

    void bvisit(const HadamardProduct &x)
    {
        // diag x (diag | nodiag | indeterminate) x ... = diag
        // (indet | nodiag) x (indet | nodiag) x ... = indeterminate
        for (auto &elt : x.get_factors()) {
            elt->accept(*this);
            if (is_true(is_diagonal_)) {
                return;
            }
        }
        is_diagonal_ = tribool::indeterminate;
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

    void bvisit(const ImmutableDenseMatrix &x)
    {
        size_t nrows = x.nrows();
        size_t ncols = x.ncols();
        if (nrows != ncols) {
            is_lower_ = tribool::trifalse;
            return;
        }
        ZeroVisitor visitor(assumptions_);
        is_lower_ = tribool::tritrue;
        for (size_t i = 0; i < nrows; i++) {
            for (size_t j = i + 1; j < nrows; j++) {
                is_lower_ = and_tribool(is_lower_, visitor.apply(*x.get(i, j)));
                if (is_false(is_lower_)) {
                    return;
                }
            }
        }
    }

    void bvisit(const MatrixAdd &x)
    {
        bool found_nonlower = false;
        for (auto &elt : x.get_terms()) {
            elt->accept(*this);
            if (is_indeterminate(is_lower_)) {
                return;
            } else if (is_false(is_lower_)) {
                if (found_nonlower) {
                    return;
                } else {
                    found_nonlower = true;
                }
            }
        }
        if (found_nonlower) {
            is_lower_ = tribool::trifalse;
        } else {
            is_lower_ = tribool::tritrue;
        }
    };

    void bvisit(const HadamardProduct &x)
    {
        // lower x (lower | nolower | indeterminate) x ... = lower
        // (indet | nolower) x (indet | nocwlower) x ... = indeterminate
        for (auto &elt : x.get_factors()) {
            elt->accept(*this);
            if (is_true(is_lower_)) {
                return;
            }
        }
        is_lower_ = tribool::indeterminate;
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

    void bvisit(const ImmutableDenseMatrix &x)
    {
        size_t nrows = x.nrows();
        size_t ncols = x.ncols();
        if (nrows != ncols) {
            is_upper_ = tribool::trifalse;
            return;
        }
        ZeroVisitor visitor(assumptions_);
        is_upper_ = tribool::tritrue;
        for (size_t i = 1; i < nrows; i++) {
            for (size_t j = 0; j < i; j++) {
                is_upper_ = and_tribool(is_upper_, visitor.apply(*x.get(i, j)));
                if (is_false(is_upper_)) {
                    return;
                }
            }
        }
    }

    void bvisit(const MatrixAdd &x)
    {
        bool found_nonupper = false;
        for (auto &elt : x.get_terms()) {
            elt->accept(*this);
            if (is_indeterminate(is_upper_)) {
                return;
            } else if (is_false(is_upper_)) {
                if (found_nonupper) {
                    return;
                } else {
                    found_nonupper = true;
                }
            }
        }
        if (found_nonupper) {
            is_upper_ = tribool::trifalse;
        } else {
            is_upper_ = tribool::tritrue;
        }
    };

    void bvisit(const HadamardProduct &x)
    {
        for (auto &elt : x.get_factors()) {
            elt->accept(*this);
            if (is_true(is_upper_)) {
                return;
            }
        }
        is_upper_ = tribool::indeterminate;
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

    void bvisit(const ImmutableDenseMatrix &x)
    {
        size_t i_start, j_start, i, j;
        ZeroVisitor visitor(assumptions_);
        is_toeplitz_ = tribool::tritrue;
        // Loop over all diagonals
        for (size_t w = 0; w < std::max(x.nrows(), x.ncols()) - 1; w++) {
            // Loop over diagonals starting from the first row and the first
            // column
            for (size_t k = 0; k < 2; k++) {
                if (k == 0 && w <= x.ncols()) {
                    i_start = 0;
                    j_start = w;
                } else if (k == 1 && w <= x.nrows() && w != 0) {
                    i_start = w;
                    j_start = 0;
                } else {
                    continue;
                }
                auto first = x.get(i_start, j_start);
                // Loop along the diagonal
                for (i = i_start + 1, j = j_start + 1;
                     i < x.nrows() && j < x.ncols(); i++, j++) {
                    is_toeplitz_ = and_tribool(
                        is_toeplitz_, visitor.apply(*sub(first, x.get(i, j))));
                    if (is_false(is_toeplitz_)) {
                        return;
                    }
                }
            }
        }
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

    void bvisit(const MatrixSymbol &x){
        // Do not initialize nrows_ and ncols_
    };

    void bvisit(const DiagonalMatrix &x)
    {
        nrows_ = integer(x.get_container().size());
        ncols_ = nrows_;
    };

    void bvisit(const ImmutableDenseMatrix &x)
    {
        nrows_ = integer(x.nrows());
        ncols_ = integer(x.ncols());
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
