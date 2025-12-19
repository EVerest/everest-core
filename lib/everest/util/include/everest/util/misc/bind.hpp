#pragma once

#include <functional>

namespace everest::lib::util {
/**
 * @brief Bind member function to std::function
 * @param[in] func Member function
 * @param[in] ptr to object (likely this)
 */

template <typename R, typename C, typename... Args> std::function<R(Args...)> bind_obj(R (C::*func)(Args...), C* obj) {
    return [func, obj](Args... args) -> R { return (obj->*func)(std::forward<Args>(args)...); };
}

} // namespace everest::lib::util
