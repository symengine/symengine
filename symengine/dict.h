/**
 *  \file dict.h
 *  Dictionary implementation
 *
 **/

#ifndef SYMENGINE_DICT_H
#define SYMENGINE_DICT_H
#include <symengine/symengine_config.h>
#include <symengine/mp_class.h>

namespace SymEngine
{

class Basic;
class Number;
class Integer;
class Expression;
class Symbol;
struct RCPBasicHash;
struct RCPBasicKeyEq;
struct RCPBasicKeyLess;
struct RCPIntegerKeyLess;
struct RCPSymbolHash;
struct RCPSymbolCompare;
struct RCPSymbolEq;

typedef std::unordered_map<RCP<const Basic>, RCP<const Number>, RCPBasicHash,
                           RCPBasicKeyEq> umap_basic_num;
typedef std::unordered_map<short, RCP<const Basic>> umap_short_basic;
typedef std::unordered_map<int, RCP<const Basic>> umap_int_basic;
typedef std::unordered_map<RCP<const Basic>, RCP<const Basic>, RCPBasicHash,
                           RCPBasicKeyEq> umap_basic_basic;

typedef std::vector<int> vec_int;
typedef std::vector<RCP<const Basic>> vec_basic;
typedef std::vector<RCP<const Integer>> vec_integer;
typedef std::set<RCP<const Basic>, RCPBasicKeyLess> set_basic;
typedef std::multiset<RCP<const Basic>, RCPBasicKeyLess> multiset_basic;
typedef std::map<vec_int, long long int> map_vec_int;
typedef std::map<vec_int, integer_class> map_vec_mpz;
typedef std::map<RCP<const Basic>, RCP<const Number>, RCPBasicKeyLess>
    map_basic_num;
typedef std::map<RCP<const Basic>, RCP<const Basic>, RCPBasicKeyLess>
    map_basic_basic;
typedef std::map<RCP<const Integer>, unsigned, RCPIntegerKeyLess>
    map_integer_uint;
typedef std::map<unsigned, integer_class> map_uint_mpz;
typedef std::map<int, Expression> map_int_Expr;

//! `insert(m, first, second)` is equivalent to `m[first] = second`, just
//! faster,
//! because no default constructor is called on the `second` type.
template <typename T1, typename T2, typename T3>
inline void insert(T1 &m, const T2 &first, const T3 &second)
{
    m.insert(std::pair<T2, T3>(first, second));
}

//! \return true if the two dictionaries `a` and `b` are equal. Otherwise false.
template <class T>
bool umap_eq(const T &a, const T &b)
{
    // This follows the same algorithm as Python's dictionary comparison
    // (a==b), which is implemented by "dict_equal" function in
    // Objects/dictobject.c.

    // Can't be equal if # of entries differ:
    if (a.size() != b.size())
        return false;
    // Loop over keys in "a":
    for (const auto &p : a) {
        // O(1) lookup of the key in "b":
        auto f = b.find(p.first);
        if (f == b.end())
            return false; // no such element in "b"
        if (neq(*p.second, *f->second))
            return false; // values not equal
    }
    return true;
}

//! \return true if the two dictionaries `a` and `b` are equal. Otherwise false.
template <class T>
bool map_eq(const T &A, const T &B)
{
    // Can't be equal if # of entries differ:
    if (A.size() != B.size())
        return false;
    // Loop over keys in "a":
    auto a = A.begin();
    auto b = B.begin();
    for (; a != A.end(); ++a, ++b) {
        if (neq(*a->first, *b->first))
            return false; // keys not equal
        if (neq(*a->second, *b->second))
            return false; // values not equal
    }
    return true;
}

//! \return true if the two vectors `a` and `b` are equal. Otherwise false.
bool vec_basic_eq(const vec_basic &a, const vec_basic &b);
//! \return true if the two vectors `a` and `b` are equal up to a permutation.
//! Otherwise false.
bool vec_basic_eq_perm(const vec_basic &a, const vec_basic &b);
//! \return true if the two dictionaries `a` and `b` are equal. Otherwise false.
bool map_uint_mpz_eq(const map_uint_mpz &a, const map_uint_mpz &b);
bool map_int_Expr_eq(const map_int_Expr &a, const map_int_Expr &b);
//! \return true if the two multisets `a` and `b` are equal. Otherwise false.
bool multiset_basic_eq(const multiset_basic &a, const multiset_basic &b);

//! \return -1, 0, 1 for a < b, a == b, a > b
template <class T>
int map_compare(const T &A, const T &B)
{
    if (A.size() != B.size())
        return (A.size() < B.size()) ? -1 : 1;
    auto a = A.begin();
    auto b = B.begin();
    int cmp;
    for (; a != A.end(); ++a, ++b) {
        cmp = a->first->__cmp__(*b->first);
        if (cmp != 0)
            return cmp;
        cmp = a->second->__cmp__(*b->second);
        if (cmp != 0)
            return cmp;
    }
    return 0;
}

//! \return -1, 0, 1 for a < b, a == b, a > b
int vec_basic_compare(const vec_basic &a, const vec_basic &b);
//! \return -1, 0, 1 for a < b, a == b, a > b
int multiset_basic_compare(const multiset_basic &a, const multiset_basic &b);
//! \return -1, 0, 1 for a < b, a == b, a > b
int map_uint_mpz_compare(const map_uint_mpz &a, const map_uint_mpz &b);
int map_int_Expr_compare(const map_int_Expr &a, const map_int_Expr &b);

//! Part of umap_vec_mpz:
typedef struct {
    inline std::size_t operator()(const vec_int &k) const
    {
        std::size_t h = 0;
        for (const auto &p : k) {
            h = (h << 4) + p;
        }
        return h;
    }
} vec_int_hash;

typedef std::unordered_map<vec_int, integer_class, vec_int_hash> umap_vec_mpz;

int umap_vec_mpz_compare(const umap_vec_mpz &a, const umap_vec_mpz &b);
long mpz_hash(const integer_class z);

typedef std::vector<unsigned int> vec_uint;

class vec_uint_hash{
public:
    std::size_t operator()(const vec_uint &v) const {
        std::size_t h = 0;
        for (unsigned int i : v) {
            h ^= i + 0x9e3779b + (h << 6) + (h >> 2);
        }
        return h;
    }
};

class vec_uint_eq{
public:
    bool operator()(const vec_uint &a, const vec_uint &b) const{
        if (a.size() != b.size())
            return false;
        for (unsigned int i = 0; i < a.size(); i++) {
            if(a[i] != b[i])
              return false;
        }
        return true;
    }
};

class vec_uint_compare{
public:
    bool operator()(const vec_uint &a, const vec_uint &b) const{
        if (a.size() != b.size())
            return a.size() < b.size();
        unsigned int sum1 = 0;
        unsigned int sum2 = 0;
        for (unsigned int x : a) {
            sum1 += x;
        }
        for (unsigned int x : b) {
            sum2 += x;
        }
        if (sum1 != sum2)
            return sum1 < sum2;
        return a < b;
    }
};

typedef std::set< RCP<const Symbol>, RCPSymbolCompare> set_sym;
typedef std::unordered_map<RCP<const Symbol>, unsigned int, RCPSymbolHash, RCPSymbolEq>
    umap_sym_uint;
typedef std::unordered_map<vec_uint, integer_class, vec_uint_hash, vec_uint_eq> umap_uvec_mpz;
typedef std::unordered_map<vec_uint,Expression, vec_uint_hash, vec_uint_eq> umap_uvec_expr;

//Takes an unordered map of type M with key type K and returns a vector of K ordered by C.
template<class K, class M, class C>
std::vector<K> order_umap(const M &d) {
    std::vector<K> v;
    for (auto bucket : d) {
        auto iter = v.begin();
        while(iter != v.end() && C()(bucket.first,*iter)){
            iter++;
        }
        v.insert(iter, bucket.first);
    }
    return v;
}

int umap_uvec_mpz_compare(const umap_uvec_mpz &a, const umap_uvec_mpz &b);

//copied from umap_eq, with derefrencing of image in map removed.
bool umap_uvec_mpz_eq(const umap_uvec_mpz &a, const umap_uvec_mpz &b);

int umap_uvec_expr_compare(const umap_uvec_expr &a, const umap_uvec_expr &b);
bool umap_uvec_expr_eq(const umap_uvec_expr &a, const umap_uvec_expr &b);

template<class T>
bool set_eq(const T &A, const T &B)
{
    // Can't be equal if # of entries differ:
    if (A.size() != B.size()) return false;
    // Loop over elements in "a" and "b":
    auto a = A.begin();
    auto b = B.begin();
    for (; a != A.end(); ++a, ++b) {
        if (neq(**a, **b)) return false; // values not equal
    }
    return true;
}

template<class T>
int set_compare(const T &A, const T &B)
{
    if (A.size() != B.size())
        return (A.size() < B.size()) ? -1 : 1;
    auto a = A.begin();
    auto b = B.begin();
    int cmp;
    for (; a != A.end(); ++a, ++b) {
        cmp = (*a)->__cmp__(**b);
        if (cmp != 0) return cmp;
    }
    return 0;
}

std::ostream &operator<<(std::ostream &out, const SymEngine::umap_basic_num &d);
std::ostream &operator<<(std::ostream &out, const SymEngine::map_basic_num &d);
std::ostream &operator<<(std::ostream &out,
                         const SymEngine::map_basic_basic &d);
std::ostream &operator<<(std::ostream &out,
                         const SymEngine::umap_basic_basic &d);
std::ostream &operator<<(std::ostream &out, const SymEngine::vec_basic &d);
std::ostream &operator<<(std::ostream &out, const SymEngine::set_basic &d);

} // SymEngine

#endif
