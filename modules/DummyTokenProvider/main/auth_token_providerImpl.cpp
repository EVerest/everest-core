// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include "auth_token_providerImpl.hpp"

namespace module {
namespace main {

void auth_token_providerImpl::init() {
    mod->r_evse->subscribe_session_event([this](types::evse_manager::SessionEvent event) {
        if (event.event == types::evse_manager::SessionEventEnum::SessionStarted) {
            types::authorization::ProvidedIdToken token;

            token.id_token = {config.token, types::authorization::IdTokenType::ISO14443};
            token.authorization_type = types::authorization::string_to_authorization_type(config.type);
            if (config.connector_id > 0) {
                token.connectors.emplace({config.connector_id});
            }

            EVLOG_info << "Publishing new dummy token: " << token.id_token << " ("
                       << types::authorization::authorization_type_to_string(token.authorization_type) << ")";
            publish_provided_token(token);
        }
    });
}

void auth_token_providerImpl::ready() {
}

} // namespace main
} // namespace module
