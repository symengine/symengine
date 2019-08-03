#include <iostream>
#include <chrono>
#include <symengine/parser.h>

using SymEngine::Basic;
using SymEngine::RCP;
using SymEngine::print_stack_on_segfault;
using SymEngine::parse;
using SymEngine::parse_old;
using SymEngine::parse_new;

int main(int argc, char *argv[])
{
    SymEngine::print_stack_on_segfault();

    RCP<const Basic> a;
    int N;

    N = 5000;
    std::string text;
    std::string t0 = "(x + y - sin(x)/(z**2-4) - x**(y**z))";
    text = t0;
    for (int i = 0; i < N; i++) {
        text = text + " * " + t0;
    }

    auto t1 = std::chrono::high_resolution_clock::now();
    a = parse(text);
    auto t2 = std::chrono::high_resolution_clock::now();
    std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1)
                     .count()
              << "ms" << std::endl;
    std::cout << *a << std::endl;

    t1 = std::chrono::high_resolution_clock::now();
    a = parse_old(text);
    t2 = std::chrono::high_resolution_clock::now();
    std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1)
                     .count()
              << "ms" << std::endl;
    std::cout << *a << std::endl;

    t1 = std::chrono::high_resolution_clock::now();
    a = parse_new(text);
    t2 = std::chrono::high_resolution_clock::now();
    std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1)
                     .count()
              << "ms" << std::endl;
    std::cout << *a << std::endl;

    /* ------------------------------------------------- */

    N = 3000;
    t1 = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < N; i++) {
        a = parse(t0);
    }
    t2 = std::chrono::high_resolution_clock::now();
    std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1)
                     .count()
              << "ms" << std::endl;

    t1 = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < N; i++) {
        a = parse_old(t0);
    }
    t2 = std::chrono::high_resolution_clock::now();
    std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1)
                     .count()
              << "ms" << std::endl;

    return 0;
}
