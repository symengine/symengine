#include <symengine/sets.h>
#include <iterator>

namespace SymEngine
{

Interval::Interval(const RCP<const Number> &start, const RCP<const Number> &end,
                   const bool left_open, const bool right_open)
    : start_(start), end_(end), left_open_(left_open), right_open_(right_open)
{
    SYMENGINE_ASSERT(
        Interval::is_canonical(start_, end_, left_open_, right_open_));
}

bool Interval::is_canonical(const RCP<const Number> &s,
                            const RCP<const Number> &e, bool left_open,
                            bool right_open)
{
    if (is_a<Complex>(*s) or is_a<Complex>(*e))
        throw std::runtime_error("Complex set not implemented");
    if (eq(*e, *s)) {
        return false;
    } else if (eq(*min({s, e}), *e)) {
        return false;
    }
    return true;
}

std::size_t Interval::__hash__() const
{
    std::size_t seed = INTERVAL;
    hash_combine<Basic>(seed, *start_);
    hash_combine<Basic>(seed, *end_);
    hash_combine<bool>(seed, left_open_);
    hash_combine<bool>(seed, right_open_);
    return seed;
}

bool Interval::__eq__(const Basic &o) const
{
    if (is_a<Interval>(o)) {
        const Interval &s = static_cast<const Interval &>(o);
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
    const Interval &o = static_cast<const Interval &>(s);
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

bool Interval::contains(const RCP<const Basic> &a) const
{
    if (not is_a_Number(*a))
        throw std::runtime_error("Not implemented");
    if ((eq(*start_, *a) and left_open_) or (eq(*end_, *a) and right_open_))
        return false;
    if (eq(*start_, *a) or eq(*end_, *a))
        return true;
    if (eq(*min({end_, a}), *end_))
        return false;
    if (eq(*max({start_, a}), *start_))
        return false;
    return true;
}

RCP<const Set> Interval::set_intersection(const RCP<const Set> &o) const
{
    if (is_a<Interval>(*o)) {
        const Interval &other = static_cast<const Interval &>(*o);
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
    return (*o).set_intersection(rcp_from_this_cast<const Set>());
}

RCP<const Set> Interval::set_union(const RCP<const Set> &o) const
{
    if (is_a<Interval>(*o)) {
        const Interval &other = static_cast<const Interval &>(*o);
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
            throw std::runtime_error("not implemented");
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
    return (*o).set_union(rcp_from_this_cast<const Set>());
}

bool Interval::is_subset(const RCP<const Set> &o) const
{
    if (is_a<Interval>(*o)) {
        return this->__eq__(*this->set_intersection(o));
    }
    return (*o).is_superset(rcp_from_this_cast<const Set>());
}

bool Interval::is_proper_subset(const RCP<const Set> &o) const
{
    if (is_a<Interval>(*o)) {
        return this->__eq__(*this->set_intersection(o))
               and (not this->__eq__(*o));
    } else {
        return (*o).is_proper_superset(rcp_from_this_cast<const Set>());
    }
}

bool Interval::is_superset(const RCP<const Set> &o) const
{
    return (*o).is_subset(rcp_from_this_cast<const Set>());
}

bool Interval::is_proper_superset(const RCP<const Set> &o) const
{
    return (*o).is_subset(rcp_from_this_cast<const Set>()) and (not __eq__(*o));
}

RCP<const Set> EmptySet::set_intersection(const RCP<const Set> &o) const
{
    return emptyset();
}

RCP<const Set> EmptySet::set_union(const RCP<const Set> &o) const
{
    return o;
}

std::size_t EmptySet::__hash__() const
{
    std::size_t seed = EMPTYSET;
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

bool EmptySet::is_proper_subset(const RCP<const Set> &o) const
{
    if (is_a<EmptySet>(*o)) {
        return false;
    }
    return true;
}

bool EmptySet::is_superset(const RCP<const Set> &o) const
{
    if (is_a<EmptySet>(*o)) {
        return true;
    }
    return false;
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

std::size_t UniversalSet::__hash__() const
{
    std::size_t seed = UNIVERSALSET;
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

bool UniversalSet::is_subset(const RCP<const Set> &o) const
{
    if (is_a<UniversalSet>(*o)) {
        return true;
    }
    return false;
}

bool UniversalSet::is_proper_superset(const RCP<const Set> &o) const
{
    if (is_a<UniversalSet>(*o)) {
        return false;
    }
    return true;
}

const RCP<const UniversalSet> &UniversalSet::getInstance()
{
    const static auto a = make_rcp<const UniversalSet>();
    return a;
}

FiniteSet::FiniteSet(const set_basic container) : container_(container)
{
    SYMENGINE_ASSERT(FiniteSet::is_canonical(container_));
}

bool FiniteSet::is_canonical(const set_basic container)
{
    return container.size() != 0;
}

std::size_t FiniteSet::__hash__() const
{
    std::size_t seed = FINITESET;
    for (const auto &a : container_)
        hash_combine<Basic>(seed, *a);
    return seed;
}

bool FiniteSet::__eq__(const Basic &o) const
{
    if (is_a<FiniteSet>(o)) {
        const FiniteSet &other = static_cast<const FiniteSet &>(o);
        return unified_eq(container_, other.container_);
    }
    return false;
}

int FiniteSet::compare(const Basic &o) const
{
    // compares two FiniteSet based on their length
    SYMENGINE_ASSERT(is_a<FiniteSet>(o))
    const FiniteSet &other = static_cast<const FiniteSet &>(o);
    return unified_compare(container_, other.container_);
}

bool FiniteSet::contains(const RCP<const Basic> &a) const
{
    return container_.find(a) != container_.end();
}

RCP<const Set> FiniteSet::set_union(const RCP<const Set> &o) const
{
    if (is_a<FiniteSet>(*o)) {
        const FiniteSet &other = static_cast<const FiniteSet &>(*o);
        set_basic container;
        std::set_union(container_.begin(), container_.end(),
                       other.container_.begin(), other.container_.end(),
                       std::inserter(container, container.begin()),
                       RCPBasicKeyLess{});
        return finiteset(container);
    }
    if (is_a<Interval>(*o)) {
        set_basic container;
        const Interval &other = static_cast<const Interval &>(*o);
        bool left = other.left_open_, right = other.right_open_;
        for (const auto &a : container_) {
            if (not o->contains(a)) {
                if (left)
                    if (eq(*other.start_, *a)) {
                        left = false;
                        continue;
                    }
                if (right)
                    if (eq(*other.end_, *a)) {
                        right = false;
                        continue;
                    }
                container.insert(a);
            }
        }
        if (not container.empty()) {
            throw std::runtime_error("not implemented");
        } else {
            if (left == other.left_open_ and right == other.right_open_)
                return o;
            else
                return interval(other.start_, other.end_, left, right);
        }
    }
    return (*o).set_union(rcp_from_this_cast<const Set>());
}

RCP<const Set> FiniteSet::set_intersection(const RCP<const Set> &o) const
{
    if (is_a<FiniteSet>(*o)) {
        const FiniteSet &other = static_cast<const FiniteSet &>(*o);
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
            if (o->contains(a))
                container.insert(a);
        }
        return finiteset(container);
    }
    return (*o).set_intersection(rcp_from_this_cast<const Set>());
}

bool FiniteSet::is_subset(const RCP<const Set> &o) const
{
    if (is_a<FiniteSet>(*o)) {
        const FiniteSet &other = static_cast<const FiniteSet &>(*o);
        return std::includes(other.container_.begin(), other.container_.end(),
                             container_.begin(), container_.end(),
                             RCPBasicKeyLess{});
    }
    if (is_a<Interval>(*o)) {
        for (const auto &a : container_) {
            if (not o->contains(a))
                return false;
        }
        return true;
    }
    return (*o).is_superset(rcp_from_this_cast<const Set>());
}

bool FiniteSet::is_proper_subset(const RCP<const Set> &o) const
{
    if (is_a<FiniteSet>(*o)) {
        const FiniteSet &other = static_cast<const FiniteSet &>(*o);
        return (container_.size() < other.container_.size())
               and std::includes(other.container_.begin(),
                                 other.container_.end(), container_.begin(),
                                 container_.end(), RCPBasicKeyLess{});
    }
    if (is_a<Interval>(*o)) {
        return is_subset(o);
    }
    return (*o).is_proper_superset(rcp_from_this_cast<const Set>());
}

bool FiniteSet::is_superset(const RCP<const Set> &o) const
{
    if (is_a<Interval>(*o)) {
        return false;
    }
    return (*o).is_subset(rcp_from_this_cast<const Set>());
}

bool FiniteSet::is_proper_superset(const RCP<const Set> &o) const
{
    return (*o).is_subset(rcp_from_this_cast<const Set>()) and (not __eq__(*o));
}
} // SymEngine
