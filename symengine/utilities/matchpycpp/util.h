#ifndef SYMENGINE_UTILITIES_MATCHPYCPP_UTIL_H_
#define SYMENGINE_UTILITIES_MATCHPYCPP_UTIL_H_

#include <vector>
#include <map>
#include <functional>

#include "bipartite.h"

using namespace std;

template <typename T>
using generator = vector<T>;

RCP<const Basic> None = symbol("None");

template <typename T>
map<T, int> count_multiset(multiset<T> m)
{
    map<T, int> result;
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

    for (int i = 1; i < v.size(); ++i) {
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

class VariableWithCount
{
public:
    VariableWithCount(string name, int count, int minimum,
                      RCP<const Basic> defaultv)
        : name(name), count(count), minimum(minimum), defaultv(defaultv)
    {
    }
    string name;
    int count;
    int minimum;
    RCP<const Basic> defaultv;
};

/*
 * Algorithm described in "Algorithms for Enumerating All Perfect, Maximum and
 * Maximal Matchings in Bipartite Graphs"
 * By Takeaki Uno in "Algorithms and Computation: 8th International Symposium,
 * ISAAC '97 Singapore,
 * December 17-19, 1997 Proceedings"
 * See http://dx.doi.org/10.1007/3-540-63890-3_11
 */

template <typename TLeft, typename TRight, typename TEdgeValue>
generator<vector<map<TLeft, TRight>>> _enum_maximum_matchings_iter(
    BipartiteGraph<TLeft, TRight, TEdgeValue> graph,
    map<TLeft, TRight> matching,
    _DirectedMatchGraph<TLeft, TRight, TEdgeValue> directed_match_graph)
{
    vector<map<TLeft, TRight>> result;
    map<TLeft, TRight> new_match;
    typedef tuple<TLeft, TRight> Edge;
    // WARNING: assuming TLeft = TRight:
    typedef TLeft Node;
    typedef vector<Node> NodeList;
    Edge edge;
    NodeList cycle;
    _DirectedMatchGraph<TLeft, TRight, TEdgeValue> directed_match_graph_minus,
        directed_match_graph_plus;
    set<Node> old_value;

    BipartiteGraph<TLeft, TRight, TEdgeValue> graph_plus, graph_minus;
    _DirectedMatchGraph<TLeft, TRight, TEdgeValue> dgm_plus, dgm_minus;

    TLeft *left1, *left2;
    TRight *right;
    list<Edge> edges;

    //# Step 1
    // if len(graph) == 0:
    //    return
    if (graph.empty()) {
        return result;
    }

    //# Step 2
    //# Find a circle in the directed matching graph
    //# Note that this circle alternates between nodes from the left and the
    // right part of the graph
    // raw_cycle = directed_match_graph.find_cycle()
    //
    // if raw_cycle:
    //    # Make sure the circle "starts"" in the the left part
    //    # If not, start the circle from the second node, which is in the
    //    left part
    //    if raw_cycle[0][0] != LEFT:
    //        cycle = tuple([raw_cycle[-1][1]] + list(x[1] for x in
    //        raw_cycle[:-1]))
    //    else:
    //        cycle = tuple(x[1] for x in raw_cycle)
    // step3();
    // else: step8()
    //
    NodeList &raw_cycle = directed_match_graph.find_cycle();
    if (!raw_cycle.empty()) {
        if (get<0>(raw_cycle[0]) != LEFT) {
            cycle = NodeList();
            cycle.push_back(cycle.back());
            cycle.insert(cycle.end(), raw_cycle.begin() + 1, raw_cycle.end());
        } else {
            cycle = NodeList(raw_cycle.begin() + 1, raw_cycle.end());
        }

        //# Step 3 - TODO: Properly find right edge? (to get complexity bound)
        // edge = cast(Edge, cycle[:2])
        edge = make_tuple(cycle[0], cycle[1]);

        //# Step 4
        //# already done because we are not really finding the optimal edge

        //# Step 5
        //# Construct new matching M' by flipping edges along the cycle, i.e.
        // change the direction of all the
        //# edges in the circle
        // new_match = matching.copy()
        // for i in range(0, len(cycle), 2):
        //    new_match[cycle[i]] = cycle[i - 1]  # type: ignore
        //
        // yield new_match
        //
        //# Construct G+(e) and G-(e)
        // old_value = graph[edge]
        // del graph[edge]
        new_match = matching;
        for (int i = 0; i < cycle.size(); i += 2) {
            new_match[(TLeft)cycle[i]] = cycle[i - 1];
        }
        result.push_back(new_match);
        old_value = graph._graph.at(edge);
        graph.__delitem__(edge);
        //# Step 7
        //# Recurse with the new matching M' but without the edge e
        // directed_match_graph_minus = _DirectedMatchGraph(graph, new_match)
        //
        // yield from _enum_maximum_matchings_iter(graph, new_match,
        // directed_match_graph_minus)
        //
        // graph[edge] = old_value
        directed_match_graph_minus
            = _DirectedMatchGraph<TLeft, TRight, TEdgeValue>(graph, new_match);
        generator<map<TLeft, TRight>> iter_enum
            = _enum_maximum_matchings_iter<TLeft, TRight, TEdgeValue>(
                graph, new_match, directed_match_graph_minus);

        while (true) {
            shared_ptr<TEdgeValue> v = iter_enum.next();
            if (v == NULL) {
                break;
            }
            result.push_back(*v);
            return result;
        }
        //# Step 6
        //# Recurse with the old matching M but without the edge e

        // graph_plus = graph
        graph_plus = graph;

        // edges = []
        edges = list<Edge>();

        // for left, right in list(graph_plus.edges()):
        //    if left == edge[0] or right == edge[1]:
        //        edges.append((left, right, graph_plus[left, right]))
        //        del graph_plus[left, right]
        for (const pair<TLeft, TRight> &p : graph_plus._edges) {
            TLeft &left = p.first;
            TRight &right = p.second;
            Edge lredge = make_tuple(left, right);
            if ((left == get<0>(edge)) && (right == get<1>(edge))) {
                edges.push_back(
                    Edge(left, right, graph_plus.__getitem__(lredge)));
                graph_plus.__delitem__(lredge);
            }
        }
        // directed_match_graph_plus = _DirectedMatchGraph(graph_plus, matching)
        directed_match_graph_plus
            = _DirectedMatchGraph<TLeft, TRight, TEdgeValue>(graph_plus,
                                                             matching);
        // yield from _enum_maximum_matchings_iter(graph_plus, matching,
        // directed_match_graph_plus)
        iter_enum = _enum_maximum_matchings_iter<TLeft, TRight, TEdgeValue>(
            graph_plus, matching, directed_match_graph_plus);

        while (true) {
            shared_ptr<TEdgeValue> v = iter_enum.next();
            if (v == NULL) {
                break;
            }
            result.push_back(*v);
            return result;
        }

        // for left, right, value in edges:
        //    graph_plus[left, right] = value
        for (const tuple<TLeft, TRight, TEdgeValue> &p : edges) {
            Node node(get<0>(p), get<1>(p));
            graph_plus[node] = get<2>(p);
        }
    } else {
        //# Step 8
        //# Find feasible path of length 2 in D(graph, matching)
        //# This path has the form left1 -> right -> left2
        //# left1 must be in the left part of the graph and in matching
        //# right must be in the right part of the graph
        //# left2 is also in the left part of the graph and but must not be in
        // matching
        // left1 = None  # type: TLeft
        // left2 = None  # type: TLeft
        // right = None  # type: TRight
        //
        left1 = nullptr;
        left2 = nullptr;
        right = nullptr;
        // for part1, node1 in directed_match_graph:
        //    if part1 == LEFT and node1 in matching:
        //        left1 = cast(TLeft, node1)
        //        right = matching[left1]
        //        if (RIGHT, right) in directed_match_graph:
        //            for _, node2 in directed_match_graph[(RIGHT, right)]:
        //                if node2 not in matching:
        //                    left2 = cast(TLeft, node2)
        //                    break
        //            if left2 is not None:
        //                break
        for (pair<int, Node> &p : directed_match_graph._map) {
            int part1 = p.first;
            Node node1 = p.second;
            if ((part1 == LEFT)
                && (matching.find((TLeft)node1) != matching.end())) {
                left1 = (TLeft *)&node1;
                right = &matching[*left1];
                if (directed_match_graph._map.find(make_tuple(RIGHT, right))
                    != directed_match_graph._map.end()) {
                    for (pair<int, Node> &p2 : directed_match_graph._map.at(
                             make_tuple(RIGHT, right))) {
                        Node node2 = p2.second;
                        if (matching.find((TLeft)node2) == matching.end()) {
                            left2 = (TLeft)node2;
                            break;
                        }
                        if (left2 != nullptr) {
                            break;
                        }
                    }
                }
            }
        }
        // if left2 is None:
        //    return
        if (left2 == nullptr) {
            return result;
        }
        //# Construct M'
        //# Exchange the direction of the path left1 -> right -> left2
        //# to left1 <- right <- left2 in the new matching
        // new_match = matching.copy()
        // del new_match[left1]
        // new_match[left2] = right
        new_match = matching;
        new_match.erase(*left1);
        new_match[*left2] = *right;

        // yield new_match
        result.push_back(new_match);

        // edge = (left2, right)
        edge = make_tuple(*left2, *right);

        //# Construct G+(e) and G-(e)
        graph_plus = graph.without_nodes(edge);
        graph_minus = graph.without_edge(edge);

        dgm_plus = _DirectedMatchGraph<TLeft, TRight, TEdgeValue>(graph_plus,
                                                                  new_match);
        dgm_minus = _DirectedMatchGraph<TLeft, TRight, TEdgeValue>(graph_minus,
                                                                   matching);

        //# Step 9
        // yield from _enum_maximum_matchings_iter(graph_plus, new_match,
        // dgm_plus)
        generator<map<TLeft, TRight>> iter_enum
            = _enum_maximum_matchings_iter<TLeft, TRight, TEdgeValue>(
                graph_plus, new_match, dgm_plus);

        while (true) {
            shared_ptr<TEdgeValue> v = iter_enum.next();
            if (v == NULL) {
                break;
            }
            result.push_back(*v);
        }

        //# Step 10
        // yield from _enum_maximum_matchings_iter(graph_minus, matching,
        // dgm_minus)
        iter_enum = _enum_maximum_matchings_iter<TLeft, TRight, TEdgeValue>(
            graph_minus, matching, dgm_minus);

        while (true) {
            shared_ptr<TEdgeValue> v = iter_enum.next();
            if (v == NULL) {
                break;
            }
            result.push_back(*v);
        }
    }
    return result;
}

template <typename TLeft, typename TRight, typename TEdgeValue>
generator<map<TLeft, TRight>>
enum_maximum_matchings_iter(BipartiteGraph<TLeft, TRight, TEdgeValue> graph)
{
    vector<map<TLeft, TRight>> result;
    Matching matching = graph.find_matching();
    if (matching) {
        result.push_back(matching);
        graph = graph.__copy__();
        vector<map<TLeft, TRight>> extension = _enum_maximum_matchings_iter(
            graph, matching,
            _DirectedMatchGraph<TLeft, TRight, TEdgeValue>(graph, matching));
        result.insert(extension.begin(), extension.end());
    }
}

generator<map<string, multiset<int>>>
_commutative_single_variable_partiton_iter(multiset<RCP<const Basic>> values,
                                           VariableWithCount variable)
{
    string name = variable.name;
    int count = variable.count;
    int minimum = variable.minimum;
    RCP<const Basic> defaultv = variable.defaultv;

    map<string, multiset<int>> result;

    if (values.size() == 0 && defaultv != None) {
        result[name] = defaultv;
        return result;
    }
    if (count == 1) {
        if (values.size() >= minimum) {
            if (name != None) {
                result[name] = values;
            }
        }
    } else {
        multiset<int> new_values;
        for (const pair<RCP<const Basic>, int> &p : count_multiset(values)) {
            RCP<const Basic> element = p.first;
            int element_count = p.second;
            if (element_count % count != 0) {
                return map<string, multiset<int>>();
            }
            new_values[element] = element_count; // count
        }
        if (new_values.size() >= minimum) {
            if (name != None) {
                result[name] = new_values;
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

generator<Substitution> commutative_sequence_variable_partition_iter(
    multiset<RCP<const Basic>> values, vector<VariableWithCount> variables)
{

    if (variables.size() == 1) {
        return _commutative_single_variable_partiton_iter(values, variables[0]);
    }

    vector<function<void(Substitution)>> generators;
    for (const pair<RCP<const Basic>, int> &p : values) {
        RCP<const Basic> value = p.first;
        int count = p.second;
        generators.push_back(
            _make_variable_generator_factory(value, count, variables));
    }

    map<string, multiset<int>> initial;
    for (VariableWithCount &var : variables) {
        initial[var] = multiset<int>();
    }
    bool valid;
    vector<map<string, multiset<int>>> result;
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

#endif /* SYMENGINE_UTILITIES_MATCHPYCPP_UTIL_H_ */
