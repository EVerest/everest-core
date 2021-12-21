// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2021 Pionix GmbH and Contributors to EVerest
#include "example_userImpl.hpp"

namespace module {
namespace example_user {

void example_userImpl::init() {
}

void example_userImpl::ready() {
    mod->r_example->call_uses_something("hello_there");
}

} // namespace example_user
} // namespace module
