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

//#include <symengine/utilities/matchpycpp/hopcroft_karp.h>

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
        map<tuple<int, TLeft>, set<tuple<int, TRight>>> directed_graph;
        //= {}  # type: Dict[Tuple[int, TLeft], Set[Tuple[int, TRight]]]

        for (const pair<Edge, TEdgeValue> &p : _edges) {
            TLeft left = get<0>(p.first);
            TRight right = get<1>(p.first);
            tuple<int, TLeft> tail = make_tuple(LEFT, left);
            tuple<int, TRight> head = make_tuple(RIGHT, right);
            if (directed_graph.find(tail) == directed_graph.end()) {
                directed_graph[tail] = {head};
            } else {
                directed_graph[tail].insert(head);
            }
        }

        return map<TLeft, TRight>();

        //        HopcroftKarp<TLeft, TRight, TEdgeValue> hk(directed_graph);
        //        int number_matchings = hk.hopcroft_karp();
        //
        //        map<tuple<int, tuple<int, int>>, tuple<int, tuple<int, int>>>
        //        matching;
        //        for (const pair<TLeft, TRight> &p : hk.pair_left) {
        //        	matching[p.first] = p.second;
        //        }
        //        for (const pair<TRight, TLeft> &p : hk.pair_right) {
        //        	matching[p.first] = p.second;
        //        }
        //
        //        // Filter out the partitions (LEFT and RIGHT) and only return
        //        the matching edges
        //        // that go from LEFT to RIGHT
        //        // return dict((tail[1], head[1]) for tail, head in
        //        matching.items() if tail[0] == LEFT);
        //
        //        map<TLeft, TRight> result;
        //        for (const pair<int, tuple<int, int>> &elem : matching) {
        //        	result[elem.first] = elem.second;
        //        }
        //        return result;
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

template <typename TLeft, typename TRight, typename TEdgeValue>
class _DirectedMatchGraph
{
public:
    // map<Node, NodeSet> _map_left;
    // map<Node, NodeSet> _map_right;
    TYPES_DERIVED_FROM_TLEFT_TRIGHT

    map<Node, NodeSet> _map;

    _DirectedMatchGraph()
    {
    }

    _DirectedMatchGraph(BipartiteGraph<TLeft, TRight, TEdgeValue> graph,
                        map<TLeft, TRight> matching)
    {
        /*
        def __init__(self, graph: BipartiteGraph[TLeft, TRight, TEdgeValue],
        matching: Dict[TLeft, TRight]) -> None:
            super(_DirectedMatchGraph, self).__init__()
            for (tail, head) in graph:
                if tail in matching and matching[tail] == head:
                    self[(LEFT, tail)] = {(RIGHT, head)}
                else:
                    if (RIGHT, head) not in self:
                        self[(RIGHT, head)] = set()
                    self[(RIGHT, head)].add((LEFT, tail))
*/
        //        for (tail, head) in graph:
        for (const pair<Edge, TEdgeValue> &p : graph._edges) {
            TLeft tail = get<0>(p.first);
            TRight head = get<1>(p.first);
            // if tail in matching and matching[tail] == head:
            //    self[(LEFT, tail)] = {(RIGHT, head)}
            if ((matching.find(tail) != matching.end())
                && (matching.at(tail) == head)) {
                set<tuple<int, TRight>> s;
                s.insert(make_tuple(RIGHT, head));
                _map[make_tuple(RIGHT, tail)] = s;
            } else {
                // if (RIGHT, head) not in self:
                //    self[(RIGHT, head)] = set()
                // self[(RIGHT, head)].add((LEFT, tail))
                // Node head_node = make_tuple(RIGHT, head);
                if (_map.find(make_tuple(RIGHT, head)) == _map.end()) {
                    _map[make_tuple(RIGHT, head)] = NodeSet();
                }
                // Node tail_node = make_tuple(LEFT, tail);
                _map[make_tuple(RIGHT, head)].insert(make_tuple(LEFT, tail));
            }
        }
    }

    // def find_cycle(self) -> NodeList:
    //    visited = cast(NodeSet, set())
    //    for n in self:
    //    cycle = self._find_cycle(n, cast(NodeList, []), visited)
    //    if cycle:
    //        return cycle
    //    return cast(NodeList, [])

    NodeList find_cycle()
    {
        // set<variant<TLeft, TRight>> visited;
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
        // if node in visited:
        //    try:
        //        index = path.index(node)
        //        return path[index:]
        //    except ValueError:
        //        return cast(NodeList, [])
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
        // visited.add(node)
        visited.insert(node);
        // if node not in self:
        //    return cast(NodeList, [])
        if (_map.find(node) == _map.end()) {
            return NodeList();
        }
        // for other in self[node]:
        //    cycle = self._find_cycle(other, path + [node], visited)
        //    if cycle:
        //        return cycle
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
            _DirectedMatchGraph<TLeft, TRight, TEdgeValue>(graph, matching));
        result.insert(result.end(), extension.begin(), extension.end());
    }
    return result;
}

#endif
