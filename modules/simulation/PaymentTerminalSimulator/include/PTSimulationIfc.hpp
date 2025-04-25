/*
 * Licensor: Pionix GmbH, 2024
 * License: BaseCamp - License Version 1.0
 *
 * Licensed under the terms and conditions of the BaseCamp License contained in the "LICENSE" file, also available
 * under: https://pionix.com/pionix-license-terms
 * You may not use this file/code except in compliance with said License.
 */

#pragma once

#include <functional>
#include <string>
#include <vector>

#include "include/types.hpp"

namespace pterminal {
class PTSimulationIfc {
public:
    virtual ~PTSimulationIfc() = default;

    virtual void add_open_preauthorization(const receipt_id_t& receipt_id, money_amount_t money_amount) = 0;
    virtual void present_RFID_Card(const std::string& card_id) = 0;
    virtual void present_Banking_Card(const std::string& card_id) = 0;
    virtual void set_publish_preauthorizations_callback(const std::function<void(std::string)>& callback) = 0;
};

} // namespace pterminal
