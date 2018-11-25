#ifndef MATCHPY_BIPARTITE_H
#define MATCHPY_BIPARTITE_H

#include <deque>
#include <map>
#include <set>
#include <symengine/basic.h>
#include <symengine/pow.h>
#include <symengine/add.h>

#include "common.h"

using namespace std;

typedef tuple<int, int> TItem; // either TLeft or TRight, use <variant> instead?
typedef TItem TRight;
typedef TItem TLeft;


const int LEFT = 0;
const int RIGHT = 1;

typedef map<tuple<int, int>, tuple<int, int> > Matching;

//(Generic[TLeft, TRight, TEdgeValue], MutableMapping[Tuple[TLeft, TRight], TEdgeValue])
/*
 * A bipartite graph representation.
 */
template<typename TEdgeValue>
class BipartiteGraph {
public:

	//Node = Tuple[int, Union[TLeft, TRight]]
	// typedef tuple<int, variant<TLeft, TRight>> Node; ???
	typedef tuple<int, TItem> Node;
	typedef vector<Node> NodeList;
	typedef set<Node> NodeSet;
	typedef tuple<TLeft, TRight> Edge;

    map<Edge, TEdgeValue> _edges;
    map<int, int> _matching;
    vector<int> _dfs_paths;
    map<int, int> _dfs_parent;
    set<TLeft> _left;
    set<TRight> _right;
    map<Node, set<Node>> _graph;

    BipartiteGraph() {}

    void __setitem__(Edge key, TEdgeValue value) {
//        if not isinstance(key, tuple) or len(key) != 2:
//            raise TypeError("The edge must be a 2-tuple")
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

    TEdgeValue &__getitem__(Edge key) {
        return _edges[key];
    }

    void __delitem__(Edge key) {
        _edges.erase(key);
        //if all(l != key[0] for (l, _) in self._edges):
        //    self._left.remove(key[0])
        for (const pair<Edge, TEdgeValue> &p : _edges) {
            TLeft l = get<0>(p.first);
            if (l == get<0>(key)) {
                _left.erase(get<0>(key));
                break;
            }
        }
        //if all(r != key[1] for (_, r) in self._edges):
        //    self._right.remove(key[1])
        for (const pair<Edge, TEdgeValue> &p : _edges) {
            TRight l = get<1>(p.first);
            if (l == get<1>(key)) {
                _right.erase(get<0>(key));
                break;
            }
        }
        //self._graph[(LEFT, key[0])].remove((RIGHT, key[1]))
        _graph[make_tuple(LEFT, get<0>(key))].erase(make_tuple(RIGHT, get<1>(key)));
        //self._graph[(RIGHT, key[1])].remove((LEFT, key[0]))
        _graph[make_tuple(RIGHT, get<1>(key))].erase(make_tuple(LEFT, get<0>(key)));
    }

//    def edges_with_labels(self):
//        """Returns a view on the edges with labels."""
//        return self._edges.items()

//    def edges(self):
//        return self._edges.keys()

    void clear() {
        _edges.clear();
        _left.clear();
        _right.clear();
        _graph.clear();
    }

};


template<typename TEdgeType>
class HopcroftKarp {
public:

	HopcroftKarp(BipartiteGraph<TEdgeType> &bipartite) : bipartite(bipartite) {
		reference_distance = -1;
	}

	int hopcroft_karp() {
		pair_left.clear();
		pair_right.clear();
		dist_left.clear();
		vertex_queue.clear();
		int matching = 0;
		while (true) {
			if (!_bfs_hopcroft_karp())
				break;
			for (const TLeft &left : bipartite._left) {
				if ((pair_left.find(left) == pair_left.end()) && _dfs_hopcroft_karp(left)) {
					matching++;
				}
			}
		}
		return matching;
	}

	void dump_state() {
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

	bool _bfs_hopcroft_karp() {
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
			for (const tuple<int, TRight> &p : bipartite._graph[make_tuple(LEFT, left_vertex)]) {
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

	bool _dfs_hopcroft_karp(const TLeft &left) {
		for (const tuple<int, TRight> &p : bipartite._graph[make_tuple(LEFT, left)]) {
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

int get0(tuple<int, int> a) {
	return get<0>(a);
}
int get1(tuple<int, int> a) {
	return get<1>(a);
}

#endif
