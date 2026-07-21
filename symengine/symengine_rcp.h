#ifndef SYMENGINE_RCP_H
#define SYMENGINE_RCP_H

#include <iostream>
#include <cstddef>
#include <stdexcept>
#include <string>
#if __cplusplus <= 201703L
#include <ciso646>
#else
#include <version>
#endif

#include <symengine/symengine_config.h>
#include <symengine/symengine_assert.h>

#if defined(WITH_SYMENGINE_COOPERATIVE_INTRUSIVE_RCP)

#include <cstdint>

#elif defined(WITH_SYMENGINE_RCP)

#if defined(WITH_SYMENGINE_THREAD_SAFE)
#include <atomic>
#endif

#else

// Include all Teuchos headers here:
#include <symengine/utilities/teuchos/Teuchos_RCP.hpp>
#include <symengine/utilities/teuchos/Teuchos_TypeNameTraits.hpp>

#endif

namespace SymEngine
{

// ---------------------------------------------------------------------------
// noexcept is part of a function-pointer type only since C++17. Keep the
// public hook types usable by the C++11/C++14 configurations that SymEngine
// supports; a noexcept callback still converts to either pointer type.
#if __cplusplus >= 201703L
using cooperative_incref_hook = void (*)(void *) noexcept;
using cooperative_decref_hook = void (*)(void *) noexcept;
#else
using cooperative_incref_hook = void (*)(void *);
using cooperative_decref_hook = void (*)(void *);
#endif

void cooperative_intrusive_init(cooperative_incref_hook incref,
                                cooperative_decref_hook decref) noexcept;

// Counter wrapper for the cooperative_intrusive backend (declaration only;
// definitions live in symengine_rcp_cooperative.cpp).
// ---------------------------------------------------------------------------
#if defined(WITH_SYMENGINE_COOPERATIVE_INTRUSIVE_RCP)
//! Counter wrapper for the cooperative_intrusive backend.
//! In external-owned mode, use_count() returns 0 because the foreign runtime
//! holds the live references. Use is_uniquely_owned_by_cpp() for ownership-
//! gated optimizations.
class symengine_cooperative_intrusive_counter
{
public:
    symengine_cooperative_intrusive_counter() noexcept : m_state(1) {}
    void inc_ref() const noexcept;
    bool dec_ref() const noexcept;
    void set_self_external(void *o) noexcept;
    void *self_external() const noexcept;
    unsigned int use_count() const noexcept;
    bool is_external_owned() const noexcept;
    bool is_uniquely_owned_by_cpp() const noexcept;

    // Detach the foreign wrapper: atomically reset to C++-owned (refcount 0)
    // and return the previous runtime object pointer so the caller can
    // release it in the foreign runtime. This is used during runtime shutdown.
    // The CAS prevents a stale state snapshot from overwriting a concurrent
    // mode transition; callers must still synchronize shutdown with hook calls.
    // Returns nullptr if already C++-owned.
    void *detach_external() const noexcept;

private:
    mutable uintptr_t m_state;
};
static_assert(sizeof(symengine_cooperative_intrusive_counter) == sizeof(void *),
              "cooperative-intrusive counter must stay pointer-sized for the C ABI");
#endif

// ---------------------------------------------------------------------------
// Shared between both intrusive backends (cooperative_intrusive and symengine):
// Ptr<T>, ENull, rcp(), rcp_*_cast, outArg, ptrFromRef, typeName,
// print_stack_on_segfault.
// ---------------------------------------------------------------------------
#if defined(WITH_SYMENGINE_COOPERATIVE_INTRUSIVE_RCP) || defined(WITH_SYMENGINE_RCP)

/* Ptr */

// Ptr is always pointing to a valid object (can never be nullptr).

template <class T>
class Ptr
{
public:
    inline explicit Ptr(T *ptr) : ptr_(ptr)
    {
        SYMENGINE_ASSERT(ptr_ != nullptr)
    }
    inline Ptr(const Ptr<T> &ptr) : ptr_(ptr.ptr_) {}
    template <class T2>
    inline Ptr(const Ptr<T2> &ptr) : ptr_(ptr.get())
    {
    }
    Ptr<T> &operator=(const Ptr<T> &ptr)
    {
        ptr_ = ptr.get();
        return *this;
    }
#if defined(HAVE_DEFAULT_CONSTRUCTORS)
    inline Ptr(Ptr &&) = default;
    Ptr<T> &operator=(Ptr &&) = default;
#endif
    inline T *operator->() const
    {
        return ptr_;
    }
    inline T &operator*() const
    {
        return *ptr_;
    }
    inline T *get() const
    {
        return ptr_;
    }
    inline T *getRawPtr() const
    {
        return get();
    }
    inline const Ptr<T> ptr() const
    {
        return *this;
    }

private:
    T *ptr_;
};

template <typename T>
inline Ptr<T> outArg(T &arg)
{
    return Ptr<T>(&arg);
}

/** \brief Create a pointer to a object from an object reference.
 *
 * \relates Ptr
 */
template <typename T>
inline Ptr<T> ptrFromRef(T &arg)
{
    return Ptr<T>(&arg);
}

/* RCP */

enum ENull { null };

// RCP can be null. Functionally it should be equivalent to Teuchos::RCP.

// ---------------------------------------------------------------------------
// cooperative_intrusive RCP<T>: uses inc_ref()/dec_ref() methods instead of direct
// field access.  Counter lives inside the object (via EnableRCPFromThis<T>).
// ---------------------------------------------------------------------------
#if defined(WITH_SYMENGINE_COOPERATIVE_INTRUSIVE_RCP)

template <class T>
class RCP
{
public:
    RCP(ENull null_arg = null) : ptr_(nullptr) {}
    explicit RCP(T *p) : ptr_(p)
    {
        SYMENGINE_ASSERT(ptr_ != nullptr)
        ptr_->inc_ref();
    }
    RCP(const RCP<T> &rp) : ptr_(rp.ptr_)
    {
        if (not is_null())
            ptr_->inc_ref();
    }
    template <class T2>
    RCP(const RCP<T2> &r_ptr) : ptr_(r_ptr.get())
    {
        if (not is_null())
            ptr_->inc_ref();
    }
    RCP(RCP<T> &&rp) SYMENGINE_NOEXCEPT : ptr_(rp.ptr_)
    {
        rp.ptr_ = nullptr;
    }
    template <class T2>
    RCP(RCP<T2> &&r_ptr)
    SYMENGINE_NOEXCEPT : ptr_(r_ptr.get())
    {
        r_ptr._set_null();
    }
    ~RCP() SYMENGINE_NOEXCEPT
    {
        if (ptr_ != nullptr and ptr_->dec_ref())
            delete ptr_;
    }
    T *operator->() const
    {
        SYMENGINE_ASSERT(ptr_ != nullptr)
        return ptr_;
    }
    T &operator*() const
    {
        SYMENGINE_ASSERT(ptr_ != nullptr)
        return *ptr_;
    }
    T *get() const
    {
        return ptr_;
    }
    Ptr<T> ptr() const
    {
        return Ptr<T>(get());
    }
    bool is_null() const
    {
        return ptr_ == nullptr;
    }
    template <class T2>
    bool operator==(const RCP<T2> &p2) const
    {
        return ptr_ == p2.ptr_;
    }
    template <class T2>
    bool operator!=(const RCP<T2> &p2) const
    {
        return ptr_ != p2.ptr_;
    }
    RCP<T> &operator=(const RCP<T> &r_ptr)
    {
        T *r = r_ptr.ptr_;
        if (r != nullptr)
            r->inc_ref();
        if (ptr_ != nullptr and ptr_->dec_ref())
            delete ptr_;
        ptr_ = r;
        return *this;
    }
    RCP<T> &operator=(RCP<T> &&r_ptr)
    {
        std::swap(ptr_, r_ptr.ptr_);
        return *this;
    }
    void reset()
    {
        if (ptr_ != nullptr and ptr_->dec_ref())
            delete ptr_;
        ptr_ = nullptr;
    }
    // Don't use this function directly:
    void _set_null()
    {
        ptr_ = nullptr;
    }

private:
    T *ptr_;
};

#else // WITH_SYMENGINE_RCP (original intrusive backend)

template <class T>
class RCP
{
public:
    RCP(ENull null_arg = null) : ptr_(nullptr) {}
    explicit RCP(T *p) : ptr_(p)
    {
        SYMENGINE_ASSERT(ptr_ != nullptr)
        (ptr_->refcount_)++;
    }
    // Copy constructor
    RCP(const RCP<T> &rp) : ptr_(rp.ptr_)
    {
        if (not is_null())
            (ptr_->refcount_)++;
    }
    // Copy constructor
    template <class T2>
    RCP(const RCP<T2> &r_ptr) : ptr_(r_ptr.get())
    {
        if (not is_null())
            (ptr_->refcount_)++;
    }
    // Move constructor
    RCP(RCP<T> &&rp) SYMENGINE_NOEXCEPT : ptr_(rp.ptr_)
    {
        rp.ptr_ = nullptr;
    }
    // Move constructor
    template <class T2>
    RCP(RCP<T2> &&r_ptr)
    SYMENGINE_NOEXCEPT : ptr_(r_ptr.get())
    {
        r_ptr._set_null();
    }
    ~RCP() SYMENGINE_NOEXCEPT
    {
        if (ptr_ != nullptr and --(ptr_->refcount_) == 0)
            delete ptr_;
    }
    T *operator->() const
    {
        SYMENGINE_ASSERT(ptr_ != nullptr)
        return ptr_;
    }
    T &operator*() const
    {
        SYMENGINE_ASSERT(ptr_ != nullptr)
        return *ptr_;
    }
    T *get() const
    {
        return ptr_;
    }
    Ptr<T> ptr() const
    {
        return Ptr<T>(get());
    }
    bool is_null() const
    {
        return ptr_ == nullptr;
    }
    template <class T2>
    bool operator==(const RCP<T2> &p2) const
    {
        return ptr_ == p2.ptr_;
    }
    template <class T2>
    bool operator!=(const RCP<T2> &p2) const
    {
        return ptr_ != p2.ptr_;
    }
    // Copy assignment
    RCP<T> &operator=(const RCP<T> &r_ptr)
    {
        T *r_ptr_ptr_ = r_ptr.ptr_;
        if (not r_ptr.is_null())
            (r_ptr_ptr_->refcount_)++;
        if (not is_null() and --(ptr_->refcount_) == 0)
            delete ptr_;
        ptr_ = r_ptr_ptr_;
        return *this;
    }
    // Move assignment
    RCP<T> &operator=(RCP<T> &&r_ptr)
    {
        std::swap(ptr_, r_ptr.ptr_);
        return *this;
    }
    void reset()
    {
        if (not is_null() and --(ptr_->refcount_) == 0)
            delete ptr_;
        ptr_ = nullptr;
    }
    // Don't use this function directly:
    void _set_null()
    {
        ptr_ = nullptr;
    }

private:
    T *ptr_;
};

#endif // WITH_SYMENGINE_COOPERATIVE_INTRUSIVE_RCP / WITH_SYMENGINE_RCP RCP<T>

template <class T>
inline RCP<T> rcp(T *p)
{
    return RCP<T>(p);
}

template <class T2, class T1>
inline RCP<T2> rcp_static_cast(const RCP<T1> &p1)
{
    // Make the compiler check if the conversion is legal
    T2 *check = static_cast<T2 *>(p1.get());
    return RCP<T2>(check);
}

template <class T2, class T1>
inline RCP<T2> rcp_dynamic_cast(const RCP<T1> &p1)
{
    if (not p1.is_null()) {
        T2 *p = nullptr;
        // Make the compiler check if the conversion is legal
        p = dynamic_cast<T2 *>(p1.get());
        if (p) {
            return RCP<T2>(p);
        }
    }
    throw std::runtime_error("rcp_dynamic_cast: cannot convert.");
}

template <class T2, class T1>
inline RCP<T2> rcp_const_cast(const RCP<T1> &p1)
{
    // Make the compiler check if the conversion is legal
    T2 *check = const_cast<T2 *>(p1.get());
    return RCP<T2>(check);
}

template <class T>
inline bool operator==(const RCP<T> &p, ENull)
{
    return p.get() == nullptr;
}

template <typename T>
std::string typeName(const T &t)
{
    return "RCP<>";
}

void print_stack_on_segfault();

#endif // shared intrusive block

// ---------------------------------------------------------------------------
// Teuchos backend aliases
// ---------------------------------------------------------------------------
#if !defined(WITH_SYMENGINE_COOPERATIVE_INTRUSIVE_RCP) && !defined(WITH_SYMENGINE_RCP)

using Teuchos::null;
using Teuchos::outArg;
using Teuchos::print_stack_on_segfault;
using Teuchos::Ptr;
using Teuchos::ptrFromRef;
using Teuchos::RCP;
using Teuchos::rcp;
using Teuchos::rcp_const_cast;
using Teuchos::rcp_dynamic_cast;
using Teuchos::rcp_static_cast;
using Teuchos::typeName;

#endif

template <class T>
class EnableRCPFromThis
{
    // Public interface
public:
    //! Get RCP<T> pointer to self (it will cast the pointer to T)
    inline RCP<T> rcp_from_this()
    {
#if defined(WITH_SYMENGINE_COOPERATIVE_INTRUSIVE_RCP) || defined(WITH_SYMENGINE_RCP)
        return rcp(static_cast<T *>(this));
#else
        return rcp_static_cast<T>(weak_self_ptr_.create_strong());
#endif
    }

    //! Get RCP<const T> pointer to self (it will cast the pointer to const T)
    inline RCP<const T> rcp_from_this() const
    {
#if defined(WITH_SYMENGINE_COOPERATIVE_INTRUSIVE_RCP) || defined(WITH_SYMENGINE_RCP)
        return rcp(static_cast<const T *>(this));
#else
        return rcp_static_cast<const T>(weak_self_ptr_.create_strong());
#endif
    }

    //! Get RCP<T2> pointer to self (it will cast the pointer to T2)
    template <class T2>
    inline RCP<const T2> rcp_from_this_cast() const
    {
#if defined(WITH_SYMENGINE_COOPERATIVE_INTRUSIVE_RCP) || defined(WITH_SYMENGINE_RCP)
        return rcp(static_cast<const T2 *>(this));
#else
        return rcp_static_cast<const T2>(weak_self_ptr_.create_strong());
#endif
    }

    //! In the cooperative_intrusive backend, use_count() returns 0 for
    //! external-owned objects, so it is NOT a total reference count in that
    //! mode. In every backend the value is advisory while references can be
    //! changed concurrently; use is_uniquely_owned() for ownership-gated
    //! optimizations.
    unsigned int use_count() const
    {
#if defined(WITH_SYMENGINE_COOPERATIVE_INTRUSIVE_RCP)
        return refcount_.use_count();
#elif defined(WITH_SYMENGINE_RCP)
        return refcount_;
#else
        return weak_self_ptr_.strong_count();
#endif
    }

    //! Whether this object can be safely treated as uniquely C++-owned for
    //! move-out optimizations. Externally-owned cooperative objects are never
    //! unique: a foreign runtime can still reach their wrapper.
    bool is_uniquely_owned() const noexcept
    {
#if defined(WITH_SYMENGINE_COOPERATIVE_INTRUSIVE_RCP)
        return refcount_.is_uniquely_owned_by_cpp();
#elif defined(WITH_SYMENGINE_RCP)
#if defined(WITH_SYMENGINE_THREAD_SAFE)
        // Keep the historical no-steal behavior in thread-safe builds. A
        // relaxed load could be considered in a future, separately reviewed
        // optimization, but it is intentionally not enabled here.
        return false;
#else
        return refcount_ == 1;
#endif
#else
        return weak_self_ptr_.strong_count() == 1;
#endif
    }

#if defined(WITH_SYMENGINE_COOPERATIVE_INTRUSIVE_RCP)

    // Everything below is private interface for cooperative_intrusive
private:
    mutable symengine_cooperative_intrusive_counter refcount_;

public:
    EnableRCPFromThis() = default;

    void inc_ref() const noexcept { refcount_.inc_ref(); }
    bool dec_ref() const noexcept { return refcount_.dec_ref(); }
    void set_self_external(void *o) const noexcept { refcount_.set_self_external(o); }
    void *self_external() const noexcept { return refcount_.self_external(); }

    bool is_external_owned() const noexcept
    {
        return refcount_.is_external_owned();
    }
    bool is_uniquely_owned_by_cpp() const noexcept
    {
        return refcount_.is_uniquely_owned_by_cpp();
    }

    // Detach the foreign wrapper: atomically reset to C++-owned (refcount 0)
    // and return the previous runtime object pointer so the caller can release
    // it in the foreign runtime. Used by runtime-specific shutdown cleanup.
    void *detach_external() const noexcept
    {
        return refcount_.detach_external();
    }

private:
#elif defined(WITH_SYMENGINE_RCP)

    // Everything below is private interface for symengine RCP
private:
//! Public variables if defined with SYMENGINE_RCP
// The reference counter is defined either as "unsigned int" (faster, but
// not thread safe) or as std::atomic<unsigned int> (slower, but thread
// safe). Semantically they are almost equivalent, except that the
// pre-decrement operator `operator--()` returns a copy for std::atomic
// instead of a reference to itself.
// The refcount_ is defined as mutable, because it does not change the
// state of the instance, but changes when more copies
// of the same instance are made.
#if defined(WITH_SYMENGINE_THREAD_SAFE)
    mutable std::atomic<unsigned int> refcount_; // reference counter
#else
    mutable unsigned int refcount_; // reference counter
#endif // WITH_SYMENGINE_THREAD_SAFE
public:
    EnableRCPFromThis() : refcount_(0) {}

private:
#else

    // Everything below is private interface for Teuchos
private:
    mutable RCP<T> weak_self_ptr_;

    void set_weak_self_ptr(const RCP<T> &w)
    {
        weak_self_ptr_ = w;
    }

    void set_weak_self_ptr(const RCP<const T> &w) const
    {
        weak_self_ptr_ = rcp_const_cast<T>(w);
    }
#endif // WITH_SYMENGINE_COOPERATIVE_INTRUSIVE_RCP / WITH_SYMENGINE_RCP

#if defined(WITH_SYMENGINE_COOPERATIVE_INTRUSIVE_RCP) || defined(WITH_SYMENGINE_RCP)
    template <class T_>
    friend class RCP;
#endif

    template <typename T_, typename... Args>
    friend inline RCP<T_> make_rcp(Args &&...args);
};

template <typename T, typename... Args>
inline RCP<T> make_rcp(Args &&...args)
{
#if defined(WITH_SYMENGINE_COOPERATIVE_INTRUSIVE_RCP) || defined(WITH_SYMENGINE_RCP)
    return rcp(new T(std::forward<Args>(args)...));
#else
    RCP<T> p = rcp(new T(std::forward<Args>(args)...));
    p->set_weak_self_ptr(p.create_weak());
    return p;
#endif
}

} // namespace SymEngine

#endif
