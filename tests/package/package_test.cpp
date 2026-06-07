// Copyright (c) 2026 ripple contributors
// SPDX-License-Identifier: MIT

#include <ripple/ripple.hpp>

#include <string>

int main() {
    ripple::var<std::string> title{"Ready"};
    ripple::var<bool> loading{false};

    auto status = ripple::computed([&] {
        return loading.get() ? std::string{"Loading"} : title.get();
    });

    if (status.get() != "Ready") {
        return 1;
    }

    loading.set(true);
    if (status.get() != "Loading") {
        return 1;
    }

    return 0;
}
