#include <symengine/constants.h>
#include <symengine/symengine_casts.h>

namespace SymEngine
{

Symbol::Symbol(const std::string &name)
    : name_{name} {SYMENGINE_ASSIGN_TYPEID()}

      hash_t Symbol::__hash__() const
{
    hash_t seed = 0;
    hash_combine(seed, name_);
    return seed;
}

bool Symbol::__eq__(const Basic &o) const
{
    if (is_a<Symbol>(o))
        return name_ == down_cast<const Symbol &>(o).name_;
    return false;
}

int Symbol::compare(const Basic &o) const
{
    SYMENGINE_ASSERT(is_a<Symbol>(o))
    const Symbol &s = down_cast<const Symbol &>(o);
    if (name_ == s.name_)
        return 0;
    return name_ < s.name_ ? -1 : 1;
}

RCP<const Symbol> Symbol::as_dummy() const
{
    return dummy(name_);
}

#ifdef WITH_SYMENGINE_THREAD_SAFE
std::atomic<size_t> Dummy::count_{0};
#else
size_t Dummy::count_ = 0;
#endif

constexpr char Dummy::default_Dummy_prefix_[]; // <--- C++14 compatibility

Dummy::Dummy()
    : Symbol(default_Dummy_prefix_
             + to_string(
#ifdef WITH_SYMENGINE_THREAD_SAFE
                 Dummy::count_.fetch_add(1, std::memory_order_relaxed)
#else
                 count_
#endif
                     ))
{
    SYMENGINE_ASSIGN_TYPEID()
#ifdef WITH_SYMENGINE_THREAD_SAFE
    // this is inefficient: we should not construct Dummy instances using
    // Dummy() (since we need the thread-safely incremented value *at
    // construction of Symbol*).
    dummy_index = std::stoul(get_name().substr(default_Dummy_prefix_len_));
#else
    dummy_index = count_;
    count_ += 1;
#endif
}

Dummy::Dummy(const std::string &name) : Symbol(name)
{
    SYMENGINE_ASSIGN_TYPEID()
#ifdef WITH_SYMENGINE_THREAD_SAFE
    dummy_index = Dummy::count_.fetch_add(1, std::memory_order_relaxed);
#else
    dummy_index = count_;
    count_ += 1;
#endif
}

Dummy::Dummy(const std::string &name, size_t dummy_index)
    : Symbol(name), dummy_index(dummy_index){SYMENGINE_ASSIGN_TYPEID()}

      hash_t Dummy::__hash__() const
{
    hash_t seed = 0;
    hash_combine(seed, get_name());
    hash_combine(seed, dummy_index);
    return seed;
}

bool Dummy::__eq__(const Basic &o) const
{
    if (is_a<Dummy>(o))
        return ((get_name() == down_cast<const Dummy &>(o).get_name())
                and (dummy_index == down_cast<const Dummy &>(o).get_index()));
    return false;
}

int Dummy::compare(const Basic &o) const
{
    SYMENGINE_ASSERT(is_a<Dummy>(o))
    const Dummy &s = down_cast<const Dummy &>(o);
    if (get_name() == s.get_name()) {
        if (dummy_index == s.get_index())
            return 0;
        return dummy_index < s.get_index() ? -1 : 1;
    }
    return get_name() < s.get_name() ? -1 : 1;
}

} // namespace SymEngine
