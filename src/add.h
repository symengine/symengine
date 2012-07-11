#ifndef CSYMPY_ADD_H
#define CSYMPY_ADD_H

#include <unordered_map>

namespace CSymPy {

class Add {
private:
    std::unordered_map<int, int> dict;

public:
    Add(std::unordered_map<int, int> &dict);
};

} // CSymPy

#endif
