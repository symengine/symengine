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

// bool operator<(RCP<const Basic> &a, RCP<const Basic> &b)
//{
//	return (a->__hash__() < b->__hash__())? true : false;
//}

struct lessBasic {
    bool operator()(const RCP<const Basic> &x, const RCP<const Basic> &y) const
    {
        return (x->__hash__()) < (y->__hash__());
    }
};

bool operator==(RCP<const Basic> &a, RCP<const Basic> &b)
{
    return (a->__hash__()) == (b->__hash__());
}

typedef multiset<RCP<const Basic>, lessBasic> MultisetOfBasic;

typedef map<string, MultisetOfBasic> Substitution;

int try_add_variable(Substitution subst, const string variable_name,
                     const MultisetOfBasic &replacement)
{
    if (subst.find(variable_name) == subst.end()) {
        subst[variable_name] = replacement;
    } else {
        MultisetOfBasic existing_value = subst.at(variable_name);
        if (existing_value.size() != replacement.size()) {
            return 1;
        }
        MultisetOfBasic::iterator it1 = existing_value.begin();
        MultisetOfBasic::iterator it2 = replacement.begin();
        for (size_t i = 0; i < existing_value.size(); i++) {
            if ((*it1) != (*it2)) {
                return 1;
            }
        }
        return 1;
    }
    return 0;
}

int try_add_variable(Substitution subst, const string variable_name,
                     vector<RCP<const Basic>> &replacement)
{
    MultisetOfBasic new_repl;
    new_repl.insert(replacement.begin(), replacement.end());
    return try_add_variable(subst, variable_name, new_repl);
}

int try_add_variable(Substitution subst, const string variable_name,
                     const RCP<const Basic> &replacement)
{
    MultisetOfBasic new_repl = {replacement};
    return try_add_variable(subst, variable_name, new_repl);
}

Substitution substitution_union(Substitution subst,
                                const vector<Substitution> &others)
{
    Substitution new_subst; //(*this);

    for (const Substitution &other : others) {
        for (const pair<string, MultisetOfBasic> &p : other) {
            int ret = try_add_variable(subst, p.first, p.second);
            assert(ret == 0);
        }
    }
    return new_subst;
}

#endif /* SYMENGINE_UTILITIES_MATCHPYCPP_SUBSTITUTION_H_ */
