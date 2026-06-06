// Copyright (c) 2026 ripple contributors
// SPDX-License-Identifier: MIT

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include <ripple/batch.hpp>
#include <ripple/effect.hpp>
#include <ripple/var.hpp>

namespace {

TEST_CASE("batch interface is usable") {
  ripple::var<int> first{0};
  ripple::var<int> second{0};

  int runs = 0;
  int sum = 0;

  auto sub = ripple::effect([&] {
    ++runs;
    sum = first.get() + second.get();
  });

  CHECK(runs == 1);
  CHECK(sum == 0);

  ripple::batch([&] {
    first.set(1);
    second.set(2);
    first.set(3);
  });

  CHECK(runs == 2);
  CHECK(sum == 5);

  sub.dispose();
}

TEST_CASE("batch can return a value") {
  auto value = ripple::batch([] { return 42; });

  CHECK(value == 42);
}

}  // namespace
