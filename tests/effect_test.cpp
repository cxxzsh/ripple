// Copyright (c) 2026 ripple contributors
// SPDX-License-Identifier: MIT

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include <type_traits>

#include <ripple/effect.hpp>
#include <ripple/subscription.hpp>
#include <ripple/var.hpp>

namespace {

TEST_CASE("subscription is move-only") {
  static_assert(!std::is_copy_constructible<ripple::subscription>::value,
                "ripple::subscription is move-only");
  static_assert(std::is_move_constructible<ripple::subscription>::value,
                "ripple::subscription is movable");
}

TEST_CASE("effect runs immediately and after dependency changes") {
  ripple::var<int> count{0};
  int runs = 0;
  int observed = -1;

  auto sub = ripple::effect([&] {
    ++runs;
    observed = count.get();
  });

  CHECK(runs == 1);
  CHECK(observed == 0);
  CHECK_FALSE(sub.disposed());

  count.set(1);
  CHECK(runs == 2);
  CHECK(observed == 1);
}

TEST_CASE("disposed effect no longer runs") {
  ripple::var<int> count{0};
  int runs = 0;
  int observed = -1;

  auto sub = ripple::effect([&] {
    ++runs;
    observed = count.get();
  });

  sub.dispose();
  CHECK(sub.disposed());

  count.set(2);
  CHECK(runs == 1);
  CHECK(observed == 0);
}

TEST_CASE("subscription disposes on destruction") {
  ripple::var<int> count{0};
  int runs = 0;

  {
    auto sub = ripple::effect([&] {
      ++runs;
      (void)count.get();
    });
    CHECK_FALSE(sub.disposed());
    CHECK(runs == 1);
  }

  count.set(1);
  CHECK(runs == 1);
}

}  // namespace
