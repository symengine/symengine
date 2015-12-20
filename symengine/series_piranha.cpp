
#include <symengine/series_piranha.h>
#include <symengine/series_visitor.h>

namespace SymEngine {
RCP<const URatPSeriesPiranha> URatPSeriesPiranha::series(const RCP<const Basic> &t, const RCP<const Symbol> &x,
                                                         unsigned int prec) {
    SeriesVisitor<pp_t, piranha::rational, URatPSeriesPiranha, false> visitor(pp_t(x->get_name()), x->get_name(), prec);
    return visitor.series(t);
}
}