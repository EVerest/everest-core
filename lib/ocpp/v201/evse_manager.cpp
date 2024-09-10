// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2024 Pionix GmbH and Contributors to EVerest

#include <ocpp/v201/evse_manager.hpp>

namespace ocpp {
namespace v201 {

using EvseIteratorImpl = VectorOfUniquePtrIterator<EvseInterface>;

EvseManager::EvseManager(const std::map<int32_t, int32_t>& evse_connector_structure, DeviceModel& device_model,
                         std::shared_ptr<DatabaseHandler> database_handler,
                         std::shared_ptr<ComponentStateManagerInterface> component_state_manager,
                         const std::function<void(const MeterValue& meter_value, EnhancedTransaction& transaction)>&
                             transaction_meter_value_req,
                         const std::function<void(int32_t evse_id)>& pause_charging_callback) {
    evses.reserve(evse_connector_structure.size());
    for (const auto& [evse_id, connectors] : evse_connector_structure) {
        evses.push_back(std::make_unique<Evse>(evse_id, connectors, device_model, database_handler,
                                               component_state_manager, transaction_meter_value_req,
                                               pause_charging_callback));
    }
}

EvseManager::EvseIterator EvseManager::begin() {
    return EvseIterator(std::make_unique<EvseIteratorImpl>(this->evses.begin()));
}
EvseManager::EvseIterator EvseManager::end() {
    return EvseIterator(std::make_unique<EvseIteratorImpl>(this->evses.end()));
}

EvseInterface& EvseManager::get_evse(int32_t id) {
    if (id > this->evses.size()) {
        throw EvseOutOfRangeException(id);
    }
    return *this->evses.at(id - 1);
}

const EvseInterface& EvseManager::get_evse(int32_t id) const {
    if (id > this->evses.size()) {
        throw EvseOutOfRangeException(id);
    }
    return *this->evses.at(id - 1);
}

bool EvseManager::does_evse_exist(int32_t id) const {
    return id <= this->evses.size();
}

size_t EvseManager::get_number_of_evses() const {
    return this->evses.size();
}

} // namespace v201
} // namespace ocpp
