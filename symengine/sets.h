/**
 *  \file sets.h
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

class Set : public Basic {
	public:
		virtual bool is_Interval() const = 0;
		virtual bool is_EmptySet() const = 0;
		virtual bool is_FiniteSet() const = 0;
		virtual RCP<const Set> _intersection(const Set &other) const = 0;
		virtual RCP<const Set> _union(const Set &other) const = 0;
		virtual bool is_subset(const Set &other) const = 0;
		virtual bool is_proper_subset (const Set &other) const = 0;
		virtual bool is_superset(const Set &other) const = 0;
		virtual bool is_proper_superset(const Set &other) const = 0;
};

class EmptySet : public Set {
	private:
		EmptySet() {};
	public:
		IMPLEMENT_TYPEID(EMPTYSET)
		//EmptySet(EmptySet const&) = delete;
		//void operator=(EmptySet const&) = delete;	
		static RCP<const EmptySet> getInstance();
		virtual std::size_t __hash__() const;
		virtual bool __eq__(const Basic &o) const;
		virtual int compare(const Basic &o) const;
		virtual vec_basic get_args() const { return {}; }

		inline virtual bool is_Interval() const { return false; }

		inline virtual bool is_EmptySet() const { return true; }

		inline virtual bool is_FiniteSet() const { return true; }

		virtual RCP<const Set> _intersection(const Set &other) const;
		virtual RCP<const Set> _union(const Set &other) const;

		virtual bool is_subset(const Set &other) const { return true; };
		virtual bool is_proper_subset (const Set &other) const;
		virtual bool is_superset(const Set &other) const { return false;};
		virtual bool is_proper_superset(const Set &other) const { return false; };
};

class Interval : public Set {
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

		inline virtual bool is_Interval() const { return true; }

		inline virtual bool is_EmptySet() const { return false; }

		inline virtual bool is_FiniteSet() const { return (eq(*start_, *end_) and not (left_open_ or right_open_)); }

		Interval(const RCP<const Number> &start, const RCP<const Number> &end, const bool left_open = false, const bool right_open = false);

		RCP<const Set> open() const;

		RCP<const Set> close() const;

		RCP<const Set> Lopen() const;

		RCP<const Set> Ropen() const;

		bool is_canonical(const RCP<const Number> &start, const RCP<const Number> &end, bool left_open, bool right_open) const;

		virtual RCP<const Set> _union(const Set &other) const;

		virtual RCP<const Set> _intersection(const Set &other) const;

		virtual bool is_subset(const Set &other) const;

		virtual bool is_proper_subset (const Set &other) const;

		virtual bool is_superset(const Set &other) const;

		virtual bool is_proper_superset(const Set &other) const;

};

//! \return RCP<const Set>
inline RCP<const Interval> interval(const RCP<const Number> &start, const RCP<const Number> &end, const bool left_open = false, const bool right_open = false)
{
	return make_rcp<const Interval>(start, end, left_open, right_open);
}

//! \return RCP<const Set>
inline RCP<const EmptySet> emptyset()
{
	return EmptySet::getInstance();
}


}
#endif
