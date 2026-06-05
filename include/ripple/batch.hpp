// Copyright (c) 2026 ripple contributors
// SPDX-License-Identifier: MIT

#pragma once

#include <type_traits>
#include <utility>

#include <ripple/detail/observer_stack.hpp>

namespace ripple {

template <class F>
decltype(auto) batch(F&& fn) {
  detail::batch_scope scope;

  if constexpr (std::is_void<std::invoke_result_t<F&>>::value) {
    std::forward<F>(fn)();
    scope.close();
  } else {
    decltype(auto) result = std::forward<F>(fn)();
    scope.close();
    return result;
  }
}

}  // namespace ripple
