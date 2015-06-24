/**
 *  \file dict.h
 *  Dictionary implementation
 *
 **/


#ifndef SYMENGINE_DICT_H
#define SYMENGINE_DICT_H

#include <gmpxx.h>

#include <piranha/piranha.hpp>
#include <boost/functional/hash.hpp>

namespace std
{
template <>
struct hash<std::pair<unsigned long long, piranha::integer>>
{
    typedef size_t result_type;
    typedef std::pair<unsigned long long,piranha::integer> argument_type;
    result_type operator()(const argument_type &p) const noexcept
    {
             return std::hash<unsigned long long>()(p.first);
    }
};
}

namespace SymEngine {

class Basic;
class Number;
class Integer;
struct RCPBasicHash;
struct RCPBasicKeyEq;
struct RCPBasicKeyLess;
struct RCPIntegerKeyLess;

struct my_pair
{
    long long first;
    mutable piranha::integer second;
};

typedef struct
{
    typedef size_t result_type;
    typedef my_pair argument_type;
    result_type operator()(const argument_type &p) const noexcept
    {
             return std::hash<long long>()(p.first);
    }
} my_hash;

typedef struct
{
    //! \return true if `x==y`
    inline bool operator() (const my_pair &x, const my_pair &y) const {
        return x.first == y.first;
    }
} hash_eq;

typedef std::unordered_map<RCP<const Basic>, RCP<const Number>,
        RCPBasicHash, RCPBasicKeyEq> umap_basic_num;
typedef std::unordered_map<RCP<const Basic>, RCP<const Basic>,
        RCPBasicHash, RCPBasicKeyEq> umap_basic_basic;
typedef std::unordered_map<long long, piranha::integer> umap_ull_mpz;
typedef piranha::hash_set<my_pair, my_hash, hash_eq> hash_set;

typedef std::vector<int> vec_int;
typedef std::vector<RCP<const Basic>> vec_basic;
typedef std::vector<RCP<const Integer>> vec_integer;
typedef std::set<RCP<const Basic>, RCPBasicKeyLess> set_basic;
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

//! \return true if the two dictionaries `a` and `b` are equal. Otherwise false.
bool umap_basic_num_eq(const umap_basic_num &a, const umap_basic_num &b);
//! \return true if the two dictionaries `a` and `b` are equal. Otherwise false.
bool map_basic_num_eq(const map_basic_num &a, const map_basic_num &b);
//! \return true if the two dictionaries `a` and `b` are equal. Otherwise false.
bool map_basic_basic_eq(const map_basic_basic &a, const map_basic_basic &b);
//! \return true if the two dictionaries `a` and `b` are equal. Otherwise false.
bool umap_basic_basic_eq(const umap_basic_basic &a,
        const umap_basic_basic &b);
//! \return true if the two vectors `a` and `b` are equal. Otherwise false.
bool vec_basic_eq(const vec_basic &a, const vec_basic &b);
//! \return true if the two vectors `a` and `b` are equal up to a permutation. Otherwise false.
bool vec_basic_eq_perm(const vec_basic &a, const vec_basic &b);
//! \return true if the two dictionaries `a` and `b` are equal. Otherwise false.
bool map_uint_mpz_eq(const map_uint_mpz &a, const map_uint_mpz &b);

//! \return -1, 0, 1 for a < b, a == b, a > b
int map_basic_basic_compare(const map_basic_basic &a, const map_basic_basic &b);
//! \return -1, 0, 1 for a < b, a == b, a > b
int map_basic_num_compare(const map_basic_num &a, const map_basic_num &b);
//! \return -1, 0, 1 for a < b, a == b, a > b
int vec_basic_compare(const vec_basic &a, const vec_basic &b);
//! \return -1, 0, 1 for a < b, a == b, a > b
int map_uint_mpz_compare(const map_uint_mpz &a, const map_uint_mpz &b);

//! kronecker packing of exponents
unsigned long long pack_vec_int(const vec_int &a);
//! check if the packing is possible
bool if_pack_vec_int(const vec_int &a);

//! Part of umap_vec_mpz:
typedef struct
{
    inline std::size_t operator() (const vec_int &k) const {
        std::size_t h = 0;
        for (auto &p: k) {
            h = (h << 4) + p;
        }
        return h;
    }
} vec_int_hash;

typedef struct
{
    //! \return true if `x==y`
    inline bool operator() (const vec_int &x, const vec_int &y) const {
        return x == y;
    }
} vec_int_eq;

typedef std::unordered_map<vec_int, mpz_class,
        vec_int_hash, vec_int_eq> umap_vec_mpz;

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
std::ostream& operator<<(std::ostream& out, const SymEngine::umap_ull_mpz& d);

#endif

