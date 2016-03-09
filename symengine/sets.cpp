#include <symengine/sets.h>
#include <symengine/integer.h>

namespace SymEngine {

	Interval::Interval(const RCP<const Number> &start, const RCP<const Number> &end, const bool left_open, const bool right_open)
		: start_(start), end_(end), left_open_(left_open), right_open_(right_open) {
			SYMENGINE_ASSERT(is_canonical(start_, end_, left_open_, right_open_));
		}

	bool Interval::is_canonical(const RCP<const Number> &s, const RCP<const Number> &e, bool left_open, bool right_open) const {
		if (is_a<Complex>(*s) or is_a<Complex>(*e))
			throw std::runtime_error("Complex set not implemented");
		if (eq(*e, *s)) {
			if (left_open or right_open)
				throw std::runtime_error("Empty set not implemented");
		}
		else if (eq(*min({s, e}), *e)){
			throw std::runtime_error("Empty set not implemented");
		}
		return true;
	}

	std::size_t Interval::__hash__() const {
		std::size_t seed = INTERVAL;
		hash_combine<long long int>(seed, start_->__hash__());
		hash_combine<long long int>(seed, end_->__hash__());
		hash_combine<bool>(seed, left_open_);
		hash_combine<bool>(seed, right_open_);
		return seed;
	}

	bool Interval::__eq__(const Basic &o) const	{
		if (is_a<Interval>(o)) {
			const Interval &s = static_cast<const Interval&>(o);
			return ((eq(*this->start_, *s.start_) and eq(*this->end_, *s.end_)) and
					((this->left_open_ == s.left_open_) and (this->right_open_ == s.right_open_)));
		}
		return false;
	}

	int Interval::compare(const Basic &o) const {
		SYMENGINE_ASSERT(is_a<Interval>(o))
		const Interval &s = static_cast<const Interval &>(o);
		if ((eq(*this->start_, *s.start_) and eq(*this->end_, *s.end_)) and
					((this->left_open_ == s.left_open_) and (this->right_open_ == s.right_open_)))
			return 0;
		else
			throw std::runtime_error("Not implemented");
	}

	RCP<const Interval> Interval::open() const {
		return interval(start_, end_, true, true);
	}

	RCP<const Interval> Interval::Lopen() const {
		return interval(start_, end_, true, false);
	}

	RCP<const Interval> Interval::Ropen() const {
		return interval(start_, end_, false, true);
	}

	RCP<const Interval> Interval::close() const {
		return interval(start_, end_, false, false);
	}

	RCP<const Interval> Interval::interval_intersection(const Interval &other) const {
		RCP<const Number> start, end;
		bool left_open, right_open;
		RCP<const Basic> start_end, end_start;
		start_end = min({this->start_, other.end_});
		end_start = min({this->end_, other.start_});

		if (eq(*this->start_, *start_end) and eq(*other.start_ , *end_start)) {
			RCP<const Basic> start_start, end_end;
			start_start = min({this->start_, other.start_});
			end_end = min({this->end_, other.end_});
			if (neq(*this->start_, *other.start_)) {
				if (eq(*this->start_, *start_start)) {
					start = other.start_;
					left_open = other.left_open_;
				}
				else {
					start = this->start_;
					left_open = this->left_open_;
				}
			}
			else {
				start = this->start_;
				left_open = this->left_open_ or other.left_open_ ;
			}

			if (neq(*this->end_, *other.end_)) {
				if (eq(*this->end_, *end_end)) {
					end = this->end_;
					right_open = this->right_open_;
				}
				else {
					end = other.end_;
					right_open = other.right_open_;
				}
			}
			else {
				end = this->end_;
				right_open = this->right_open_ or other.right_open_;
			}
			return interval(start, end, left_open, right_open);
		}
		else {
			throw std::runtime_error("Empty set not implemented");
		}
	}

	RCP<const Interval> Interval::interval_union(const Interval &other) const {
		RCP<const Basic> start_start, end_end, m;
		RCP<const Number> start, end;
		bool left_open, right_open;
		start_start = max({this->start_, other.start_});
		end_end = min({this->end_, other.end_});
		m = min({start_start, end_end});
		if ((eq(*end_end, *start_start) and eq(*end_end, *m) and
			((eq(*end_end, *this->end_) and this->right_open_) or
			 (eq(*end_end, *other.end_) and other.right_open_))) or
			(eq(*end_end, *m) and not eq (*end_end, *start_start))) {
			throw std::runtime_error("not implemented");
		}
		else {
			if (eq(*min({this->start_, other.start_}), *this->start_))
				start = this->start_;
			else
				start = other.start_;
			if (eq(*max({this->end_, other.end_}), *this->end_))
				end = this->end_;
			else
				end = other.end_;
			left_open = ((neq(*this->start_, *start) or this->left_open_) and
							 (neq(*other.start_, *start) or other.left_open_));
			right_open = ((neq(*this->end_, *end) or this->right_open_) and
							(neq(*other.end_, *end) or other.right_open_));
			return interval(start, end, left_open, right_open);
		}
	}

	bool Interval::is_subset(const Interval &other) const {
		return __eq__(*this->Interval::interval_intersection(other));
	}

	bool Interval::is_proper_subset(const Interval &other) const {
		return __eq__(*this->Interval::interval_intersection(other)) and (not __eq__(other));
	}

	bool Interval::is_superset(const Interval &other) const {
		return other.is_subset(*this);
	}

	bool Interval::is_proper_superset(const Interval &other) const {
		return other.is_subset(*this) and (not __eq__(other));
	}
}
