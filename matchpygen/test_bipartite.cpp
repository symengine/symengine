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
template <typename T1, typename T2>
void test_directed_graph_find_cycle(map<T1, set<T2>> graph,
                                    vector<T1> expected_cycle)
{
    _DirectedMatchGraph<T1, T2, int> dmg;
    for (const pair<T1, set<T2>> &p : graph) {
        if (p.first % 2 == 0)
            dmg._map_left[p.first] = p.second;
        else
            dmg._map_right[p.first] = p.second;
    }
    vector<pair<T1, T2>> node_list = dmg.find_cycle();
    //	cout << "Size dmg: " << dmg._map_left.size() << endl;
    //	cout << "Size graph: " << graph.size() << endl;
    //	cout << "Size: " << node_list.size() << endl;
    cout << "Found cycle ";
    for (auto &i : node_list) {
        cout << i.first << " " << i.second << " ";
    }
    cout << endl << "Expected cycle: ";
    for (auto &i : expected_cycle) {
        cout << i << " ";
    }
    cout << endl << endl;
}

void run_test_directed_graph_find_cycle()
{
    map<int, set<int>> graph;
    vector<int> expected_cycle;

    graph = {};
    expected_cycle = {};
    test_directed_graph_find_cycle<int, int>(graph, expected_cycle);
    //	        ({0: {1}},                  []),
    graph = {{0, {1}}};
    expected_cycle = {};
    test_directed_graph_find_cycle<int, int>(graph, expected_cycle);
    //	        ({0: {1}, 1: {2}},          []),
    graph = {{0, {1}}, {1, {2}}};
    expected_cycle = {};
    test_directed_graph_find_cycle<int, int>(graph, expected_cycle);
    //	        ({0: {1}, 1: {0}},          [0, 1]),
    graph = {{0, {1}}, {1, {0}}};
    expected_cycle = {0, 1};
    test_directed_graph_find_cycle<int, int>(graph, expected_cycle);
    //	        ({0: {1}, 1: {0}},          [1, 0]),
    graph = {{0, {1}}, {1, {0}}};
    expected_cycle = {1, 0};
    test_directed_graph_find_cycle<int, int>(graph, expected_cycle);
    //	        ({0: {1}, 1: {0, 2}},       [0, 1]),
    graph = {{0, {1}}, {1, {0, 2}}};
    expected_cycle = {0, 1};
    test_directed_graph_find_cycle<int, int>(graph, expected_cycle);
    //	        ({0: {1, 2}, 1: {0, 2}},    [0, 1]),
    graph = {{0, {1, 2}}, {1, {0, 2}}};
    expected_cycle = {0, 1};
    test_directed_graph_find_cycle<int, int>(graph, expected_cycle);
    //	        ({0: {1, 2}, 1: {0}},       [0, 1]),
    graph = {{0, {1, 2}}, {1, {0}}};
    expected_cycle = {0, 1};
    test_directed_graph_find_cycle<int, int>(graph, expected_cycle);
    //	        ({0: {1}, 1: {2}, 2: {0}},  [0, 1, 2]),
    graph = {{0, {1}}, {1, {2}}, {2, {0}}};
    expected_cycle = {0, 1, 2};
    test_directed_graph_find_cycle<int, int>(graph, expected_cycle);
    //	        ({0: {2}, 1: {2}},          []),
    graph = {{0, {2}}, {1, {2}}};
    expected_cycle = {};
    test_directed_graph_find_cycle<int, int>(graph, expected_cycle);
    //	        ({0: {2}, 1: {2}, 2: {0}},  [0, 2]),
    graph = {{0, {2}}, {1, {2}}, {2, {0}}};
    expected_cycle = {0, 2};
    test_directed_graph_find_cycle<int, int>(graph, expected_cycle);
    //	        ({0: {2}, 1: {2}, 2: {1}},  [1, 2]),
    graph = {{0, {2}}, {1, {2}}, {2, {1}}};
    expected_cycle = {1, 2};
    test_directed_graph_find_cycle<int, int>(graph, expected_cycle);
}

int main(int argc, char **argv)
{
    cout << "Test BipartiteGraph\n";

    test_hopcroft_karp_bipartite1();
    test_hopcroft_karp_bipartite2();
    run_test_directed_graph_find_cycle();
}
