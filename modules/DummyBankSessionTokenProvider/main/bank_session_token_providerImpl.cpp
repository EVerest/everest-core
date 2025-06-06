// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include "bank_session_token_providerImpl.hpp"

#include <everest/staging/helpers/helpers.hpp>

namespace module {
namespace main {

void bank_session_token_providerImpl::init() {
}

void bank_session_token_providerImpl::ready() {
}

types::payment_terminal::BankSessionToken bank_session_token_providerImpl::handle_get_bank_session_token() {
    types::payment_terminal::BankSessionToken bank_session_token;
    bank_session_token.token = config.token;

    if (config.randomize) {
        bank_session_token.token = everest::staging::helpers::get_uuid();
    }
    return bank_session_token;
}

} // namespace main
} // namespace module
