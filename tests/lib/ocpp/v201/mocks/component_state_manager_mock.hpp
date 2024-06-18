// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

#include <gmock/gmock.h>

#include "ocpp/v201/component_state_manager.hpp"

namespace ocpp::v201 {
class ComponentStateManagerMock : public ComponentStateManagerInterface {
    MOCK_METHOD(void, set_cs_effective_availability_changed_callback,
                (const std::function<void(const OperationalStatusEnum new_status)>& callback));

    MOCK_METHOD(void, set_evse_effective_availability_changed_callback,
                (const std::function<void(const int32_t evse_id, const OperationalStatusEnum new_status)>& callback));

    MOCK_METHOD(void, set_connector_effective_availability_changed_callback,
                (const std::function<void(const int32_t evse_id, const int32_t connector_id,
                                          const OperationalStatusEnum new_status)>& callback));

    MOCK_METHOD(OperationalStatusEnum, get_cs_individual_operational_status, ());
    MOCK_METHOD(OperationalStatusEnum, get_evse_individual_operational_status, (int32_t evse_id));
    MOCK_METHOD(OperationalStatusEnum, get_connector_individual_operational_status,
                (int32_t evse_id, int32_t connector_id));
    MOCK_METHOD(OperationalStatusEnum, get_cs_persisted_operational_status, ());
    MOCK_METHOD(OperationalStatusEnum, get_evse_persisted_operational_status, (int32_t evse_id));
    MOCK_METHOD(OperationalStatusEnum, get_connector_persisted_operational_status,
                (int32_t evse_id, int32_t connector_id));
    MOCK_METHOD(void, set_cs_individual_operational_status, (OperationalStatusEnum new_status, bool persist));
    MOCK_METHOD(void, set_evse_individual_operational_status,
                (int32_t evse_id, OperationalStatusEnum new_status, bool persist));
    MOCK_METHOD(void, set_connector_individual_operational_status,
                (int32_t evse_id, int32_t connector_id, OperationalStatusEnum new_status, bool persist));
    MOCK_METHOD(OperationalStatusEnum, get_evse_effective_operational_status, (int32_t evse_id));
    MOCK_METHOD(OperationalStatusEnum, get_connector_effective_operational_status,
                (int32_t evse_id, int32_t connector_id));
    MOCK_METHOD(ConnectorStatusEnum, get_connector_effective_status, (int32_t evse_id, int32_t connector_id));
    MOCK_METHOD(void, set_connector_occupied, (int32_t evse_id, int32_t connector_id, bool is_occupied));
    MOCK_METHOD(void, set_connector_reserved, (int32_t evse_id, int32_t connector_id, bool is_reserved));
    MOCK_METHOD(void, set_connector_faulted, (int32_t evse_id, int32_t connector_id, bool is_faulted));
    MOCK_METHOD(void, set_connector_unavailable, (int32_t evse_id, int32_t connector_id, bool is_unavailable));
    MOCK_METHOD(void, trigger_all_effective_availability_changed_callbacks, ());
    MOCK_METHOD(void, send_status_notification_all_connectors, ());
    MOCK_METHOD(void, send_status_notification_changed_connectors, ());
    MOCK_METHOD(void, send_status_notification_single_connector, (int32_t evse_id, int32_t connector_id));
};

} // namespace ocpp::v201
