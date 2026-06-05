// Copyright (c) 2026 ripple contributors
// SPDX-License-Identifier: MIT

#pragma once

#include <algorithm>
#include <memory>
#include <utility>
#include <vector>

#include <ripple/detail/observer_stack.hpp>

namespace ripple {
namespace detail {

class dependency_state {
 public:
  void subscribe(std::weak_ptr<observer> observer) {
    auto shared = observer.lock();
    if (!shared) {
      return;
    }

    compact();

    const auto already_subscribed = std::any_of(
        observers_.begin(), observers_.end(), [&](const std::weak_ptr<detail::observer>& existing) {
          auto locked = existing.lock();
          return locked && locked.get() == shared.get();
        });
    if (already_subscribed) {
      return;
    }

    observers_.push_back(std::move(shared));
  }

  void unsubscribe(const observer* observer) noexcept {
    observers_.erase(std::remove_if(observers_.begin(), observers_.end(),
                                    [&](const std::weak_ptr<detail::observer>& existing) {
                                      auto locked = existing.lock();
                                      return !locked || locked.get() == observer;
                                    }),
                     observers_.end());
  }

  void compact() {
    observers_.erase(
        std::remove_if(observers_.begin(), observers_.end(),
                       [](const std::weak_ptr<observer>& observer) { return observer.expired(); }),
        observers_.end());
  }

  std::vector<std::weak_ptr<observer>> observers_;
};

class dependency {
 public:
  dependency() : state_(std::make_shared<dependency_state>()) {}

  dependency(const dependency&) = delete;
  dependency& operator=(const dependency&) = delete;

  dependency(dependency&&) = delete;
  dependency& operator=(dependency&&) = delete;

  ~dependency() = default;

  void observe() {
    auto* observer = current_observer();
    if (observer == nullptr || !observer->observing()) {
      return;
    }

    observer->depend_on(*this);
  }

  void notify() {
    auto state = state_;
    state->compact();

    if (batch_depth() != 0) {
      for (const auto& observer : state->observers_) {
        schedule_dirty(observer);
      }
      return;
    }

    ++notification_depth();

    std::vector<std::shared_ptr<observer>> observers;
    observers.reserve(state->observers_.size());
    for (const auto& weak_observer : state->observers_) {
      auto observer = weak_observer.lock();
      if (observer) {
        observers.push_back(std::move(observer));
      }
    }

    for (const auto& observer : observers) {
      observer->mark_dirty();
    }

    --notification_depth();

    flush_pending_effects();
  }

 private:
  friend class observer;

  std::shared_ptr<dependency_state> state_;
};

inline observer::~observer() { clear_dependencies(); }

inline void observer::depend_on(dependency& dep) {
  auto state = dep.state_;
  const auto already_tracked = std::any_of(dependencies_.begin(), dependencies_.end(),
                                           [&](const std::weak_ptr<dependency_state>& existing) {
                                             auto locked = existing.lock();
                                             return locked && locked.get() == state.get();
                                           });
  if (already_tracked) {
    return;
  }

  auto self = weak_from_this();
  if (self.expired()) {
    return;
  }

  state->subscribe(self);
  dependencies_.push_back(std::move(state));
}

inline void observer::clear_dependencies() noexcept {
  for (const auto& weak_dependency : dependencies_) {
    auto dependency = weak_dependency.lock();
    if (dependency) {
      dependency->unsubscribe(this);
    }
  }
  dependencies_.clear();
}

}  // namespace detail
}  // namespace ripple
