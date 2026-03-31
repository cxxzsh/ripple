// Copyright (c) 2026 ripple contributors
// SPDX-License-Identifier: MIT

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include <memory>
#include <string>

#include <ripple/variable.hpp>

namespace ripple {
namespace {

TEST_CASE("variable: default initialization") {
  variable<int> value;
  CHECK(value.get() == 0);
}

TEST_CASE("variable: reads back initial value") {
  variable<int> value{42};
  CHECK(value.get() == 42);
}

TEST_CASE("variable: set replaces stored value") {
  variable<int> value{1};
  value.set(7);
  CHECK(value.get() == 7);
}

TEST_CASE("variable: assignment replaces stored value") {
  variable<int> value{3};
  value = 9;
  CHECK(value.get() == 9);
}

TEST_CASE("variable: supports std::string") {
  variable<std::string> value{"hello"};
  CHECK(value.get() == "hello");

  value = std::string{"world"};
  CHECK(value.get() == "world");
}

TEST_CASE("variable: supports move-only types") {
  variable<std::unique_ptr<int>> value;
  value.set(std::make_unique<int>(123));

  REQUIRE(value.get() != nullptr);
  CHECK(*value.get() == 123);
}

}  // namespace
}  // namespace ripple