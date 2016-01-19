#include <symengine/basic.h>
#include <symengine/integer.h>
#include <symengine/symbol.h>

namespace SymEngine {

template<class T>
inline std::ostream& print_map(std::ostream& out, T& d)
{
    out << "{";
    for (auto p = d.begin(); p != d.end(); p++) {
        if (p != d.begin()) out << ", ";
        out << (p->first) << ": " << (p->second);
    }
    out << "}";
    return out;
}

template<class T>
inline std::ostream& print_map_rcp(std::ostream& out, T& d)
{
    out << "{";
    for (auto p = d.begin(); p != d.end(); p++) {
        if (p != d.begin()) out << ", ";
        out << *(p->first) << ": " << *(p->second);
    }
    out << "}";
    return out;
}

template<class T>
inline std::ostream& print_vec(std::ostream& out, T& d)
{
    out << "[";
    for (auto p = d.begin(); p != d.end(); p++) {
        if (p != d.begin()) out << ", ";
        out << *p;
    }
    out << "]";
    return out;
}

template<class T>
inline std::ostream& print_vec_rcp(std::ostream& out, T& d)
{
    out << "[";
    for (auto p = d.begin(); p != d.end(); p++) {
        if (p != d.begin()) out << ", ";
        out << **p;
    }
    out << "]";
    return out;
}

} // SymEngine


std::ostream& operator<<(std::ostream& out, const SymEngine::umap_basic_num& d)
{
    return print_map_rcp(out, d);
}

std::ostream& operator<<(std::ostream& out, const SymEngine::vec_int& d)
{
    return SymEngine::print_vec(out, d);
}

std::ostream& operator<<(std::ostream& out, const SymEngine::map_vec_int& d)
{
    return SymEngine::print_map(out, d);
}

std::ostream& operator<<(std::ostream& out, const SymEngine::map_vec_mpz& d)
{
    return SymEngine::print_map(out, d);
}

std::ostream& operator<<(std::ostream& out, const SymEngine::umap_vec_mpz& d)
{
    return print_map(out, d);
}

std::ostream& operator<<(std::ostream& out, const SymEngine::map_basic_num& d)
{
    return print_map_rcp(out, d);
}

std::ostream& operator<<(std::ostream& out, const SymEngine::map_basic_basic& d)
{
    return print_map_rcp(out, d);
}

std::ostream& operator<<(std::ostream& out, const SymEngine::umap_basic_basic& d)
{
    return print_map_rcp(out, d);
}

std::ostream& operator<<(std::ostream& out, const SymEngine::vec_basic& d)
{
    return print_vec_rcp(out, d);
}

std::ostream& operator<<(std::ostream& out, const SymEngine::set_basic& d)
{
    return print_vec_rcp(out, d);
}

#if defined(HAVE_SYMENGINE_PIRANHA)
std::ostream& operator<<(std::ostream& out, const SymEngine::hash_set& d)
{
    return print_map(out, d);
}
#endif

namespace SymEngine {

#if defined(HAVE_SYMENGINE_PIRANHA)
bool hash_set_eq(const hash_set &a,
        const hash_set &b)
{
    // Can't be equal if # of elements in set differ:
    if (a.size() != b.size()) return false;
    // Loop over elements in "a":
    for (auto &p: a) {
        auto f = b.find(p);
        if (f == b.end()) return false; // no such element in "b"
    }
    return true;
}

#else
bool unordered_set_eq(const unordered_set &a,
        const unordered_set &b)
{
    // Can't be equal if # of elements in set differ:
    if (a.size() != b.size()) return false;
    // Loop over elements in "a":
    for (auto &p: a) {
        auto f = b.find(p);
        if (f == b.end()) return false; // no such element in "b"
    }
    return true;
}

long long vec_encode(const vec_int &v) {
    long long retval = 0;
    int i = 1;
    for(auto &a: v) {
        retval += a*i;
        i *= 10;
    }
    return retval;
}

vec_int vec_decode(const int code) {
    vec_int v = {0};
    int n = code;
    while(n > 0) {
        v.push_back(n % 10);
        n = n/10;
    }
    return v;
}
#endif

bool vec_basic_eq(const vec_basic &a, const vec_basic &b)
{
    // Can't be equal if # of entries differ:
    if (a.size() != b.size()) return false;
    // Loop over elements in "a" and "b":
    for (size_t i = 0; i < a.size(); i++) {
        if (neq(*a[i], *b[i])) return false; // values not equal
    }
    return true;
}

bool vec_symbol_eq(const vec_symbol &a, const vec_symbol &b)
{
    // Can't be equal if # of entries differ:
    if (a.size() != b.size()) return false;
    // Loop over elements in "a" and "b":
    for (size_t i = 0; i < a.size(); i++) {
        if (a[i]->get_name() != b[i]->get_name()) return false; // values not equal
    }
    return true;
}

bool vec_basic_eq_perm(const vec_basic &a, const vec_basic &b)
{
    // Can't be equal if # of entries differ:
    if (a.size() != b.size()) return false;
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
        if (not found) return false;
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
        if (cmp != 0) return cmp;
    }
    return 0;
}

bool map_uint_mpz_eq(const map_uint_mpz &a,
        const map_uint_mpz &b)
{
    // Can't be equal if # of entries differ:
    if (a.size() != b.size()) return false;
    // Loop over keys in "a":
    for (const auto &p: a) {
        auto f = b.find(p.first);
        if (f == b.end()) return false; // no such element in "b"
        if (p.second != f->second) return false; // values not equal
    }
    return true;
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

#if defined(HAVE_SYMENGINE_PIRANHA)
int hash_set_compare(const hash_set &A, const hash_set &B)
{
    if (A.size() != B.size())
        return (A.size() < B.size()) ? -1 : 1;
    auto a = A.begin();
    auto b = B.begin();
    for (; a != A.end(); ++a, ++b) {
        if ((*a).first != (*b).first)
            return ((*a).first < (*b).first) ? -1 : 1;
        if ((*a).second != (*b).second)
            return ((*a).second < (*b).second) ? -1 : 1;
    }
    return 0;
}
#endif

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

bool multiset_basic_eq(const multiset_basic &a, const multiset_basic &b)
{
    return set_eq<multiset_basic>(a, b);
}

int multiset_basic_compare(const multiset_basic &a, const multiset_basic &b)
{
    return set_compare<multiset_basic>(a, b);
}

}
