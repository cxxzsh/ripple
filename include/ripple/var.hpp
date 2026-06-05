// Copyright (c) 2026 ripple contributors
// SPDX-License-Identifier: MIT

#pragma once

#include <type_traits>
#include <utility>

#include <ripple/detail/dependency.hpp>

namespace ripple {

template <class T>
class var {
 public:
  using value_type = T;

  var() = default;

  explicit var(T value) : value_(std::move(value)) {}

  var(const var&) = delete;
  var& operator=(const var&) = delete;

  var(var&&) = delete;
  var& operator=(var&&) = delete;

  const T& get() const {
    dependency_.observe();
    return value_;
  }

  void set(T value) {
    value_ = std::move(value);
    dependency_.notify();
  }

  template <class U = T, class = std::enable_if_t<std::is_assignable<T&, U&&>::value>>
  var& operator=(U&& value) {
    set(T(std::forward<U>(value)));
    return *this;
  }

 private:
  T value_{};
  mutable detail::dependency dependency_;
};

}  // namespace ripple
