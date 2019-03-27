// Automatically generated

#ifndef SymEngineParserBase_h_included
#define SymEngineParserBase_h_included

#include <exception>
#include <vector>
#include <iostream>

// $insert preincludes
#include <memory>
#include "symengine/visitor.h"

namespace // anonymous
{
struct PI__;
}

// $insert namespace-open
namespace SymEngine
{

// $insert polymorphic
enum class Tag__ {
    basic_vec,
    string,
    basic,
};

namespace Meta__
{
template <Tag__ tag>
struct TypeOf;

template <typename Tp_>
struct TagOf;

// $insert polymorphicSpecializations
template <>
struct TagOf<vec_basic> {
    static Tag__ const tag = Tag__::basic_vec;
};

template <>
struct TagOf<std::string> {
    static Tag__ const tag = Tag__::string;
};

template <>
struct TagOf<RCP<const Basic>> {
    static Tag__ const tag = Tag__::basic;
};

template <>
struct TypeOf<Tag__::basic_vec> {
    typedef vec_basic type;
};

template <>
struct TypeOf<Tag__::string> {
    typedef std::string type;
};

template <>
struct TypeOf<Tag__::basic> {
    typedef RCP<const Basic> type;
};

// The Base class:
// Individual semantic value classes are derived from this class.
// This class offers a member returning the value's Tag__
// and two member templates get() offering const/non-const access to
// the actual semantic value type.
class Base
{
    Tag__ d_tag;

protected:
    Base(Tag__ tag);

public:
    Base(Base const &other) = delete;

    Tag__ tag() const;

    template <Tag__ tg_>
    typename TypeOf<tg_>::type &get();

    template <Tag__ tg_>
    typename TypeOf<tg_>::type const &get() const;
};

// The class Semantic is derived from Base. It stores a particular
// semantic value type.
template <Tag__ tg_>
class Semantic : public Base
{
    typedef typename TypeOf<tg_>::type DataType;

    DataType d_data;

public:
    // The constructor forwards arguments to d_data, allowing
    // it to be initialized using whatever constructor is
    // available for DataType
    template <typename... Params>
    Semantic(Params &&... params);

    DataType &data();
    DataType const &data() const;
};

// If Type is default constructible, Initializer::value is
// initialized to new Type, otherwise it's initialized to 0, allowing
// struct SType: public std::shared_ptr<Base> to initialize its
// shared_ptr<Base> class whether or not Base is default
// constructible.
template <bool constructible, typename Type>
struct Initializer {
    static Type *value;
};
template <bool constructible, typename Type>
Type *Initializer<constructible, Type>::value = new Type;

template <typename Type>
struct Initializer<false, Type> {
    static constexpr Type *value = 0;
};

// The class Stype wraps the shared_ptr holding a pointer to Base.
// It becomes the polymorphic STYPE__
// It also wraps Base's get members, allowing constructions like
// $$.get<INT> to be used, rather than $$->get<INT>.
// Its operator= can be used to assign a Semantic *
// directly to the SType object. The free functions (in the parser's
// namespace (if defined)) semantic__ can be used to obtain a
// Semantic *.
struct SType : public std::shared_ptr<Base> {
    SType();

    template <typename Tp_>
    SType &operator=(Tp_ &&value);

    Tag__ tag() const;

    // this get()-member checks for 0-pointer and correct tag
    // in shared_ptr<Base>, and resets the shared_ptr's Base *
    // to point to Meta::__Semantic<tg_>() if not
    template <Tag__ tg_>
    typename TypeOf<tg_>::type &get();
    template <Tag__ tg_>
    typename TypeOf<tg_>::type const &get() const;

    // the data()-member does not check, and may result in a
    // segfault if used incorrectly
    template <Tag__ tg_>
    typename TypeOf<tg_>::type &data();
    template <Tag__ tg_>
    typename TypeOf<tg_>::type const &data() const;

    template <Tag__ tg_, typename... Args>
    void emplace(Args &&... args);
};

} // namespace Meta__

class ParserBase
{
public:
    // $insert tokens

    // Symbolic tokens:
    enum Tokens__ {
        IDENTIFIER = 257,
        NUMERIC,
        IMPLICIT_MUL,
        EQ,
        LE,
        GE,
        UMINUS,
        POW,
        NOT,
    };

    // $insert STYPE
    typedef Meta__::SType STYPE__;

private:
    int d_stackIdx__;
    std::vector<size_t> d_stateStack__;
    std::vector<STYPE__> d_valueStack__;

protected:
    enum Return__ {
        PARSE_ACCEPT__ = 0, // values used as parse()'s return values
        PARSE_ABORT__ = 1
    };
    enum ErrorRecovery__ {
        DEFAULT_RECOVERY_MODE__,
        UNEXPECTED_TOKEN__,
    };
    bool d_debug__;
    size_t d_nErrors__;
    size_t d_requiredTokens__;
    size_t d_acceptedTokens__;
    int d_token__;
    int d_nextToken__;
    size_t d_state__;
    STYPE__ *d_vsp__;
    STYPE__ d_val__;
    STYPE__ d_nextVal__;

    ParserBase();

    void ABORT() const;
    void ACCEPT() const;
    void ERROR() const;
    void clearin();
    bool debug() const;
    void pop__(size_t count = 1);
    void push__(size_t nextState);
    void popToken__();
    void pushToken__(int token);
    void reduce__(PI__ const &productionInfo);
    void errorVerbose__();
    size_t top__() const;

public:
    void setDebug(bool mode);
};

inline bool ParserBase::debug() const
{
    return d_debug__;
}

inline void ParserBase::setDebug(bool mode)
{
    d_debug__ = mode;
}

inline void ParserBase::ABORT() const
{
    throw PARSE_ABORT__;
}

inline void ParserBase::ACCEPT() const
{
    throw PARSE_ACCEPT__;
}

inline void ParserBase::ERROR() const
{
    throw UNEXPECTED_TOKEN__;
}

// $insert polymorphicInline
namespace Meta__
{

inline Base::Base(Tag__ tag) : d_tag(tag)
{
}

inline Tag__ Base::tag() const
{
    return d_tag;
}

template <Tag__ tg_>
template <typename... Params>
inline Semantic<tg_>::Semantic(Params &&... params)
    : Base(tg_), d_data(std::forward<Params>(params)...)
{
}

template <Tag__ tg_>
inline typename TypeOf<tg_>::type &Semantic<tg_>::data()
{
    return d_data;
}

template <Tag__ tg_>
inline typename TypeOf<tg_>::type const &Semantic<tg_>::data() const
{
    return d_data;
}

template <Tag__ tg_>
inline typename TypeOf<tg_>::type &Base::get()
{
    return static_cast<Semantic<tg_> *>(this)->data();
}

template <Tag__ tg_>
inline typename TypeOf<tg_>::type const &Base::get() const
{
    return static_cast<Semantic<tg_> *>(this)->data();
}

inline SType::SType()
    : std::shared_ptr<Base>{
          Initializer<std::is_default_constructible<Base>::value, Base>::value}
{
}

inline Tag__ SType::tag() const
{
    return std::shared_ptr<Base>::get()->tag();
}

template <Tag__ tg_>
inline typename TypeOf<tg_>::type &SType::get()
{
    // if we're not yet holding a (tg_) value, initialize to
    // a Semantic<tg_> holding a default value
    if (std::shared_ptr<Base>::get() == 0 || tag() != tg_) {
        typedef Semantic<tg_> SemType;

        if (not std::is_default_constructible<
                typename TypeOf<tg_>::type>::value)
            throw std::runtime_error(
                "STYPE::get<tag>: no default constructor available");

        reset(new SemType);
    }

    return std::shared_ptr<Base>::get()->get<tg_>();
}

template <Tag__ tg_>
inline typename TypeOf<tg_>::type &SType::data()
{
    return std::shared_ptr<Base>::get()->get<tg_>();
}

template <Tag__ tg_>
inline typename TypeOf<tg_>::type const &SType::data() const
{
    return std::shared_ptr<Base>::get()->get<tg_>();
}

template <Tag__ tg_, typename... Params>
inline void SType::emplace(Params &&... params)
{
    reset(new Semantic<tg_>(std::forward<Params>(params)...));
}

template <bool, typename Tp_>
struct Assign;

template <typename Tp_>
struct Assign<true, Tp_> {
    static SType &assign(SType *lhs, Tp_ &&tp);
};

template <typename Tp_>
struct Assign<false, Tp_> {
    static SType &assign(SType *lhs, Tp_ const &tp);
};

template <>
struct Assign<false, SType> {
    static SType &assign(SType *lhs, SType const &tp);
};

template <typename Tp_>
inline SType &Assign<true, Tp_>::assign(SType *lhs, Tp_ &&tp)
{
    lhs->reset(new Semantic<TagOf<Tp_>::tag>(std::move(tp)));
    return *lhs;
}

template <typename Tp_>
inline SType &Assign<false, Tp_>::assign(SType *lhs, Tp_ const &tp)
{
    lhs->reset(new Semantic<TagOf<Tp_>::tag>(tp));
    return *lhs;
}

inline SType &Assign<false, SType>::assign(SType *lhs, SType const &tp)
{
    return lhs->operator=(tp);
}

template <typename Tp_>
inline SType &SType::operator=(Tp_ &&rhs)
{
    return Assign<std::is_rvalue_reference<Tp_ &&>::value,
                  typename std::remove_reference<Tp_>::type>::
        assign(this, std::forward<Tp_>(rhs));
}

} // namespace Meta__

// As a convenience, when including ParserBase.h its symbols are available as
// symbols in the class Parser, too.
#define Parser ParserBase

// $insert namespace-close
}

#endif
