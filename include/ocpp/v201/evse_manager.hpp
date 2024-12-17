// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2024 Pionix GmbH and Contributors to EVerest

#pragma once

#include <ocpp/common/custom_iterators.hpp>
#include <ocpp/v201/evse.hpp>

namespace ocpp {
namespace v201 {

/// \brief Class used to access the Evse instances
class EvseManagerInterface {
public:
    using EvseIterator = ForwardIterator<EvseInterface>;

    /// \brief Default destructor
    virtual ~EvseManagerInterface() = default;

    /// \brief Get a reference to the evse with \p id
    /// \note If \p id is not present this could throw an EvseOutOfRangeException
    virtual EvseInterface& get_evse(int32_t id) = 0;

    /// \brief Get a const reference to the evse with \p id
    /// \note If \p id is not present this could throw an EvseOutOfRangeException
    virtual const EvseInterface& get_evse(int32_t id) const = 0;

    /// \brief Check if the connector exists on the given evse id.
    /// \param evse_id          The evse id to check for.
    /// \param connector_type   The connector type.
    /// \return False if evse id does not exist or evse does not have the given connector type.
    virtual bool does_connector_exist(const int32_t evse_id, ConnectorEnum connector_type) const = 0;

    /// \brief Check if an evse with \p id exists
    virtual bool does_evse_exist(int32_t id) const = 0;

    /// \brief Get the number of evses
    virtual size_t get_number_of_evses() const = 0;

    /// \brief Gets an iterator pointing to the first evse
    virtual EvseIterator begin() = 0;
    /// \brief Gets an iterator pointing past the last evse
    virtual EvseIterator end() = 0;
};

class EvseManager : public EvseManagerInterface {
private:
    std::vector<std::unique_ptr<EvseInterface>> evses;

public:
    EvseManager(const std::map<int32_t, int32_t>& evse_connector_structure, DeviceModel& device_model,
                std::shared_ptr<DatabaseHandler> database_handler,
                std::shared_ptr<ComponentStateManagerInterface> component_state_manager,
                const std::function<void(const MeterValue& meter_value, EnhancedTransaction& transaction)>&
                    transaction_meter_value_req,
                const std::function<void(int32_t evse_id)>& pause_charging_callback);

    EvseInterface& get_evse(int32_t id) override;
    const EvseInterface& get_evse(const int32_t id) const override;

    virtual bool does_connector_exist(const int32_t evse_id, const ConnectorEnum connector_type) const override;
    bool does_evse_exist(const int32_t id) const override;

    size_t get_number_of_evses() const override;

    EvseIterator begin() override;
    EvseIterator end() override;
};

} // namespace v201
} // namespace ocpp
