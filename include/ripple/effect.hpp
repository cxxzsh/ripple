// Copyright (c) 2026 ripple contributors
// SPDX-License-Identifier: MIT

#pragma once

#include <functional>
#include <memory>
#include <type_traits>
#include <utility>

#include <ripple/detail/dependency.hpp>
#include <ripple/subscription.hpp>

namespace ripple {
namespace detail {

class effect_state final : public observer, public subscription_state {
 public:
  using effect_fn = std::function<void()>;

  explicit effect_state(effect_fn fn) : fn_(std::move(fn)) {}

  void start() { run(); }

  void mark_dirty() override {
    if (!active_ || pending_) {
      return;
    }

    pending_ = true;
    schedule_effect(weak_from_this());
  }

  void run_pending() override {
    if (!active_) {
      return;
    }

    pending_ = false;
    run();
  }

  bool observing() const noexcept override { return active_; }

  void dispose() noexcept override {
    if (!active_) {
      return;
    }

    active_ = false;
    pending_ = false;
    clear_dependencies();
  }

  bool disposed() const noexcept override { return !active_; }

 private:
  void run() {
    if (!active_) {
      return;
    }

    clear_dependencies();

    observer_scope scope(*this);
    fn_();
  }

  effect_fn fn_;
  bool active_{true};
  bool pending_{false};
};

}  // namespace detail

template <class F>
subscription effect(F&& fn) {
  using result_type = std::invoke_result_t<F&>;
  static_assert(std::is_void<result_type>::value,
                "ripple::effect requires a callable that returns void");

  auto state =
      std::make_shared<detail::effect_state>(detail::effect_state::effect_fn(std::forward<F>(fn)));
  state->start();
  return subscription(std::move(state));
}

}  // namespace ripple
