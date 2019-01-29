#ifndef SYMENGINE_UTILITIES_MATCHPYCPP_UTILS_H_
#define SYMENGINE_UTILITIES_MATCHPYCPP_UTILS_H_

#include <vector>
#include <map>
#include <functional>

#include "common.h"
#include "bipartite.h"

using namespace std;

RCP<const Basic> None = symbol("None");

template <typename T, typename Comparison>
map<T, int, Comparison> count_multiset(multiset<T, Comparison> m)
{
    map<T, int, Comparison> result;
    for (const T &elem : m) {
        result[elem]++;
    }
    return result;
}

template <typename T>
vector<vector<T>> itertools_product(const vector<vector<T>> &v)
{
    vector<vector<T>> result;
    vector<vector<T>> temp;
    for (const T &e : v[0]) {
        vector<T> val;
        val.push_back(e);
        result.push_back(val);
    }

    for (unsigned int i = 1; i < v.size(); ++i) {
        temp.clear();
        const vector<T> &vi = v[i];
        for (vector<T> current : result) {
            for (const T &elem : vi) {
                vector<T> copycurr(current.begin(), current.end());
                copycurr.push_back(elem);
                temp.push_back(copycurr);
            }
        }
        result = temp;
    }
    return result;
}

// template <typename T>
class VariableWithCount
{
public:
    VariableWithCount(RCP<const Basic> name, int count, int minimum,
                      RCP<const Basic> defaultv)
        : name(name), count(count), minimum(minimum), defaultv(defaultv)
    {
    }
    RCP<const Basic> name;
    int count;
    int minimum;
    RCP<const Basic> defaultv;
};

generator<Substitution>
_commutative_single_variable_partiton_iter(MultisetOfBasic values,
                                           VariableWithCount variable)
{
    string name = variable.name->__str__();
    int count = variable.count;
    int minimum = variable.minimum;
    RCP<const Basic> defaultv = variable.defaultv;

    generator<Substitution> result;

    if (values.size() == 0 && defaultv != None) {
        result.push_back(Substitution{{name, {defaultv}}});
        return result;
    }
    if (count == 1) {
        if (values.size() >= minimum) {
            if (name != "None") {
                result.push_back(Substitution{{name, {values}}});
            }
        }
    } else {
        MultisetOfBasic new_values;
        for (const pair<RCP<const Basic>, int> &p : count_multiset(values)) {
            RCP<const Basic> element = p.first;
            int element_count = p.second;
            if (element_count % count != 0) {
                return generator<Substitution>();
            }
            for (int i = 0; i < element_count; i++) {
                new_values.insert(element); // TODO: make this more efficient
            }
        }
        if (new_values.size() >= minimum) {
            if (name != "None") {
                result.push_back(Substitution{{name, {new_values}}});
            }
        }
    }
    return result;
}

function<void(Substitution)>
_make_variable_generator_factory(RCP<const Basic> value, int total,
                                 vector<VariableWithCount> variables)
{
    vector<int> var_counts;
    for (VariableWithCount &v : variables) {
        var_counts.push_back(v.count);
    }
    // vector<int> cache_key;
    // cache_key.push_back(total);
    // cache_key.insert(var_counts.begin(), var_counts.end());

    auto _factory = [&](Substitution subst) {
        // if cache_key in _linear_diop_solution_cache:
        //    solutions = _linear_diop_solution_cache[cache_key]
        // else:
        vector<Substitution> result;
        // TODO: finish
        /*
            solutions = list(solve_linear_diop(total, *var_counts))
            _linear_diop_solution_cache[cache_key] = solutions
        for (auto &solution : solutions) {
            new_subst = copy.copy(subst)
            for var, count in zip(variables, solution):
                new_subst[var.name][value] = count
            result.push_back(new_subst);
        }
        */
        return result;
    };

    return _factory;
}

// generator<Substitution>
generator<Substitution> commutative_sequence_variable_partition_iter(
    MultisetOfBasic values, vector<VariableWithCount> variables)
{
    generator<Substitution> result;
    return result;

    if (variables.size() == 1) {
        return _commutative_single_variable_partiton_iter(values, variables[0]);
    }

    vector<function<void(Substitution)>> generators;
    for (const pair<RCP<const Basic>, int> &p : count_multiset(values)) {
        RCP<const Basic> value = p.first;
        int count = p.second;
        generators.push_back(
            _make_variable_generator_factory(value, count, variables));
    }

    map<string, multiset<int>> initial;
    for (VariableWithCount &var : variables) {
        initial[var.name->__str__()] = multiset<int>();
    }
    bool valid;
    // vector<map<string, multiset<int>>> result;
    // TODO:
    /*
    for (map<string, multiset<int>> &subst :
         generator_chain(initial, *generators)) {
        valid = true;
        for (VariableWithCount &var : variables) {
            if (var.defaultv != None && subst[var.name].size() == 0) {
                subst[var.name] = var.defaultv;
            } else if (subst[var.name].size() < var.minimum) {
                valid = false;
                break;
            }
        }
        if (valid) {
            if (subst.find("") != subst.end()) {
                delete subst[None];
            }
            result.push_back(subst);
        }
    }
    */
    return result;
}

#endif /* SYMENGINE_UTILITIES_MATCHPYCPP_UTILS_H_ */
