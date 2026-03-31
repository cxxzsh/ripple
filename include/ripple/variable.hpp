// Copyright (c) 2026 ripple contributors
// SPDX-License-Identifier: MIT

#pragma once

#include <type_traits>
#include <utility>

namespace ripple {

template <class T>
class variable {
 public:
  using value_type = T;

  variable() = default;

  explicit variable(T value) : value_(std::move(value)) {}

  const T& get() const noexcept { return value_; }

  void set(T value) { value_ = std::move(value); }

  template <class U = T, class = std::enable_if_t<std::is_assignable<T&, U&&>::value>>
  variable& operator=(U&& value) {
    set(T(std::forward<U>(value)));
    return *this;
  }

 private:
  T value_{};
};

}  // namespace ripple
