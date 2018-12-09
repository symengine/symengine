#include "bipartite.h"

void test_hopcroft_karp_bipartite1()
{
    BipartiteGraph<tuple<int, int>, tuple<int, int>, string> bipartite;

    bipartite.__setitem__(make_tuple(make_tuple(LEFT, 0), make_tuple(RIGHT, 0)),
                          "X");
    bipartite.__setitem__(make_tuple(make_tuple(LEFT, 0), make_tuple(RIGHT, 1)),
                          "X");
    bipartite.__setitem__(make_tuple(make_tuple(LEFT, 1), make_tuple(RIGHT, 0)),
                          "X");
    bipartite.__setitem__(make_tuple(make_tuple(LEFT, 1), make_tuple(RIGHT, 4)),
                          "X");
    bipartite.__setitem__(make_tuple(make_tuple(LEFT, 2), make_tuple(RIGHT, 2)),
                          "X");
    bipartite.__setitem__(make_tuple(make_tuple(LEFT, 2), make_tuple(RIGHT, 3)),
                          "X");
    bipartite.__setitem__(make_tuple(make_tuple(LEFT, 3), make_tuple(RIGHT, 0)),
                          "X");
    bipartite.__setitem__(make_tuple(make_tuple(LEFT, 3), make_tuple(RIGHT, 4)),
                          "X");
    bipartite.__setitem__(make_tuple(make_tuple(LEFT, 4), make_tuple(RIGHT, 1)),
                          "X");
    bipartite.__setitem__(make_tuple(make_tuple(LEFT, 4), make_tuple(RIGHT, 3)),
                          "X");

    // TODO: turn tuples into ints
    HopcroftKarp<tuple<int, int>, tuple<int, int>, string> hk(bipartite);
    cout << "Maximum matchings: " << hk.hopcroft_karp() << endl;
    hk.dump_state();
}

void test_hopcroft_karp_bipartite2()
{
    BipartiteGraph<tuple<int, int>, tuple<int, int>, string> bipartite;

    bipartite.__setitem__(
        make_tuple(make_tuple(LEFT, 'a' - 'a'), make_tuple(RIGHT, 1)), "X");
    bipartite.__setitem__(
        make_tuple(make_tuple(LEFT, 'b' - 'a'), make_tuple(RIGHT, 1)), "X");
    bipartite.__setitem__(
        make_tuple(make_tuple(LEFT, 'b' - 'a'), make_tuple(RIGHT, 2)), "X");
    bipartite.__setitem__(
        make_tuple(make_tuple(LEFT, 'c' - 'a'), make_tuple(RIGHT, 1)), "X");
    bipartite.__setitem__(
        make_tuple(make_tuple(LEFT, 'c' - 'a'), make_tuple(RIGHT, 2)), "X");
    bipartite.__setitem__(
        make_tuple(make_tuple(LEFT, 'd' - 'a'), make_tuple(RIGHT, 2)), "X");
    bipartite.__setitem__(
        make_tuple(make_tuple(LEFT, 'd' - 'a'), make_tuple(RIGHT, 3)), "X");
    bipartite.__setitem__(
        make_tuple(make_tuple(LEFT, 'd' - 'a'), make_tuple(RIGHT, 4)), "X");
    bipartite.__setitem__(
        make_tuple(make_tuple(LEFT, 'e' - 'a'), make_tuple(RIGHT, 3)), "X");
    bipartite.__setitem__(
        make_tuple(make_tuple(LEFT, 'e' - 'a'), make_tuple(RIGHT, 4)), "X");
    bipartite.__setitem__(
        make_tuple(make_tuple(LEFT, 'f' - 'a'), make_tuple(RIGHT, 4)), "X");
    bipartite.__setitem__(
        make_tuple(make_tuple(LEFT, 'f' - 'a'), make_tuple(RIGHT, 5)), "X");
    bipartite.__setitem__(
        make_tuple(make_tuple(LEFT, 'f' - 'a'), make_tuple(RIGHT, 6)), "X");
    bipartite.__setitem__(
        make_tuple(make_tuple(LEFT, 'g' - 'a'), make_tuple(RIGHT, 5)), "X");
    bipartite.__setitem__(
        make_tuple(make_tuple(LEFT, 'g' - 'a'), make_tuple(RIGHT, 6)), "X");
    bipartite.__setitem__(
        make_tuple(make_tuple(LEFT, 'g' - 'a'), make_tuple(RIGHT, 7)), "X");
    bipartite.__setitem__(
        make_tuple(make_tuple(LEFT, 'h' - 'a'), make_tuple(RIGHT, 8)), "X");

    HopcroftKarp<tuple<int, int>, tuple<int, int>, string> hk(bipartite);
    cout << "Maximum matchings: " << hk.hopcroft_karp() << endl;
    hk.dump_state();
}

/*
@pytest.mark.parametrize(
    '   graph,                      expected_cycle',
    [
        ({},                        []),
        ({0: {1}},                  []),
        ({0: {1}, 1: {2}},          []),
        ({0: {1}, 1: {0}},          [0, 1]),
        ({0: {1}, 1: {0}},          [1, 0]),
        ({0: {1}, 1: {0, 2}},       [0, 1]),
        ({0: {1, 2}, 1: {0, 2}},    [0, 1]),
        ({0: {1, 2}, 1: {0}},       [0, 1]),
        ({0: {1}, 1: {2}, 2: {0}},  [0, 1, 2]),
        ({0: {2}, 1: {2}},          []),
        ({0: {2}, 1: {2}, 2: {0}},  [0, 2]),
        ({0: {2}, 1: {2}, 2: {1}},  [1, 2]),
    ]
)  # yapf: disable
def test_directed_graph_find_cycle(graph, expected_cycle):
    dmg = _DirectedMatchGraph({}, {})
    dmg.update(graph)
    cycle = dmg.find_cycle()
    if len(expected_cycle) > 0:
        assert expected_cycle[0] in cycle
        start = cycle.index(expected_cycle[0])
        cycle = cycle[start:] + cycle[:start]
    assert cycle == expected_cycle
*/

template<typename T1, typename T2>
void test_directed_graph_find_cycle(map<T1, set<T2>> graph, vector<int> expected_cycle) {
	_DirectedMatchGraph<T1, T2, int> dmg;
	for (const pair<T1, set<T2>> &p : graph) {
		dmg._map_left[p.first] = p.second;
	}
	vector<variant<T1, T2>> node_list_v = dmg.find_cycle();
	vector<T1> node_list;
	for (auto &i : node_list_v) {
		node_list.push_back(get<0>(i));
	}
	cout << "Size dmg: " << dmg._map_left.size() << endl;
	cout << "Size graph: " << graph.size() << endl;
	cout << "Size: " << node_list.size() << endl;
	for (auto &i : node_list) {
		cout << i << " ";
	}
	cout << endl;
}

void run_test_directed_graph_find_cycle() {
	map<int, set<double>> graph;
	vector<int> expected_cycle;
	set<double> node_set;
        //({0: {1}, 1: {2}, 2: {0}},  [0, 1, 2]),
	node_set.insert(1);
	graph[0] = node_set;
	node_set.clear();
	node_set.insert(2);
	graph[1] = node_set;
	node_set.clear();
	node_set.insert(0);
	graph[2] = node_set;
	node_set.clear();
	for (auto &i : graph) {
		cout << "set size " << i.second.size() << endl;
		for (auto &j : i.second)
			cout << j << " ";
		cout << endl;
	}
	test_directed_graph_find_cycle<int, double>(graph, expected_cycle);
}

int main(int argc, char **argv)
{
    cout << "Test BipartiteGraph\n";

    test_hopcroft_karp_bipartite1();
    test_hopcroft_karp_bipartite2();
    run_test_directed_graph_find_cycle();
}

