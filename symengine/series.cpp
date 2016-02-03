/**
 *  \file expansion.h
 *  Series expansions of expressions using Piranha
 *
 **/

#include <symengine/symengine_config.h>

#ifdef HAVE_SYMENGINE_PIRANHA
#include <symengine/series_piranha.h>
#endif

#ifdef HAVE_SYMENGINE_FLINT
#include <symengine/series_flint.h>
#endif

#include <symengine/visitor.h>

namespace SymEngine {

RCP<const SeriesCoeffInterface> series(const RCP<const Basic> &ex, const RCP<const Symbol> &var, unsigned int prec)
{
    auto syms = free_symbols(*ex);
#ifdef HAVE_SYMENGINE_PIRANHA
    if (prec == 0)
        return make_rcp<const UPSeriesPiranha>(p_expr{Expression()}, var->get_name(), prec);
    if (not has_symbol(*ex, var))
        return make_rcp<const UPSeriesPiranha>(p_expr{Expression(ex)}, var->get_name(), prec);
    if (is_a<Symbol>(*ex))
        return make_rcp<const UPSeriesPiranha>(p_expr{Expression(ex)}, var->get_name(), prec);

    if (syms.size() == 1)
#   ifdef HAVE_SYMENGINE_FLINT
        return URatPSeriesFlint::series(ex, var->get_name(), prec);
#   else
        return URatPSeriesPiranha::series(ex, var->get_name(), prec);
#   endif

    return UPSeriesPiranha::series(ex, var->get_name(), prec);
#elif defined(HAVE_SYMENGINE_FLINT)
//! TODO: call generic impl. where needed
    if (prec == 0)
        return URatPSeriesFlint::series(integer(0), var->get_name(), prec);

    if (syms.size() > 1)
        throw std::runtime_error("Only univariate series expansion implemented with Flint");
    
    return URatPSeriesFlint::series(ex, var->get_name(), prec);
#else
    throw std::runtime_error("Series expansion is supported only with Piranha or Flint");
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
