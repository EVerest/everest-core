// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include "iso15118_ocpp_extensionsImpl.hpp"
#include "log.hpp"

namespace module {
namespace extensions_ocpp {

void iso15118_ocpp_extensionsImpl::init() {
    if (!v2g_ctx) {
        dlog(DLOG_LEVEL_ERROR, "v2g_ctx not created");
        return;
    }

    mod->r_ext_ocpp2->subscribe_iso15118_certificate_request([this](const auto o) {
        if (not mod->selected_iso20()) {
            publish_iso15118_certificate_request(o);
        }
    });

    mod->r_ext_ocpp20->subscribe_iso15118_certificate_request([this](const auto o) {
        if (mod->selected_iso20()) {
            publish_iso15118_certificate_request(o);
        }
    });
}

void iso15118_ocpp_extensionsImpl::ready() {
}

void iso15118_ocpp_extensionsImpl::handle_set_get_certificate_response(
    types::iso15118_charger::ResponseExiStreamStatus& certificate_response) {
    if (mod->selected_iso20()) {
        mod->r_ext_ocpp20->call_set_get_certificate_response(certificate_response);
    } else {
        mod->r_ext_ocpp20->call_set_get_certificate_response(certificate_response);
    }
}

} // namespace extensions_ocpp
} // namespace module
