#include <iostream>
#include <chrono>
#include <symengine/parser.h>
#include <symengine/parser/parser.h>

using SymEngine::print_stack_on_segfault;
using SymEngine::parse;

int main(int argc, char *argv[])
{
    SymEngine::print_stack_on_segfault();

    int N;

    N = 5000;
    std::string text;
    std::string t0 = "(2*x + 3*y - x/(z**2-4) - x**(y**z))";
    text = t0;
    for (int i = 0; i < N; i++) {
        text = text + " * " + t0;
    }

    auto t1 = std::chrono::high_resolution_clock::now();
    parse(text);
    auto t2 = std::chrono::high_resolution_clock::now();
    std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1)
                     .count()
              << "ms" << std::endl;

    return 0;
}
