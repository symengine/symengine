#include "bipartite.h"

void test_hopcroft_karp_bipartite1() {
	BipartiteGraph<string> bipartite;

	bipartite.__setitem__(make_tuple(make_tuple(LEFT, 0), make_tuple(RIGHT, 0)), "X");
	bipartite.__setitem__(make_tuple(make_tuple(LEFT, 0), make_tuple(RIGHT, 1)), "X");
	bipartite.__setitem__(make_tuple(make_tuple(LEFT, 1), make_tuple(RIGHT, 0)), "X");
	bipartite.__setitem__(make_tuple(make_tuple(LEFT, 1), make_tuple(RIGHT, 4)), "X");
	bipartite.__setitem__(make_tuple(make_tuple(LEFT, 2), make_tuple(RIGHT, 2)), "X");
	bipartite.__setitem__(make_tuple(make_tuple(LEFT, 2), make_tuple(RIGHT, 3)), "X");
	bipartite.__setitem__(make_tuple(make_tuple(LEFT, 3), make_tuple(RIGHT, 0)), "X");
	bipartite.__setitem__(make_tuple(make_tuple(LEFT, 3), make_tuple(RIGHT, 4)), "X");
	bipartite.__setitem__(make_tuple(make_tuple(LEFT, 4), make_tuple(RIGHT, 1)), "X");
	bipartite.__setitem__(make_tuple(make_tuple(LEFT, 4), make_tuple(RIGHT, 3)), "X");

	HopcroftKarp<string> hk(bipartite);
	cout << "Maximum matchings: " << hk.hopcroft_karp() << endl;
	hk.dump_state();
}

void test_hopcroft_karp_bipartite2() {
	BipartiteGraph<string> bipartite;

	bipartite.__setitem__(make_tuple(make_tuple(LEFT, 'a'-'a'), make_tuple(RIGHT, 1)), "X");
	bipartite.__setitem__(make_tuple(make_tuple(LEFT, 'b'-'a'), make_tuple(RIGHT, 1)), "X");
	bipartite.__setitem__(make_tuple(make_tuple(LEFT, 'b'-'a'), make_tuple(RIGHT, 2)), "X");
	bipartite.__setitem__(make_tuple(make_tuple(LEFT, 'c'-'a'), make_tuple(RIGHT, 1)), "X");
	bipartite.__setitem__(make_tuple(make_tuple(LEFT, 'c'-'a'), make_tuple(RIGHT, 2)), "X");
	bipartite.__setitem__(make_tuple(make_tuple(LEFT, 'd'-'a'), make_tuple(RIGHT, 2)), "X");
	bipartite.__setitem__(make_tuple(make_tuple(LEFT, 'd'-'a'), make_tuple(RIGHT, 3)), "X");
	bipartite.__setitem__(make_tuple(make_tuple(LEFT, 'd'-'a'), make_tuple(RIGHT, 4)), "X");
	bipartite.__setitem__(make_tuple(make_tuple(LEFT, 'e'-'a'), make_tuple(RIGHT, 3)), "X");
	bipartite.__setitem__(make_tuple(make_tuple(LEFT, 'e'-'a'), make_tuple(RIGHT, 4)), "X");
	bipartite.__setitem__(make_tuple(make_tuple(LEFT, 'f'-'a'), make_tuple(RIGHT, 4)), "X");
	bipartite.__setitem__(make_tuple(make_tuple(LEFT, 'f'-'a'), make_tuple(RIGHT, 5)), "X");
	bipartite.__setitem__(make_tuple(make_tuple(LEFT, 'f'-'a'), make_tuple(RIGHT, 6)), "X");
	bipartite.__setitem__(make_tuple(make_tuple(LEFT, 'g'-'a'), make_tuple(RIGHT, 5)), "X");
	bipartite.__setitem__(make_tuple(make_tuple(LEFT, 'g'-'a'), make_tuple(RIGHT, 6)), "X");
	bipartite.__setitem__(make_tuple(make_tuple(LEFT, 'g'-'a'), make_tuple(RIGHT, 7)), "X");
	bipartite.__setitem__(make_tuple(make_tuple(LEFT, 'h'-'a'), make_tuple(RIGHT, 8)), "X");

	HopcroftKarp<string> hk(bipartite);
	cout << "Maximum matchings: " << hk.hopcroft_karp() << endl;
	hk.dump_state();
}

int main(int argc, char **argv) {
	cout << "hello\n";

	test_hopcroft_karp_bipartite1();
	test_hopcroft_karp_bipartite2();

}

