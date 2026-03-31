// Copyright (c) 2026 ripple contributors
// SPDX-License-Identifier: MIT

#include <functional>
#include <type_traits>
#include <utility>

namespace ripple {

template <class T>
class derived {
 public:
  using value_type = T;
  using compute_fn = std::function<T()>;

  derived(const derived&) = delete;
  derived& operator=(const derived&) = delete;

  derived(derived&&) noexcept = default;
  derived& operator=(derived&&) noexcept = default;

  const T get() const { return fn_(); }

 private:
  template <class F>
  friend auto make_derived(F&& fn);

  explicit derived(compute_fn fn) : fn_(std::move(fn)) {}

  compute_fn fn_;
};

template <class F>
auto make_derived(F&& fn) {
  using result_type = std::decay_t<std::invoke_result_t<F&>>;
  using derived_type = derived<result_type>;
  return derived_type(typename derived_type::compute_fn(std::forward<F>(fn)));
}

}  // namespace ripple