/**
 *  \file dict.h
 *  Dictionary implementation
 *
 **/


#ifndef SYMENGINE_DICT_H
#define SYMENGINE_DICT_H

#define __GMPXX_USE_CXX11 1
#include <gmpxx.h>
#if defined(HAVE_SYMENGINE_PIRANHA)
#include <piranha/piranha.hpp>
#endif

namespace SymEngine {

class Basic;
class Number;
class Integer;
class Symbol;
struct RCPBasicHash;
struct RCPBasicKeyEq;
struct RCPBasicKeyLess;
struct RCPIntegerKeyLess;

typedef std::unordered_map<RCP<const Basic>, RCP<const Number>,
        RCPBasicHash, RCPBasicKeyEq> umap_basic_num;
typedef std::unordered_map<short, RCP<const Basic>> umap_short_basic;
typedef std::unordered_map<RCP<const Basic>, RCP<const Basic>,
        RCPBasicHash, RCPBasicKeyEq> umap_basic_basic;

typedef std::vector<int> vec_int;
typedef std::vector<RCP<const Basic>> vec_basic;
typedef std::vector<RCP<const Integer>> vec_integer;
typedef std::vector<RCP<const Symbol>> vec_symbol;
typedef std::set<RCP<const Basic>, RCPBasicKeyLess> set_basic;
typedef std::multiset<RCP<const Basic>, RCPBasicKeyLess> multiset_basic;
typedef std::map<vec_int, long long int> map_vec_int;
typedef std::map<vec_int, mpz_class> map_vec_mpz;
typedef std::map<RCP<const Basic>, RCP<const Number>,
        RCPBasicKeyLess> map_basic_num;
typedef std::map<RCP<const Basic>, RCP<const Basic>,
        RCPBasicKeyLess> map_basic_basic;
typedef std::map<RCP<const Integer>, unsigned,
        RCPIntegerKeyLess> map_integer_uint;
typedef std::map<unsigned, mpz_class>
       map_uint_mpz;

//! `insert(m, first, second)` is equivalent to `m[first] = second`, just faster,
//! because no default constructor is called on the `second` type.
template<typename T1, typename T2, typename T3> inline
void insert(T1 &m, const T2 &first, const T3 &second) {
    m.insert(std::pair<T2, T3>(first, second));
}

//! Definition of pair, for the Polynomial terms
struct m_pair
{
    long long first;
#if defined(HAVE_SYMENGINE_PIRANHA)
    mutable piranha::integer second;
#else
    mutable mpz_class second;
#endif
};

typedef struct
{
    typedef size_t result_type;
    typedef m_pair argument_type;
    result_type operator()(const argument_type &p) const noexcept
    {
             return std::hash<long long>()(p.first);
    }
} pair_hash;

typedef struct
{
    //! \return true if `x==y`
    inline bool operator() (const m_pair &x, const m_pair &y) const {
        return x.first == y.first;
    }
} pair_eq;

#if defined(HAVE_SYMENGINE_PIRANHA)
//! hash_set container from Piranha
typedef piranha::hash_set<m_pair, pair_hash, pair_eq> hash_set;
#else
typedef std::unordered_set<m_pair, pair_hash, pair_eq> unordered_set;

long long vec_encode(const vec_int&);

vec_int vec_decode(const int);
#endif

#if defined(HAVE_SYMENGINE_PIRANHA)
//! \return true if the two hash_set `a` and `b` are equal. Otherwise false.
bool hash_set_eq(const hash_set &a, const hash_set &b);
//! \return -1, 0, 1 for a <b, a == b, a > b
int hash_set_compare(const hash_set &A, const hash_set &B);
#else
bool unordered_set_eq(const unordered_set &a, const unordered_set &b);

#endif

//! \return true if the two dictionaries `a` and `b` are equal. Otherwise false.
template<class T>
bool umap_eq(const T &a, const T &b)
{
    // This follows the same algorithm as Python's dictionary comparison
    // (a==b), which is implemented by "dict_equal" function in
    // Objects/dictobject.c.

    // Can't be equal if # of entries differ:
    if (a.size() != b.size()) return false;
    // Loop over keys in "a":
    for (const auto &p: a) {
        // O(1) lookup of the key in "b":
        auto f = b.find(p.first);
        if (f == b.end()) return false; // no such element in "b"
        if (neq(*p.second, *f->second)) return false; // values not equal
    }
    return true;
}

//! \return true if the two dictionaries `a` and `b` are equal. Otherwise false.
template<class T>
bool map_eq(const T &A, const T &B)
{
    // Can't be equal if # of entries differ:
    if (A.size() != B.size()) return false;
    // Loop over keys in "a":
    auto a = A.begin();
    auto b = B.begin();
    for (; a != A.end(); ++a, ++b) {
        if (neq(*a->first, *b->first)) return false; // keys not equal
        if (neq(*a->second, *b->second)) return false; // values not equal
    }
    return true;
}

//! \return true if the two vectors `a` and `b` are equal. Otherwise false.
bool vec_basic_eq(const vec_basic &a, const vec_basic &b);
//! \return true if the two symbol vectors `a` and `b` are equal. Otherwise false.
bool vec_symbol_eq(const vec_symbol &a, const vec_symbol &b);
//! \return true if the two vectors `a` and `b` are equal up to a permutation. Otherwise false.
bool vec_basic_eq_perm(const vec_basic &a, const vec_basic &b);
//! \return true if the two dictionaries `a` and `b` are equal. Otherwise false.
bool map_uint_mpz_eq(const map_uint_mpz &a, const map_uint_mpz &b);
//! \return true if the two multisets `a` and `b` are equal. Otherwise false.
bool multiset_basic_eq(const multiset_basic &a, const multiset_basic &b);


//! \return -1, 0, 1 for a < b, a == b, a > b
template<class T>
int map_compare(const T &A, const T &B)
{
    if (A.size() != B.size())
        return (A.size() < B.size()) ? -1 : 1;
    auto a = A.begin();
    auto b = B.begin();
    int cmp;
    for (; a != A.end(); ++a, ++b) {
        cmp = a->first->__cmp__(*b->first);
        if (cmp != 0) return cmp;
        cmp = a->second->__cmp__(*b->second);
        if (cmp != 0) return cmp;
    }
    return 0;
}

//! \return -1, 0, 1 for a < b, a == b, a > b
int vec_basic_compare(const vec_basic &a, const vec_basic &b);
//! \return -1, 0, 1 for a < b, a == b, a > b
int multiset_basic_compare(const multiset_basic &a, const multiset_basic &b);
//! \return -1, 0, 1 for a < b, a == b, a > b
int map_uint_mpz_compare(const map_uint_mpz &a, const map_uint_mpz &b);


//! Part of umap_vec_mpz:
typedef struct
{
    inline std::size_t operator() (const vec_int &k) const {
        std::size_t h = 0;
        for (const auto &p: k) {
            h = (h << 4) + p;
        }
        return h;
    }
} vec_int_hash;

typedef std::unordered_map<vec_int, mpz_class,
        vec_int_hash> umap_vec_mpz;

} // SymEngine


std::ostream& operator<<(std::ostream& out, const SymEngine::umap_basic_num& d);
std::ostream& operator<<(std::ostream& out, const SymEngine::vec_int& d);
std::ostream& operator<<(std::ostream& out, const SymEngine::map_vec_int& d);
std::ostream& operator<<(std::ostream& out, const SymEngine::map_vec_mpz& d);
std::ostream& operator<<(std::ostream& out, const SymEngine::umap_vec_mpz& d);
std::ostream& operator<<(std::ostream& out, const SymEngine::map_basic_num& d);
std::ostream& operator<<(std::ostream& out, const SymEngine::map_basic_basic& d);
std::ostream& operator<<(std::ostream& out, const SymEngine::umap_basic_basic& d);
std::ostream& operator<<(std::ostream& out, const SymEngine::vec_basic& d);
std::ostream& operator<<(std::ostream& out, const SymEngine::set_basic& d);
#if defined(HAVE_SYMENGINE_PIRANHA)
std::ostream& operator<<(std::ostream& out, const SymEngine::hash_set& d);
#endif

#endif
