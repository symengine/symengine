#ifndef CSYMPY_BASIC_INL_H
#define CSYMPY_BASIC_INL_H

namespace CSymPy {

inline std::size_t Basic::hash() const
{
    if (hash_ == 0)
        hash_ = __hash__();
    return hash_;
}
    
inline bool Basic::__neq__(const Basic &o) const
{
    return !(this->__eq__(o));
}

inline bool eq(const RCP<const Basic> &a, const RCP<const Basic> &b)
{
    return a->__eq__(*b);
}

inline bool neq(const RCP<const Basic> &a, const RCP<const Basic> &b)
{
    return !(a->__eq__(*b));
}

template <class T>
inline bool is_a(const Basic &b)
{
    return typeid(T) == typeid(b);
}

template <class T>
inline bool is_a_sub(const Basic &b)
{
    return dynamic_cast<const T *>(&b) != NULL;
}

} // CSymPy

// global namespace functions
inline std::ostream& operator<<(std::ostream& out, const CSymPy::Basic& p)
{
    out << p.__str__();
    return out;
}

template <class T>
inline void hash_combine(std::size_t& seed, const T& v)
{
    std::hash<T> hasher;
    seed ^= hasher(v) + 0x9e3779b9 + (seed<<6) + (seed>>2);
}

// std namespace functions
namespace std
{
    // Specialise std::hash for Basic. We just call Basic.__hash__()
    template<>
    struct hash<CSymPy::Basic>
    {
        std::size_t operator()(const CSymPy::Basic& b) const
        {
            return b.hash();
        }
    };
}

#endif

