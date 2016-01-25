/**
 *  \file expansion.h
 *  Series expansions of expressions using Piranha
 *
 **/

#include <symengine/symengine_config.h>

#ifdef HAVE_SYMENGINE_PIRANHA
#include <symengine/series_piranha.h>
#endif

#include <symengine/visitor.h>

namespace SymEngine {

RCP<const SeriesCoeffInterface> series(const RCP<const Basic> &ex, const RCP<const Symbol> &var, unsigned int prec)
{
#ifdef HAVE_SYMENGINE_PIRANHA
    if (prec == 0)
        return make_rcp<const UPSeriesPiranha>(p_expr{Expression()}, var->get_name(), prec);
    if (not has_symbol(*ex, var))
        return make_rcp<const UPSeriesPiranha>(p_expr{Expression(ex)}, var->get_name(), prec);
    if (is_a<Symbol>(*ex))
        return make_rcp<const UPSeriesPiranha>(p_expr{Expression(ex)}, var->get_name(), prec);

    return UPSeriesPiranha::series(ex, var->get_name(), prec);
#else
    throw std::runtime_error("Series expansion is supported only with Piranha");
#endif
}

RCP<const SeriesCoeffInterface> series_invfunc(const RCP<const Basic> &ex, const RCP<const Symbol> &var, unsigned int prec)
{
#ifdef HAVE_SYMENGINE_PIRANHA
    if (prec == 0)
        return make_rcp<const UPSeriesPiranha>(p_expr{Expression()}, var->get_name(), prec);
    if (is_a<Symbol>(*ex))
        return make_rcp<const UPSeriesPiranha>(p_expr{Expression(ex)}, var->get_name(), prec);

    return make_rcp<const UPSeriesPiranha>(UPSeriesPiranha::series_reverse(UPSeriesPiranha::series(ex, var->get_name(), prec)->p_, p_expr(var->get_name()), prec), var->get_name(), prec);

#else
    throw std::runtime_error("Series expansion is supported only with Piranha");
#endif
}

} //SymEngine
