/**
 *  \file sets.h
 *  Polynomial Manipulation
 *
 **/
#ifndef SYMENGINE_SETS_H
#define SYMENGINE_SETS_H

#include <symengine/basic.h>
#include <symengine/functions.h>
#include <symengine/number.h>
#include <symengine/rational.h>
#include <symengine/complex.h>


namespace SymEngine {

class Interval : public Basic  {
	public:
		RCP<const Number> start_;
		RCP<const Number> end_;
		bool left_open_, right_open_;
	public:
		IMPLEMENT_TYPEID(INTERVAL)
		virtual std::size_t __hash__() const;
		virtual bool __eq__(const Basic &o) const;
		virtual int compare(const Basic &o) const;
		virtual vec_basic get_args() const { return {}; }

		Interval(const RCP<const Number> &start, const RCP<const Number> &end, const bool left_open = false, const bool right_open = false);

		RCP<const Interval> open() const;

		RCP<const Interval> close() const;

		RCP<const Interval> Lopen() const;

		RCP<const Interval> Ropen() const;

		bool is_canonical(const RCP<const Number> &start, const RCP<const Number> &end, bool left_open, bool right_open) const;

		RCP<const Interval> interval_union(const Interval &other) const;

		RCP<const Interval> interval_intersection(const Interval &other) const;

		bool is_subset(const Interval &other) const;

		bool is_proper_subset (const Interval &other) const;

		bool is_superset(const Interval &other) const;

		bool is_proper_superset(const Interval &other) const;
	};

//! \return RCP<const Interval>
inline RCP<const Interval> interval(const RCP<const Number> &start, const RCP<const Number> &end, const bool left_open = false, const bool right_open = false)
{
	return make_rcp<const Interval>(start, end, left_open, right_open);
}

}
#endif
