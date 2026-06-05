// Copyright (c) 2026 ripple contributors
// SPDX-License-Identifier: MIT

#pragma once

#include <cassert>
#include <functional>
#include <memory>
#include <optional>
#include <type_traits>
#include <utility>

#include <ripple/detail/dependency.hpp>

namespace ripple {
namespace detail {

template <class T>
class computed_state final : public observer {
 public:
  using compute_fn = std::function<T()>;

  explicit computed_state(compute_fn fn) : fn_(std::move(fn)) {}

  const T& get() {
    dependency_.observe();

    if (dirty_ || !value_) {
      recompute();
    }

    return *value_;
  }

  void mark_dirty() override {
    if (dirty_) {
      return;
    }

    dirty_ = true;
    dependency_.notify();
  }

 private:
  void recompute() {
    clear_dependencies();

    observer_scope scope(*this);
    value_.emplace(fn_());
    dirty_ = false;
  }

  compute_fn fn_;
  std::optional<T> value_;
  dependency dependency_;
  bool dirty_{true};
};

}  // namespace detail

template <class T>
class computed_value {
 public:
  using value_type = T;

  computed_value(const computed_value&) noexcept = default;
  computed_value& operator=(const computed_value&) noexcept = default;

  computed_value(computed_value&&) noexcept = default;
  computed_value& operator=(computed_value&&) noexcept = default;

  const T& get() const {
    assert(state_ != nullptr);
    return state_->get();
  }

 private:
  template <class F>
  friend auto computed(F&& fn);

  explicit computed_value(std::shared_ptr<detail::computed_state<T>> state) noexcept
      : state_(std::move(state)) {}

  std::shared_ptr<detail::computed_state<T>> state_;
};

template <class F>
auto computed(F&& fn) {
  using result_type = std::decay_t<std::invoke_result_t<F&>>;
  static_assert(!std::is_void<result_type>::value,
                "ripple::computed requires a callable that returns a value");

  using state_type = detail::computed_state<result_type>;
  auto state = std::make_shared<state_type>(typename state_type::compute_fn(std::forward<F>(fn)));
  return computed_value<result_type>(std::move(state));
}

}  // namespace ripple
