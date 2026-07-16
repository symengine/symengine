#ifndef SYMENGINE_SERIALIZE_CEREAL_H
#define SYMENGINE_SERIALIZE_CEREAL_H

#include <cctype>

#include <symengine/basic.h>
#include <symengine/number.h>
#include <symengine/integer.h>
#include <symengine/symbol.h>
#include <symengine/visitor.h>
#include <symengine/utilities/stream_fmt.h>

#include <cereal/cereal.hpp>
#include <cereal/version.hpp>
#include <cereal/types/polymorphic.hpp>
#include <cereal/types/string.hpp>
#include <cereal/details/helpers.hpp>
#include <cereal/types/map.hpp>
#include <cereal/types/unordered_map.hpp>
#include <cereal/types/set.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/utility.hpp>
#include <cereal/archives/portable_binary.hpp>

namespace SymEngine
{

template <class Archive>
class RCPBasicAwareOutputArchive : public Archive
{
    using Archive::Archive;

public:
    void save_rcp_basic(const RCP<const Basic> &ptr)
    {
        uintptr_t addr = (uintptr_t)(void *)ptr.get();
        (*this)(addr);

        auto id = _addresses.find(addr);
        uint8_t first_seen = (id == _addresses.end());
        (*this)(first_seen);

        if (not first_seen) {
            return;
        }
        // Keep a reference to every serialized object for the lifetime of the
        // archive, as some are temporary objects (e.g. Rational::get_num).
        // Without this a temp object can be freed and its address
        // then re-used by another temp object, which is then incorrectly
        // serialized as the previous object seen at this address.
        _keep_alive.push_back(ptr);
        TypeID type_code = ptr->get_type_code();
        save_typeid(*this, type_code);
        switch (type_code) {
#define SYMENGINE_ENUM(type, Class)                                            \
    case type:                                                                 \
        save_basic(*this, static_cast<const Class &>(*ptr));                   \
        break;
#include "symengine/type_codes.inc"
#undef SYMENGINE_ENUM
            default:
                save_basic(*this, *ptr);
        }
        _addresses.insert(addr);
    }

private:
    std::set<uintptr_t> _addresses;
    std::vector<RCP<const Basic>> _keep_alive;
    //! Overload the rtti function to enable dynamic_cast
    void rtti(){};
};

template <class Archive>
class RCPBasicAwareInputArchive : public Archive
{
    using Archive::Archive;

public:
    template <class T>
    RCP<const T> load_rcp_basic()
    {
        try {
            uintptr_t addr;
            (*this)(addr);

            uint8_t first_seen;
            (*this)(first_seen);

            if (first_seen >= 2) {
                throw SerializationError("Invalid input");
            }

            if (not first_seen) {
                auto it = _rcp_map.find(addr);
                if (it == _rcp_map.end()) {
                    throw SerializationError("Invalid shared pointer");
                }
                RCP<const Basic> b = it->second;
                switch (b->get_type_code()) {
#define SYMENGINE_ENUM(type_enum, Class)                                       \
    case type_enum: {                                                          \
        if (not std::is_base_of<T, Class>::value) {                            \
            throw SerializationError("Cannot convert to given type");          \
        } else {                                                               \
            return rcp_static_cast<const T>(b);                                \
        }                                                                      \
    }
#include "symengine/type_codes.inc"
#undef SYMENGINE_ENUM
                    default:
                        throw SerializationError("Unknown typeID");
                }
            }

            TypeID type_code;
            load_typeid(*this, type_code);
            switch (type_code) {
#define SYMENGINE_ENUM(type_enum, Class)                                       \
    case type_enum: {                                                          \
        RCP<const Class> dummy_ptr;                                            \
        RCP<const Basic> basic_ptr = load_basic(*this, dummy_ptr);             \
        _rcp_map[addr] = basic_ptr;                                            \
        if (not std::is_base_of<T, Class>::value) {                            \
            throw SerializationError("Cannot convert to given type");          \
        } else {                                                               \
            return rcp_static_cast<const T>(basic_ptr);                        \
        }                                                                      \
    }
#include "symengine/type_codes.inc"
#undef SYMENGINE_ENUM
                default:
                    throw SerializationError("Unknown typeID");
            }
        } catch (cereal::Exception &e) {
            throw SerializationError(e.what());
        }
    }

private:
    std::unordered_map<uintptr_t, RCP<const Basic>> _rcp_map;
    //! Overload the rtti function to enable dynamic_cast
    void rtti(){};
};

template <class Archive>
inline void save_basic(Archive &ar, const Basic &b)
{
    const auto t_code = b.get_type_code();
    throw SerializationError(StreamFmt()
                             << __FILE__ << ":" << __LINE__
#ifndef _MSC_VER
                             << ": " << __PRETTY_FUNCTION__
#endif
                             << " not supported: " << type_code_name(t_code)
                             << " (" << t_code << ")"
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
inline void save_basic(Archive &ar, const Dummy &b)
{
    ar(b.get_name(), b.get_index());
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
template <typename Archive>
void save_helper(Archive &ar, const integer_class &intgr)
{
    std::ostringstream s;
    s << intgr; // stream to string
    ar(s.str());
}
template <typename Archive>
void save_helper(Archive &ar, const rational_class &rat)
{
    integer_class num = get_num(rat);
    integer_class den = get_den(rat);
    save_helper(ar, num);
    save_helper(ar, den);
}
template <typename Archive>
void save_helper(Archive &ar, const Expression &c)
{
    ar(c.get_basic());
}
template <typename Archive, typename Container>
void save_poly_dict(Archive &ar, const Container &c)
{
    ar(c.dict_.size());
    for (const auto &term : c.dict_) {
        ar(term.first);
        save_helper(ar, term.second);
    }
}
template <typename Archive>
void save_basic(Archive &ar, const UIntPoly &b)
{
    ar(b.get_var());
    save_poly_dict(ar, b.get_poly());
}
template <typename Archive>
void save_basic(Archive &ar, const URatPoly &b)
{
    ar(b.get_var());
    save_poly_dict(ar, b.get_poly());
}
template <typename Archive>
void save_basic(Archive &ar, const UExprPoly &b)
{
    ar(b.get_var());
    save_poly_dict(ar, b.get_poly());
}
template <typename Archive>
void save_basic(Archive &ar, const MIntPoly &b)
{
    ar(b.get_vars());
    save_poly_dict(ar, b.get_poly());
}
template <typename Archive>
void save_basic(Archive &ar, const MExprPoly &b)
{
    ar(b.get_vars());
    save_poly_dict(ar, b.get_poly());
}
#if defined(HAVE_SYMENGINE_FLINT) || defined(HAVE_SYMENGINE_PIRANHA)
template <typename Archive, typename Poly>
void save_dense_poly(Archive &ar, const Poly &b)
{
    ar(b.get_var());
    unsigned int size = static_cast<unsigned int>(b.size());
    ar(size);
    for (unsigned int i = 0; i < size; ++i) {
        save_helper(ar, b.get_coeff(i));
    }
}
#endif
#ifdef HAVE_SYMENGINE_FLINT
template <typename Archive>
void save_basic(Archive &ar, const UIntPolyFlint &b)
{
    save_dense_poly(ar, b);
}
template <typename Archive>
void save_basic(Archive &ar, const URatPolyFlint &b)
{
    save_dense_poly(ar, b);
}
#endif
#ifdef HAVE_SYMENGINE_PIRANHA
template <typename Archive>
void save_basic(Archive &ar, const UIntPolyPiranha &b)
{
    save_dense_poly(ar, b);
}
template <typename Archive>
void save_basic(Archive &ar, const URatPolyPiranha &b)
{
    save_dense_poly(ar, b);
}
#endif
template <class Archive>
inline void save_basic(Archive &ar, const Integer &b)
{
    ar(b.__str__());
}
template <class Archive>
inline void save_basic(Archive &ar, const RealDouble &b)
{
    ar(b.i);
}
template <class Archive>
inline void save_basic(Archive &ar, const Rational &b)
{
    ar(b.get_num(), b.get_den());
}
template <class Archive>
inline void save_basic(Archive &ar, const ComplexBase &b)
{
    ar(b.real_part(), b.imaginary_part());
}
template <class Archive>
inline void save_basic(Archive &ar, const Interval &b)
{
    ar(b.get_left_open(), b.get_start(), b.get_right_open(), b.get_end());
}
template <class Archive>
inline void save_basic(Archive &ar, const BooleanAtom &b)
{
    ar(b.get_val());
}
template <class Archive>
inline void save_basic(Archive &ar, const Infty &b)
{
    ar(b.get_direction());
}

template <class Archive>
inline void save_basic(Archive &ar, const NaN &b)
{
}

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
inline void save_basic(Archive &ar, const TwoArgFunction &b)
{
    ar(b.get_arg1(), b.get_arg2());
}

template <class Archive>
inline void save_basic(Archive &ar, const Relational &b)
{
    ar(b.get_arg1(), b.get_arg2());
}
template <class Archive>
inline void save_basic(Archive &ar, const And &b)
{
    ar(b.get_container());
}
template <class Archive>
inline void save_basic(Archive &ar, const Or &b)
{
    ar(b.get_container());
}
template <class Archive>
inline void save_basic(Archive &ar, const Xor &b)
{
    ar(b.get_container());
}
template <class Archive>
inline void save_basic(Archive &ar, const Not &b)
{
    ar(b.get_arg());
}
template <class Archive>
inline void save_basic(Archive &ar, const Contains &b)
{
    ar(b.get_expr(), b.get_set());
}
template <class Archive>
inline void save_basic(Archive &ar, const Piecewise &b)
{
    ar(b.get_vec());
}
template <class Archive>
inline void save_basic(Archive &ar, const Reals &b)
{
}
template <class Archive>
inline void save_basic(Archive &ar, const Rationals &b)
{
}
template <class Archive>
inline void save_basic(Archive &ar, const EmptySet &b)
{
}
template <class Archive>
inline void save_basic(Archive &ar, const Integers &b)
{
}
template <class Archive>
inline void save_basic(Archive &ar, const UniversalSet &b)
{
}
template <class Archive>
inline void save_basic(Archive &ar, const Union &b)
{
    ar(b.get_container());
}
template <class Archive>
inline void save_basic(Archive &ar, const Complement &b)
{
    ar(b.get_universe(), b.get_container());
}
template <class Archive>
inline void save_basic(Archive &ar, const ImageSet &b)
{
    ar(b.get_symbol(), b.get_expr(), b.get_baseset());
}
template <class Archive>
inline void save_basic(Archive &ar, const FiniteSet &b)
{
    ar(b.get_container());
}
template <class Archive>
inline void save_basic(Archive &ar, const ConditionSet &b)
{
    ar(b.get_symbol(), b.get_condition());
}
#ifdef HAVE_SYMENGINE_MPFR
template <class Archive>
inline void save_basic(Archive &ar, const RealMPFR &b)
{
    ar(b.__str__(), b.get_prec());
}
#endif
template <class Archive>
inline void save_basic(Archive &ar, const GaloisField &b)
{
    throw NotImplementedError("GaloisField saving is not implemented yet.");
}
template <class Archive>
inline void save_basic(Archive &ar, const SeriesCoeffInterface &)
{
    throw NotImplementedError("Series saving is not implemented yet.");
}
template <class Archive>
inline void save_basic(Archive &ar, const MultiArgFunction &b)
{
    ar(b.get_args());
}
template <class Archive>
inline void save_basic(Archive &ar, const FunctionSymbol &b)
{
    ar(b.get_name(), b.get_args());
}
template <class Archive>
inline void save_basic(Archive &ar, const Derivative &b)
{
    ar(b.get_arg(), b.get_symbols());
}
template <class Archive>
inline void save_basic(Archive &ar, const Subs &b)
{
    ar(b.get_arg(), b.get_dict());
}
template <class Archive>
inline void save_basic(Archive &ar, const NumberWrapper &b)
{
    throw NotImplementedError("NumberWrapper saving is not implemented yet.");
}
template <class Archive>
inline void save_basic(Archive &ar, const FunctionWrapper &b)
{
    throw NotImplementedError("FunctionWrapper saving is not implemented yet.");
}

//! Saving for SymEngine::RCP
template <class Archive, class T>
inline void CEREAL_SAVE_FUNCTION_NAME(Archive &ar, RCP<const T> const &ptr)
{
    RCPBasicAwareOutputArchive<Archive> *ar_ptr
        = dynamic_cast<RCPBasicAwareOutputArchive<Archive> *>(&ar);
    if (not ar_ptr) {
        throw SerializationError("Need a RCPBasicAwareOutputArchive");
    }
    ar_ptr->save_rcp_basic(rcp_static_cast<const Basic>(ptr));
}
template <class Archive>
RCP<const Basic> load_basic(Archive &ar, RCP<const RealDouble> &)
{
    double val;
    ar(val);
    return real_double(val);
}
template <class Archive>
RCP<const Basic> load_basic(Archive &ar, RCP<const Infty> &)
{
    RCP<const Number> direction;
    ar(direction);
    return Infty::from_direction(direction);
}
template <class Archive>
RCP<const Basic> load_basic(Archive &ar, RCP<const NaN> &)
{
    return rcp_static_cast<const Basic>(Nan);
}
template <class Archive>
RCP<const Basic> load_basic(Archive &ar, RCP<const Symbol> &)
{
    std::string name;
    ar(name);
    return symbol(name);
}
template <class Archive>
RCP<const Basic> load_basic(Archive &ar, RCP<const Dummy> &)
{
    std::string name;
    size_t index;
    ar(name, index);
    return dummy(name, index);
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
template <typename Archive>
void load_helper(Archive &ar, integer_class &intgr)
{
    std::string int_str;
    ar(int_str);
    if (int_str.size() == 0) {
        throw SerializationError("invalid integer");
    }
    if (not(int_str[0] == '-' or std::isdigit(int_str[0]))) {
        throw SerializationError("invalid integer");
    }
    for (auto it = ++int_str.begin(); it < int_str.end(); it++) {
        if (not std::isdigit(*it)) {
            throw SerializationError("invalid integer");
        }
    }
    intgr = integer_class(std::move(int_str));
}
template <typename Archive>
void load_helper(Archive &ar, rational_class &rat)
{
    integer_class num, den;
    load_helper(ar, num);
    load_helper(ar, den);
    rat = rational_class(num, den);
}
template <typename Archive>
void load_helper(Archive &ar, Expression &c)
{
    RCP<const Basic> b;
    ar(b);
    c = Expression(std::move(b));
}
template <typename RawDict, typename Archive>
RawDict load_poly_dict(Archive &ar)
{
    size_t size;
    ar(size);
    RawDict dict;
    for (size_t i = 0; i < size; ++i) {
        typename RawDict::key_type exponent;
        typename RawDict::mapped_type coeff;
        ar(exponent);
        load_helper(ar, coeff);
        dict.emplace(std::move(exponent), std::move(coeff));
    }
    return dict;
}
template <typename Archive>
RCP<const Basic> load_basic(Archive &ar, RCP<const UIntPoly> &)
{
    RCP<const Basic> var;
    ar(var);
    return make_rcp<const UIntPoly>(var,
                                    UIntDict(load_poly_dict<map_uint_mpz>(ar)));
}
template <typename Archive>
RCP<const Basic> load_basic(Archive &ar, RCP<const URatPoly> &)
{
    RCP<const Basic> var;
    ar(var);
    return make_rcp<const URatPoly>(var,
                                    URatDict(load_poly_dict<map_uint_mpq>(ar)));
}
template <typename Archive>
RCP<const Basic> load_basic(Archive &ar, RCP<const UExprPoly> &)
{
    RCP<const Basic> var;
    ar(var);
    return make_rcp<const UExprPoly>(
        var, UExprDict(load_poly_dict<map_int_Expr>(ar)));
}
template <typename Archive>
RCP<const Basic> load_basic(Archive &ar, RCP<const MIntPoly> &)
{
    set_basic vars;
    ar(vars);
    auto dict = load_poly_dict<umap_uvec_mpz>(ar);
    return MIntPoly::from_container(
        vars, MIntDict(std::move(dict), static_cast<unsigned>(vars.size())));
}
template <typename Archive>
RCP<const Basic> load_basic(Archive &ar, RCP<const MExprPoly> &)
{
    set_basic vars;
    ar(vars);
    auto dict = load_poly_dict<umap_vec_expr>(ar);
    return MExprPoly::from_container(
        vars, MExprDict(std::move(dict), static_cast<unsigned>(vars.size())));
}
#if defined(HAVE_SYMENGINE_FLINT) || defined(HAVE_SYMENGINE_PIRANHA)
template <typename Poly, typename Cf, typename Archive>
RCP<const Basic> load_dense_poly(Archive &ar)
{
    RCP<const Basic> var;
    unsigned int size;
    ar(var, size);
    std::map<unsigned, Cf> dict;
    for (unsigned int i = 0; i < size; ++i) {
        Cf coeff;
        load_helper(ar, coeff);
        dict.emplace(i, std::move(coeff));
    }
    return Poly::from_dict(var, std::move(dict));
}
#endif
#ifdef HAVE_SYMENGINE_FLINT
template <typename Archive>
RCP<const Basic> load_basic(Archive &ar, RCP<const UIntPolyFlint> &)
{
    return load_dense_poly<UIntPolyFlint, integer_class>(ar);
}
template <typename Archive>
RCP<const Basic> load_basic(Archive &ar, RCP<const URatPolyFlint> &)
{
    return load_dense_poly<URatPolyFlint, rational_class>(ar);
}
#endif
#ifdef HAVE_SYMENGINE_PIRANHA
template <typename Archive>
RCP<const Basic> load_basic(Archive &ar, RCP<const UIntPolyPiranha> &)
{
    return load_dense_poly<UIntPolyPiranha, integer_class>(ar);
}
template <typename Archive>
RCP<const Basic> load_basic(Archive &ar, RCP<const URatPolyPiranha> &)
{
    return load_dense_poly<URatPolyPiranha, rational_class>(ar);
}
#endif
template <class Archive>
RCP<const Basic> load_basic(Archive &ar, RCP<const Integer> &)
{
    integer_class c;
    load_helper(ar, c);
    return integer(std::move(c));
}
template <class Archive>
RCP<const Basic> load_basic(Archive &ar, RCP<const Constant> &)
{
    std::string name;
    ar(name);
    return constant(name);
}
template <class Archive>
RCP<const Basic> load_basic(Archive &ar, RCP<const Rational> &)
{
    RCP<const Integer> num, den;
    ar(num, den);
    return Rational::from_two_ints(*num, *den);
}
template <class Archive>
RCP<const Basic> load_basic(Archive &ar, RCP<const Complex> &)
{
    RCP<const Number> num, den;
    ar(num, den);
    return Complex::from_two_nums(*num, *den);
}
template <class Archive, class T>
RCP<const Basic>
load_basic(Archive &ar, RCP<const T> &,
           typename std::enable_if<std::is_base_of<ComplexBase, T>::value,
                                   int>::type * = nullptr)
{
    RCP<const Number> num, den;
    ar(num, den);
    return addnum(num, mulnum(I, den));
}
template <class Archive>
RCP<const Basic> load_basic(Archive &ar, RCP<const Interval> &)
{
    RCP<const Number> start, end;
    bool left_open, right_open;
    ar(left_open, start, right_open, end);
    return make_rcp<const Interval>(start, end, left_open, right_open);
}
template <class Archive>
RCP<const Basic> load_basic(Archive &ar, RCP<const BooleanAtom> &)
{
    bool val;
    ar(val);
    return boolean(val);
}
template <class Archive>
RCP<const Basic> load_basic(Archive &ar, RCP<const And> &)
{
    set_boolean container;
    ar(container);
    return make_rcp<const And>(std::move(container));
}
template <class Archive>
RCP<const Basic> load_basic(Archive &ar, RCP<const Or> &)
{
    set_boolean container;
    ar(container);
    return make_rcp<const Or>(std::move(container));
}
template <class Archive>
RCP<const Basic> load_basic(Archive &ar, RCP<const Xor> &)
{
    vec_boolean container;
    ar(container);
    return make_rcp<const Xor>(std::move(container));
}
template <class Archive>
RCP<const Basic> load_basic(Archive &ar, RCP<const Not> &)
{
    RCP<const Boolean> arg;
    ar(arg);
    return make_rcp<const Not>(arg);
}
template <class Archive>
RCP<const Basic> load_basic(Archive &ar, RCP<const Piecewise> &)
{
    PiecewiseVec vec;
    ar(vec);
    return make_rcp<const Piecewise>(std::move(vec));
}
template <class Archive>
RCP<const Basic> load_basic(Archive &ar, RCP<const Contains> &)
{
    RCP<const Basic> expr;
    RCP<const Set> contains_set;
    ar(expr, contains_set);
    return make_rcp<const Contains>(expr, contains_set);
}
template <class Archive>
RCP<const Basic> load_basic(Archive &ar, RCP<const Reals> &)
{
    return reals();
}
template <class Archive>
RCP<const Basic> load_basic(Archive &ar, RCP<const Rationals> &)
{
    return rationals();
}
template <class Archive>
RCP<const Basic> load_basic(Archive &ar, RCP<const EmptySet> &)
{
    return emptyset();
}
template <class Archive>
RCP<const Basic> load_basic(Archive &ar, RCP<const Integers> &)
{
    return integers();
}
template <class Archive>
RCP<const Basic> load_basic(Archive &ar, RCP<const UniversalSet> &)
{
    return universalset();
}
template <class Archive>
RCP<const Basic> load_basic(Archive &ar, RCP<const Union> &)
{
    set_set union_set;
    ar(union_set);
    return make_rcp<const Union>(std::move(union_set));
}
template <class Archive>
RCP<const Basic> load_basic(Archive &ar, RCP<const Complement> &)
{
    RCP<const Set> universe, container;
    ar(universe, container);
    return make_rcp<const Complement>(universe, container);
}
template <class Archive>
RCP<const Basic> load_basic(Archive &ar, RCP<const ImageSet> &)
{
    RCP<const Basic> sym, expr;
    RCP<const Set> base;
    ar(sym, expr, base);
    return make_rcp<const ImageSet>(sym, expr, base);
}
template <class Archive>
RCP<const Basic> load_basic(Archive &ar, RCP<const FiniteSet> &)
{
    set_basic set;
    ar(set);
    return make_rcp<const FiniteSet>(set);
}
template <class Archive>
RCP<const Basic> load_basic(Archive &ar, RCP<const ConditionSet> &)
{
    RCP<const Basic> sym;
    RCP<const Boolean> condition;
    ar(sym, condition);
    return make_rcp<const ConditionSet>(sym, condition);
}
#ifdef HAVE_SYMENGINE_MPFR
template <class Archive>
RCP<const Basic> load_basic(Archive &ar, RCP<const RealMPFR> &)
{
    std::string num;
    mpfr_prec_t prec;
    ar(num, prec);
    return make_rcp<const RealMPFR>(mpfr_class(num, prec, 10));
}
#endif
template <class Archive>
RCP<const Basic> load_basic(Archive &ar, RCP<const Derivative> &)
{
    RCP<const Basic> arg;
    multiset_basic set;
    ar(arg, set);
    return make_rcp<const Derivative>(arg, std::move(set));
}
template <class Archive>
RCP<const Basic> load_basic(Archive &ar, RCP<const Subs> &)
{
    RCP<const Basic> arg;
    map_basic_basic dict;
    ar(arg, dict);
    return make_rcp<const Subs>(arg, std::move(dict));
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
           typename std::enable_if<std::is_base_of<TwoArgFunction, T>::value,
                                   int>::type * = nullptr)
{
    RCP<const Basic> arg1, arg2;
    ar(arg1, arg2);
    return make_rcp<const T>(arg1, arg2);
}
template <class Archive>
RCP<const Basic> load_basic(Archive &ar, RCP<const FunctionSymbol> &)
{
    std::string name;
    vec_basic vec;
    ar(name, vec);
    return make_rcp<const FunctionSymbol>(name, std::move(vec));
}
template <class Archive>
RCP<const Basic> load_basic(Archive &ar, RCP<const FunctionWrapper> &)
{
    throw SerializationError(StreamFmt()
                             << __FILE__ << ":" << __LINE__
#ifndef _MSC_VER
                             << ": " << __PRETTY_FUNCTION__
#endif
                             << "Loading of this type is not implemented.");
}
template <class Archive, class T>
RCP<const Basic>
load_basic(Archive &ar, RCP<const T> &,
           typename std::enable_if<std::is_base_of<MultiArgFunction, T>::value,
                                   int>::type * = nullptr)
{
    vec_basic args;
    ar(args);
    return make_rcp<const T>(std::move(args));
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
                                or std::is_base_of<ComplexBase, T>::value
                                or std::is_base_of<OneArgFunction, T>::value
                                or std::is_base_of<MultiArgFunction, T>::value
                                or std::is_base_of<TwoArgFunction, T>::value),
                            int>::type * = nullptr)
{
    throw SerializationError(StreamFmt()
                             << __FILE__ << ":" << __LINE__
#ifndef _MSC_VER
                             << ": " << __PRETTY_FUNCTION__
#endif
                             << "Loading of this type is not implemented.");
}

template <class Archive>
inline void save_typeid(Archive &ar, TypeID &t)
{
    uint8_t i = t;
    static_assert(TypeID::TypeID_Count < (1 << 8),
                  "TypeID cannot be saved to a 8 bit int.");
    ar(i);
}

template <class Archive>
inline void load_typeid(Archive &ar, TypeID &t)
{
    uint8_t i;
    ar(i);
    if (i >= TypeID::TypeID_Count) {
        throw SerializationError("TypeID out of range");
    }
    t = static_cast<TypeID>(i);
}

//! Loading for SymEngine::RCP
template <class Archive, class T>
inline void CEREAL_LOAD_FUNCTION_NAME(Archive &ar, RCP<const T> &ptr)
{
    RCPBasicAwareInputArchive<Archive> *ar_ptr
        = dynamic_cast<RCPBasicAwareInputArchive<Archive> *>(&ar);
    if (not ar_ptr) {
        throw SerializationError("Need a RCPBasicAwareInputArchive");
    }
    ptr = ar_ptr->template load_rcp_basic<T>();
}
} // namespace SymEngine
#endif // SYMENGINE_SERIALIZE_CEREAL_H
