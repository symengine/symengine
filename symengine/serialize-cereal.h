#pragma once
#include <symengine/basic.h>
#include <symengine/integer.h>
#include <symengine/symbol.h>
#include <symengine/visitor.h>
#include <symengine/utilities/stream_fmt.h>

#include <cereal/cereal.hpp>
#include <cereal/types/polymorphic.hpp>
#include <cereal/types/string.hpp>
#include <cereal/details/helpers.hpp>
#include <cereal/types/map.hpp>
#include <cereal/types/unordered_map.hpp>

namespace SymEngine
{

template <class Archive>
inline void save_basic(Archive &ar, const Basic &b)
{
    const auto t_code = b.get_type_code();
    throw std::runtime_error(StreamFmt()
                             << __FILE__ << ":" << __LINE__ << ": "
                             << __PRETTY_FUNCTION__ << " not supported: "
                             << type_code_name(t_code) << " (" << t_code << ")"
#if !defined(NDEBUG)
                             << ", " << b.__str__()
#endif
                                 );
}
template <class Archive>
inline void save_basic(Archive &ar, const Symbol &b)
{
    ar(b.__str__());
}
template <class Archive>
inline void save_basic(Archive &ar, const Mul &b)
{
    ar(b.get_coef());
    ar(b.get_dict());
}
template <class Archive>
inline void save_basic(Archive &ar, const Add &b)
{
    ar(b.get_coef());
    ar(b.get_dict());
}
template <class Archive>
inline void save_basic(Archive &ar, const Pow &b)
{
    ar(b.get_base());
    ar(b.get_exp());
}
template <class Archive>
inline void save_basic(Archive &ar, const URatPoly &b)
{
    ar(b.get_var());
    ar(b.get_poly());
}
template <class Archive>
inline void save_basic(Archive &ar, const Integer &b)
{
    ar(b.__str__());
}
// template <class Archive>
// inline void save_basic(Archive &ar, const Rational &b)
// {
//     ar(b.get_num(), b.get_den());
// }

template <class Archive>
inline void save_basic(Archive &ar, const Constant &b)
{
    ar(b.get_name());
}

template <class Archive>
inline void save_basic(Archive &ar, const OneArgFunction &b)
{
    ar(b.get_arg());
}

template <class Archive>
inline void save_basic(Archive &ar, const Relational &b)
{
    ar(b.get_arg1(), b.get_arg2());
}

template <class Archive>
inline void save_basic(Archive &ar, RCP<const Basic> const &ptr)
{
    uint32_t id = ar.registerSharedPointer(ptr.get());
    ar(CEREAL_NVP(id));

    if (id & cereal::detail::msb_32bit) {
        ar(ptr->get_type_code());
        switch (ptr->get_type_code()) {
#define SYMENGINE_ENUM(type, Class)                                            \
    case type:                                                                 \
        save_basic(ar, static_cast<const Class &>(*ptr));                      \
        break;
#include "symengine/type_codes.inc"
#undef SYMENGINE_ENUM
            default:
                save_basic(ar, *ptr);
        }
    }
}

//! Saving for SymEngine::RCP
template <class Archive, class T>
inline void CEREAL_SAVE_FUNCTION_NAME(Archive &ar, RCP<const T> const &ptr)
{
    save_basic(ar, rcp_static_cast<const Basic>(ptr));
}

template <class Archive>
RCP<const Basic> load_basic(Archive &ar, RCP<const Symbol> &)
{
    std::string name;
    ar(name);
    return symbol(name);
}
template <class Archive>
RCP<const Basic> load_basic(Archive &ar, RCP<const Mul> &)
{
    RCP<const Number> coeff;
    map_basic_basic dict;
    ar(coeff);
    ar(dict);
    return make_rcp<const Mul>(coeff, std::move(dict));
}
template <class Archive>
RCP<const Basic> load_basic(Archive &ar, RCP<const Add> &)
{
    RCP<const Number> coeff;
    umap_basic_num dict;
    ar(coeff);
    ar(dict);
    return make_rcp<const Add>(coeff, std::move(dict));
}
template <class Archive>
RCP<const Basic> load_basic(Archive &ar, RCP<const Pow> &)
{
    RCP<const Basic> base, exp;
    ar(base);
    ar(exp);
    return make_rcp<const Pow>(base, exp);
}
template <class Archive>
RCP<const Basic> load_basic(Archive &ar, RCP<const URatPoly> &)
{
    RCP<const Basic> var;
    URatDict poly;
    ar(var, poly);
    return make_rcp<const URatPoly>(var, std::move(poly));
}
template <class Archive>
RCP<const Basic> load_basic(Archive &ar, RCP<const Integer> &)
{
    std::string name;
    ar(name);
    return integer(integer_class(name));
}
template <class Archive>
RCP<const Basic> load_basic(Archive &ar, RCP<const Constant> &)
{
    std::string name;
    ar(name);
    return constant(name);
}
template <class Archive, class T>
RCP<const Basic>
load_basic(Archive &ar, RCP<const T> &,
           typename std::enable_if<std::is_base_of<OneArgFunction, T>::value,
                                   int>::type * = nullptr)
{
    RCP<const Basic> arg;
    ar(arg);
    return make_rcp<const T>(arg);
}
template <class Archive, class T>
RCP<const Basic>
load_basic(Archive &ar, RCP<const T> &,
           typename std::enable_if<std::is_base_of<Relational, T>::value,
                                   int>::type * = nullptr)
{
    RCP<const Basic> arg1, arg2;
    ar(arg1, arg2);
    return make_rcp<const T>(arg1, arg2);
}
template <class Archive, class T>
RCP<const Basic> load_basic(
    Archive &ar, RCP<const T> &,
    typename std::enable_if<not(std::is_base_of<Relational, T>::value
                                or std::is_base_of<OneArgFunction, T>::value),
                            int>::type * = nullptr)
{
    throw std::runtime_error(StreamFmt()
                             << __FILE__ << ":" << __LINE__ << ": "
                             << __PRETTY_FUNCTION__
                             << "Loading of this type is not implemented.");
}

//! Loading for SymEngine::RCP
template <class Archive, class T>
inline void CEREAL_LOAD_FUNCTION_NAME(Archive &ar, RCP<const T> &ptr)
{
    uint32_t id;
    ar(CEREAL_NVP(id));

    if (id & cereal::detail::msb_32bit) {
        TypeID type_code;
        ar(type_code);
        switch (type_code) {
#define SYMENGINE_ENUM(type_enum, Class)                                       \
    case type_enum: {                                                          \
        if (not std::is_base_of<T, Class>::value) {                            \
            throw std::runtime_error("Cannot convert to type.");               \
        } else {                                                               \
            RCP<const Class> dummy_ptr;                                        \
            ptr = rcp_static_cast<const T>(                                    \
                rcp_static_cast<const Basic>(load_basic(ar, dummy_ptr)));      \
            break;                                                             \
        }                                                                      \
    }
#include "symengine/type_codes.inc"
#undef SYMENGINE_ENUM
            default:
                throw std::runtime_error("Unknown type");
        }
        std::shared_ptr<void> sharedPtr = std::static_pointer_cast<void>(
            std::make_shared<RCP<const Basic>>(ptr));

        ar.registerSharedPointer(id, sharedPtr);
    } else {
        std::shared_ptr<RCP<const T>> sharedPtr
            = std::static_pointer_cast<RCP<const T>>(ar.getSharedPointer(id));
        ptr = *sharedPtr.get();
    }
}
template <typename Archive>
void serialize(Archive &ar, URatDict &urd)
{
    ar(urd.dict_);
}
template <typename Archive>
void save(Archive &ar, const rational_class &rat)
{
    integer_class num = get_num(rat);
    integer_class den = get_den(rat);
    ar(num, den);
}
template <typename Archive>
void load(Archive &ar, rational_class &rat)
{
    integer_class num;
    integer_class den;
    ar(num, den);
    rat = rational_class(num, den);
}
template <typename Archive>
void save(Archive &ar, const integer_class &intgr)
{
    std::ostringstream s;
    s << intgr; // stream to string
    ar(s.str());
}
template <typename Archive>
void load(Archive &ar, integer_class &intgr)
{
    std::string s;
    ar(s);
    intgr = integer_class(s);
}
} // namespace SymEngine
