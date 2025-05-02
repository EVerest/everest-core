// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include "bank_session_token_providerImpl.hpp"

#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <string>

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
        bank_session_token.token = boost::uuids::to_string(boost::uuids::random_generator()());
    }
    return bank_session_token;
}

} // namespace main
} // namespace module
