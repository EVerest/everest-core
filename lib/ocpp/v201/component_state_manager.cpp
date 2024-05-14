// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

#include <ocpp/v201/component_state_manager.hpp>
#include <utility>

using QueryExecutionException = ocpp::common::QueryExecutionException;

namespace ocpp::v201 {

ComponentStateManagerInterface::~ComponentStateManagerInterface() {
}

void ComponentStateManager::read_all_states_from_database_or_set_defaults(
    const std::map<int32_t, int32_t>& evse_connector_structure) {

    this->database->insert_cs_availability(OperationalStatusEnum::Operative, false);
    this->cs_individual_status = this->database->get_cs_availability();

    int num_evses = evse_connector_structure.size();
    for (int evse_id = 1; evse_id <= num_evses; evse_id++) {
        if (evse_connector_structure.count(evse_id) == 0) {
            throw std::invalid_argument("evse_connector_structure should contain EVSE ids counting from 1 upwards.");
        }
        int num_connectors = evse_connector_structure.at(evse_id);
        std::vector<FullConnectorStatus> connector_statuses;

        this->database->insert_evse_availability(evse_id, OperationalStatusEnum::Operative, false);
        OperationalStatusEnum evse_operational = this->database->get_evse_availability(evse_id);

        for (int connector_id = 1; connector_id <= num_connectors; connector_id++) {
            this->database->insert_connector_availability(evse_id, connector_id, OperationalStatusEnum::Operative,
                                                          false);
            OperationalStatusEnum connector_operational =
                this->database->get_connector_availability(evse_id, connector_id);
            FullConnectorStatus full_connector_status{connector_operational, false, false, false, false};
            connector_statuses.push_back(full_connector_status);
        }

        this->evse_and_connector_individual_statuses.push_back(std::make_pair(evse_operational, connector_statuses));
    }
}

void ComponentStateManager::initialize_reported_state_cache() {
    // Initialize the cached statuses (after everything else is done)
    this->last_cs_effective_operational_status = this->get_cs_individual_operational_status();
    for (int evse_id = 1; evse_id <= this->num_evses(); evse_id++) {
        int num_connectors = this->num_connectors(evse_id);
        std::vector<ConnectorStatusEnum> connector_statuses;
        std::vector<OperationalStatusEnum> connector_op_statuses;

        OperationalStatusEnum evse_effective = this->get_evse_effective_operational_status(evse_id);
        for (int connector_id = 1; connector_id <= num_connectors; connector_id++) {
            ConnectorStatusEnum connector_status =
                this->individual_connector_status(evse_id, connector_id).to_connector_status();
            connector_statuses.push_back(connector_status);
            connector_op_statuses.push_back(this->get_connector_effective_operational_status(evse_id, connector_id));
        }

        this->last_evse_and_connector_effective_operational_statuses.push_back(
            std::make_pair(evse_effective, connector_op_statuses));
        this->last_connector_reported_statuses.push_back(connector_statuses);
    }
}

ComponentStateManager::ComponentStateManager(
    const std::map<int32_t, int32_t>& evse_connector_structure, std::shared_ptr<DatabaseHandler> db_handler,
    std::function<bool(const int32_t evse_id, const int32_t connector_id, const ConnectorStatusEnum new_status)>
        send_connector_status_notification_callback) :
    database(std::move(db_handler)),
    send_connector_status_notification_callback(std::move(send_connector_status_notification_callback)) {
    this->read_all_states_from_database_or_set_defaults(evse_connector_structure);
    this->initialize_reported_state_cache();
}

int32_t ComponentStateManager::num_evses() {
    return this->evse_and_connector_individual_statuses.size();
}

void ComponentStateManager::check_evse_id(int32_t evse_id) {
    if (evse_id <= 0 || evse_id > this->num_evses()) {
        std::stringstream errmsg;
        errmsg << "EVSE ID " << evse_id << " out of bounds.";
        throw std::out_of_range(errmsg.str());
    }
}

int32_t ComponentStateManager::num_connectors(int32_t evse_id) {
    check_evse_id(evse_id);
    return this->evse_and_connector_individual_statuses[evse_id - 1].second.size();
}

void ComponentStateManager::check_evse_and_connector_id(int32_t evse_id, int32_t connector_id) {
    this->check_evse_id(evse_id);
    if (connector_id <= 0 || connector_id > this->num_connectors(evse_id)) {
        std::stringstream errmsg;
        errmsg << "Connector ID " << connector_id << "out of bounds for EVSE ID " << evse_id << ".";
        throw std::out_of_range(errmsg.str());
    }
}

OperationalStatusEnum& ComponentStateManager::individual_evse_status(int32_t evse_id) {
    this->check_evse_id(evse_id);
    return this->evse_and_connector_individual_statuses[evse_id - 1].first;
}

FullConnectorStatus& ComponentStateManager::individual_connector_status(int32_t evse_id, int32_t connector_id) {
    this->check_evse_and_connector_id(evse_id, connector_id);
    return this->evse_and_connector_individual_statuses[evse_id - 1].second[connector_id - 1];
}

OperationalStatusEnum& ComponentStateManager::last_evse_effective_status(int32_t evse_id) {
    this->check_evse_id(evse_id);
    return this->last_evse_and_connector_effective_operational_statuses[evse_id - 1].first;
}

OperationalStatusEnum& ComponentStateManager::last_connector_effective_status(int32_t evse_id, int32_t connector_id) {
    this->check_evse_and_connector_id(evse_id, connector_id);
    return this->last_evse_and_connector_effective_operational_statuses[evse_id - 1].second[connector_id - 1];
}

ConnectorStatusEnum& ComponentStateManager::last_connector_reported_status(int32_t evse_id, int32_t connector_id) {
    this->check_evse_and_connector_id(evse_id, connector_id);
    return this->last_connector_reported_statuses[evse_id - 1][connector_id - 1];
}

void ComponentStateManager::trigger_callbacks_cs(bool only_if_state_changed) {
    OperationalStatusEnum current_effective_status = this->get_cs_individual_operational_status();
    if (!only_if_state_changed || this->last_cs_effective_operational_status != current_effective_status) {
        if (this->cs_effective_availability_changed_callback.has_value()) {
            this->cs_effective_availability_changed_callback.value()(current_effective_status);
        }
        this->last_cs_effective_operational_status = current_effective_status;
        for (int32_t evse_id = 1; evse_id <= this->num_evses(); evse_id++) {
            this->trigger_callbacks_evse(evse_id, only_if_state_changed);
        }
    }
}

void ComponentStateManager::trigger_callbacks_evse(int32_t evse_id, bool only_if_state_changed) {
    OperationalStatusEnum current_effective_status = this->get_evse_effective_operational_status(evse_id);
    if (!only_if_state_changed || this->last_evse_effective_status(evse_id) != current_effective_status) {
        if (this->evse_effective_availability_changed_callback.has_value()) {
            this->evse_effective_availability_changed_callback.value()(evse_id, current_effective_status);
        }
        this->last_evse_effective_status(evse_id) = current_effective_status;
        for (int32_t connector_id = 1; connector_id <= this->num_connectors(evse_id); connector_id++) {
            this->trigger_callbacks_connector(evse_id, connector_id, only_if_state_changed);
        }
    }
}

void ComponentStateManager::trigger_callbacks_connector(int32_t evse_id, int32_t connector_id,
                                                        bool only_if_state_changed) {
    // Operational status callbacks
    OperationalStatusEnum current_effective_operational_status =
        this->get_connector_effective_operational_status(evse_id, connector_id);
    OperationalStatusEnum& last_effective_status = this->last_connector_effective_status(evse_id, connector_id);
    if (!only_if_state_changed || last_effective_status != current_effective_operational_status) {
        if (this->connector_effective_availability_changed_callback.has_value()) {
            this->connector_effective_availability_changed_callback.value()(evse_id, connector_id,
                                                                            current_effective_operational_status);
        }
        last_effective_status = current_effective_operational_status;
    }
}

void ComponentStateManager::set_cs_effective_availability_changed_callback(
    const std::function<void(const OperationalStatusEnum new_status)>& callback) {
    this->cs_effective_availability_changed_callback = callback;
}

void ComponentStateManager::set_evse_effective_availability_changed_callback(
    const std::function<void(const int32_t evse_id, const OperationalStatusEnum new_status)>& callback) {
    this->evse_effective_availability_changed_callback = callback;
}

void ComponentStateManager::set_connector_effective_availability_changed_callback(
    const std::function<void(const int32_t evse_id, const int32_t connector_id,
                             const OperationalStatusEnum new_status)>& callback) {
    this->connector_effective_availability_changed_callback = callback;
}

OperationalStatusEnum ComponentStateManager::get_cs_individual_operational_status() {
    return this->cs_individual_status;
}
OperationalStatusEnum ComponentStateManager::get_evse_individual_operational_status(int32_t evse_id) {
    return this->individual_evse_status(evse_id);
}
OperationalStatusEnum ComponentStateManager::get_connector_individual_operational_status(int32_t evse_id,
                                                                                         int32_t connector_id) {
    return this->individual_connector_status(evse_id, connector_id).individual_operational_status;
}

void ComponentStateManager::set_cs_individual_operational_status(OperationalStatusEnum new_status, bool persist) {
    this->cs_individual_status = new_status;
    if (persist) {
        try {
            this->database->insert_cs_availability(new_status, true);
        } catch (const QueryExecutionException& e) {
            EVLOG_warning << "Could not insert charging station availability of id into database: " << e.what();
        }
    }
    this->trigger_callbacks_cs(true);
}
void ComponentStateManager::set_evse_individual_operational_status(int32_t evse_id, OperationalStatusEnum new_status,
                                                                   bool persist) {
    this->individual_evse_status(evse_id) = new_status;
    if (persist) {
        try {
            this->database->insert_evse_availability(evse_id, new_status, true);
        } catch (const QueryExecutionException& e) {
            EVLOG_warning << "Could not insert evse availability of id " << evse_id << " into database: " << e.what();
        }
        this->database->insert_evse_availability(evse_id, new_status, true);
    }
    this->trigger_callbacks_evse(evse_id, true);
}
void ComponentStateManager::set_connector_individual_operational_status(int32_t evse_id, int32_t connector_id,
                                                                        OperationalStatusEnum new_status,
                                                                        bool persist) {
    this->individual_connector_status(evse_id, connector_id).individual_operational_status = new_status;
    if (persist) {
        try {
            this->database->insert_connector_availability(evse_id, connector_id, new_status, true);
        } catch (const QueryExecutionException& e) {
            EVLOG_warning << "Could not insert connector availability of id " << connector_id
                          << " into database: " << e.what();
        }
    }
    this->trigger_callbacks_connector(evse_id, connector_id, true);
}

ConnectorStatusEnum FullConnectorStatus::to_connector_status() {
    // faulted has precedence over unavailable
    if (this->faulted) {
        return ConnectorStatusEnum::Faulted;
    }
    if (this->unavailable) {
        return ConnectorStatusEnum::Unavailable;
    }
    if (this->occupied) {
        return ConnectorStatusEnum::Occupied;
    }
    if (this->reserved) {
        return ConnectorStatusEnum::Reserved;
    }
    return ConnectorStatusEnum::Available;
}

OperationalStatusEnum ComponentStateManager::get_evse_effective_operational_status(int32_t evse_id) {
    this->check_evse_id(evse_id);
    if (this->cs_individual_status == OperationalStatusEnum::Inoperative) {
        return OperationalStatusEnum::Inoperative;
    }
    return this->individual_evse_status(evse_id);
}
ConnectorStatusEnum ComponentStateManager::get_connector_effective_status(int32_t evse_id, int32_t connector_id) {
    this->check_evse_and_connector_id(evse_id, connector_id);
    if (this->get_evse_effective_operational_status(evse_id) == OperationalStatusEnum::Inoperative) {
        return ConnectorStatusEnum::Unavailable;
    }

    return this->individual_connector_status(evse_id, connector_id).to_connector_status();
}
OperationalStatusEnum ComponentStateManager::get_connector_effective_operational_status(int32_t evse_id,
                                                                                        int32_t connector_id) {
    if (this->get_evse_effective_operational_status(evse_id) == OperationalStatusEnum::Inoperative) {
        return OperationalStatusEnum::Inoperative;
    }
    return this->individual_connector_status(evse_id, connector_id).individual_operational_status;
}

OperationalStatusEnum ComponentStateManager::get_cs_persisted_operational_status() {
    return this->database->get_cs_availability();
}
OperationalStatusEnum ComponentStateManager::get_evse_persisted_operational_status(int32_t evse_id) {
    this->check_evse_id(evse_id);
    return this->database->get_evse_availability(evse_id);
}
OperationalStatusEnum ComponentStateManager::get_connector_persisted_operational_status(int32_t evse_id,
                                                                                        int32_t connector_id) {
    this->check_evse_and_connector_id(evse_id, connector_id);
    return this->database->get_connector_availability(evse_id, connector_id);
}

void ComponentStateManager::set_connector_occupied(int32_t evse_id, int32_t connector_id, bool is_occupied) {
    this->individual_connector_status(evse_id, connector_id).occupied = is_occupied;
    this->send_status_notification_single_connector_internal(evse_id, connector_id, true);
}
void ComponentStateManager::set_connector_reserved(int32_t evse_id, int32_t connector_id, bool is_reserved) {
    this->individual_connector_status(evse_id, connector_id).reserved = is_reserved;
    this->send_status_notification_single_connector_internal(evse_id, connector_id, true);
}
void ComponentStateManager::set_connector_faulted(int32_t evse_id, int32_t connector_id, bool is_faulted) {
    this->individual_connector_status(evse_id, connector_id).faulted = is_faulted;
    this->send_status_notification_single_connector_internal(evse_id, connector_id, true);
}

void ComponentStateManager::set_connector_unavailable(int32_t evse_id, int32_t connector_id, bool is_unavailable) {
    this->individual_connector_status(evse_id, connector_id).unavailable = is_unavailable;
    this->send_status_notification_single_connector_internal(evse_id, connector_id, true);
}

void ComponentStateManager::trigger_all_effective_availability_changed_callbacks() {
    this->trigger_callbacks_cs(false);
}

// TODO(Piet): Move to connector file
void ComponentStateManager::send_status_notification_single_connector_internal(int32_t evse_id, int32_t connector_id,
                                                                               bool only_if_changed) {
    ConnectorStatusEnum connector_status =
        this->individual_connector_status(evse_id, connector_id).to_connector_status();
    ConnectorStatusEnum& last_reported_status = this->last_connector_reported_status(evse_id, connector_id);
    if (!only_if_changed || last_reported_status != connector_status) {
        if (this->send_connector_status_notification_callback(evse_id, connector_id, connector_status)) {
            last_reported_status = connector_status;
        }
    }
}
void ComponentStateManager::send_status_notification_all_connectors() {
    for (int evse_id = 1; evse_id <= this->num_evses(); evse_id++) {
        for (int connector_id = 1; connector_id <= this->num_connectors(evse_id); connector_id++) {
            this->send_status_notification_single_connector_internal(evse_id, connector_id, false);
        }
    }
}
void ComponentStateManager::send_status_notification_changed_connectors() {
    for (int evse_id = 1; evse_id <= this->num_evses(); evse_id++) {
        for (int connector_id = 1; connector_id <= this->num_connectors(evse_id); connector_id++) {
            this->send_status_notification_single_connector_internal(evse_id, connector_id, true);
        }
    }
}
void ComponentStateManager::send_status_notification_single_connector(int32_t evse_id, int32_t connector_id) {
    this->send_status_notification_single_connector_internal(evse_id, connector_id, false);
}

} // namespace ocpp::v201
