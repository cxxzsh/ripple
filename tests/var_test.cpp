// Copyright (c) 2026 ripple contributors
// SPDX-License-Identifier: MIT

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include <memory>
#include <string>
#include <type_traits>

#include <ripple/var.hpp>

namespace {

TEST_CASE("var is move-only reactive source") {
  static_assert(!std::is_copy_constructible<ripple::var<int>>::value,
                "ripple::var is a reactive source, not a copyable value");
}

TEST_CASE("var reads and writes values") {
  ripple::var<int> count{0};
  CHECK(count.get() == 0);

  count.set(1);
  CHECK(count.get() == 1);

  count = 2;
  CHECK(count.get() == 2);
}

TEST_CASE("var supports std::string") {
  ripple::var<std::string> text{"hello"};
  CHECK(text.get() == "hello");

  text.set("world");
  CHECK(text.get() == "world");
}

TEST_CASE("var supports move-only values") {
  ripple::var<std::unique_ptr<int>> ptr;
  ptr.set(std::make_unique<int>(42));

  REQUIRE(ptr.get() != nullptr);
  CHECK(*ptr.get() == 42);
}

}  // namespace
