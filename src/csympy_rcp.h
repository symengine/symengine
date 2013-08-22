#ifndef CSYMPY_RCP_H
#define CSYMPY_RCP_H

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

/* RCP */

// RCP is always pointing to a valid object (can never be NULL).

template<class T>
class RCP {
public:
    explicit RCP(T *p) : ptr_(p) {
        CSYMPY_ASSERT(p != NULL)
        (ptr_->refcount_)++;
    }
    RCP(const RCP<T> &rp) : ptr_(rp.ptr_) { (ptr_->refcount_)++; }
    template<class T2> RCP(const RCP<T2>& r_ptr) : ptr_(r_ptr.get()) {
        (ptr_->refcount_)++;
    }
    ~RCP() { if (--(ptr_->refcount_) == 0) delete ptr_; }
    T* operator->() const { return ptr_; }
    T& operator*() const { return *ptr_; }
    T* get() const { return ptr_; }
    template<class T2> bool operator==(const RCP<T2> &p2) {
        return ptr_ == p2.ptr_;
    }
    template<class T2> bool operator!=(const RCP<T2> &p2) {
        return ptr_ != p2.ptr_;
    }
    RCP<T>& operator=(const RCP<T> &r_ptr) {
        T *r_ptr_ptr_ = r_ptr.ptr_;
        (r_ptr_ptr_->refcount_)++;
        if (--(ptr_->refcount_) == 0) delete ptr_;
        ptr_ = r_ptr_ptr_;
        return *this;
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
    T2 *p = NULL;
    // Make the compiler check if the conversion is legal
    p = dynamic_cast<T2*>(p1.get());
    if (p) {
        return RCP<T2>(p);
    }
    throw std::runtime_error("rcp_dynamic_cast: cannot convert.");
}


template<typename T>
std::string typeName(const T &t)
{
    return "RCP<>";
}

inline void print_stack_on_segfault()
{
}

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


#else

using Teuchos::RCP;
using Teuchos::Ptr;
using Teuchos::outArg;
using Teuchos::rcp;
using Teuchos::rcp_dynamic_cast;
using Teuchos::rcp_static_cast;
using Teuchos::typeName;
using Teuchos::print_stack_on_segfault;

#endif

} // CSymPy


#endif
