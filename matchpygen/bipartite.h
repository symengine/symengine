#ifndef MATCHPY_BIPARTITE_H
#define MATCHPY_BIPARTITE_H

#include <deque>
#include <map>
#include <set>

#include <symengine/basic.h>
#include <symengine/pow.h>
#include <symengine/add.h>

#include "common.h"
#include "generator_trick.h"

using namespace std;

typedef tuple<int, int> TItem; // either TLeft or TRight, use <variant> instead?
typedef TItem TRight;
typedef TItem TLeft;
// Node = Tuple[int, Union[TLeft, TRight]]
// typedef tuple<int, variant<TLeft, TRight>> Node; ???
typedef tuple<int, TItem> Node;
typedef vector<Node> NodeList;
typedef set<Node> NodeSet;
typedef tuple<TLeft, TRight> Edge;

const int LEFT = 0;
const int RIGHT = 1;

typedef map<tuple<int, int>, tuple<int, int>> Matching;

//(Generic[TLeft, TRight, TEdgeValue], MutableMapping[Tuple[TLeft, TRight],
// TEdgeValue])
/*
 * A bipartite graph representation.
 */
template <typename TEdgeValue>
class BipartiteGraph
{
public:
    map<Edge, TEdgeValue> _edges;
    map<int, int> _matching;
    vector<int> _dfs_paths;
    map<int, int> _dfs_parent;
    set<TLeft> _left;
    set<TRight> _right;
    map<Node, set<Node>> _graph;

    BipartiteGraph()
    {
    }

    void __setitem__(Edge key, TEdgeValue value)
    {
        // if not isinstance(key, tuple) or len(key) != 2:
        //    raise TypeError("The edge must be a 2-tuple")
        _edges[key] = value;
        _left.insert(get<0>(key));
        _right.insert(get<1>(key));

        //_graph.setdefault((LEFT, key[0]), set()).add((RIGHT, key[1]));
        tuple<int, TLeft> k1 = make_tuple(LEFT, get<0>(key));
        if (_graph.find(k1) == _graph.end()) {
            _graph[k1] = set<tuple<int, TRight>>();
        }
        _graph[k1].insert(make_tuple(RIGHT, get<1>(key)));

        //_graph.setdefault((RIGHT, key[1]), set()).add((LEFT, key[0]));
        tuple<int, TRight> k2 = make_tuple(RIGHT, get<1>(key));
        if (_graph.find(k2) == _graph.end()) {
            _graph[k2] = set<tuple<int, TLeft>>();
        }
        _graph[k2].insert(make_tuple(LEFT, get<0>(key)));
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
        _graph[make_tuple(LEFT, get<0>(key))].erase(
            make_tuple(RIGHT, get<1>(key)));
        // self._graph[(RIGHT, key[1])].remove((LEFT, key[0]))
        _graph[make_tuple(RIGHT, get<1>(key))].erase(
            make_tuple(LEFT, get<0>(key)));
    }

    // def without_nodes(self, edge: Edge) -> 'BipartiteGraph[TLeft, TRight,
    // TEdgeValue]':
    //    """Returns a copy of this bipartite graph with the given edge and its
    //    adjacent nodes removed."""
    //    return BipartiteGraph(((n1, n2), v) for (n1, n2), v in
    //    self._edges.items() if n1 != edge[0] and n2 != edge[1])
    BipartiteGraph<TEdgeValue> without_nodes(Edge &edge)
    {
        //((n1, n2), v) for (n1, n2), v in self._edges.items() if n1 != edge[0]
        // and n2 != edge[1])
        BipartiteGraph<TEdgeValue> new_graph;
        for (const pair<Edge, TEdgeValue> &p : _edges) {
            Node &node = p.first;
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
    BipartiteGraph<TEdgeValue> without_edge(Edge &edge)
    {
        //((e2, v) for e2, v in self._edges.items() if edge != e2)
        BipartiteGraph<TEdgeValue> new_graph;
        for (const pair<Edge, TEdgeValue> &p : _edges) {
            Node &e2 = p.first;
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
        _graph.clear();
    }
};

template <typename TEdgeType>
class HopcroftKarp
{
public:
    HopcroftKarp(BipartiteGraph<TEdgeType> &bipartite) : bipartite(bipartite)
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
    BipartiteGraph<TEdgeType> &bipartite;
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
            for (const tuple<int, TRight> &p :
                 bipartite._graph[make_tuple(LEFT, left_vertex)]) {
                if (get<0>(p) == LEFT) {
                    // not a left vertex, continue
                    throw("inconsistent graph");
                }
                const TRight &right_vertex = get<1>(p);
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
        for (const tuple<int, TRight> &p :
             bipartite._graph[make_tuple(LEFT, left)]) {
            const TRight &right = get<1>(p);
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

template <typename TEdgeValue>
class _DirectedMatchGraph
{
public:
    //  map<TLeft, TRight> _map_left;
    //  map<TRight, TLeft> _map_right;
    map<Node, set<Node>> _map;

    _DirectedMatchGraph(BipartiteGraph<TEdgeValue> graph,
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
                _map[make_tuple(LEFT, tail)] = s;
            } else {
                // if (RIGHT, head) not in self:
                //    self[(RIGHT, head)] = set()
                // self[(RIGHT, head)].add((LEFT, tail))
                Node head_node = make_tuple(RIGHT, head);
                if (_map.find(head_node) == _map.end()) {
                    _map[head_node] = set<Node>();
                }
                Node tail_node = make_tuple(LEFT, tail);
                _map[head_node].insert(tail_node);
            }
        }
    }

    // def find_cycle(self) -> NodeList:
    //    visited = cast(NodeSet, set())
    //    for n in self:
    //	cycle = self._find_cycle(n, cast(NodeList, []), visited)
    //	if cycle:
    //	    return cycle
    //    return cast(NodeList, [])
    NodeList find_cycle()
    {
        NodeSet visited = set<Node>();
        for (pair<Node, set<Node>> &n : _map) {
            NodeList node_list;
            NodeList &cycle = _find_cycle(n.first, node_list, visited);
            if (cycle.size() > 0) {
                return cycle;
            }
        }
        return NodeList();
    }

    NodeList _find_cycle(const Node &node, NodeList &path, NodeSet &visited)
    {
        // if node in visited:
        //	try:
        //	    index = path.index(node)
        //	    return path[index:]
        //	except ValueError:
        //	    return cast(NodeList, [])
        if (visited.find(node) != visited.end()) {
            NodeList::iterator &found_end
                = find(path.begin(), path.end(), node);
            if (found_end != path.end()) {
                return NodeList(path.begin(), found_end);
            } else {
                return NodeList();
            }
        }
        // visited.add(node)
        visited.insert(node);
        // if node not in self:
        //	return cast(NodeList, [])
        if (_map.find(node) == _map.end()) {
            return NodeList();
        }
        // for other in self[node]:
        //	cycle = self._find_cycle(other, path + [node], visited)
        //	if cycle:
        //	    return cycle
        for (const Node &other : _map[node]) {
            NodeList new_path(path.begin(), path.end());
            new_path.push_back(node);
            NodeList &cycle = _find_cycle(other, new_path, visited);
            if (cycle.size() > 0) {
                return cycle;
            }
        }
        // return cast(NodeList, [])
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
class _enum_maximum_matchings_iter : public virtual GeneratorTrick<map<TLeft, TRight>>
{
private:
    BipartiteGraph<TEdgeValue> graph;
    map<TLeft, TRight> matching;
    _DirectedMatchGraph<TEdgeValue> directed_match_graph;

public:
    _enum_maximum_matchings_iter(
        BipartiteGraph<TEdgeValue> graph, map<TLeft, TRight> matching,
        _DirectedMatchGraph<TEdgeValue> directed_match_graph)
        : graph(graph), matching(matching),
          directed_match_graph(directed_match_graph)
    {
    }
    virtual ~_enum_maximum_matchings_iter()
    {
    }

private:
    map<TLeft, TRight> new_match;
    Edge edge;
    NodeList cycle;
    _DirectedMatchGraph<TEdgeValue> directed_match_graph_minus,
        directed_match_graph_plus;
    set<Node> old_value;

    BipartiteGraph<TEdgeValue> graph_plus, graph_minus;
    _DirectedMatchGraph<TEdgeValue> dgm_plus, dgm_minus;
    _enum_maximum_matchings_iter<TLeft, TRight, TEdgeValue> iter_enum;

    TLeft *left1, *left2;
    TRight *right;
    list<Edge> edges;

    virtual void start()
    {
        this->current = bind(&_enum_maximum_matchings_iter::step1, this);
    }

    void step1()
    {
        //# Step 1
        // if len(graph) == 0:
        //    return
        if (graph.empty()) {
            this->generator_stop = true;
        } else {
            this->current = bind(&_enum_maximum_matchings_iter::step2, this);
        }
    }

    void step2()
    {
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
                cycle.insert(cycle.end(), raw_cycle.begin() + 1,
                             raw_cycle.end());
            } else {
                cycle = NodeList(raw_cycle.begin() + 1, raw_cycle.end());
            }
            this->current = bind(&_enum_maximum_matchings_iter::step3, this);
        } else {
            this->current = bind(&_enum_maximum_matchings_iter::step8, this);
        }
    }
    void step3()
    {
        //# Step 3 - TODO: Properly find right edge? (to get complexity bound)
        // edge = cast(Edge, cycle[:2])
        edge = make_tuple(cycle[0], cycle[1]);
        this->current = bind(&_enum_maximum_matchings_iter::step4, this);
    }
    void step4()
    {
        //# Step 4
        //# already done because we are not really finding the optimal edge
        this->current = bind(&_enum_maximum_matchings_iter::step5, this);
    }
    void step5()
    {
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
        yield(new_match);
        old_value = graph._graph.at(edge);
        graph.__delitem__(edge);
        this->current = bind(&_enum_maximum_matchings_iter::step7, this);
    }

    void step7()
    {
        //# Step 7
        //# Recurse with the new matching M' but without the edge e
        // directed_match_graph_minus = _DirectedMatchGraph(graph, new_match)
        //
        // yield from _enum_maximum_matchings_iter(graph, new_match,
        // directed_match_graph_minus)
        //
        // graph[edge] = old_value
        directed_match_graph_minus
            = _DirectedMatchGraph<TEdgeValue>(graph, new_match);
        iter_enum = _enum_maximum_matchings_iter<TLeft, TRight, TEdgeValue>(
            graph, new_match, directed_match_graph_minus);
        this->current = bind(&_enum_maximum_matchings_iter::step7part002, this);
    }

    void step7part002()
    {
        while (true) {
            shared_ptr<TEdgeValue> v = iter_enum.next();
            if (v == NULL) {
                break;
            }
            yield(*v);
            return;
        }
        this->current = bind(&_enum_maximum_matchings_iter::step6, this);
    }

    void step6()
    {
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
            = _DirectedMatchGraph<TEdgeValue>(graph_plus, matching);
        // yield from _enum_maximum_matchings_iter(graph_plus, matching,
        // directed_match_graph_plus)
        iter_enum = _enum_maximum_matchings_iter<TLeft, TRight, TEdgeValue>(
            graph_plus, matching, directed_match_graph_plus);
        this->current = bind(&_enum_maximum_matchings_iter::step6part002, this);
    }

    void step6part002()
    {
        while (true) {
            shared_ptr<TEdgeValue> v = iter_enum.next();
            if (v == NULL) {
                break;
            }
            yield(*v);
            return;
        }
        this->current = bind(&_enum_maximum_matchings_iter::step6part003, this);
    }

    void step6part003()
    {
        // for left, right, value in edges:
        //    graph_plus[left, right] = value
        for (const tuple<TLeft, TRight, TEdgeValue> &p : edges) {
            Node node(get<0>(p), get<1>(p));
            graph_plus[node] = get<2>(p);
        }
        this->generator_stop = true;
    }

    void step8()
    {
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
            this->generator_stop = true;
            return;
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
        yield(new_match);

        // edge = (left2, right)
        edge = make_tuple(*left2, *right);

        //# Construct G+(e) and G-(e)
        graph_plus = graph.without_nodes(edge);
        graph_minus = graph.without_edge(edge);

        dgm_plus = _DirectedMatchGraph<TEdgeValue>(graph_plus, new_match);
        dgm_minus = _DirectedMatchGraph<TEdgeValue>(graph_minus, matching);
        this->current = bind(&_enum_maximum_matchings_iter::step9, this);
    }

    void step9()
    {
        //# Step 9
        // yield from _enum_maximum_matchings_iter(graph_plus, new_match,
        // dgm_plus)
        iter_enum = _enum_maximum_matchings_iter<TLeft, TRight, TEdgeValue>(
            graph_plus, new_match, dgm_plus);
        this->current = bind(&_enum_maximum_matchings_iter::step9part002, this);
    }

    void step9part002()
    {
        while (true) {
            shared_ptr<TEdgeValue> v = iter_enum.next();
            if (v == NULL) {
                break;
            }
            yield(*v);
            return;
        }
        this->current = bind(&_enum_maximum_matchings_iter::step10, this);
    }

    void step10()
    {
        //# Step 10
        // yield from _enum_maximum_matchings_iter(graph_minus, matching,
        // dgm_minus)
        iter_enum = _enum_maximum_matchings_iter<TLeft, TRight, TEdgeValue>(
            graph_minus, matching, dgm_minus);
        this->current = bind(&_enum_maximum_matchings_iter::step10part002, this);
    }

    void step10part002()
    {
        while (true) {
            shared_ptr<TEdgeValue> v = iter_enum.next();
            if (v == NULL) {
                break;
            }
            yield(*v);
            return;
        }
        this->generator_stop = true;
    }
};

#endif
