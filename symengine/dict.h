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
struct RCPBasicHash;
struct RCPBasicKeyEq;
struct RCPBasicKeyLess;
struct RCPIntegerKeyLess;

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

//! `insert(m, first, second)` is equivalent to `m[first] = second`, just
//! faster,
//! because no default constructor is called on the `second` type.
template <typename T1, typename T2, typename T3>
inline void insert(T1 &m, const T2 &first, const T3 &second)
{
    m.insert(std::pair<T2, T3>(first, second));
}

//! eq function base
//! \return true if the two dictionaries `a` and `b` are equal. Otherwise false
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

template <class T>
bool vec_set_eq(const T &A, const T &B)
{
    // Can't be equal if # of entries differ:
    if (A.size() != B.size())
        return false;
    // Loop over elements in "a" and "b":
    auto a = A.begin();
    auto b = B.begin();
    for (; a != A.end(); ++a, ++b) {
        if (neq(**a, **b))
            return false; // values not equal
    }
    return true;
}

//! compare functions base (might also need umap_compare in the future)
//! \return -1, 0, 1 for a < b, a == b, a > b
template <class T>
int vec_set_compare(const T &A, const T &B)
{
    if (A.size() != B.size())
        return (A.size() < B.size()) ? -1 : 1;
    auto a = A.begin();
    auto b = B.begin();
    int cmp;
    for (; a != A.end(); ++a, ++b) {
        cmp = (*a)->__cmp__(**b);
        if (cmp != 0)
            return cmp;
    }
    return 0;
}

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

//! derived from bse functions
bool vec_basic_eq(const vec_basic &a, const vec_basic &b);
bool multiset_basic_eq(const multiset_basic &a, const multiset_basic &b);

int vec_basic_compare(const vec_basic &a, const vec_basic &b);
int multiset_basic_compare(const multiset_basic &a, const multiset_basic &b);

//! map functions
int map_uint_mpz_compare(const map_uint_mpz &a, const map_uint_mpz &b);
int map_int_Expr_compare(const map_int_Expr &a, const map_int_Expr &b);

//! misc functions
bool vec_basic_eq_perm(const vec_basic &a, const vec_basic &b);

//! print functions
std::ostream &operator<<(std::ostream &out, const SymEngine::umap_basic_num &d);
std::ostream &operator<<(std::ostream &out, const SymEngine::map_basic_num &d);
std::ostream &operator<<(std::ostream &out,
                         const SymEngine::map_basic_basic &d);
std::ostream &operator<<(std::ostream &out,
                         const SymEngine::umap_basic_basic &d);
std::ostream &operator<<(std::ostream &out, const SymEngine::vec_basic &d);
std::ostream &operator<<(std::ostream &out, const SymEngine::set_basic &d);
std::ostream &operator<<(std::ostream &out, const SymEngine::map_int_Expr &d);

// dict wrapper
template <typename Key, typename Value, typename Wrapper>
class ODictWrapper
{
public:
    std::map<Key, Value> dict_;

public:
    ODictWrapper() SYMENGINE_NOEXCEPT
    {
    }
    ~ODictWrapper() SYMENGINE_NOEXCEPT
    {
    }

    ODictWrapper(const int &i)
    {
        if (i != 0)
            dict_ = {{0, Value(i)}};
    }

    ODictWrapper(const std::map<Key, Value> &p)
    {
        for (auto &iter : p) {
            if (iter.second != Value(0))
                dict_[iter.first] = iter.second;
        }
    }

    ODictWrapper(const Value &p)
    {
        if (p != Value(0))
            dict_[0] = p;
    }

    Wrapper &operator=(Wrapper &&other) SYMENGINE_NOEXCEPT
    {
        if (this != &other)
            dict_ = std::move(other.dict_);
        return static_cast<Wrapper &>(*this);
    }

    friend Wrapper operator+(const Wrapper &a, const Wrapper &b)
    {
        Wrapper c = a;
        c += b;
        return c;
    }

    Wrapper &operator+=(const Wrapper &other)
    {
        for (auto &iter : other.dict_) {
            auto t = dict_.lower_bound(iter.first);
            if (t != dict_.end() and t->first == iter.first) {
                t->second += iter.second;
                if (t->second == 0) {
                    dict_.erase(t);
                }
            } else {
                dict_.insert(t, {iter.first, iter.second});
            }
        }
        return static_cast<Wrapper &>(*this);
    }

    friend Wrapper operator-(const Wrapper &a, const Wrapper &b)
    {
        Wrapper c = a;
        c -= b;
        return c;
    }

    Wrapper operator-() const
    {
        ODictWrapper c = *this;
        for (auto &iter : c.dict_)
            iter.second *= -1;
        return static_cast<Wrapper &>(c);
    }

    Wrapper &operator-=(const Wrapper &other)
    {
        for (auto &iter : other.dict_) {
            auto t = dict_.lower_bound(iter.first);
            if (t != dict_.end() and t->first == iter.first) {
                t->second -= iter.second;
                if (t->second == 0) {
                    dict_.erase(t);
                }
            } else {
                dict_.insert(t, {iter.first, -iter.second});
            }
        }
        return static_cast<Wrapper &>(*this);
    }

    friend Wrapper operator*(const Wrapper &a, const Wrapper &b)
    {
        Wrapper c = a;
        c *= b;
        return c;
    }

    Wrapper &operator*=(const Wrapper &other)
    {
        // are these checks really needed? they'll be handled
        // automatically

        // if (dict_.empty())
        //     return static_cast<Wrapper &>(*this);

        // if (other.dict_.empty()) {
        //     *this = other;
        //     return static_cast<Wrapper &>(*this);
        // }

        // //! other is a just constant term
        // if (other.dict_.size() == 1
        //     and other.dict_.find(0) != other.dict_.end()) {
        //     for (const auto &i1 : dict_)
        //         for (const auto &i2 : other.dict_)
        //             dict_[i1.first + i2.first] = i1.second * i2.second;
        //     return static_cast<Wrapper &>(*this);
        // }

        std::map<Key, Value> p;
        for (const auto &i1 : dict_)
            for (const auto &i2 : other.dict_)
                p[i1.first + i2.first] += i1.second * i2.second;

        *this = Wrapper(p);
        return static_cast<Wrapper &>(*this);
    }

    bool operator==(const Wrapper &other) const
    {
        return dict_ == other.dict_;
    }

    bool operator!=(const Wrapper &other) const
    {
        return not(*this == other);
    }

    const std::map<Key, Value> &get_dict() const
    {
        return dict_;
    }

    unsigned int size() const
    {
        return dict_.size();
    }

    bool empty() const
    {
        return dict_.empty();
    }
};
} // SymEngine

#endif
