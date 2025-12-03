// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2024 Pionix GmbH and Contributors to EVerest

#pragma once

#include <ocpp/common/custom_iterators.hpp>
#include <ocpp/v2/evse.hpp>

namespace ocpp {
namespace v2 {

///
/// \brief Set all connectors of a given evse to unavailable.
/// \param evse The evse.
/// \param persist  True if unavailability should persist. If it is set to false, there will be a check per
///                 connector if it was already set to true and if that is the case, it will be persisted anyway.
///
void set_evse_connectors_unavailable(EvseInterface& evse, bool persist);

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
    virtual bool does_connector_exist(const int32_t evse_id, const CiString<20> connector_type) const = 0;

    /// \brief Check if an evse with \p id exists
    virtual bool does_evse_exist(int32_t id) const = 0;

    /// \brief Checks if all connectors are effectively inoperative.
    /// If this is the case, calls the all_connectors_unavailable_callback
    /// This is used e.g. to allow firmware updates once all transactions have finished
    virtual bool are_all_connectors_effectively_inoperative() const = 0;

    /// \brief Get the number of evses
    virtual size_t get_number_of_evses() const = 0;

    /// \brief Get evseid for the given transaction id.
    /// \param transaction_id   The transactionid
    /// \return The evse id belonging the the transaction id. std::nullopt if there is no transaction with the given
    ///         transaction id.
    ///
    virtual std::optional<int32_t> get_transaction_evseid(const CiString<36>& transaction_id) const = 0;

    /// \brief Helper function to determine if there is any active transaction for the given \p evse
    /// \param evse if optional is not set, this function will check if there is any transaction active for the whole
    /// charging station
    /// \return
    virtual bool any_transaction_active(const std::optional<EVSE>& evse) const = 0;

    ///
    /// \brief Check if the given evse is valid.
    /// \param evse The evse to check.
    /// \return True when evse is valid.
    ///
    virtual bool is_valid_evse(const EVSE& evse) const = 0;

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

    virtual bool does_connector_exist(const int32_t evse_id, const CiString<20> connector_type) const override;
    bool does_evse_exist(const int32_t id) const override;

    bool are_all_connectors_effectively_inoperative() const override;

    size_t get_number_of_evses() const override;

    std::optional<int32_t> get_transaction_evseid(const CiString<36>& transaction_id) const override;

    bool any_transaction_active(const std::optional<EVSE>& evse) const override;
    bool is_valid_evse(const EVSE& evse) const override;

    EvseIterator begin() override;
    EvseIterator end() override;
};

} // namespace v2
} // namespace ocpp
