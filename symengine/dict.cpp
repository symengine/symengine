#include <symengine/basic.h>
#include <symengine/integer.h>
#include <symengine/expression.h>

namespace SymEngine
{

namespace
{
template <class T>
inline std::ostream &print_map(std::ostream &out, T &d)
{
    out << "{";
    for (auto p = d.begin(); p != d.end(); p++) {
        if (p != d.begin())
            out << ", ";
        out << (p->first) << ": " << (p->second);
    }
    out << "}";
    return out;
}

template <class T>
inline std::ostream &print_map_rcp(std::ostream &out, T &d)
{
    out << "{";
    for (auto p = d.begin(); p != d.end(); p++) {
        if (p != d.begin())
            out << ", ";
        out << *(p->first) << ": " << *(p->second);
    }
    out << "}";
    return out;
}

template <class T>
inline std::ostream &print_vec(std::ostream &out, T &d)
{
    out << "[";
    for (auto p = d.begin(); p != d.end(); p++) {
        if (p != d.begin())
            out << ", ";
        out << *p;
    }
    out << "]";
    return out;
}

template <class T>
inline std::ostream &print_vec_rcp(std::ostream &out, T &d)
{
    out << "[";
    for (auto p = d.begin(); p != d.end(); p++) {
        if (p != d.begin())
            out << ", ";
        out << **p;
    }
    out << "]";
    return out;
}

} // anonymous namespace

std::ostream &operator<<(std::ostream &out, const SymEngine::umap_basic_num &d)
{
    return SymEngine::print_map_rcp(out, d);
}

std::ostream &operator<<(std::ostream &out, const SymEngine::map_basic_num &d)
{
    return SymEngine::print_map_rcp(out, d);
}

std::ostream &operator<<(std::ostream &out, const SymEngine::map_basic_basic &d)
{
    return SymEngine::print_map_rcp(out, d);
}

std::ostream &operator<<(std::ostream &out,
                         const SymEngine::umap_basic_basic &d)
{
    return SymEngine::print_map_rcp(out, d);
}

std::ostream &operator<<(std::ostream &out, const SymEngine::vec_basic &d)
{
    return SymEngine::print_vec_rcp(out, d);
}

std::ostream &operator<<(std::ostream &out, const SymEngine::set_basic &d)
{
    return SymEngine::print_vec_rcp(out, d);
}

bool vec_basic_eq(const vec_basic &a, const vec_basic &b)
{
    // Can't be equal if # of entries differ:
    if (a.size() != b.size())
        return false;
    // Loop over elements in "a" and "b":
    for (size_t i = 0; i < a.size(); i++) {
        if (neq(*a[i], *b[i]))
            return false; // values not equal
    }
    return true;
}

bool vec_basic_eq_perm(const vec_basic &a, const vec_basic &b)
{
    // Can't be equal if # of entries differ:
    if (a.size() != b.size())
        return false;
    // Loop over elements in "a"
    for (size_t i = 0; i < a.size(); i++) {
        // Find the element a[i] in "b"
        bool found = false;
        for (size_t j = 0; j < a.size(); j++) {
            if (eq(*a[i], *b[j])) {
                found = true;
                break;
            }
        }
        // If not found, then a != b
        if (not found)
            return false;
    }
    // If all elements were found, then a == b
    return true;
}

int vec_basic_compare(const vec_basic &A, const vec_basic &B)
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

bool map_uint_mpz_eq(const map_uint_mpz &a, const map_uint_mpz &b)
{
    return a.size() == b.size()
           and std::equal(a.begin(), a.end(), b.begin(),
                          [](const std::pair<unsigned, integer_class> &u,
                             const std::pair<unsigned, integer_class> &v) {
                              return u.first == v.first
                                     and u.second == v.second;
                          });
}

int map_uint_mpz_compare(const map_uint_mpz &A, const map_uint_mpz &B)
{
    if (A.size() != B.size())
        return (A.size() < B.size()) ? -1 : 1;
    auto a = A.begin();
    auto b = B.begin();
    for (; a != A.end(); ++a, ++b) {
        if (a->first != b->first)
            return (a->first < b->first) ? -1 : 1;
        if (a->second != b->second)
            return (a->second < b->second) ? -1 : 1;
    }
    return 0;
}

bool map_int_Expr_eq(const map_int_Expr &a, const map_int_Expr &b)
{
    return a.size() == b.size()
           and std::equal(a.begin(), a.end(), b.begin(),
                          [](const std::pair<unsigned, Expression> &u,
                             const std::pair<unsigned, Expression> &v) {
                              return u.first == v.first
                                     and u.second == v.second;
                          });
}

int map_int_Expr_compare(const map_int_Expr &A, const map_int_Expr &B)
{
    if (A.size() != B.size())
        return (A.size() < B.size()) ? -1 : 1;
    auto a = A.begin();
    auto b = B.begin();
    for (; a != A.end(); ++a, ++b) {
        if (a->first != b->first)
            return (a->first < b->first) ? -1 : 1;
        if (a->second != b->second)
            return (a->second.get_basic()->__cmp__(*b->second.get_basic())) ? -1
                                                                            : 1;
    }
    return 0;
}

template <class T>
bool set_eq(const T &A, const T &B)
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

template <class T>
int set_compare(const T &A, const T &B)
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

bool multiset_basic_eq(const multiset_basic &a, const multiset_basic &b)
{
    return set_eq<multiset_basic>(a, b);
}

int multiset_basic_compare(const multiset_basic &a, const multiset_basic &b)
{
    return set_compare<multiset_basic>(a, b);
}
}
