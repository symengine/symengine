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
        and std::equal(a.begin(), a.end(), b.begin(), [] (const std::pair<unsigned, integer_class> &u,
                                                         const std::pair<unsigned, integer_class> &v)
                                                            { return u.first == v.first
                                                                and u.second == v.second; });
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
        and std::equal(a.begin(), a.end(), b.begin(), [] (const std::pair<unsigned, Expression> &u,
                                                         const std::pair<unsigned, Expression> &v)
                                                            { return u.first == v.first
                                                                and u.second == v.second; });
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

bool multiset_basic_eq(const multiset_basic &a, const multiset_basic &b)
{
    return set_eq<multiset_basic>(a, b);
}

int multiset_basic_compare(const multiset_basic &a, const multiset_basic &b)
{
    return set_compare<multiset_basic>(a, b);
}

long mpz_hash(const integer_class z){
    return mp_get_si(z);
}

int umap_uvec_mpz_compare(const umap_uvec_mpz &a, const umap_uvec_mpz &b) {
    std::vector<vec_uint> va = order_umap<vec_uint, umap_uvec_mpz, vec_uint_compare>(a);
    std::vector<vec_uint> vb = order_umap<vec_uint, umap_uvec_mpz, vec_uint_compare>(b);

    if (va.empty())
        if (!vb.empty())
            return -1;
    if (vb.empty())
        if (!va.empty())
            return 1;
    if (va.empty() && vb.empty())
        return 0;

    for (unsigned int i = 0; i < va.size() && i < vb.size(); i++) {
        if (vec_uint_compare()(va[i], vb[i])) {
            return -1;
        } else if (!vec_uint_compare()(va[i], vb[i]) && va[i] != vb[i]) {
            return 1;
        } else {
            if (a.find(va[i])->second != b.find(vb[i])->second) {
                if (a.find(va[i])->second < b.find(vb[i])->second) {
                    return -1;
                } else {
                    return 1;
                }
            }
        }
    }
    if (va.size() < vb.size())
        return -1;
    if (vb.size() < va.size())
        return 1;
    return 0;
}

//Copied from umap_eq, with derefrencing of image in map removed.
bool umap_uvec_mpz_eq(const umap_uvec_mpz &a, const umap_uvec_mpz &b) {
    // This follows the same algorithm as Python's dictionary comparison
    // (a==b), which is implemented by "dict_equal" function in
    // Objects/dictobject.c.

    // Can't be equal if # of entries differ:
    if (a.size() != b.size()) return false;
    // Loop over keys in "a":
    for (const auto &p: a) {
        // O(1) lookup of the key in "b":
        auto f = b.find(p.first);
        if (f == b.end()) return false; // no such element in "b"
        if (p.second != f->second) return false; // values not equal
    }
    return true;

}

int umap_uvec_expr_compare(const umap_uvec_expr &a, const umap_uvec_expr &b) {
    std::vector<vec_uint> va = order_umap<vec_uint, umap_uvec_expr, vec_uint_compare>(a);
    std::vector<vec_uint> vb = order_umap<vec_uint, umap_uvec_expr, vec_uint_compare>(b);

    if (va.empty())
        if (!vb.empty())
            return -1;
    if (vb.empty())
        if (!va.empty())
            return 1;
    if (va.empty() && vb.empty())
        return 0;

    for (unsigned int i = 0; i < va.size() && i < vb.size(); i++) {
        if (vec_uint_compare()(va[i], vb[i])) {
            return -1;
        } else if (!vec_uint_compare()(va[i], vb[i]) && va[i] != vb[i]) {
            return 1;
        } else {
            if (a.find(va[i])->second != b.find(vb[i])->second) {
                return (a.find(va[i])->second).get_basic()->__cmp__( *( (b.find(vb[i])->second).get_basic() ) );
/*
                if (a.find(va[i])->second < b.find(vb[i])->second) { //probably will want to replace this with compare
                    return -1;
                } else {
                    return 1;
                }*/
            }
        }
    }
    if (va.size() < vb.size())
        return -1;
    if (vb.size() < va.size())
        return 1;
    return 0;
}

//Copied from umap_eq, with derefrencing of image in map removed.
bool umap_uvec_expr_eq(const umap_uvec_expr &a, const umap_uvec_expr &b){
    // This follows the same algorithm as Python's dictionary comparison
    // (a==b), which is implemented by "dict_equal" function in
    // Objects/dictobject.c.

    // Can't be equal if # of entries differ:
    if (a.size() != b.size()) return false;
    // Loop over keys in "a":
    for (const auto &p: a) {
        // O(1) lookup of the key in "b":
        auto f = b.find(p.first);
        if (f == b.end()) return false; // no such element in "b"
        if (p.second != f->second) return false; // values not equal
    }
    return true;

}


}
