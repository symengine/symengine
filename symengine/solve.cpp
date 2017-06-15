#include <symengine/solve.h>

namespace SymEngine
{


	RCP<const Set> solve(const RCP<const Basic> &f, const RCP<const Symbol> &sym, const RCP<const Set> &domain); 
	{
		if (eq(*f,*boolTrue))
			return domain;
		if (eq(*f,*boolFalse))
			return emptyset(); 
		if (is_a<Eq>(*f)) {
			return solve(sub(down_cast<const Relational&>(*f).get_arg1(),down_cast<const Relational&>(*f).get_arg2()),sym,domain);
		} else if (is_a<Ne>(*f)) {
			auto soln = solve(sub(down_cast<const Relational&>(*f).get_arg1(),down_cast<const Relational&>(*f).get_arg2()),sym,domain);
			return set_complement(domain,soln);
		} else if (is_a<Relational>(*f)) {
			// Solving inequalities not implemented yet.
			return conditionset(sym,f,domain);
		}
		RCP<const Basic> newf = f;
		if (is_a<Mul>(*f))
			newf = make_rcp<const Mul>(one, std::move(down_cast<const Mul&>(*f).get_dict()));
		
	}

}