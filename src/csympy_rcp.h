#ifndef CSYMPY_RCP_H
#define CSYMPY_RCP_H

#include <cstddef>
#include <stdexcept>

#include "csympy_config.h"
#include "csympy_assert.h"


#if defined(WITH_CSYMPY_RCP)

#else

// Include all Teuchos headers here:
#include "Teuchos_RCP.hpp"
#include "Teuchos_TypeNameTraits.hpp"

#endif

namespace CSymPy {


#if defined(WITH_CSYMPY_RCP)


/* Ptr */

// Ptr is always pointing to a valid object (can never be NULL).

template<class T>
class Ptr {
public:
    inline explicit Ptr( T *ptr ) : ptr_(ptr) {
        CSYMPY_ASSERT(ptr_ != NULL)
    }
    inline Ptr(const Ptr<T>& ptr) : ptr_(ptr.ptr_) {}
    template<class T2> inline Ptr(const Ptr<T2>& ptr) : ptr_(ptr.get()) {}
    Ptr<T>& operator=(const Ptr<T>& ptr) { ptr_ = ptr.get(); return *this; }
    inline T* operator->() const { return ptr_; }
    inline T& operator*() const { return *ptr_; }
    inline T* get() const { return ptr_; }
    inline T* getRawPtr() const { return get(); }
    inline const Ptr<T> ptr() const { return *this; }
private:
    T *ptr_;
};

template<typename T> inline
Ptr<T> outArg( T& arg )
{
    return Ptr<T>(&arg);
}

/* RCP */

enum ENull { null };

// RCP can be null. Functionally it should be equivalent to Teuchos::RCP.

template<class T>
class RCP {
public:
    RCP(ENull null_arg = null) : ptr_(NULL) {}
    explicit RCP(T *p) : ptr_(p) {
        CSYMPY_ASSERT(ptr_ != NULL)
        (ptr_->refcount_)++;
    }
    RCP(const RCP<T> &rp) : ptr_(rp.ptr_) {
        if (!is_null()) (ptr_->refcount_)++;
    }
    template<class T2> RCP(const RCP<T2>& r_ptr) : ptr_(r_ptr.get()) {
        if (!is_null()) (ptr_->refcount_)++;
    }
    ~RCP() {
        if (ptr_ != NULL && --(ptr_->refcount_) == 0) delete ptr_;
    }
    T* operator->() const {
        CSYMPY_ASSERT(ptr_ != NULL)
        return ptr_;
    }
    T& operator*() const {
        CSYMPY_ASSERT(ptr_ != NULL)
        return *ptr_;
    }
    T* get() const { return ptr_; }
    Ptr<T> ptr() const { return Ptr<T>(get()); }
    bool is_null() const { return ptr_ == NULL; }
    template<class T2> bool operator==(const RCP<T2> &p2) {
        return ptr_ == p2.ptr_;
    }
    template<class T2> bool operator!=(const RCP<T2> &p2) {
        return ptr_ != p2.ptr_;
    }
    RCP<T>& operator=(const RCP<T> &r_ptr) {
        T *r_ptr_ptr_ = r_ptr.ptr_;
        if (!r_ptr.is_null()) (r_ptr_ptr_->refcount_)++;
        if (!is_null() && --(ptr_->refcount_) == 0) delete ptr_;
        ptr_ = r_ptr_ptr_;
        return *this;
    }
    void reset() {
        if (!is_null() && --(ptr_->refcount_) == 0) delete ptr_;
        ptr_ = NULL;
    }
private:
    T *ptr_;
};

template<class T>
inline RCP<T> rcp(T* p)
{
    return RCP<T>(p);
}

template<class T2, class T1>
inline RCP<T2> rcp_static_cast(const RCP<T1>& p1)
{
    // Make the compiler check if the conversion is legal
    T2 *check = static_cast<T2*>(p1.get());
    return RCP<T2>(check);
}

template<class T2, class T1>
inline RCP<T2> rcp_dynamic_cast(const RCP<T1>& p1)
{
    if (!p1.is_null()) {
        T2 *p = NULL;
        // Make the compiler check if the conversion is legal
        p = dynamic_cast<T2*>(p1.get());
        if (p) {
            return RCP<T2>(p);
        }
    }
    throw std::runtime_error("rcp_dynamic_cast: cannot convert.");
}

template<class T2, class T1>
inline RCP<T2> rcp_const_cast(const RCP<T1>& p1)
{
  // Make the compiler check if the conversion is legal
  T2 *check = const_cast<T2*>(p1.get());
  return RCP<T2>(check);
}


template<class T>
inline bool operator==(const RCP<T> &p, ENull)
{
  return p.get() == NULL;
}


template<typename T>
std::string typeName(const T &t)
{
    return "RCP<>";
}

void print_stack_on_segfault();


#else

using Teuchos::RCP;
using Teuchos::Ptr;
using Teuchos::outArg;
using Teuchos::rcp;
using Teuchos::rcp_dynamic_cast;
using Teuchos::rcp_static_cast;
using Teuchos::rcp_const_cast;
using Teuchos::typeName;
using Teuchos::null;
using Teuchos::print_stack_on_segfault;

#endif

} // CSymPy


#endif
