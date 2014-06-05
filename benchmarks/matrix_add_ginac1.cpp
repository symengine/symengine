#include <iostream>
#include <chrono>

#include <ginac/ginac.h>

using namespace GiNaC;

int main()
{
    matrix A(4, 4), B(4, 4), C(4, 4);

    A = 1, 2, 3, 4,
        5, 6, 7, 8,
        9, 10, 11, 12,
        13, 14, 15, 16;
    
    B = 1, 2, 3, 4,
        5, 6, 7, 8,
        9, 10, 11, 12,
        13, 14, 15, 16;
        
    unsigned N = 10000;
    
    auto t1 = std::chrono::high_resolution_clock::now();
    for (unsigned i = 0; i < N; i++)
        C = A.add(B);;
    auto t2 = std::chrono::high_resolution_clock::now();

    std::cout
        << std::chrono::duration_cast<std::chrono::microseconds>(t2-t1).count()/N
        << " microseconds" << std::endl;   
}

//g++ -o simple -Wl,--no-as-needed `pkg-config --cflags --libs ginac` matrix_add_ginac.cpp 

