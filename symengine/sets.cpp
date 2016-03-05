#include <symengine/sets.h>
#include <symengine/integer.h>

namespace SymEngine {

	Interval::Interval(rational_class start, rational_class end, bool left_open, bool right_open)
		: start_(start), end_(end), left_open_(left_open), right_open_(right_open) {
			SYMENGINE_ASSERT(is_canonical(start_, end_, left_open_, right_open_));
		}

	bool Interval::is_canonical(const rational_class &s, const rational_class &e, bool left_open, bool right_open) const {
		rational_class start = s;
		rational_class end = e;
		canonicalize(start);
		canonicalize(end);
		if (end < start) {
			throw std::runtime_error("Empty set not implemented");
		}
		else if (end == start) {
			if (left_open or right_open)
				throw std::runtime_error("Empty set not implemented");
		}
		return true;
	}

	std::size_t Interval::__hash__() const {
		std::size_t seed = INTERVAL;
		hash_combine<long long int>(seed, mp_get_si(get_num(this->start_)));
	    hash_combine<long long int>(seed, mp_get_si(get_den(this->start_)));
	    hash_combine<long long int>(seed, mp_get_si(get_num(this->end_)));
	    hash_combine<long long int>(seed, mp_get_si(get_den(this->end_)));
		hash_combine<bool>(seed, left_open_);
		hash_combine<bool>(seed, right_open_);
		return seed;
	}

	bool Interval::__eq__(const Basic &o) const	{
		if (is_a<Interval>(o)) {
			const Interval &s = static_cast<const Interval &>(o);
			return (((this->start_ == s.start_) and (this->end_ == s.end_)) and
					((this->left_open_ == s.left_open_) and (this->right_open_ == s.right_open_)));
		}
		return false;
	}

	int Interval::compare(const Basic &o) const {
		SYMENGINE_ASSERT(is_a<Interval>(o))
		const Interval &s = static_cast<const Interval &>(o);
		if ((start_ == s.start_ and end_ == s.end_) and (left_open_ == s.left_open_ and right_open_ == s.right_open_))
			return 0;
		else
			throw std::runtime_error("Not implemented");
	}

	RCP<const Interval> Interval::from_mpq(const rational_class start, const rational_class end, const bool left_open, const bool right_open)
	{
		return make_rcp<const Interval>(start, end, left_open, right_open);
	}

	RCP<const Interval> Interval::from_two_nums(const Number &start, const Number &end, const bool left_open, const bool right_open)
	{
		if (is_a<Integer>(start) and is_a<Integer>(end)) {
			rational_class start_mpq(static_cast<const Integer&>(start).i, static_cast<const Integer&>(*one).i);
			rational_class end_mpq(static_cast<const Integer&>(end).i, static_cast<const Integer&>(*one).i);
			return Interval::from_mpq(start_mpq, end_mpq, left_open, right_open);
		} else if (is_a<Rational>(start) and is_a<Integer>(end)) {
			rational_class start_mpq = static_cast<const Rational&>(start).i;
			rational_class end_mpq(static_cast<const Integer&>(end).i, static_cast<const Integer&>(*one).i);
			return Interval::from_mpq(start_mpq, end_mpq, left_open, right_open);
		} else if (is_a<Integer>(start) and is_a<Rational>(end)) {
			rational_class start_mpq(static_cast<const Integer&>(start).i, static_cast<const Integer&>(*one).i);
			rational_class end_mpq = static_cast<const Rational&>(end).i;
			return Interval::from_mpq(start_mpq, end_mpq, left_open, right_open);
		} else if (is_a<Rational>(start) and is_a<Rational>(end)) {
			rational_class start_mpq = static_cast<const Rational&>(start).i;
			rational_class end_mpq = static_cast<const Rational&>(end).i;
			return Interval::from_mpq(start_mpq, end_mpq, left_open, right_open);
		} else {
			throw std::runtime_error("Invalid Format: Expected Integer or Rational");
		}
	}

	RCP<const Interval> Interval::open() const {
		return Interval::from_mpq(start_, end_, true, true);
	}

	RCP<const Interval> Interval::Lopen() const {
		return Interval::from_mpq(start_, end_, true, false);
	}

	RCP<const Interval> Interval::Ropen() const {
		return Interval::from_mpq(start_, end_, false, true);
	}

	RCP<const Interval> Interval::close() const {
		return Interval::from_mpq(start_, end_, false, false);
	}

	RCP<const Interval> Interval::interval_intersection(const Interval &second) const {
		rational_class start, end;
		bool left_open, right_open;
		if ((this->start_ <= second.end_) and (this->end_ >= second.start_)) {
			if (this->start_ < second.start_) {
				start = second.start_;
				left_open = second.left_open_;
			}
			else if (this->start_ > second.start_) {
				start = this->start_;
				left_open = this->left_open_;
			}
			else {
				start = this->start_;
				left_open = this->left_open_ or second.left_open_ ;
			}

			if (this->end_ < second.end_) {
				end = this->end_;
				right_open = this->right_open_;
			}
			else if (this->end_ > second.end_) {
				end = second.end_;
				right_open = second.right_open_;
			}
			else {
				end = this->end_;
				right_open = this->right_open_ or second.right_open_;
			}
			return Interval::from_mpq(start, end, left_open, right_open);
		}
		else {
			throw std::runtime_error("Empty set not implemented");
		}
	}

	RCP<const Interval> Interval::interval_union(const Interval &second) const {
		rational_class start, end;
		bool left_open, right_open;
		start = std::max(this->start_, second.start_);
		end = std::min(this->end_, second.end_);
		if ((end < start) or ((end == start) and
			((end == this->end_ and this->right_open_) or
			 (end == second.end_ and second.right_open_)))) {
			throw std::runtime_error("not implemented");
		}
		else {
			start = std::min(this->start_, second.start_);
			end = std::max(this->end_, second.end_);
			left_open = ((this->start_ != start or this->left_open_) and
							 (second.start_ != start or second.left_open_));
			right_open = ((this->end_ != end or this->right_open_) and
							(second.end_ != end or second.right_open_));
			return Interval::from_mpq(start, end, left_open, right_open);
		}
	}

	bool Interval::is_subset(const Interval &second) const {
		return __eq__(*this->Interval::interval_intersection(second));
	}

	bool Interval::is_proper_subset(const Interval &second) const {
		return __eq__(*this->Interval::interval_intersection(second)) and (not __eq__(second));
	}

	bool Interval::is_superset(const Interval &second) const {
		return second.is_subset(*this);
	}

	bool Interval::is_proper_superset(const Interval &second) const {
		return second.is_subset(*this) and (not __eq__(second));
	}
}
