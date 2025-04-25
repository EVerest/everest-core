/*
 * Licensor: Pionix GmbH, 2024
 * License: BaseCamp - License Version 1.0
 *
 * Licensed under the terms and conditions of the BaseCamp License contained in the "LICENSE" file, also available
 * under: https://pionix.com/pionix-license-terms
 * You may not use this file/code except in compliance with said License.
 */

#pragma once

#include <cstdint>
#include <string>

namespace pterminal {

using receipt_id_t = uint16_t;
using bank_session_token_t = std::string;
using authorization_id_t = std::string;
using card_id_t = std::string;
using money_amount_t = uint32_t;

constexpr receipt_id_t INVALID_RECEIPT_ID{0xFFFF};

} // namespace pterminal