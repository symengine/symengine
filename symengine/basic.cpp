#include <symengine/printers.h>
#include <symengine/subs.h>
#include <symengine/serialize-cereal.h>
#include <cereal/archives/portable_binary.hpp>

namespace SymEngine
{

std::string type_code_name(TypeID id)
{
#define STRINGIFY0(x) #x
#define STRINGIFY(x) STRINGIFY0(x)
    const static std::array<std::string,
                            static_cast<int>(TypeID::TypeID_Count) + 1>
        type_names{{
#define SYMENGINE_ENUM(type, Class) STRINGIFY(Class),
#include "symengine/type_codes.inc"
#undef SYMENGINE_ENUM
            "TypeID_Count"}};
#undef STRINGIFY0
#undef STRINGIFY

    if ((id < 0) || (id > TypeID::TypeID_Count)) {
        throw std::runtime_error("type_id out of range");
    }
    return type_names[id];
}

int Basic::__cmp__(const Basic &o) const
{
    auto a = this->get_type_code();
    auto b = o.get_type_code();
    if (a == b) {
        return this->compare(o);
    } else {
        // We return the order given by the numerical value of the TypeID enum
        // type.
        // The types don't need to be ordered in any given way, they just need
        // to be ordered.
        return a < b ? -1 : 1;
    }
}

std::string Basic::__str__() const
{
    return str(*this);
}

std::string Basic::dumps() const
{
    std::ostringstream oss;
    cereal::PortableBinaryOutputArchive{oss}(this->rcp_from_this());
    return oss.str();
}

RCP<const Basic> Basic::loads(const std::string &serialized)
{
    RCP<const Basic> obj;
    std::istringstream iss(serialized);
    cereal::PortableBinaryInputArchive{iss}(obj);
    return obj;
}

RCP<const Basic> Basic::subs(const map_basic_basic &subs_dict) const
{
    return SymEngine::subs(this->rcp_from_this(), subs_dict);
}

RCP<const Basic> Basic::xreplace(const map_basic_basic &xreplace_dict) const
{
    return SymEngine::xreplace(this->rcp_from_this(), xreplace_dict);
}

const char *get_version()
{
    return SYMENGINE_VERSION;
}

bool is_a_Atom(const Basic &b)
{
    return is_a_Number(b) or is_a<Symbol>(b) or is_a<Constant>(b);
}

} // SymEngine
