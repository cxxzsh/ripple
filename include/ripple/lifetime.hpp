// Copyright (c) 2026 ripple contributors
// SPDX-License-Identifier: MIT

#pragma once

namespace ripple {

class lifetime {
 public:
  lifetime() = default;

  lifetime(const lifetime&) = delete;
  lifetime& operator=(const lifetime&) = delete;

  lifetime(lifetime&&) noexcept = default;
  lifetime& operator=(lifetime&&) noexcept = default;

  ~lifetime() = default;

  void reset() noexcept {}

  bool empty() const noexcept { return true; }
};

}  // namespace ripple
