#ifndef SYMENGINE_UTILITIES_MATCHPYCPP_SUBSTITUTION_H_
#define SYMENGINE_UTILITIES_MATCHPYCPP_SUBSTITUTION_H_

#include <symengine/basic.h>
#include <symengine/add.h>
#include <symengine/pow.h>
#include <symengine/mul.h>

#include <map>
#include <vector>

using namespace std;
using namespace SymEngine;

typedef map<string, RCP<const Basic>> Substitution;

/*
class Substitution
{
private:
	typedef map<string, RCP<const Basic>> Dict;
	Dict dict;
public:
	Substitution(Substitution &subst)
	{
		dict.insert(subst.dict.begin(), subst.dict.end());
	}
	Substitution()
	{
	}

	Dict::iterator find(const string &a)
	{
		return dict.find(a);
	}
	Dict::iterator begin()
	{
		return dict.begin();
	}
	Dict::iterator end()
	{
		return dict.end();
	}
	Dict &get_dict()
	{
		return dict;
	}
*/
    int try_add_variable(Substitution subst, const string variable_name,
                         const RCP<const Basic> &replacement)
    {
        if (subst.find(variable_name) == subst.end()) {
        	subst[variable_name] = replacement;
        } else {
        }
        return 0;
    }

    Substitution substitution_union(Substitution subst, vector<Substitution> &others)
    {
        Substitution new_subst; //(*this);

        for (Substitution &other : others) {
            for (const pair<string, RCP<const Basic>> &p : other) {
                try_add_variable(subst, p.first, p.second);
            }
        }
        return new_subst;
    }
//};

#endif /* SYMENGINE_UTILITIES_MATCHPYCPP_SUBSTITUTION_H_ */
