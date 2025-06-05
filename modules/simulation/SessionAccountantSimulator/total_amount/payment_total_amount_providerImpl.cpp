/*
 * Licensor: Pionix GmbH, 2024
 * License: BaseCamp - License Version 1.0
 *
 * Licensed under the terms and conditions of the BaseCamp License contained in the "LICENSE" file, also available
 * under: https://pionix.com/pionix-license-terms
 * You may not use this file/code except in compliance with said License.
 */

#include "payment_total_amount_providerImpl.hpp"

namespace module {
namespace total_amount {

void payment_total_amount_providerImpl::init() {
}

void payment_total_amount_providerImpl::ready() {
}

bool payment_total_amount_providerImpl::handle_cmd_announce_preauthorization(
    types::payment::BankingPreauthorization& preauthorization) {
    return mod->add_preauthorization(preauthorization);
}

bool payment_total_amount_providerImpl::handle_cmd_withdraw_preauthorization(types::authorization::IdToken& id_token) {
    return mod->withdraw_preauthorization(id_token);
}

} // namespace total_amount
} // namespace module
