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
        SYMENGINE_ASSERT(is_canonical(n));
    }

    IMPLEMENT_TYPEID(SYMENGINE_IDENTITYMATRIX)
    hash_t __hash__() const override;
    bool __eq__(const Basic &o) const override;
    int compare(const Basic &o) const override;
    vec_basic get_args() const override;
    bool is_canonical(const RCP<const Basic> &n) const;

    inline const RCP<const Basic> &size() const
    {
        return n_;
    }
};

RCP<const MatrixExpr> identity_matrix(const RCP<const Basic> &n);

class ZeroMatrix : public MatrixExpr
{
private:
    RCP<const Basic> m_, n_; // m >= 0, n >= 0

public:
    ZeroMatrix(const RCP<const Basic> &m, const RCP<const Basic> &n)
        : m_(m), n_(n)
    {
        SYMENGINE_ASSIGN_TYPEID();
        SYMENGINE_ASSERT(is_canonical(m, n));
    }

    IMPLEMENT_TYPEID(SYMENGINE_ZEROMATRIX)
    hash_t __hash__() const override;
    bool __eq__(const Basic &o) const override;
    int compare(const Basic &o) const override;
    vec_basic get_args() const override;
    bool is_canonical(const RCP<const Basic> &m,
                      const RCP<const Basic> &n) const;

    inline const RCP<const Basic> &nrows() const
    {
        return m_;
    }

    inline const RCP<const Basic> &ncols() const
    {
        return n_;
    }
};

RCP<const MatrixExpr> zero_matrix(const RCP<const Basic> &m,
                                  const RCP<const Basic> &n);

class MatrixSymbol : public MatrixExpr
{
private:
    std::string name_;

public:
    MatrixSymbol(const std::string &name) : name_(name)
    {
        SYMENGINE_ASSIGN_TYPEID();
    }

    IMPLEMENT_TYPEID(SYMENGINE_MATRIXSYMBOL)
    hash_t __hash__() const override;
    bool __eq__(const Basic &o) const override;
    int compare(const Basic &o) const override;

    const std::string &get_name() const
    {
        return name_;
    }

    vec_basic get_args() const override
    {
        return {};
    }
};

RCP<const MatrixExpr> matrix_symbol(const std::string &name);

class DiagonalMatrix : public MatrixExpr
{
private:
    vec_basic diag_;

public:
    DiagonalMatrix(const vec_basic &container) : diag_(container)
    {
        SYMENGINE_ASSIGN_TYPEID();
        SYMENGINE_ASSERT(is_canonical(container));
    }

    IMPLEMENT_TYPEID(SYMENGINE_DIAGONALMATRIX)
    hash_t __hash__() const override;
    bool __eq__(const Basic &o) const override;
    int compare(const Basic &o) const override;
    bool is_canonical(const vec_basic &container) const;

    vec_basic get_args() const override
    {
        return vec_basic(diag_.begin(), diag_.end());
    }

    inline const vec_basic &get_container() const
    {
        return diag_;
    }

    inline RCP<const Basic> get(size_t i) const
    {
        return diag_[i];
    }
};

RCP<const MatrixExpr> diagonal_matrix(const vec_basic &container);

class ImmutableDenseMatrix : public MatrixExpr
{
private:
    size_t m_, n_;
    vec_basic values_;

public:
    ImmutableDenseMatrix(size_t m, size_t n, const vec_basic &values)
        : m_(m), n_(n), values_(values)
    {
        SYMENGINE_ASSIGN_TYPEID();
        SYMENGINE_ASSERT(is_canonical(m, n, values));
    }

    IMPLEMENT_TYPEID(SYMENGINE_IMMUTABLEDENSEMATRIX)
    hash_t __hash__() const override;
    bool __eq__(const Basic &o) const override;
    int compare(const Basic &o) const override;
    bool is_canonical(size_t m, size_t n, const vec_basic &values) const;

    vec_basic get_args() const override
    {
        vec_basic args = vec_basic({integer(m_), integer(n_)});
        args.insert(args.begin(), values_.begin(), values_.end());
        return args;
    }

    inline RCP<const Basic> get(size_t i, size_t j) const
    {
        return values_[i * n_ + j];
    }

    inline size_t nrows() const
    {
        return m_;
    }

    inline size_t ncols() const
    {
        return n_;
    }

    inline const vec_basic &get_values() const
    {
        return values_;
    }
};

RCP<const MatrixExpr> immutable_dense_matrix(size_t m, size_t n,
                                             const vec_basic &container);

class Trace : public Basic
{
private:
    RCP<const Basic> arg_;

public:
    Trace(const RCP<const MatrixExpr> &arg) : arg_(arg)
    {
        SYMENGINE_ASSIGN_TYPEID();
    }

    IMPLEMENT_TYPEID(SYMENGINE_TRACE)
    hash_t __hash__() const override;
    bool __eq__(const Basic &o) const override;
    int compare(const Basic &o) const override;
    vec_basic get_args() const override;
};

RCP<const Basic> trace(const RCP<const MatrixExpr> &arg);

class MatrixAdd : public MatrixExpr
{
private:
    vec_basic terms_;

public:
    MatrixAdd(const vec_basic &terms) : terms_(terms)
    {
        SYMENGINE_ASSIGN_TYPEID();
        SYMENGINE_ASSERT(is_canonical(terms));
    }

    IMPLEMENT_TYPEID(SYMENGINE_MATRIXADD)
    hash_t __hash__() const override;
    bool __eq__(const Basic &o) const override;
    int compare(const Basic &o) const override;
    bool is_canonical(const vec_basic terms) const;
    vec_basic get_args() const override
    {
        return vec_basic(terms_.begin(), terms_.end());
    }
    inline const vec_basic &get_terms() const
    {
        return terms_;
    }
};

RCP<const MatrixExpr> matrix_add(const vec_basic &terms);

class HadamardProduct : public MatrixExpr
{
private:
    vec_basic factors_;

public:
    HadamardProduct(const vec_basic &factors) : factors_(factors)
    {
        SYMENGINE_ASSIGN_TYPEID();
        SYMENGINE_ASSERT(is_canonical(factors));
    }

    IMPLEMENT_TYPEID(SYMENGINE_HADAMARDPRODUCT)
    hash_t __hash__() const override;
    bool __eq__(const Basic &o) const override;
    int compare(const Basic &o) const override;
    bool is_canonical(const vec_basic factors) const;
    vec_basic get_args() const override
    {
        return vec_basic(factors_.begin(), factors_.end());
    }
    inline const vec_basic &get_factors() const
    {
        return factors_;
    }
};

RCP<const MatrixExpr> hadamard_product(const vec_basic &factors);

std::pair<RCP<const Basic>, RCP<const Basic>> size(const MatrixExpr &m);
tribool is_zero(const MatrixExpr &m, const Assumptions *assumptions = nullptr);
tribool is_real(const MatrixExpr &m, const Assumptions *assumptions = nullptr);
tribool is_symmetric(const MatrixExpr &m,
                     const Assumptions *assumptions = nullptr);
tribool is_square(const MatrixExpr &m,
                  const Assumptions *assumptions = nullptr);
tribool is_diagonal(const MatrixExpr &m,
                    const Assumptions *assumptions = nullptr);
tribool is_lower(const MatrixExpr &m, const Assumptions *assumptions = nullptr);
tribool is_upper(const MatrixExpr &m, const Assumptions *assumptions = nullptr);
tribool is_toeplitz(const MatrixExpr &m,
                    const Assumptions *assumptions = nullptr);

} // namespace SymEngine

#endif
