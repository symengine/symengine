#include <symengine/basic.h>
#include <symengine/symbol.h>
#include <symengine/parser.h>
#include <symengine/visitor.h>

#include <cereal/archives/binary.hpp>
#include <cereal/cereal.hpp>
#include <cereal/types/polymorphic.hpp>
#include <cereal/types/string.hpp>
#include <cereal/details/helpers.hpp>

namespace SymEngine
{

template <class Archive>
inline void save_basic(Archive &ar, const Basic &b)
{
    throw std::runtime_error("not supported");
}
template <class Archive>
inline void save_basic(Archive &ar, const Symbol &b)
{
    ar(b.__str__());
}
template <class Archive>
inline void save_basic(Archive &ar, const Pow &b)
{
    ar(b.get_base());
    ar(b.get_exp());
}

//! Saving for boost::intrusive_ptr
template <class Archive>
inline void CEREAL_SAVE_FUNCTION_NAME(Archive &ar, RCP<const Basic> const &ptr)
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

template <class Archive>
RCP<const Basic> load_basic(Archive &ar, RCP<const Symbol> &)
{
    std::string name;
    ar(name);
    return symbol(name);
}
template <class Archive>
RCP<const Basic> load_basic(Archive &ar, RCP<const Pow> &)
{
    RCP<const Basic> base, exp;
    ar(base);
    ar(exp);
    return make_rcp<const Pow>(base, exp);
}
template <class Archive, typename T>
RCP<const Basic> load_basic(Archive &ar, RCP<const T> &)
{
    throw std::runtime_error("not implemented");
}

//! Loading for boost::intrusive_ptr
template <class Archive>
inline void CEREAL_LOAD_FUNCTION_NAME(Archive &ar, RCP<const Basic> &ptr)
{
    uint32_t id;
    ar(CEREAL_NVP(id));

    if (id & cereal::detail::msb_32bit) {
        TypeID type_code;
        ar(type_code);
        switch (type_code) {
#define SYMENGINE_ENUM(type_enum, Class)                                       \
    case type_enum: {                                                          \
        RCP<const Class> dummy_ptr;                                            \
        ptr = load_basic(ar, dummy_ptr);                                       \
        break;                                                                 \
    }
#include "symengine/type_codes.inc"
#undef SYMENGINE_ENUM
            default:
                load_basic(ar, ptr);
        }

        std::shared_ptr<RCP<const Basic>> shared_rcp_ptr
            = std::make_shared<RCP<const Basic>>(ptr);
        std::shared_ptr<void> sharedPtr
            = std::static_pointer_cast<void>(shared_rcp_ptr);
        ar.registerSharedPointer(id, sharedPtr);
    } else {
        std::shared_ptr<RCP<const Basic>> sharedPtr
            = std::static_pointer_cast<RCP<const Basic>>(
                ar.getSharedPointer(id));
        ptr = *sharedPtr.get();
    }
}

} // namespace SymEngine
