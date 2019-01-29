#ifndef MATCHPY_BIPARTITE_H
#define MATCHPY_BIPARTITE_H

#include <deque>
#include <map>
#include <set>
#include <variant>
#include <any>

#include <symengine/basic.h>
#include <symengine/pow.h>
#include <symengine/add.h>

#include <symengine/utilities/matchpycpp/hopcroft_karp.h>

#include "common.h"
#include "generator_trick.h"

using namespace std;

// typedef map<int, int> Matching;

//(Generic[TLeft, TRight, TEdgeValue], MutableMapping[Tuple[TLeft, TRight],
// TEdgeValue])
/*
 * A bipartite graph representation.
 *
 * Only TLeft = int, TRight = int are supported.
 */
template <typename TLeft, typename TRight, typename TEdgeValue>
class BipartiteGraph
{
public:
    TYPES_DERIVED_FROM_TLEFT_TRIGHT

    map<Edge, TEdgeValue> _edges;
    map<TLeft, TRight> _matching;
    vector<int> _dfs_paths;
    map<int, int> _dfs_parent;
    set<TLeft> _left;
    set<TRight> _right;
    // map<Node, set<Node>> _graph;
    map<TLeft, set<TRight>> _graph_left;
    map<TRight, set<TLeft>> _graph_right;

    BipartiteGraph()
    {
    }

    BipartiteGraph(map<Edge, TEdgeValue> edges)
    {
        _edges = edges;
        for (const pair<Edge, TEdgeValue> &p : edges) {
            TLeft nl = get<0>(p.first);
            TRight nr = get<1>(p.first);
            _left.insert(nl);
            _right.insert(nr);
            _graph_left[nl].insert(nr);
            _graph_right[nr].insert(nl);
        }
    }

    void __setitem__(Edge key, TEdgeValue value)
    {
        // if not isinstance(key, tuple) or len(key) != 2:
        //    raise TypeError("The edge must be a 2-tuple")
        _edges[key] = value;
        _left.insert(get<0>(key));
        _right.insert(get<1>(key));

        //_graph.setdefault((LEFT, key[0]), set()).add((RIGHT, key[1]));
        TLeft k1 = get<0>(key);
        if (_graph_left.find(k1) == _graph_left.end()) {
            _graph_left[k1] = set<TRight>();
        }
        _graph_left[k1].insert(get<1>(key));

        //_graph.setdefault((RIGHT, key[1]), set()).add((LEFT, key[0]));
        TRight k2 = get<1>(key);
        if (_graph_right.find(k2) == _graph_right.end()) {
            _graph_right[k2] = set<TLeft>();
        }
        _graph_right[k2].insert(get<0>(key));
    }

    TEdgeValue setdefault(Edge key, TEdgeValue value)
    {
        if (_edges.find(key) != _edges.end()) {
            return _edges[key];
        } else {
            __setitem__(key, value);
            return value;
        }
    }

    TEdgeValue &__getitem__(Edge key)
    {
        return _edges[key];
    }

    void __delitem__(Edge key)
    {
        _edges.erase(key);
        // if all(l != key[0] for (l, _) in self._edges):
        //    self._left.remove(key[0])
        for (const pair<Edge, TEdgeValue> &p : _edges) {
            TLeft l = get<0>(p.first);
            if (l == get<0>(key)) {
                _left.erase(get<0>(key));
                break;
            }
        }
        // if all(r != key[1] for (_, r) in self._edges):
        //    self._right.remove(key[1])
        for (const pair<Edge, TEdgeValue> &p : _edges) {
            TRight l = get<1>(p.first);
            if (l == get<1>(key)) {
                _right.erase(get<0>(key));
                break;
            }
        }
        // self._graph[(LEFT, key[0])].remove((RIGHT, key[1]))
        _graph_right[get<0>(key)].erase(get<1>(key));
        // self._graph[(RIGHT, key[1])].remove((LEFT, key[0]))
        _graph_left[get<1>(key)].erase(get<0>(key));
    }

    // def without_nodes(self, edge: Edge) -> 'BipartiteGraph[TLeft, TRight,
    // TEdgeValue]':
    //    """Returns a copy of this bipartite graph with the given edge and its
    //    adjacent nodes removed."""
    //    return BipartiteGraph(((n1, n2), v) for (n1, n2), v in
    //    self._edges.items() if n1 != edge[0] and n2 != edge[1])
    BipartiteGraph<TLeft, TRight, TEdgeValue> without_nodes(Edge &edge)
    {
        //((n1, n2), v) for (n1, n2), v in self._edges.items() if n1 != edge[0]
        // and n2 != edge[1])
        BipartiteGraph<TLeft, TRight, TEdgeValue> new_graph;
        for (const pair<Edge, TEdgeValue> &p : _edges) {
            Edge node = p.first;
            TEdgeValue v = p.second;
            TLeft &n1 = get<0>(node);
            TRight &n2 = get<1>(node);
            if ((n1 == get<0>(edge)) || (n2 == get<1>(edge))) {
                continue;
            }
            new_graph.__setitem__(node, v);
        }
        return new_graph;
    }

    // def without_edge(self, edge: Edge) -> 'BipartiteGraph[TLeft, TRight,
    // TEdgeValue]':
    //    """Returns a copy of this bipartite graph with the given edge
    //    removed."""
    //    return BipartiteGraph((e2, v) for e2, v in self._edges.items() if edge
    //    != e2)
    BipartiteGraph<TLeft, TRight, TEdgeValue> without_edge(Edge &edge)
    {
        //((e2, v) for e2, v in self._edges.items() if edge != e2)
        BipartiteGraph<TLeft, TRight, TEdgeValue> new_graph;
        for (const pair<Edge, TEdgeValue> &p : _edges) {
            Edge e2 = p.first;
            TEdgeValue v = p.second;
            if (edge == e2) {
                continue;
            }
            new_graph.__setitem__(e2, v);
        }
        return new_graph;
    }

    // def edges_with_labels(self):
    //    """Returns a view on the edges with labels."""
    //    return self._edges.items()

    // def edges(self):
    //    return self._edges.keys()

    map<TLeft, TRight> find_matching()
    {
        map<TLeft, set<TRight>> directed_graph;

        for (const pair<Edge, TEdgeValue> &p : _edges) {
            TLeft left = get<0>(p.first);
            TRight right = get<1>(p.first);
            if (directed_graph.find(left) == directed_graph.end()) {
                directed_graph[left] = {right};
            } else {
                directed_graph[left].insert(right);
            }
        }

        HopcroftKarp<TLeft, TRight> hk(directed_graph);
        int number_matchings = hk.hopcroft_karp();

        return hk.pair_left;
    }

    void clear()
    {
        _edges.clear();
        _left.clear();
        _right.clear();
        _graph_left.clear();
        _graph_right.clear();
    }
};

int get0(tuple<int, int> a)
{
    return get<0>(a);
}
int get1(tuple<int, int> a)
{
    return get<1>(a);
}

template <typename TLeft, typename TRight>
class _DirectedMatchGraph
{
public:
    TYPES_DERIVED_FROM_TLEFT_TRIGHT

    map<Node, NodeSet> _map;

    _DirectedMatchGraph()
    {
    }

    template <typename TEdgeValue>
    _DirectedMatchGraph(BipartiteGraph<TLeft, TRight, TEdgeValue> graph,
                        map<TLeft, TRight> matching)
    {
        for (const pair<Edge, TEdgeValue> &p : graph._edges) {
            TLeft tail = get<0>(p.first);
            TRight head = get<1>(p.first);
            if ((matching.find(tail) != matching.end())
                && (matching.at(tail) == head)) {
                set<tuple<int, TRight>> s;
                s.insert(make_tuple(RIGHT, head));
                _map[make_tuple(LEFT, tail)] = s;
            } else {

                if (_map.find(make_tuple(RIGHT, head)) == _map.end()) {
                    _map[make_tuple(RIGHT, head)] = NodeSet();
                }
                _map[make_tuple(RIGHT, head)].insert(make_tuple(LEFT, tail));
            }
        }
    }

    NodeList find_cycle()
    {
        set<Node> visited;
        for (const pair<Node, NodeSet> &n : _map) {
            NodeList node_list;
            NodeList cycle;
            cycle = _find_cycle(n.first, node_list, visited);
            if (cycle.size() > 0) {
                return cycle;
            }
        }
        return NodeList();
    }

    NodeList _find_cycle(const Node &node, NodeList &path, set<Node> &visited)
    {
        if (visited.find(node) != visited.end()) {
            typename NodeList::iterator found_end;
            found_end = find_if(path.begin(), path.end(),
                                [&node](const Node &p) { return p == node; });
            if (found_end != path.end()) {
                return NodeList(path.begin(), found_end + 1);
            } else {
                return NodeList();
            }
        }
        visited.insert(node);
        if (_map.find(node) == _map.end()) {
            return NodeList();
        }
        for (const Node &other : _map[node]) {
            NodeList new_path(path.begin(), path.end());
            new_path.push_back(node);
            NodeList cycle = _find_cycle(other, new_path, visited);
            if (cycle.size() > 0) {
                return cycle;
            }
        }
        return NodeList();
    }
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
generator<map<TLeft, TRight>> _enum_maximum_matchings_iter(
    BipartiteGraph<TLeft, TRight, TEdgeValue> graph,
    map<TLeft, TRight> matching,
    _DirectedMatchGraph<TLeft, TRight> directed_match_graph)
{
    TYPES_DERIVED_FROM_TLEFT_TRIGHT

    vector<map<TLeft, TRight>> result;
    map<TLeft, TRight> new_match;
    _DirectedMatchGraph<TLeft, TRight> directed_match_graph_minus,
        directed_match_graph_plus;

    BipartiteGraph<TLeft, TRight, TEdgeValue> graph_plus, graph_minus;
    _DirectedMatchGraph<TLeft, TRight> dgm_plus, dgm_minus;

    //# Step 1
    // if len(graph) == 0:
    //    return
    if (graph._edges.empty()) {
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
    //
    NodeList raw_cycle = directed_match_graph.find_cycle();
    vector<TLeft> cycle;
    if (!raw_cycle.empty()) {
        if (get<0>(raw_cycle[0]) != LEFT) {
            cycle.push_back(get<1>(*raw_cycle.end()));
            for (size_t i = 1; i < raw_cycle.size(); i++) {
                cycle.push_back(get<1>(raw_cycle[i]));
            }
        } else {
            for (Node &i : raw_cycle) {
                cycle.push_back(get<1>(i));
            }
        }

        //# Step 3 - TODO: Properly find right edge? (to get complexity bound)
        // edge = cast(Edge, cycle[:2])
        Edge edge = make_tuple(cycle[0], cycle[1]);

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
        for (size_t i = 0; i < cycle.size(); i += 2) {
            new_match[(TLeft)cycle[i]] = cycle[i - 1];
        }
        result.push_back(new_match);
        TEdgeValue old_value = graph._edges.at(edge);
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
            = _DirectedMatchGraph<TLeft, TRight>(graph, new_match);
        generator<map<TLeft, TRight>> g = _enum_maximum_matchings_iter(
            graph, new_match, directed_match_graph_minus);
        result.insert(result.end(), g.begin(), g.end());

        graph.__setitem__(edge, old_value);

        //# Step 6
        //# Recurse with the old matching M but without the edge e

        // graph_plus = graph
        graph_plus = graph;

        // edges = []
        vector<tuple<TLeft, TRight, TEdgeValue>> edges;

        // for left, right in list(graph_plus.edges()):
        //    if left == edge[0] or right == edge[1]:
        //        edges.append((left, right, graph_plus[left, right]))
        //        del graph_plus[left, right]
        for (const pair<Edge, TEdgeValue> &p : graph_plus._edges) {
            TLeft left = get<0>(p.first);
            TRight right = get<1>(p.first);
            if ((left == get<0>(edge)) && (right == get<1>(edge))) {
                Edge lredge = make_tuple(left, right);
                edges.push_back(
                    make_tuple(left, right, graph_plus.__getitem__(lredge)));
                graph_plus.__delitem__(lredge);
            }
        }
        // directed_match_graph_plus = _DirectedMatchGraph(graph_plus, matching)
        directed_match_graph_plus
            = _DirectedMatchGraph<TLeft, TRight>(graph_plus,
                                                             matching);
        // yield from _enum_maximum_matchings_iter(graph_plus, matching,
        // directed_match_graph_plus)
        g = _enum_maximum_matchings_iter(graph_plus, matching,
                                         directed_match_graph_plus);

        result.insert(result.end(), g.begin(), g.end());

        // for left, right, value in edges:
        //    graph_plus[left, right] = value
        for (const tuple<TLeft, TRight, TEdgeValue> &p : edges) {
            Edge edge0 = make_tuple(get<0>(p), get<1>(p));
            graph_plus.__setitem__(edge0, get<2>(p));
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
        TLeft left1;
        TLeft *left2 = nullptr;
        TRight right;
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
        for (const pair<Node, NodeSet> &p : directed_match_graph._map) {
            int part1 = get<0>(p.first);
            TLeft node1 = get<1>(p.first);
            if ((part1 == LEFT)
                && (matching.find((TLeft)node1) != matching.end())) {
                left1 = (TLeft)node1;
                right = matching[left1];
                if (directed_match_graph._map.find(make_tuple(RIGHT, right))
                    != directed_match_graph._map.end()) {
                    for (const tuple<int, TLeft> &p2 :
                         directed_match_graph._map.at(
                             make_tuple(RIGHT, right))) {
                        TLeft node2 = get<1>(p2);
                        if (matching.find((TLeft)node2) == matching.end()) {
                            left2 = &node2;
                            break;
                        }
                    }
                    if (left2 != nullptr) {
                        break;
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
        new_match.erase(left1);
        new_match[*left2] = right;

        // yield new_match
        result.push_back(new_match);

        // edge = (left2, right)
        Edge edge = make_tuple(*left2, right);

        //# Construct G+(e) and G-(e)
        graph_plus = graph.without_nodes(edge);
        graph_minus = graph.without_edge(edge);

        dgm_plus = _DirectedMatchGraph<TLeft, TRight>(graph_plus,
                                                                  new_match);
        dgm_minus = _DirectedMatchGraph<TLeft, TRight>(graph_minus,
                                                                   matching);

        //# Step 9
        // yield from _enum_maximum_matchings_iter(graph_plus, new_match,
        // dgm_plus)
        generator<map<TLeft, TRight>> g
            = _enum_maximum_matchings_iter(graph_plus, new_match, dgm_plus);

        result.insert(result.end(), g.begin(), g.end());

        //# Step 10
        // yield from _enum_maximum_matchings_iter(graph_minus, matching,
        // dgm_minus)
        g = _enum_maximum_matchings_iter(graph_minus, matching, dgm_minus);

        result.insert(result.end(), g.begin(), g.end());
    }
    return result;
}

template <typename TLeft, typename TRight, typename TEdgeValue>
generator<map<TLeft, TRight>>
enum_maximum_matchings_iter(BipartiteGraph<TLeft, TRight, TEdgeValue> graph)
{
    vector<map<TLeft, TRight>> result;
    map<TLeft, TRight> matching = graph.find_matching();
    if (matching.size() > 0) {
        result.push_back(matching);
        // graph = graph.__copy__();
        generator<map<TLeft, TRight>> extension = _enum_maximum_matchings_iter(
            graph, matching,
            _DirectedMatchGraph<TLeft, TRight>(graph, matching));
        result.insert(result.end(), extension.begin(), extension.end());
    }
    return result;
}

#endif
