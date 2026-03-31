// Copyright (c) 2026 ripple contributors
// SPDX-License-Identifier: MIT

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include <ripple/derived.hpp>
#include <ripple/variable.hpp>

namespace ripple {
namespace {

TEST_CASE("derived: computes from source variables") {
  variable<int> a{1};
  variable<int> b{2};

  auto sum = make_derived([&] { return a.get() + b.get(); });

  CHECK(sum.get() == 3);
}

TEST_CASE("derived: reflects latest source values") {
  variable<int> a{1};
  variable<int> b{2};

  auto sum = make_derived([&] { return a.get() + b.get(); });

  CHECK(sum.get() == 3);

  a.set(10);
  CHECK(sum.get() == 12);

  b.set(-2);
  CHECK(sum.get() == 8);
}

TEST_CASE("derived: supports derived-from-derived composition") {
  variable<int> a{2};
  variable<int> b{5};

  auto sum = make_derived([&] { return a.get() + b.get(); });
  auto doubled = make_derived([&] { return sum.get() * 2; });

  CHECK(sum.get() == 7);
  CHECK(doubled.get() == 14);

  a.set(10);

  CHECK(sum.get() == 15);
  CHECK(doubled.get() == 30);
}

TEST_CASE("derived: get reevaluates compute function") {
  int evaluations = 0;

  auto value = make_derived([&] {
    ++evaluations;
    return 42;
  });

  CHECK(value.get() == 42);
  CHECK(evaluations == 1);

  CHECK(value.get() == 42);
  CHECK(evaluations == 2);

  CHECK(value.get() == 42);
  CHECK(evaluations == 3);
}

TEST_CASE("derived: nested gets reevaluate both levels") {
  int inner_evaluations = 0;
  int outer_evaluations = 0;

  auto inner = make_derived([&] {
    ++inner_evaluations;
    return 10;
  });

  auto outer = make_derived([&] {
    ++outer_evaluations;
    return inner.get() + 5;
  });

  CHECK(outer.get() == 15);
  CHECK(inner_evaluations == 1);
  CHECK(outer_evaluations == 1);

  CHECK(outer.get() == 15);
  CHECK(inner_evaluations == 2);
  CHECK(outer_evaluations == 2);
}

}  // namespace
}  // namespace ripple