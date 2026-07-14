#include "catch.hpp"
#include <symengine/add.h>
#include <symengine/constants.h>
#include <symengine/dict.h>
#include <symengine/integer.h>
#include <symengine/mul.h>
#include <symengine/symengine_rcp.h>
#include <symengine/symbol.h>

// Guard so this file is empty (no test cases) for non-cooperative_intrusive backends
#if defined(WITH_SYMENGINE_COOPERATIVE_INTRUSIVE_RCP)

// The cooperative counter lives in symengine_rcp_cooperative.cpp.
// We only need to install fake external-runtime hooks here.

using SymEngine::EnableRCPFromThis;
using SymEngine::make_rcp;
using SymEngine::RCP;

class Node : public EnableRCPFromThis<Node> {
public:
    int v = 0;
};

// Counting hooks for testing external-owned mode without a real runtime
static int g_inc_count = 0;
static int g_dec_count = 0;

static void counting_inc_hook(void *) noexcept { ++g_inc_count; }
static void counting_dec_hook(void *) noexcept { ++g_dec_count; }

// Fake foreign-runtime pointer (properly aligned - bit 0 must be 0)
alignas(2) static char fake_runtime_storage;
static void *fake_runtime = &fake_runtime_storage;

// Install hooks once at process start
struct HookInstaller {
    HookInstaller() {
        SymEngine::cooperative_intrusive_init(counting_inc_hook, counting_dec_hook);
    }
};
static HookInstaller installer;

TEST_CASE("C++-owned counting", "[cooperative_intrusive_rcp]") {
    g_inc_count = 0;
    g_dec_count = 0;

    // Fresh object before any RCP
    Node *raw = new Node();
    REQUIRE(raw->use_count() == 0);
    REQUIRE(not raw->is_external_owned());
    delete raw;

    // With RCP
    RCP<Node> a = make_rcp<Node>();
    REQUIRE(a->use_count() == 1);
    REQUIRE(a->is_uniquely_owned_by_cpp());
    REQUIRE(a->is_uniquely_owned());
    REQUIRE(not a->is_external_owned());

    {
        RCP<Node> b = a;
        REQUIRE(a->use_count() == 2);
        REQUIRE(not a->is_uniquely_owned_by_cpp());
        REQUIRE(not a->is_uniquely_owned());
        REQUIRE(not a->is_external_owned());

        {
            RCP<Node> c = b;
            REQUIRE(a->use_count() == 3);
        }
        REQUIRE(a->use_count() == 2);
    }
    REQUIRE(a->use_count() == 1);
    REQUIRE(a->is_uniquely_owned_by_cpp());
    REQUIRE(a->is_uniquely_owned());
}

TEST_CASE("Handoff changes mode", "[cooperative_intrusive_rcp]") {
    g_inc_count = 0;
    g_dec_count = 0;

    RCP<Node> a = make_rcp<Node>();
    REQUIRE(a->use_count() == 1);
    REQUIRE(not a->is_external_owned());

    // Hand off to external-owned mode
    a->set_self_external(fake_runtime);

    REQUIRE(a->is_external_owned());
    REQUIRE(a->self_external() == fake_runtime);
    REQUIRE(a->use_count() == 0);
    REQUIRE(not a->is_uniquely_owned_by_cpp());
    REQUIRE(not a->is_uniquely_owned());

    // The inc hook should have been called once per pre-existing reference
    // (replayed count). With 1 C++ reference, that's 1 call.
    REQUIRE(g_inc_count == 1);

    // The fake dec-hook doesn't free the object (no real Py_DECREF), so the
    // Node leaks even after releasing the RCP handle below.  ASAN leak
    // detection is disabled for this test via CTest ENVIRONMENT.
    a = SymEngine::null;
}

TEST_CASE("External-owned inc/dec route through hooks", "[cooperative_intrusive_rcp]") {
    g_inc_count = 0;
    g_dec_count = 0;

    RCP<Node> a = make_rcp<Node>();
    a->set_self_external(fake_runtime);
    REQUIRE(a->is_external_owned());

    // After handoff, hook was called once (replay)
    int inc_after_handoff = g_inc_count;
    int dec_after_handoff = g_dec_count;

    // Copying an RCP should call the inc hook
    {
        RCP<Node> b = a;
        REQUIRE(g_inc_count == inc_after_handoff + 1);
        REQUIRE(a->use_count() == 0);  // Always 0 when external-owned
        REQUIRE(a->is_external_owned());

        // Destroying b should call the dec hook
    }
    REQUIRE(g_dec_count == dec_after_handoff + 1);
    REQUIRE(a->use_count() == 0);
    REQUIRE(a->is_external_owned());

    // The fake dec-hook doesn't free the object (no real Py_DECREF), so the
    // Node leaks even after releasing the RCP handle below.  ASAN leak
    // detection is disabled for this test via CTest ENVIRONMENT.
    a = SymEngine::null;
}

TEST_CASE("use_count() is never 1 when external-owned", "[cooperative_intrusive_rcp]") {
    g_inc_count = 0;
    g_dec_count = 0;

    RCP<Node> a = make_rcp<Node>();
    a->set_self_external(fake_runtime);

    // Across several hook-driven inc/decs, the uniqueness predicate must
    // always be false — the safety contract for dictionary stealing.
    REQUIRE(a->use_count() == 0);
    REQUIRE(not a->is_uniquely_owned_by_cpp());
    REQUIRE(not a->is_uniquely_owned());

    {
        RCP<Node> b = a;
        REQUIRE(a->use_count() == 0);
        REQUIRE(not a->is_uniquely_owned_by_cpp());
        REQUIRE(not a->is_uniquely_owned());

        {
            RCP<Node> c = b;
            REQUIRE(a->use_count() == 0);
            REQUIRE(not a->is_uniquely_owned_by_cpp());
            REQUIRE(not a->is_uniquely_owned());
        }
        REQUIRE(a->use_count() == 0);
        REQUIRE(not a->is_uniquely_owned_by_cpp());
        REQUIRE(not a->is_uniquely_owned());
    }
    REQUIRE(a->use_count() == 0);
    REQUIRE(not a->is_uniquely_owned_by_cpp());
    REQUIRE(not a->is_uniquely_owned());

    // Cleanup: drop our RCP reference. dec_ref() invokes the fake dec-hook
    // (incrementing g_dec_count) and returns false, so the RCP destructor
    // does not delete the object.  The Node leaks — acceptable for a test
    // that validates the counter, not real Python lifetime management.
    a = SymEngine::null;
}

TEST_CASE("detach external and replay inline references", "[cooperative_intrusive_rcp]") {
    g_inc_count = 0;
    g_dec_count = 0;

    RCP<Node> a = make_rcp<Node>();
    RCP<Node> b = a;
    REQUIRE(a->use_count() == 2);
    REQUIRE(not a->is_external_owned());

    a->set_self_external(fake_runtime);
    REQUIRE(a->is_external_owned());
    REQUIRE(a->self_external() == fake_runtime);
    REQUIRE(a->use_count() == 0);
    REQUIRE(g_inc_count == 2);

    void *detached = a->detach_external();
    REQUIRE(detached == fake_runtime);
    REQUIRE(not a->is_external_owned());
    REQUIRE(a->self_external() == nullptr);
    REQUIRE(a->use_count() == 0);

    a->inc_ref();
    a->inc_ref();
    REQUIRE(a->use_count() == 2);

    b = SymEngine::null;
    REQUIRE(a->use_count() == 1);
}

TEST_CASE("Add::from_dict retains an external Mul dictionary",
          "[cooperative_intrusive_rcp]")
{
    g_inc_count = 0;
    g_dec_count = 0;

    RCP<const SymEngine::Basic> x = SymEngine::symbol("x");
    RCP<const SymEngine::Basic> y = SymEngine::symbol("y");
    RCP<const SymEngine::Basic> term = SymEngine::mul(x, y);
    RCP<const SymEngine::Mul> term_mul
        = SymEngine::rcp_static_cast<const SymEngine::Mul>(term);
    const auto original_size = term_mul->get_dict().size();

    term->set_self_external(fake_runtime);
    REQUIRE(not term->is_uniquely_owned());

    SymEngine::umap_basic_num terms;
    terms.emplace(term, SymEngine::integer(2));
    RCP<const SymEngine::Basic> collapsed
        = SymEngine::Add::from_dict(SymEngine::zero, std::move(terms));

    // An external wrapper can still hand this object back to a foreign
    // runtime. The copy path must therefore leave the original Mul intact.
    REQUIRE(term_mul->get_dict().size() == original_size);
    REQUIRE(SymEngine::eq(
        *collapsed, *SymEngine::mul(SymEngine::integer(2), SymEngine::mul(x, y))));

    // The fake runtime hooks deliberately do not free the external object.
    term = SymEngine::null;
}

TEST_CASE("Add::from_dict collapses a uniquely C++-owned Mul",
          "[cooperative_intrusive_rcp]")
{
    RCP<const SymEngine::Basic> x = SymEngine::symbol("x");
    RCP<const SymEngine::Basic> y = SymEngine::symbol("y");
    RCP<const SymEngine::Basic> term = SymEngine::mul(x, y);
    REQUIRE(term->is_uniquely_owned());

    SymEngine::umap_basic_num terms;
    terms.emplace(std::move(term), SymEngine::integer(2));
    REQUIRE(term.is_null());

    RCP<const SymEngine::Basic> collapsed
        = SymEngine::Add::from_dict(SymEngine::zero, std::move(terms));
    REQUIRE(SymEngine::eq(
        *collapsed, *SymEngine::mul(SymEngine::integer(2), SymEngine::mul(x, y))));
}

#endif // WITH_SYMENGINE_COOPERATIVE_INTRUSIVE_RCP
