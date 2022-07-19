#include <symengine/visitor.h>
#include <symengine/test_visitors.h>
#include <symengine/matrices/size.h>

namespace SymEngine
{

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
