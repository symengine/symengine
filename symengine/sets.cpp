#include <symengine/logic.h>
#include <symengine/symengine_casts.h>
#include <iterator>

namespace SymEngine
{

Interval::Interval(const RCP<const Number> &start, const RCP<const Number> &end,
                   const bool left_open, const bool right_open)
    : start_(start), end_(end), left_open_(left_open), right_open_(right_open)
{
    SYMENGINE_ASSIGN_TYPEID()
    SYMENGINE_ASSERT(
        Interval::is_canonical(start_, end_, left_open_, right_open_));
}

bool Interval::is_canonical(const RCP<const Number> &s,
                            const RCP<const Number> &e, bool left_open,
                            bool right_open)
{
    if (is_a<Complex>(*s) or is_a<Complex>(*e))
        throw NotImplementedError("Complex set not implemented");
    if (eq(*e, *s)) {
        return false;
    } else if (eq(*min({s, e}), *e)) {
        return false;
    }
    return true;
}

hash_t Interval::__hash__() const
{
    hash_t seed = INTERVAL;
    hash_combine<Basic>(seed, *start_);
    hash_combine<Basic>(seed, *end_);
    hash_combine<bool>(seed, left_open_);
    hash_combine<bool>(seed, right_open_);
    return seed;
}

bool Interval::__eq__(const Basic &o) const
{
    if (is_a<Interval>(o)) {
        const Interval &s = down_cast<const Interval &>(o);
        return ((this->left_open_ == s.left_open_)
                and (this->right_open_ == s.right_open_)
                and eq(*this->start_, *s.start_) and eq(*this->end_, *s.end_));
    }
    return false;
}

int Interval::compare(const Basic &s) const
{
    // compares two interval based on their length
    SYMENGINE_ASSERT(is_a<Interval>(s))
    const Interval &o = down_cast<const Interval &>(s);
    if (left_open_ and not o.left_open_) {
        return -1;
    } else if (not left_open_ and o.left_open_) {
        return 1;
    } else if (right_open_ and not o.right_open_) {
        return 1;
    } else if (not right_open_ and o.right_open_) {
        return -1;
    } else {
        auto temp = start_->__cmp__(*(o.start_));
        if (temp != 0) {
            return temp;
        } else {
            return end_->__cmp__(*(o.end_));
        }
    }
}

RCP<const Set> Interval::open() const
{
    return interval(start_, end_, true, true);
}

RCP<const Set> Interval::Lopen() const
{
    return interval(start_, end_, true, false);
}

RCP<const Set> Interval::Ropen() const
{
    return interval(start_, end_, false, true);
}

RCP<const Set> Interval::close() const
{
    return interval(start_, end_, false, false);
}

RCP<const Boolean> Interval::contains(const RCP<const Basic> &a) const
{
    if (not is_a_Number(*a))
        return make_rcp<Contains>(a, rcp_from_this_cast<const Set>());
    if (eq(*start_, *a))
        return boolean(not left_open_);
    if (eq(*end_, *a))
        return boolean(not right_open_);
    if (eq(*min({end_, a}), *end_) or eq(*max({start_, a}), *start_))
        return boolean(false);
    return boolean(true);
}

RCP<const Set> Interval::set_intersection(const RCP<const Set> &o) const
{
    if (is_a<Interval>(*o)) {
        const Interval &other = down_cast<const Interval &>(*o);
        RCP<const Number> start, end;
        bool left_open, right_open;
        RCP<const Basic> start_end, end_start;
        start_end = min({this->start_, other.end_});
        end_start = min({this->end_, other.start_});

        if (eq(*this->start_, *start_end) and eq(*other.start_, *end_start)) {
            RCP<const Basic> start_start, end_end;
            start_start = min({this->start_, other.start_});
            end_end = min({this->end_, other.end_});
            if (neq(*this->start_, *other.start_)) {
                if (eq(*this->start_, *start_start)) {
                    start = other.start_;
                    left_open = other.left_open_;
                } else {
                    start = this->start_;
                    left_open = this->left_open_;
                }
            } else {
                start = this->start_;
                left_open = this->left_open_ or other.left_open_;
            }

            if (neq(*this->end_, *other.end_)) {
                if (eq(*this->end_, *end_end)) {
                    end = this->end_;
                    right_open = this->right_open_;
                } else {
                    end = other.end_;
                    right_open = other.right_open_;
                }
            } else {
                end = this->end_;
                right_open = this->right_open_ or other.right_open_;
            }
            return interval(start, end, left_open, right_open);
        } else {
            return emptyset();
        }
    }
    if (is_a<UniversalSet>(*o) or is_a<EmptySet>(*o) or is_a<FiniteSet>(*o)
        or is_a<Union>(*o)) {
        return (*o).set_intersection(rcp_from_this_cast<const Set>());
    }
    return SymEngine::set_intersection({rcp_from_this_cast<const Set>(), o},
                                       false);
}

RCP<const Set> Interval::set_union(const RCP<const Set> &o) const
{
    if (is_a<Interval>(*o)) {
        const Interval &other = down_cast<const Interval &>(*o);
        RCP<const Basic> start_start, end_end, m;
        RCP<const Number> start, end;
        bool left_open, right_open;
        start_start = max({this->start_, other.start_});
        end_end = min({this->end_, other.end_});
        m = min({start_start, end_end});
        if ((eq(*end_end, *start_start) and eq(*end_end, *m)
             and ((eq(*end_end, *this->end_) and this->right_open_)
                  or (eq(*end_end, *other.end_) and other.right_open_)))
            or (eq(*end_end, *m) and not eq(*end_end, *start_start))) {
            return SymEngine::set_union({rcp_from_this_cast<const Set>(), o},
                                        false);
        } else {
            if (eq(*min({this->start_, other.start_}), *this->start_))
                start = this->start_;
            else
                start = other.start_;
            if (eq(*max({this->end_, other.end_}), *this->end_))
                end = this->end_;
            else
                end = other.end_;
            left_open = ((neq(*this->start_, *start) or this->left_open_)
                         and (neq(*other.start_, *start) or other.left_open_));
            right_open = ((neq(*this->end_, *end) or this->right_open_)
                          and (neq(*other.end_, *end) or other.right_open_));
            return interval(start, end, left_open, right_open);
        }
    }
    if (is_a<UniversalSet>(*o) or is_a<EmptySet>(*o) or is_a<FiniteSet>(*o)
        or is_a<Union>(*o)) {
        return (*o).set_union(rcp_from_this_cast<const Set>());
    }
    return SymEngine::set_union({rcp_from_this_cast<const Set>(), o}, false);
}

RCP<const Set> Interval::set_complement(const RCP<const Set> &o) const
{
    if (is_a<Interval>(*o)) {
        if (o->compare(*interval(NegInf, Inf, true, true)) == 0) {
            auto a = interval(NegInf, start_, true, not left_open_);
            auto b = interval(end_, Inf, not right_open_, true);
            return SymEngine::set_union({a, b});
        }
    }
    return SymEngine::set_complement_helper(rcp_from_this_cast<const Set>(), o);
}

vec_basic Interval::get_args() const
{
    return {start_, end_, boolean(left_open_), boolean(right_open_)};
}

RCP<const Set> EmptySet::set_intersection(const RCP<const Set> &o) const
{
    return emptyset();
}

RCP<const Set> EmptySet::set_union(const RCP<const Set> &o) const
{
    return o;
}

RCP<const Set> EmptySet::set_complement(const RCP<const Set> &o) const
{
    return o;
}

hash_t EmptySet::__hash__() const
{
    hash_t seed = EMPTYSET;
    return seed;
}

bool EmptySet::__eq__(const Basic &o) const
{
    if (is_a<EmptySet>(o))
        return true;
    return false;
}

int EmptySet::compare(const Basic &o) const
{
    SYMENGINE_ASSERT(is_a<EmptySet>(o))
    return 0;
}

const RCP<const EmptySet> &EmptySet::getInstance()
{
    const static auto a = make_rcp<const EmptySet>();
    return a;
}

RCP<const Set> UniversalSet::set_intersection(const RCP<const Set> &o) const
{
    return o;
}

RCP<const Set> UniversalSet::set_union(const RCP<const Set> &o) const
{
    return universalset();
}

RCP<const Set> UniversalSet::set_complement(const RCP<const Set> &o) const
{
    return emptyset();
}

hash_t UniversalSet::__hash__() const
{
    hash_t seed = UNIVERSALSET;
    return seed;
}

bool UniversalSet::__eq__(const Basic &o) const
{
    if (is_a<UniversalSet>(o))
        return true;
    return false;
}

int UniversalSet::compare(const Basic &o) const
{
    SYMENGINE_ASSERT(is_a<UniversalSet>(o))
    return 0;
}

const RCP<const UniversalSet> &UniversalSet::getInstance()
{
    const static auto a = make_rcp<const UniversalSet>();
    return a;
}

FiniteSet::FiniteSet(const set_basic container) : container_(container)
{
    SYMENGINE_ASSIGN_TYPEID()
    SYMENGINE_ASSERT(FiniteSet::is_canonical(container_));
}

bool FiniteSet::is_canonical(const set_basic container)
{
    return container.size() != 0;
}

hash_t FiniteSet::__hash__() const
{
    hash_t seed = FINITESET;
    for (const auto &a : container_)
        hash_combine<Basic>(seed, *a);
    return seed;
}

bool FiniteSet::__eq__(const Basic &o) const
{
    if (is_a<FiniteSet>(o)) {
        const FiniteSet &other = down_cast<const FiniteSet &>(o);
        return unified_eq(container_, other.container_);
    }
    return false;
}

int FiniteSet::compare(const Basic &o) const
{
    // compares two FiniteSet based on their length
    SYMENGINE_ASSERT(is_a<FiniteSet>(o))
    const FiniteSet &other = down_cast<const FiniteSet &>(o);
    return unified_compare(container_, other.container_);
}

RCP<const Boolean> FiniteSet::contains(const RCP<const Basic> &a) const
{
    return boolean(container_.find(a) != container_.end());
}

RCP<const Set> FiniteSet::set_union(const RCP<const Set> &o) const
{
    if (is_a<FiniteSet>(*o)) {
        const FiniteSet &other = down_cast<const FiniteSet &>(*o);
        set_basic container;
        std::set_union(container_.begin(), container_.end(),
                       other.container_.begin(), other.container_.end(),
                       std::inserter(container, container.begin()),
                       RCPBasicKeyLess{});
        return finiteset(container);
    }
    if (is_a<Interval>(*o)) {
        set_basic container;
        const Interval &other = down_cast<const Interval &>(*o);
        bool left = other.get_left_open(), right = other.get_right_open();
        for (const auto &a : container_) {
            auto contain = o->contains(a);
            if (eq(*contain, *boolFalse)) {
                if (left)
                    if (eq(*other.get_start(), *a)) {
                        left = false;
                        continue;
                    }
                if (right)
                    if (eq(*other.get_end(), *a)) {
                        right = false;
                        continue;
                    }
                container.insert(a);
            } else if (is_a<Contains>(*contain)) {
                std::runtime_error("Not implemented");
            }
        }
        if (not container.empty()) {
            if (left == other.get_left_open()
                and right == other.get_right_open())
                return SymEngine::set_union({finiteset(container), o}, false);
            else
                return SymEngine::set_union(
                    {finiteset(container),
                     interval(other.get_start(), other.get_end(), left, right)},
                    false);
        } else {
            if (left == other.get_left_open()
                and right == other.get_right_open())
                return o;
            else
                return interval(other.get_start(), other.get_end(), left,
                                right);
        }
    }
    if (is_a<UniversalSet>(*o) or is_a<EmptySet>(*o) or is_a<Union>(*o)) {
        return (*o).set_union(rcp_from_this_cast<const Set>());
    }
    return SymEngine::set_union({rcp_from_this_cast<const Set>(), o}, false);
}

RCP<const Set> FiniteSet::set_intersection(const RCP<const Set> &o) const
{
    if (is_a<FiniteSet>(*o)) {
        const FiniteSet &other = down_cast<const FiniteSet &>(*o);
        set_basic container;
        std::set_intersection(container_.begin(), container_.end(),
                              other.container_.begin(), other.container_.end(),
                              std::inserter(container, container.begin()),
                              RCPBasicKeyLess{});
        return finiteset(container);
    }
    if (is_a<Interval>(*o)) {
        set_basic container;
        for (const auto &a : container_) {
            auto contain = o->contains(a);
            if (eq(*contain, *boolTrue))
                container.insert(a);
            if (is_a<Contains>(*contain))
                throw std::runtime_error("Not implemented");
        }
        return finiteset(container);
    }
    if (is_a<UniversalSet>(*o) or is_a<EmptySet>(*o) or is_a<Union>(*o)) {
        return (*o).set_intersection(rcp_from_this_cast<const Set>());
    }
    return SymEngine::set_intersection({rcp_from_this_cast<const Set>(), o},
                                       false);
}

RCP<const Set> FiniteSet::set_complement(const RCP<const Set> &o) const
{
    if (is_a<FiniteSet>(*o)) {
        const FiniteSet &other = down_cast<const FiniteSet &>(*o);
        set_basic container;
        std::set_difference(other.container_.begin(), other.container_.end(),
                            container_.begin(), container_.end(),
                            std::inserter(container, container.begin()),
                            RCPBasicKeyLess{});
        return finiteset(container);
    }

    if (is_a<Interval>(*o)) {
        if (o->compare(*interval(NegInf, Inf, true, true)) == 0) {
            set_set intervals;
            RCP<const Number> last = NegInf;
            for (const auto &a : container_) {
                if (is_a<Integer>(*a)) {
                    auto a_int = make_rcp<const Integer>(
                        down_cast<const Integer &>(*a).as_integer_class());
                    intervals.insert(interval(last, a_int, true, true));
                    last = a_int;
                } else if (is_a<RealDouble>(*a)) {
                    auto a_rd = make_rcp<const RealDouble>(
                        down_cast<const RealDouble &>(*a).i);
                    intervals.insert(interval(last, a_rd, true, true));
                    last = a_rd;
                } else if (is_a<Rational>(*a)) {
                    auto a_rat = Rational::from_mpq(
                        down_cast<const Rational &>(*a).as_rational_class());
                    intervals.insert(interval(last, a_rat, true, true));
                    last = a_rat;
                } else {
                    throw std::runtime_error("Not implemented");
                }
            }
            intervals.insert(interval(last, Inf, true, true));
            return SymEngine::set_union(intervals, false);
        }
    }

    return SymEngine::set_complement_helper(rcp_from_this_cast<const Set>(), o);
}

Union::Union(set_set in) : container_(in)
{
    SYMENGINE_ASSIGN_TYPEID()
}

hash_t Union::__hash__() const
{
    hash_t seed = UNION;
    for (const auto &a : container_)
        hash_combine<Basic>(seed, *a);
    return seed;
}

bool Union::__eq__(const Basic &o) const
{
    if (is_a<Union>(o)) {
        const Union &other = down_cast<const Union &>(o);
        return unified_eq(container_, other.container_);
    }
    return false;
}

int Union::compare(const Basic &o) const
{
    SYMENGINE_ASSERT(is_a<Union>(o))
    const Union &other = down_cast<const Union &>(o);
    return unified_compare(container_, other.container_);
}

RCP<const Set> Union::set_union(const RCP<const Set> &o) const
{
    set_set container(container_);
    for (auto iter = container.begin(); iter != container.end(); ++iter) {
        auto temp = o->set_union(*iter);
        // If we are able to do union with `*iter`, we replace `*iter` with
        // the result of union.
        if (not eq(*temp, *SymEngine::set_union({o, *iter}, false))) {
            iter = container.erase(iter);
            container.insert(temp);
            return SymEngine::set_union(container);
        }
    }
    container.insert(o);
    return SymEngine::set_union(container, false);
}

RCP<const Set> Union::set_intersection(const RCP<const Set> &o) const
{
    set_set container;
    for (auto &a : container_) {
        container.insert(a->set_intersection(o));
    }
    return SymEngine::set_union(container);
}

RCP<const Set> Union::set_complement(const RCP<const Set> &o) const
{
    set_set container;
    for (auto &a : container_) {
        container.insert(a->set_complement(o));
    }
    return SymEngine::set_intersection(container);
}

RCP<const Boolean> Union::contains(const RCP<const Basic> &o) const
{
    for (auto &a : container_) {
        auto contain = a->contains(o);
        if (eq(*contain, *boolTrue)) {
            return boolean(true);
        }
        if (is_a<Contains>(*contain))
            throw NotImplementedError("Not implemented");
    }
    return boolean(false);
}

RCP<const Set> set_union(const set_set &in, bool solve)
{
    set_set input;
    if (solve == false && in.size() > 1)
        return make_rcp<const Union>(in);
    set_basic combined_FiniteSet;
    for (auto it = in.begin(); it != in.end(); ++it) {
        if (is_a<FiniteSet>(**it)) {
            const FiniteSet &other = down_cast<const FiniteSet &>(**it);
            combined_FiniteSet.insert(other.get_container().begin(),
                                      other.get_container().end());
        } else if (is_a<UniversalSet>(**it)) {
            return universalset();
        } else if (not is_a<EmptySet>(**it)) {
            input.insert(*it);
        }
    }
    if (input.empty()) {
        return finiteset(combined_FiniteSet);
    } else if (input.size() == 1 && combined_FiniteSet.empty()) {
        return rcp_dynamic_cast<const Set>(*input.begin());
    }
    // Now we rely on respective containers' own rules
    // TODO: Improve it to O(log n)
    RCP<const Set> combined_Rest = finiteset(combined_FiniteSet);
    for (auto it = input.begin(); it != input.end(); ++it) {
        RCP<const Set> other = rcp_dynamic_cast<const Set>(*it);
        combined_Rest = combined_Rest->set_union(other);
    }
    return combined_Rest;
}

RCP<const Set> set_intersection(const set_set &in, bool solve)
{
    if (solve == false)
        throw std::runtime_error(
            "Not implemented"); // No intersection class yet

    // https://en.wikipedia.org/wiki/Intersection_(set_theory)#Nullary_intersection
    if (in.empty())
        return universalset();

    // Global rules
    // If found any emptyset then return emptyset
    set_set incopy;
    for (const auto &input : in) {
        if (is_a<EmptySet>(*input))
            return emptyset();
        else if (not is_a<UniversalSet>(*input))
            incopy.insert(input);
    }

    if (incopy.empty())
        return universalset();
    if (incopy.size() == 1)
        return rcp_dynamic_cast<const Set>(*incopy.begin());

    // Handle finite sets
    set_set fsets, othersets;
    for (const auto &input : incopy) {
        if (is_a<FiniteSet>(*input))
            fsets.insert(input);
        else
            othersets.insert(input);
    }
    if (fsets.size() != 0) {
        const FiniteSet &fs = down_cast<const FiniteSet &>(**fsets.begin());
        fsets.erase(fsets.begin());
        set_basic finalfs;
        for (const auto &fselement : fs.get_container()) {
            bool present = true;
            for (const auto &fset : fsets) {
                present
                    = present and eq(*(fset->contains(fselement)), *boolTrue);
            }
            if (!present)
                continue;
            for (const auto &oset : othersets) {
                present
                    = present and eq(*(oset->contains(fselement)), *boolTrue);
            }
            if (present)
                finalfs.insert(fselement);
        }
        return finiteset(finalfs);
    }

    // If any of the sets is union, then return a Union of Intersections
    for (auto it = incopy.begin(); it != incopy.end(); ++it) {
        if (is_a<Union>(**it)) {
            auto container = down_cast<const Union &>(**it).get_container();
            incopy.erase(it);
            auto other = SymEngine::set_intersection(incopy);
            set_set usets;
            for (const auto &c : container) {
                usets.insert(SymEngine::set_intersection({c, other}));
            }
            return SymEngine::set_union(usets);
        }
    }

    // Simplify if any of the sets is a complement
    // Not implemented as we don't have a class for complement yet.

    // Pair-wise rules
    bool shouldContinue = true;
    while (shouldContinue) {
        for (const auto &input : incopy) {
            shouldContinue = false;
            set_set other_sets = incopy;
            auto it = other_sets.find(input);
            other_sets.erase(it);
            set_set newinput;
            for (auto &oset : other_sets) {
                // TO-DO: needs the following improvement once Intersection
                // class is implemented.
                // input_oset if found to be not simplified, then skip this
                // pair.
                try {
                    auto input_oset = input->set_intersection(oset);
                    newinput = other_sets;
                    it = newinput.find(oset);
                    newinput.erase(it);
                    newinput.insert(input_oset);
                    shouldContinue = true;
                    break;
                } catch (std::runtime_error &err) {
                    continue;
                }
            }
            if (not newinput.empty()) {
                incopy = newinput;
                break;
            }
        }
    }

    if (incopy.size() == 1)
        return *incopy.begin();
    else
        return SymEngine::set_intersection(incopy, false);
}

// helper to avoid redundant code
RCP<const Set> set_complement_helper(const RCP<const Set> &container,
                                     const RCP<const Set> &universe)
{
    if (is_a<Interval>(*universe)) {
        if (is_a<Interval>(*container) or is_a<FiniteSet>(*container)) {
            auto reals = interval(NegInf, Inf, true, true);
            auto wrtreals = container->set_complement(reals);
            return SymEngine::set_intersection({universe, wrtreals});
        }
    } else if (is_a<Union>(*universe)) {
        auto univ = down_cast<const Union &>(*universe).get_container();
        set_set container_;
        for (auto &a : univ) {
            container_.insert(container->set_complement(a));
        }
        return SymEngine::set_union(container_);
    } else if (is_a<EmptySet>(*universe)) {
        return emptyset();
    } else if (is_a<FiniteSet>(*universe)) {
        const FiniteSet &other = down_cast<const FiniteSet &>(*universe);
        set_basic container_;
        for (const auto &a : other.get_container()) {
            if (eq(*(container->contains(a)), *boolFalse))
                container_.insert(a);
        }
        return finiteset(container_);
    }
    return set_complement(universe, container, false);
}

RCP<const Set> set_complement(const RCP<const Set> &universe,
                              const RCP<const Set> &container, bool solve)
{
    // represents universe - container
    if (!solve)
        std::runtime_error("Complement class Not implemented yet");
    if (universe->is_subset(container))
        return emptyset();
    return container->set_complement(universe);
}

} // SymEngine
