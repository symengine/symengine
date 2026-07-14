/*
    Cooperative intrusive reference counter.

    This is an adapted port of nanobind/intrusive/counter.inl:
    Copyright (c) 2023 Wenzel Jakob.
    nanobind is distributed under the BSD 3-Clause License.

    The atomic operations deliberately use relaxed ordering to stay in lockstep
    with nanobind's reference implementation. Before strengthening them (for
    example with a release decrement plus an acquire fence before deletion),
    first coordinate the corresponding change upstream so the two counters do
    not silently diverge. Keep weakly ordered architectures in sanitizer CI.
*/

#include <symengine/symengine_config.h>

#if defined(WITH_SYMENGINE_COOPERATIVE_INTRUSIVE_RCP)

#include <symengine/symengine_rcp.h>

#include <cstdio>
#include <cstdlib>

namespace {

#if !defined(_MSC_VER)
#define SYMENGINE_ATOMIC_LOAD(ptr) __atomic_load_n(ptr, 0)
#define SYMENGINE_ATOMIC_CMPXCHG(ptr, cmp, xchg) \
    __atomic_compare_exchange_n(ptr, cmp, xchg, true, 0, 0)
#else
extern "C" void *_InterlockedCompareExchangePointer(
    void *volatile *Destination,
    void *Exchange, void *Comparand);
#pragma intrinsic(_InterlockedCompareExchangePointer)

#define SYMENGINE_ATOMIC_LOAD(ptr) *((volatile const uintptr_t *) ptr)
#define SYMENGINE_ATOMIC_CMPXCHG(ptr, cmp, xchg) \
    symengine_cmpxchg(ptr, cmp, xchg)

static bool symengine_cmpxchg(uintptr_t *ptr, uintptr_t *cmp, uintptr_t xchg)
{
    uintptr_t cmpv = *cmp;
    uintptr_t prev = (uintptr_t) _InterlockedCompareExchangePointer(
        (void * volatile *) ptr, (void *) xchg, (void *) cmpv);
    if (prev == cmpv) {
        return true;
    }
    *cmp = prev;
    return false;
}
#endif

SymEngine::cooperative_incref_hook g_cooperative_incref = nullptr;
SymEngine::cooperative_decref_hook g_cooperative_decref = nullptr;

[[noreturn]] void cooperative_abort(const char *message, const void *ptr)
{
    std::fprintf(stderr, "%s (%p)\n", message, ptr);
    std::abort();
}

void ensure_hooks_ready(const void *ptr)
{
    if (!g_cooperative_incref || !g_cooperative_decref) [[unlikely]] {
        cooperative_abort(
            "cooperative_intrusive backend used before cooperative_intrusive_init()",
            ptr);
    }
}

} // namespace

namespace SymEngine
{

void cooperative_intrusive_init(cooperative_incref_hook incref,
                                cooperative_decref_hook decref) noexcept
{
    g_cooperative_incref = incref;
    g_cooperative_decref = decref;
}

void symengine_cooperative_intrusive_counter::inc_ref() const noexcept
{
    uintptr_t v = SYMENGINE_ATOMIC_LOAD(&m_state);

    while (true) {
        if (v & 1) {
            if (!SYMENGINE_ATOMIC_CMPXCHG(&m_state, &v, v + 2))
                continue;
        } else {
            ensure_hooks_ready(this);
            g_cooperative_incref((void *) v);
        }
        break;
    }
}

bool symengine_cooperative_intrusive_counter::dec_ref() const noexcept
{
    uintptr_t v = SYMENGINE_ATOMIC_LOAD(&m_state);

    while (true) {
        if (v & 1) {
            if (v == 1) [[unlikely]] {
                cooperative_abort(
                    "symengine_cooperative_intrusive_counter::dec_ref underflow",
                    this);
            }

            if (!SYMENGINE_ATOMIC_CMPXCHG(&m_state, &v, v - 2))
                continue;

            if (v == 3)
                return true;
        } else {
            ensure_hooks_ready(this);
            g_cooperative_decref((void *) v);
        }

        return false;
    }
}

void symengine_cooperative_intrusive_counter::set_self_external(void *o) noexcept
{
    uintptr_t v = SYMENGINE_ATOMIC_LOAD(&m_state);

    if (!(v & 1)) [[unlikely]] {
        cooperative_abort(
            "symengine_cooperative_intrusive_counter::set_self_external already external-owned",
            this);
    }

    ensure_hooks_ready(this);

    uintptr_t transferred = 0;
    do {
        for (uintptr_t count = v >> 1; transferred < count; ++transferred)
            g_cooperative_incref(o);
    } while (!SYMENGINE_ATOMIC_CMPXCHG(&m_state, &v, (uintptr_t) o));

    for (uintptr_t count = v >> 1; transferred > count; --transferred)
        g_cooperative_decref(o);
}

void *symengine_cooperative_intrusive_counter::self_external() const noexcept
{
    uintptr_t v = SYMENGINE_ATOMIC_LOAD(&m_state);
    return (v & 1) ? nullptr : (void *) v;
}

unsigned int symengine_cooperative_intrusive_counter::use_count() const noexcept
{
    uintptr_t v = SYMENGINE_ATOMIC_LOAD(&m_state);
    if (v & 1)
        return static_cast<unsigned int>(v >> 1);
    return 0;
}

bool symengine_cooperative_intrusive_counter::is_external_owned() const noexcept
{
    return (SYMENGINE_ATOMIC_LOAD(&m_state) & 1) == 0;
}

bool symengine_cooperative_intrusive_counter::is_uniquely_owned_by_cpp() const noexcept
{
    uintptr_t v = SYMENGINE_ATOMIC_LOAD(&m_state);
    return (v & 1) && (v >> 1) == 1;
}

void *symengine_cooperative_intrusive_counter::detach_external() const noexcept
{
    uintptr_t v = SYMENGINE_ATOMIC_LOAD(&m_state);

    // Shutdown normally provides exclusive access, but retain the same CAS
    // discipline as the other mode transitions so a stale load cannot reset a
    // state that changed before publication. The shutdown caller still owns
    // synchronization with foreign-runtime hook calls.
    while (true) {
        if (v & 1)
            return nullptr;
        if (SYMENGINE_ATOMIC_CMPXCHG(&m_state, &v, (uintptr_t) 1))
            return (void *) v;
    }
}

} // namespace SymEngine

#endif
