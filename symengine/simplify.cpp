#include <symengine/simplify.h>
#include <symengine/refine.h>

namespace SymEngine
{

void SimplifyVisitor::bvisit(const OneArgFunction &x)
{
    auto farg = x.get_arg();
    auto newarg = apply(farg);
    result_ = x.create(newarg);
}

void SimplifyVisitor::bvisit(const Pow &x)
{
    auto e = apply(x.get_exp());
    auto base = apply(x.get_base());
    auto pair = simplify_pow(e, base);
    result_ = pow(pair.second, pair.first);
}

std::pair<RCP<const Basic>, RCP<const Basic>>
SimplifyVisitor::simplify_pow(const RCP<const Basic> &e,
                              const RCP<const Basic> &b)
{
    if (is_a<Csc>(*b) and eq(*e, *minus_one)) {
        // csc(expr) ** -1 = sin(expr)
        return std::make_pair(
            one, sin(down_cast<const OneArgFunction &>(*b).get_arg()));
    } else if (is_a<Sec>(*b) and eq(*e, *minus_one)) {
        // sec(expr) ** -1 = cos(expr)
        return std::make_pair(
            one, cos(down_cast<const OneArgFunction &>(*b).get_arg()));
    } else if (is_a<Cot>(*b) and eq(*e, *minus_one)) {
        // cot(expr) ** -1 = tan(expr)
        return std::make_pair(
            one, tan(down_cast<const OneArgFunction &>(*b).get_arg()));
    } else {
        return std::make_pair(e, b);
    }
}

void SimplifyVisitor::bvisit(const Mul &x)
{
    map_basic_basic map;
    for (const auto &p : x.get_dict()) {
        auto base = apply(p.first);
        auto newpair = simplify_pow(p.second, base);
        Mul::dict_add_term(map, newpair.first, newpair.second);
    }
    result_ = Mul::from_dict(x.get_coef(), std::move(map));
}

RCP<const Basic> simplify(const RCP<const Basic> &x,
                          const Assumptions *assumptions)
{
    auto expr = refine(x, assumptions);
    SimplifyVisitor b;
    return b.apply(expr);
}

} // namespace SymEngine

namespace SymEngine
{

RCP<const Basic> PiecewiseFoldVisitor::rebuild(const RCP<const Basic> &x,
                                               const map_basic_basic &args_map)
{
    args_map_ = &args_map;
    x->accept(*this);
    args_map_ = nullptr;
    return result_;
}

RCP<const Basic> PiecewiseFoldVisitor::apply(const RCP<const Basic> &x)
{
    if (args_map_ != nullptr) {
        // Rebuilding the parent node; x is one of its args
        auto it = args_map_->find(x);
        return it == args_map_->end() ? x : it->second;
    }

    if (is_a<Piecewise>(*x)) {
        x->accept(*this);
        return result_;
    }

    // Fold the args. Piecewise args with identical conditions are grouped so
    // that they are zipped together instead of taking the cartesian product.
    // Each group is (conditions, [(original arg, folded Piecewise)])
    map_basic_basic args_map;
    std::vector<std::pair<
        std::vector<RCP<const Boolean>>,
        std::vector<std::pair<RCP<const Basic>, RCP<const Piecewise>>>>>
        groups;
    bool changed = false;
    for (const auto &a : x->get_args()) {
        auto fa = apply(a);
        args_map[a] = fa;
        changed = changed or neq(*fa, *a);
        if (not is_a<Piecewise>(*fa)) {
            continue;
        }
        auto pw = rcp_static_cast<const Piecewise>(fa);
        std::vector<RCP<const Boolean>> conds;
        for (const auto &p : pw->get_vec()) {
            conds.push_back(p.second);
        }
        bool found = false;
        for (auto &g : groups) {
            if (unified_eq(g.first, conds)) {
                g.second.push_back({a, pw});
                found = true;
                break;
            }
        }
        if (not found) {
            groups.push_back({conds, {{a, pw}}});
        }
    }

    if (groups.empty()) {
        return changed ? rebuild(x, args_map) : x;
    }

    // Iterate over the cartesian product of the groups in lexicographic
    // order, which preserves the first-match semantics of Piecewise.
    PiecewiseVec vec;
    std::vector<size_t> idx(groups.size(), 0);
    while (true) {
        set_boolean conds;
        for (size_t g = 0; g < groups.size(); g++) {
            conds.insert(groups[g].first[idx[g]]);
            for (const auto &a : groups[g].second) {
                args_map[a.first] = a.second->get_vec()[idx[g]].first;
            }
        }
        auto r = rebuild(x, args_map);
        if (r.get() == x.get()) {
            // TransformVisitor does not know how to rebuild this node
            return x;
        }
        vec.push_back({r, logical_and(conds)});
        // increment the multi-index, last group varying fastest
        size_t g = groups.size();
        while (true) {
            if (g == 0) {
                return piecewise(vec);
            }
            g--;
            if (++idx[g] < groups[g].first.size()) {
                break;
            }
            idx[g] = 0;
        }
    }
}

void PiecewiseFoldVisitor::bvisit(const Piecewise &x)
{
    // Fold the branches and flatten nested Piecewise
    PiecewiseVec vec;
    // Conditions are not folded so that they stay Boolean
    for (const auto &p : x.get_vec()) {
        auto e = apply(p.first);
        if (is_a<Piecewise>(*e)) {
            for (const auto &q : down_cast<const Piecewise &>(*e).get_vec()) {
                vec.push_back({q.first, logical_and({p.second, q.second})});
            }
        } else {
            vec.push_back({e, p.second});
        }
    }
    result_ = piecewise(vec);
}

RCP<const Basic> piecewise_fold(const RCP<const Basic> &x)
{
    PiecewiseFoldVisitor v;
    return v.apply(x);
}

} // namespace SymEngine
