#include "basic.h"
#include "integer.h"

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


namespace SymEngine {

bool map_basic_num_eq(const map_basic_num &A, const map_basic_num &B)
{
    // Can't be equal if # of entries differ:
    if (A.size() != B.size()) return false;
    // Loop over keys in "a":
    auto a = A.begin();
    auto b = B.begin();
    for (; a != A.end(); ++a, ++b) {
        if (neq(a->first, b->first)) return false; // keys not equal
        if (neq(a->second, b->second)) return false; // values not equal
    }
    return true;
}

bool map_basic_basic_eq(const map_basic_basic &A, const map_basic_basic &B)
{
    // Can't be equal if # of entries differ:
    if (A.size() != B.size()) return false;
    // Loop over keys in "a":
    auto a = A.begin();
    auto b = B.begin();
    for (; a != A.end(); ++a, ++b) {
        if (neq(a->first, b->first)) return false; // keys not equal
        if (neq(a->second, b->second)) return false; // values not equal
    }
    return true;
}

int map_basic_basic_compare(const map_basic_basic &A, const map_basic_basic &B)
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

int map_basic_num_compare(const map_basic_num &A, const map_basic_num &B)
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

bool umap_basic_num_eq(const umap_basic_num &a, const umap_basic_num &b)
{
    // This follows the same algorithm as Python's dictionary comparison
    // (a==b), which is implemented by "dict_equal" function in
    // Objects/dictobject.c.

    // Can't be equal if # of entries differ:
    if (a.size() != b.size()) return false;
    // Loop over keys in "a":
    for (auto &p: a) {
        // O(1) lookup of the key in "b":
        auto f = b.find(p.first);
        if (f == b.end()) return false; // no such element in "b"
        if (neq(p.second, f->second)) return false; // values not equal
    }
    return true;
}

bool umap_basic_basic_eq(const umap_basic_basic &a,
        const umap_basic_basic &b)
{
    // This follows the same algorithm as Python's dictionary comparison
    // (a==b), which is implemented by "dict_equal" function in
    // Objects/dictobject.c.

    // Can't be equal if # of entries differ:
    if (a.size() != b.size()) return false;
    // Loop over keys in "a":
    for (auto &p: a) {
        // O(1) lookup of the key in "b":
        auto f = b.find(p.first);
        if (f == b.end()) return false; // no such element in "b"
        if (neq(p.second, f->second)) return false; // values not equal
    }
    return true;
}

bool vec_basic_eq(const vec_basic &a, const vec_basic &b)
{
    // Can't be equal if # of entries differ:
    if (a.size() != b.size()) return false;
    // Loop over elements in "a" and "b":
    for (size_t i = 0; i < a.size(); i++) {
        if (neq(a[i], b[i])) return false; // values not equal
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
            if (eq(a[i], b[j])) {
                found = true;
                break;
            }
        }
        // If not found, then a != b
        if (!found) return false;
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

}

