#ifndef SYMENGINE_UNIQUE_H
#define SYMENGINE_UNIQUE_H

#include <cstddef>
#include <memory>
#include <type_traits>
#include <utility>
#include <algorithm>

namespace SymEngine
{

template <typename T, typename... Args>
inline std::unique_ptr<T> make_unique(Args &&... args)
{
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

} // namespace SymEngine

#endif
