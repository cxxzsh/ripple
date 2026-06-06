// Copyright (c) 2026 ripple contributors
// SPDX-License-Identifier: MIT

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include <ripple/computed.hpp>
#include <ripple/var.hpp>

namespace {

TEST_CASE("computed reads dependency values") {
  ripple::var<int> count{1};

  auto doubled = ripple::computed([&] { return count.get() * 2; });

  CHECK(doubled.get() == 2);
}

TEST_CASE("computed updates after var changes") {
  ripple::var<int> count{1};

  auto doubled = ripple::computed([&] { return count.get() * 2; });

  CHECK(doubled.get() == 2);

  count.set(3);
  CHECK(doubled.get() == 6);
}

TEST_CASE("computed supports computed dependencies") {
  ripple::var<int> count{3};

  auto doubled = ripple::computed([&] { return count.get() * 2; });

  auto plus_one = ripple::computed([&] { return doubled.get() + 1; });

  CHECK(plus_one.get() == 7);

  count.set(10);
  CHECK(doubled.get() == 20);
  CHECK(plus_one.get() == 21);
}

TEST_CASE("computed recomputes lazily") {
  ripple::var<int> count{1};
  int evaluations = 0;

  auto doubled = ripple::computed([&] {
    ++evaluations;
    return count.get() * 2;
  });

  CHECK(evaluations == 0);
  CHECK(doubled.get() == 2);
  CHECK(evaluations == 1);

  CHECK(doubled.get() == 2);
  CHECK(evaluations == 1);

  count.set(4);
  CHECK(evaluations == 1);
  CHECK(doubled.get() == 8);
  CHECK(evaluations == 2);
}

}  // namespace
