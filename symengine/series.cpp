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

umap_int_basic series(const RCP<const Basic> &ex, const RCP<const Symbol> &var, unsigned int prec)
{
#ifdef HAVE_SYMENGINE_PIRANHA
    if (prec == 0)
        return {{0, zero}};
    if (not has_symbol(*ex, var))
        return {{0, ex}};
    if (is_a<Symbol>(*ex))
        return {{0, ex}};

    auto ser = UPSeriesPiranha::series(ex, var->get_name(), prec);
    return ser->as_dict();
#else
    throw std::runtime_error("Series expansion is supported only with Piranha");
#endif
}

umap_int_basic series_invfunc(const RCP<const Basic> &ex, const RCP<const Symbol> &var, unsigned int prec)
{
#ifdef HAVE_SYMENGINE_PIRANHA
    if (prec == 0)
        return {{0, zero}};
    if (is_a<Symbol>(*ex))
        return {{0, ex}};

    auto poly = UPSeriesPiranha::series_reverse(UPSeriesPiranha::series(ex, var->get_name(), prec)->p_, p_expr(var->get_name()), prec);
    umap_int_basic map;
    for (const auto& it : poly) {
        if (it.first != 0) {
            map[it.second.degree()] = it.first.get_basic();
        }
    }
    return map;

#else
    throw std::runtime_error("Series expansion is supported only with Piranha");
#endif
}

} //SymEngine
