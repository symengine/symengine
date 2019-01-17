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

class Substitution : public map<string, RCP<const Basic>>
{
public:
    int try_add_variable(const string variable_name,
                         const RCP<const Basic> &replacement)
    {
        if (this->find(variable_name) == this->end()) {
            (*this)[variable_name] = replacement;
        } else {
        }
        return 0;
    }

    Substitution &substitution_union(vector<Substitution> &others)
    {
        Substitution new_subst(*this);

        for (Substitution &other : others) {
            for (const pair<string, RCP<const Basic>> &p : other) {
                new_subst.try_add_variable(p.first, p.second);
            }
        }
        return new_subst;
    }
};

#endif /* SYMENGINE_UTILITIES_MATCHPYCPP_SUBSTITUTION_H_ */
