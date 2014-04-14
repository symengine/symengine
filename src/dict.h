/**
 *  \file dict.h
 *  Dictionary implementation
 *
 **/


#ifndef CSYMPY_DICT_H
#define CSYMPY_DICT_H

#include <gmpxx.h>

namespace CSymPy {

class Basic;
class Number;
class Integer;
class RCPBasicHash;
class RCPBasicKeyEq;
class RCPBasicKeyLess;
class RCPIntegerKeyLess;

typedef std::unordered_map<RCP<const Basic>, RCP<const Number>,
        RCPBasicHash, RCPBasicKeyEq> umap_basic_int;
typedef std::unordered_map<RCP<const Basic>, RCP<const Basic>,
        RCPBasicHash, RCPBasicKeyEq> umap_basic_basic;

typedef std::vector<int> vec_int;
typedef std::vector<RCP<const Basic>> vec_basic;
typedef std::map<vec_int, long long int> map_vec_int;
typedef std::map<vec_int, mpz_class> map_vec_mpz;
typedef std::map<RCP<const Basic>, RCP<const Number>,
        RCPBasicKeyLess> map_basic_int;
typedef std::map<RCP<const Basic>, RCP<const Basic>,
        RCPBasicKeyLess> map_basic_basic;
typedef std::map<RCP<const Integer>, unsigned,
        RCPIntegerKeyLess> map_integer_uint;

//! `insert(m, first, second)` is equivalent to `m[first] = second`, just faster,
//! because no default constructor is called on the `second` type.
template<typename T1, typename T2, typename T3> inline
void insert(T1 &m, const T2 &first, const T3 &second) {
    m.insert(std::pair<T2, T3>(first, second));
}

//! \return true if the two dictionaries `a` and `b` are equal. Otherwise false.
bool umap_basic_int_equal(const umap_basic_int &a, const umap_basic_int &b);
//! \return true if the two dictionaries `a` and `b` are equal. Otherwise false.
bool map_basic_int_equal(const map_basic_int &a, const map_basic_int &b);
//! \return true if the two dictionaries `a` and `b` are equal. Otherwise false.
bool map_basic_basic_equal(const map_basic_basic &a, const map_basic_basic &b);
//! \return true if the two dictionaries `a` and `b` are equal. Otherwise false.
bool umap_basic_basic_equal(const umap_basic_basic &a,
        const umap_basic_basic &b);
//! \return true if the two vectors `a` and `b` are equal. Otherwise false.
bool vec_basic_equal(const vec_basic &a, const vec_basic &b);
int vec_basic_compare(const vec_basic &A, const vec_basic &B);


//! \return -1, 0, 1 for a < b, a == b, a > b
int map_basic_basic_compare(const map_basic_basic &a, const map_basic_basic &b);
//! \return -1, 0, 1 for a < b, a == b, a > b
int map_basic_int_compare(const map_basic_int &a, const map_basic_int &b);

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

} // CSymPy


std::ostream& operator<<(std::ostream& out, const CSymPy::umap_basic_int& d);
std::ostream& operator<<(std::ostream& out, const CSymPy::vec_int& d);
std::ostream& operator<<(std::ostream& out, const CSymPy::map_vec_int& d);
std::ostream& operator<<(std::ostream& out, const CSymPy::map_vec_mpz& d);
std::ostream& operator<<(std::ostream& out, const CSymPy::umap_vec_mpz& d);
std::ostream& operator<<(std::ostream& out, const CSymPy::map_basic_int& d);
std::ostream& operator<<(std::ostream& out, const CSymPy::map_basic_basic& d);
std::ostream& operator<<(std::ostream& out, const CSymPy::umap_basic_basic& d);
std::ostream& operator<<(std::ostream& out, const CSymPy::vec_basic& d);

#endif

