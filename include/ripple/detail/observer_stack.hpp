// Copyright (c) 2026 ripple contributors
// SPDX-License-Identifier: MIT

#pragma once

#include <algorithm>
#include <memory>
#include <utility>
#include <vector>

namespace ripple {
namespace detail {

class dependency;
class dependency_state;

class observer : public std::enable_shared_from_this<observer> {
 public:
  observer() = default;

  observer(const observer&) = delete;
  observer& operator=(const observer&) = delete;

  observer(observer&&) = delete;
  observer& operator=(observer&&) = delete;

  virtual ~observer();

  virtual void mark_dirty() = 0;
  virtual void run_pending() {}
  virtual bool observing() const noexcept { return true; }

  void depend_on(dependency& dep);
  void clear_dependencies() noexcept;

 private:
  std::vector<std::weak_ptr<dependency_state>> dependencies_;
};

inline std::vector<observer*>& observer_stack() {
  static thread_local std::vector<observer*> stack;
  return stack;
}

inline observer* current_observer() noexcept {
  auto& stack = observer_stack();
  if (stack.empty()) {
    return nullptr;
  }
  return stack.back();
}

class observer_scope {
 public:
  explicit observer_scope(observer& observer) { observer_stack().push_back(&observer); }

  observer_scope(const observer_scope&) = delete;
  observer_scope& operator=(const observer_scope&) = delete;

  ~observer_scope() { observer_stack().pop_back(); }
};

inline int& batch_depth() noexcept {
  static int depth = 0;
  return depth;
}

inline int& notification_depth() noexcept {
  static int depth = 0;
  return depth;
}

inline std::vector<std::weak_ptr<observer>>& pending_dirty_observers() {
  static std::vector<std::weak_ptr<observer>> observers;
  return observers;
}

inline std::vector<std::weak_ptr<observer>>& pending_effect_observers() {
  static std::vector<std::weak_ptr<observer>> observers;
  return observers;
}

inline void erase_expired(std::vector<std::weak_ptr<observer>>& observers) {
  observers.erase(
      std::remove_if(observers.begin(), observers.end(),
                     [](const std::weak_ptr<observer>& observer) { return observer.expired(); }),
      observers.end());
}

inline void enqueue_unique(std::vector<std::weak_ptr<observer>>& observers,
                           std::weak_ptr<observer> weak_observer) {
  auto shared = weak_observer.lock();
  if (!shared) {
    return;
  }

  erase_expired(observers);

  const auto already_queued =
      std::any_of(observers.begin(), observers.end(), [&](const std::weak_ptr<observer>& queued) {
        auto existing = queued.lock();
        return existing && existing.get() == shared.get();
      });
  if (already_queued) {
    return;
  }

  observers.push_back(std::move(shared));
}

inline void schedule_dirty(std::weak_ptr<observer> weak_observer) {
  enqueue_unique(pending_dirty_observers(), std::move(weak_observer));
}

inline void schedule_effect(std::weak_ptr<observer> weak_observer) {
  enqueue_unique(pending_effect_observers(), std::move(weak_observer));
}

inline void flush_pending_effects() {
  if (batch_depth() != 0 || notification_depth() != 0) {
    return;
  }

  while (!pending_effect_observers().empty()) {
    std::vector<std::weak_ptr<observer>> pending;
    pending.swap(pending_effect_observers());

    for (const auto& weak_observer : pending) {
      auto observer = weak_observer.lock();
      if (observer) {
        observer->run_pending();
      }
    }
  }
}

inline void flush_pending_dirty() {
  if (batch_depth() != 0) {
    return;
  }

  ++notification_depth();
  while (!pending_dirty_observers().empty()) {
    std::vector<std::weak_ptr<observer>> pending;
    pending.swap(pending_dirty_observers());

    for (const auto& weak_observer : pending) {
      auto observer = weak_observer.lock();
      if (observer) {
        observer->mark_dirty();
      }
    }
  }
  --notification_depth();

  flush_pending_effects();
}

inline void begin_batch() noexcept { ++batch_depth(); }

inline void end_batch() {
  --batch_depth();
  if (batch_depth() == 0) {
    flush_pending_dirty();
  }
}

class batch_scope {
 public:
  batch_scope() { begin_batch(); }

  batch_scope(const batch_scope&) = delete;
  batch_scope& operator=(const batch_scope&) = delete;

  ~batch_scope() noexcept(false) {
    if (active_) {
      end_batch();
    }
  }

  void close() {
    if (!active_) {
      return;
    }

    active_ = false;
    end_batch();
  }

 private:
  bool active_{true};
};

}  // namespace detail
}  // namespace ripple
