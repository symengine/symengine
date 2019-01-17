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

#include "common.h"
#include "generator_trick.h"

using namespace std;

const int LEFT = 0;
const int RIGHT = 1;

typedef map<tuple<int, int>, tuple<int, int>> Matching;

//(Generic[TLeft, TRight, TEdgeValue], MutableMapping[Tuple[TLeft, TRight],
// TEdgeValue])
/*
 * A bipartite graph representation.
 */
template <typename TLeft, typename TRight, typename TEdgeValue>
class BipartiteGraph
{
public:
    // typedef std::variant<TLeft, TRight> Node;
    // typedef vector<Node> NodeList;
    // typedef set<Node> NodeSet;
    typedef tuple<TLeft, TRight> Edge;
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

    /*
    def __init__(self, *args, **kwargs):
        self._edges = dict(*args, **kwargs)
        self._left = set(l for (l, _) in self._edges.keys())
        self._right = set(r for (_, r) in self._edges.keys())
        self._graph = {}
        for l, r in self._edges:
            self._graph.setdefault((LEFT, l), set()).add((RIGHT, r))
            self._graph.setdefault((RIGHT, r), set()).add((LEFT, l))

        self._matching = {}
        self._dfs_paths = []
        self._dfs_parent = {}
    */
    BipartiteGraph(map<Edge, TEdgeValue> edges)
    {
        _edges = edges;
        for (const pair<Edge, TEdgeValue> &p : edges) {
            _left.insert(p.first);
            _right.insert(p.second);
            _graph_left[p.first].insert(p.second);
            _graph_right[p.second].insert(p.first);
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

    TEdgeValue &setdefault(Edge key, TEdgeValue value)
    {
    	if (_edges.find(key) != _edges.end()){
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
            Edge &node = p.first;
            TEdgeValue &v = p.second;
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
            Edge &e2 = p.first;
            TEdgeValue &v = p.second;
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

    void clear()
    {
        _edges.clear();
        _left.clear();
        _right.clear();
        _graph_left.clear();
        _graph_right.clear();
    }
};

template <typename TLeft, typename TRight, typename TEdgeType>
class HopcroftKarp
{
public:
    HopcroftKarp(BipartiteGraph<TLeft, TRight, TEdgeType> &bipartite)
        : bipartite(bipartite)
    {
        reference_distance = -1;
    }

    int hopcroft_karp()
    {
        pair_left.clear();
        pair_right.clear();
        dist_left.clear();
        vertex_queue.clear();
        int matching = 0;
        while (true) {
            if (!_bfs_hopcroft_karp())
                break;
            for (const TLeft &left : bipartite._left) {
                if ((pair_left.find(left) == pair_left.end())
                    && _dfs_hopcroft_karp(left)) {
                    matching++;
                }
            }
        }
        return matching;
    }

    void dump_state()
    {
        cout << endl;
        for (const pair<TLeft, TRight> &p : pair_left) {
            int l = get<1>(p.first);
            cout << l << " " << get<1>(p.second) << endl;
        }
        cout << endl;
        cout << "Distances:\n";
        for (const pair<TLeft, int> &p : dist_left) {
            cout << "Vertex: " << get<1>(p.first) << " " << p.second << endl;
        }
        return;
    }

private:
    map<TLeft, TRight> pair_left;
    map<TRight, TLeft> pair_right;
    BipartiteGraph<TLeft, TRight, TEdgeType> &bipartite;
    deque<TLeft> vertex_queue;
    map<TLeft, int> dist_left;
    int reference_distance;

    bool _bfs_hopcroft_karp()
    {
        for (const TLeft &left_vert : bipartite._left) {
            if (pair_left.find(left_vert) == pair_left.end()) {
                vertex_queue.push_back(left_vert);
                dist_left[left_vert] = 0;
            } else {
                dist_left.erase(left_vert);
            }
        }
        reference_distance = -1;
        while (true) {
            if (vertex_queue.size() == 0)
                break;
            TLeft &left_vertex = vertex_queue.front();
            vertex_queue.pop_front();
            if (dist_left.find(left_vertex) == dist_left.end())
                continue;
            for (const TRight &right_vertex :
                 bipartite._graph_left[left_vertex]) {
                if (pair_right.find(right_vertex) == pair_right.end()) {
                    if (reference_distance == -1) {
                        reference_distance = dist_left[left_vertex] + 1;
                    }
                } else {
                    TLeft &other_left = pair_right.at(right_vertex);
                    if (dist_left.find(other_left) == dist_left.end()) {
                        dist_left[other_left] = dist_left[left_vertex] + 1;
                        vertex_queue.push_back(other_left);
                    }
                }
            }
        }
        return reference_distance != -1;
    }

    bool _dfs_hopcroft_karp(const TLeft &left)
    {
        for (const TRight &right : bipartite._graph_left[left]) {
            int distance;

            if (pair_right.find(right) == pair_right.end()) {
                distance = reference_distance;
            } else {
                TLeft &other_left = pair_right.at(right);
                if (dist_left.find(other_left) == dist_left.end()) {
                    distance = reference_distance;
                } else {
                    distance = dist_left.at(other_left);
                }
            }
            if (distance == dist_left.at(left) + 1) {
                pair_left[left] = right;
                pair_right[right] = left;
                return true;
            }
        }
        dist_left.erase(left);
        return false;
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
    map<TLeft, set<TRight>> _map_left;
    map<TRight, set<TLeft>> _map_right;

    _DirectedMatchGraph()
    {
    }

    _DirectedMatchGraph(BipartiteGraph<TLeft, TRight, TEdgeValue> graph,
                        map<TLeft, TRight> matching)
    {
        //        for (tail, head) in graph:
        for (const pair<TLeft, TRight> &p : graph._graph) {
            TLeft &tail = get<0>(p);
            TRight &head = get<1>(p);
            // if tail in matching and matching[tail] == head:
            //    self[(LEFT, tail)] = {(RIGHT, head)}
            if ((matching.find(tail) != matching.end())
                && (matching.at(tail) == head)) {
                set<TRight> s;
                s.insert(head);
                _map_left[tail] = s;
            } else {
                // if (RIGHT, head) not in self:
                //    self[(RIGHT, head)] = set()
                // self[(RIGHT, head)].add((LEFT, tail))
                // Node head_node = make_tuple(RIGHT, head);
                if (_map_right.find(head) == _map_right.end()) {
                    _map_right[head] = set<TLeft>();
                }
                // Node tail_node = make_tuple(LEFT, tail);
                _map_right[head].insert(tail);
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

    // typedef vector<any> NodeList;
    typedef vector<pair<TLeft, TRight>> NodeList;

    NodeList find_cycle()
    {
        // set<variant<TLeft, TRight>> visited;
        set<TLeft> visited_left;
        set<TRight> visited_right;
        for (const pair<TLeft, set<TRight>> &n : _map_left) {
            NodeList node_list;
            NodeList cycle;
            cycle = _find_cycle_left(n.first, node_list, visited_left,
                                     visited_right);
            if (cycle.size() > 0) {
                return cycle;
            }
        }
        return NodeList();
    }

    NodeList _find_cycle_left(const TLeft &node, NodeList &path,
                              set<TLeft> &visited_left,
                              set<TRight> &visited_right)
    {
        // if node in visited:
        //    try:
        //        index = path.index(node)
        //        return path[index:]
        //    except ValueError:
        //        return cast(NodeList, [])
        if (visited_left.find(node) != visited_left.end()) {
            typename NodeList::iterator found_end;
            found_end = find_if(path.begin(), path.end(),
                                [&node](const pair<TLeft, TRight> &p) {
                                    return p.first == node;
                                });
            if (found_end != path.end()) {
                return NodeList(path.begin(), found_end + 1);
            } else {
                return NodeList();
            }
        }
        // visited.add(node)
        visited_left.insert(node);
        // if node not in self:
        //    return cast(NodeList, [])
        if (_map_left.find(node) == _map_left.end()) {
            return NodeList();
        }
        // for other in self[node]:
        //    cycle = self._find_cycle(other, path + [node], visited)
        //    if cycle:
        //        return cycle
        for (const TRight &other : _map_left[node]) {
            NodeList new_path(path.begin(), path.end());
            new_path.push_back(make_pair(node, other));
            NodeList cycle = _find_cycle_right(other, new_path, visited_left,
                                               visited_right);
            if (cycle.size() > 0) {
                return cycle;
            }
        }
        return NodeList();
    }

    NodeList _find_cycle_right(const TRight &node, NodeList &path,
                               set<TLeft> &visited_left,
                               set<TRight> &visited_right)
    {
        // if node in visited:
        //    try:
        //        index = path.index(node)
        //        return path[index:]
        //    except ValueError:
        //        return cast(NodeList, [])
        if (visited_right.find(node) != visited_right.end()) {
            typename NodeList::iterator found_end;
            found_end = find_if(path.begin(), path.end(),
                                [&node](const pair<TLeft, TRight> &p) {
                                    return p.second == node;
                                });
            if (found_end != path.end()) {
                return NodeList(path.begin(), found_end + 1);
            } else {
                return NodeList();
            }
        }
        // visited.add(node)
        visited_right.insert(node);
        // if node not in self:
        //    return cast(NodeList, [])
        if (_map_right.find(node) == _map_right.end()) {
            return NodeList();
        }
        // for other in self[node]:
        //    cycle = self._find_cycle(other, path + [node], visited)
        //    if cycle:
        //        return cycle
        for (const TLeft &other : _map_right[node]) {
            NodeList new_path(path.begin(), path.end());
            NodeList cycle = _find_cycle_left(other, new_path, visited_left,
                                              visited_right);
            if (cycle.size() > 0) {
                return cycle;
            }
        }
        // return cast(NodeList, [])
        return NodeList();
    }
};

#endif
