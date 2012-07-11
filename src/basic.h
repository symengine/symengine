#ifndef CSYMPY_BASIC_H
#define CSYMPY_BASIC_H

namespace CSymPy {

class Basic {
public:
    virtual std::size_t __hash__() const = 0;
};

} // CSymPy

namespace std
{
    // Specialise std::hash for Basic. We just call Basic.__hash__()
    template<>
    class hash<CSymPy::Basic> : public unary_function<CSymPy::Basic, size_t>
    {
    public:
        size_t operator()(const CSymPy::Basic& b)
        {
            return b.__hash__();
        }
    };
}

// Standard hash_combine() function. Example of usage:
//
//    std::size_t seed1 = 0;
//    hash_combine<std::string>(seed1, "x");
//    hash_combine<std::string>(seed1, "y");
//
// You can use it with any CSymPy class:
//
//    RCP<Symbol> x = rcp(new Symbol("x"));
//    RCP<Symbol> y = rcp(new Symbol("y"));
//    std::size_t seed2 = 0;
//    hash_combine<Basic>(seed2, *x);
//    hash_combine<Basic>(seed2, *y);
//
template <class T>
inline void hash_combine(std::size_t& seed, const T& v)
{
    std::hash<T> hasher;
    seed ^= hasher(v) + 0x9e3779b9 + (seed<<6) + (seed>>2);
}



#endif
