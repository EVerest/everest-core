// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include "auth_token_providerImpl.hpp"

#include <everest/staging/helpers/helpers.hpp>

namespace module {
namespace main {

void auth_token_providerImpl::init() {
    mod->mqtt.subscribe("everest_api/dummy_token_provider/cmd/provide", [this](const std::string& msg) {
        types::authorization::ProvidedIdToken token = json::parse(msg);
        EVLOG_info << "Publishing new dummy token: " << everest::staging::helpers::redact(token);
        publish_provided_token(token);
    });
}

void auth_token_providerImpl::ready() {
}

} // namespace main
} // namespace module
