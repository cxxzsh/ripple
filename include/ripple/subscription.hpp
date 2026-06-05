// Copyright (c) 2026 ripple contributors
// SPDX-License-Identifier: MIT

#pragma once

#include <memory>
#include <utility>

namespace ripple {
namespace detail {

class subscription_state {
 public:
  subscription_state() = default;

  subscription_state(const subscription_state&) = delete;
  subscription_state& operator=(const subscription_state&) = delete;

  subscription_state(subscription_state&&) = delete;
  subscription_state& operator=(subscription_state&&) = delete;

  virtual ~subscription_state() = default;

  virtual void dispose() noexcept = 0;
  virtual bool disposed() const noexcept = 0;
};

}  // namespace detail

class subscription {
 public:
  subscription() noexcept = default;

  subscription(const subscription&) = delete;
  subscription& operator=(const subscription&) = delete;

  subscription(subscription&& other) noexcept = default;

  subscription& operator=(subscription&& other) noexcept {
    if (this != &other) {
      dispose();
      state_ = std::move(other.state_);
    }
    return *this;
  }

  ~subscription() { dispose(); }

  void dispose() noexcept {
    if (!state_) {
      return;
    }

    state_->dispose();
    state_.reset();
  }

  bool disposed() const noexcept { return !state_ || state_->disposed(); }

 private:
  template <class F>
  friend subscription effect(F&& fn);

  explicit subscription(std::shared_ptr<detail::subscription_state> state) noexcept
      : state_(std::move(state)) {}

  std::shared_ptr<detail::subscription_state> state_;
};

using scoped_subscription = subscription;

}  // namespace ripple
