// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2024 Pionix GmbH and Contributors to EVerest

#include <ocpp/v2/evse_manager.hpp>

namespace ocpp {
namespace v2 {

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
    if (id <= 0 or id > this->evses.size()) {
        throw EvseOutOfRangeException(id);
    }
    return *this->evses.at(id - 1);
}

const EvseInterface& EvseManager::get_evse(const int32_t id) const {
    if (id <= 0 or id > this->evses.size()) {
        throw EvseOutOfRangeException(id);
    }
    return *this->evses.at(id - 1);
}

bool EvseManager::does_connector_exist(const int32_t evse_id, const CiString<20> connector_type) const {
    const EvseInterface* evse;
    try {
        evse = &this->get_evse(evse_id);
    } catch (const EvseOutOfRangeException&) {
        EVLOG_error << "Evse id " << evse_id << " is not a valid evse id.";
        return false;
    }

    return evse->does_connector_exist(connector_type);
}

bool EvseManager::does_evse_exist(const int32_t id) const {
    return id >= 0 && static_cast<uint64_t>(id) <= this->evses.size();
}

bool EvseManager::are_all_connectors_effectively_inoperative() const {
    // Check that all connectors on all EVSEs are inoperative
    for (const auto& evse : this->evses) {
        for (int connector_id = 1; connector_id <= evse->get_number_of_connectors(); connector_id++) {
            OperationalStatusEnum connector_status = evse->get_connector_effective_operational_status(connector_id);
            if (connector_status == OperationalStatusEnum::Operative) {
                return false;
            }
        }
    }
    return true;
}

size_t EvseManager::get_number_of_evses() const {
    return this->evses.size();
}

std::optional<int32_t> EvseManager::get_transaction_evseid(const CiString<36>& transaction_id) const {
    for (const auto& evse : this->evses) {
        if (evse->has_active_transaction()) {
            if (transaction_id == evse->get_transaction()->get_transaction().transactionId) {
                return evse->get_id();
            }
        }
    }

    return std::nullopt;
}

bool EvseManager::any_transaction_active(const std::optional<EVSE>& evse) const {
    if (!evse.has_value()) {
        for (auto const& evse : this->evses) {
            if (evse->has_active_transaction()) {
                return true;
            }
        }
        return false;
    }
    return this->get_evse(evse.value().id).has_active_transaction();
}

bool EvseManager::is_valid_evse(const EVSE& evse) const {
    return this->does_evse_exist(evse.id) and
           (!evse.connectorId.has_value() or
            this->get_evse(evse.id).get_number_of_connectors() >= evse.connectorId.value());
}

// Free functions

void set_evse_connectors_unavailable(EvseInterface& evse, bool persist) {
    uint32_t number_of_connectors = evse.get_number_of_connectors();

    for (uint32_t i = 1; i <= number_of_connectors; ++i) {
        evse.set_connector_operative_status(static_cast<int32_t>(i), OperationalStatusEnum::Inoperative, persist);
    }
}

} // namespace v2
} // namespace ocpp
