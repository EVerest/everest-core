// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include "iso15118_extensionsImpl.hpp"

namespace module {
namespace extensions {

void iso15118_extensionsImpl::init() {
}

void iso15118_extensionsImpl::ready() {
}

void iso15118_extensionsImpl::handle_set_get_certificate_response(
    types::iso15118_charger::ResponseExiStreamStatus& certificate_response) {
    // your code for cmd set_get_certificate_response goes here
}

} // namespace extensions
} // namespace module
