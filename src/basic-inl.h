#ifndef CSYMPY_BASIC_INL_H
#define CSYMPY_BASIC_INL_H

namespace SymEngine {

inline std::size_t Basic::hash() const
{
    if (hash_ == 0)
        hash_ = __hash__();
    return hash_;
}
//! \return true if not equal    
inline bool Basic::__neq__(const Basic &o) const
{
    return !(this->__eq__(o));
}
//! \return true if  `a` equal `b`
inline bool eq(const RCP<const Basic> &a, const RCP<const Basic> &b)
{
    return a->__eq__(*b);
}
//! \return true if  `a` not equal `b`
inline bool neq(const RCP<const Basic> &a, const RCP<const Basic> &b)
{
    return !(a->__eq__(*b));
}

//! Templatised version to check is_a type
template <class T>
inline bool is_a(const Basic &b)
{
    return T::type_code_id == b.get_type_code();
}

template <class T>
inline bool is_a_sub(const Basic &b)
{
    return dynamic_cast<const T *>(&b) != nullptr;
}

} // SymEngine

// global namespace functions
//! `<<` Operator
inline std::ostream& operator<<(std::ostream& out, const SymEngine::Basic& p)
{
    out << p.__str__();
    return out;
}
//! Templatised version to combine hash
template <class T>
inline void hash_combine(std::size_t& seed, const T& v)
{
    std::hash<T> hasher;
    seed ^= hasher(v) + 0x9e3779b9 + (seed<<6) + (seed>>2);
}

// std namespace functions
namespace std
{
    //! Specialise std::hash for Basic. We just call Basic.__hash__()
    template<>
    struct hash<SymEngine::Basic>
    {
        std::size_t operator()(const SymEngine::Basic& b) const
        {
            return b.hash();
        }
    };
}

#endif

