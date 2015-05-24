/**
 *  \file dict.h
 *  Dictionary implementation
 *
 **/


#ifndef SYMENGINE_DICT_H
#define SYMENGINE_DICT_H

#include <gmpxx.h>

namespace SymEngine {

class Basic;
class Number;
class Integer;
struct RCPBasicHash;
struct RCPBasicKeyEq;
struct RCPBasicKeyLess;
struct RCPIntegerKeyLess;

typedef std::unordered_map<RCP<const Basic>, RCP<const Number>,
        RCPBasicHash, RCPBasicKeyEq> umap_basic_num;
typedef std::unordered_map<RCP<const Basic>, RCP<const Basic>,
        RCPBasicHash, RCPBasicKeyEq> umap_basic_basic;

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
typedef std::map<unsigned, RCP<const Integer> >
       map_uint_integer;

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


//! \return -1, 0, 1 for a < b, a == b, a > b
int map_basic_basic_compare(const map_basic_basic &a, const map_basic_basic &b);
//! \return -1, 0, 1 for a < b, a == b, a > b
int map_basic_num_compare(const map_basic_num &a, const map_basic_num &b);
//! \return -1, 0, 1 for a < b, a == b, a > b
int vec_basic_compare(const vec_basic &a, const vec_basic &b);


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

#endif

