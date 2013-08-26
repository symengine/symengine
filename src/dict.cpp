#include "basic.h"
#include "integer.h"

std::ostream& operator<<(std::ostream& out, const CSymPy::umap_basic_int& d)
{
    out << "{";
    for (auto &p: d)
        out << *(p.first) << ": " << *(p.second) << ", ";
    out << "}";
    return out;
}

std::ostream& operator<<(std::ostream& out, const CSymPy::vec_int& d)
{
    out << "[";
    for (auto &p: d)
        out << p << ", ";
    out << "]";
    return out;
}

std::ostream& operator<<(std::ostream& out, const CSymPy::map_vec_int& d)
{
    out << "{";
    for (auto &p: d)
        out << (p.first) << ": " << (p.second) << ", ";
    out << "}";
    return out;
}

std::ostream& operator<<(std::ostream& out, const CSymPy::map_vec_mpz& d)
{
    out << "{";
    for (auto &p: d)
        out << (p.first) << ": " << (p.second) << ", ";
    out << "}";
    return out;
}

std::ostream& operator<<(std::ostream& out, const CSymPy::umap_vec_mpz& d)
{
    out << "{";
    for (auto &p: d)
        out << (p.first) << ": " << (p.second) << ", ";
    out << "}";
    return out;
}

std::ostream& operator<<(std::ostream& out, const CSymPy::map_basic_int& d)
{
    out << "{";
    for (auto &p: d)
        out << *(p.first) << ": " << *(p.second) << ", ";
    out << "}";
    return out;
}

std::ostream& operator<<(std::ostream& out, const CSymPy::map_basic_basic& d)
{
    out << "{";
    for (auto &p: d)
        out << *(p.first) << ": " << *(p.second) << ", ";
    out << "}";
    return out;
}

std::ostream& operator<<(std::ostream& out, const CSymPy::umap_basic_basic& d)
{
    out << "{";
    for (auto &p: d)
        out << *(p.first) << ": " << *(p.second) << ", ";
    out << "}";
    return out;
}


namespace CSymPy {

bool map_basic_int_equal(const map_basic_int &A, const map_basic_int &B)
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

bool map_basic_basic_equal(const map_basic_basic &A, const map_basic_basic &B)
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

int map_basic_int_compare(const map_basic_int &A, const map_basic_int &B)
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

bool dicts_equal(const umap_basic_int &a, const umap_basic_int &b)
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

bool umap_basic_basic_equal(const umap_basic_basic &a,
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

}
