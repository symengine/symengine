#include <iostream>
#include <chrono>
#include <symengine/parser.h>
#include <symengine/parser/parser.h>

using SymEngine::Basic;
using SymEngine::RCP;
using SymEngine::print_stack_on_segfault;
using SymEngine::parse;
using SymEngine::parse_old;

int main(int argc, char *argv[])
{
    SymEngine::print_stack_on_segfault();

    RCP<const Basic> a;
    int N;

    auto t1 = std::chrono::high_resolution_clock::now();
    a = parse("0");
    auto t2 = std::chrono::high_resolution_clock::now();
    std::cout << "parse('0') = " << *a << ": "
              << std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1)
                     .count()
              << "us" << std::endl;

    N = 5000;
    std::string text;
    std::string t0 = "(x + y - x/(z**2-4) - x**(y**z))";
    text = t0;
    for (int i = 0; i < N; i++) {
        text = text + " * " + t0;
    }

    t1 = std::chrono::high_resolution_clock::now();
    a = parse(text);
    t2 = std::chrono::high_resolution_clock::now();
    std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1)
                     .count()
              << "ms" << std::endl;
    std::cout << *a << std::endl;

    return 0;
}
