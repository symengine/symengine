#ifndef CSYMPY_DICT_H
#define CSYMPY_DICT_H

#include "basic.h"
#include "integer.h"

namespace CSymPy {

// This implements a "dictionary" of CSymPy objects:

// Our hash:
typedef struct
{
    long operator() (const Teuchos::RCP<Basic> &k) const {
        return k->__hash__();
    }
} RCPBasicHash;

// Our comparison (==):
typedef struct
{
    bool operator() (const Teuchos::RCP<Basic> &x, const Teuchos::RCP<Basic> &y) const {
        return x->__eq__(*y);
    }
} RCPBasicKeyEq;

// Our less operator (<):
typedef struct
{
    // true if x < y, false otherwise
    bool operator() (const Teuchos::RCP<Basic> &x, const Teuchos::RCP<Basic> &y) const {
        if (x->__eq__(*y)) return false;
        // Just compare pointer memory values (platform dependent):
        return x.getRawPtr() < y.getRawPtr();
    }
} RCPBasicKeyLess;


typedef std::unordered_map<Teuchos::RCP<Basic>, Teuchos::RCP<Number>,
        RCPBasicHash, RCPBasicKeyEq> umap_basic_int;
typedef std::unordered_map<Teuchos::RCP<Basic>, Teuchos::RCP<Basic>,
        RCPBasicHash, RCPBasicKeyEq> umap_basic_basic;

typedef std::vector<int> vec_int;
typedef std::map<vec_int, long long int> map_vec_int;
typedef std::map<vec_int, mpz_class> map_vec_mpz;
typedef std::map<Teuchos::RCP<Basic>, Teuchos::RCP<Number>,
        RCPBasicKeyLess> map_basic_int;
typedef std::map<Teuchos::RCP<Basic>, Teuchos::RCP<Basic>,
        RCPBasicKeyLess> map_basic_basic;

// Return true if the two dictionaries 'a' and 'b' are equal. Otherwise false.
bool dicts_equal(const umap_basic_int &a, const umap_basic_int &b);
bool map_basic_int_equal(const map_basic_int &a, const map_basic_int &b);
bool map_basic_basic_equal(const map_basic_basic &a, const map_basic_basic &b);
bool umap_basic_basic_equal(const umap_basic_basic &a,
        const umap_basic_basic &b);

// Return -1, 0, 1 for a < b, a == b, a > b
int map_basic_basic_compare(const map_basic_basic &a, const map_basic_basic &b);



// Part of umap_vec_mpz:
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

#endif
