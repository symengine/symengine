#ifndef SYMENGINE_UTILITIES_MATCHPYCPP_HOPCROFT_KARP_H_
#define SYMENGINE_UTILITIES_MATCHPYCPP_HOPCROFT_KARP_H_

#include <vector>
#include <map>
#include <iostream>
#include <tuple>
#include <deque>
#include <set>

using namespace std;

template <typename TLeft, typename TRight>
class HopcroftKarp
{
public:
    HopcroftKarp(set<TLeft> _left, map<TLeft, set<TRight>> _graph_left)
        : _left(_left), _graph_left(_graph_left)
    {
        reference_distance = -1;
    }

    HopcroftKarp(map<TLeft, set<TRight>> _graph_left) : _graph_left(_graph_left)
    {
        reference_distance = -1;
        _left = get_set(_graph_left);
    }

    int hopcroft_karp()
    {
        pair_left.clear();
        pair_right.clear();
        dist_left.clear();
        vertex_queue.clear();
        int matchings = 0;
        while (true) {
            if (!_bfs_hopcroft_karp())
                break;
            for (const TLeft &left : _left) {
                if ((pair_left.find(left) == pair_left.end())
                    && _dfs_hopcroft_karp(left)) {
                    matchings++;
                }
            }
        }
        return matchings;
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

    map<TLeft, TRight> pair_left;
    map<TRight, TLeft> pair_right;

private:
    set<TLeft> _left;
    map<TLeft, set<TRight>> _graph_left;
    deque<TLeft> vertex_queue;
    map<TLeft, int> dist_left;
    int reference_distance;

    set<TLeft> get_set(map<TLeft, set<TRight>> m)
    {
        set<TLeft> s;
        for (const pair<TLeft, set<TRight>> &p : m) {
            s.insert(p.first);
        }
        return s;
    }

    bool _bfs_hopcroft_karp()
    {
        for (const TLeft &left_vert : _left) {
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
            for (const TRight &right_vertex : _graph_left[left_vertex]) {
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
        for (const TRight &right : _graph_left[left]) {
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

#endif /* SYMENGINE_UTILITIES_MATCHPYCPP_HOPCROFT_KARP_H_ */
