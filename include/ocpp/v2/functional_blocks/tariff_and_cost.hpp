// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#pragma once

#include <ocpp/v2/message_handler.hpp>

#include <ocpp/common/message_dispatcher.hpp>
#include <ocpp/v2/functional_blocks/display_message.hpp>

namespace ocpp::v2 {
class DeviceModel;
class EvseManagerInterface;
class MeterValuesInterface;

struct CostUpdatedRequest;

typedef std::function<void(const RunningCost& running_cost, const uint32_t number_of_decimals,
                           std::optional<std::string> currency_code)>
    SetRunningCostCallback;

class TariffAndCostInterface : public MessageHandlerInterface {
public:
    ///
    /// \brief Create cost and / or tariff message and call the callbacks to send it, if tariff and / or cost is
    /// enabled.
    /// \param response             The TransactionEventResponse where the tariff and cost information is added to.
    /// \param original_message     The original TransactionEventRequest, which contains some information we need as
    ///                             well.
    /// \param original_transaction_event_response  The original json from the response.
    ///
    virtual void handle_cost_and_tariff(const TransactionEventResponse& response,
                                        const TransactionEventRequest& original_message,
                                        const json& original_transaction_event_response) = 0;
};

class TariffAndCost : public TariffAndCostInterface {
public:
    TariffAndCost(MessageDispatcherInterface<MessageType>& message_dispatcher, DeviceModel& device_model,
                  EvseManagerInterface& evse_manager, MeterValuesInterface& meter_values,
                  std::optional<SetDisplayMessageCallback>& set_display_message_callback,
                  std::optional<SetRunningCostCallback>& set_running_cost_callback,
                  boost::asio::io_service& io_service);
    void handle_message(const ocpp::EnhancedMessage<MessageType>& message) override;

    void handle_cost_and_tariff(const TransactionEventResponse& response,
                                const TransactionEventRequest& original_message,
                                const json& original_transaction_event_response) override;

private: // Members
    MessageDispatcherInterface<MessageType>& message_dispatcher;
    DeviceModel& device_model;
    EvseManagerInterface& evse_manager;
    MeterValuesInterface& meter_values;
    std::optional<SetDisplayMessageCallback> set_display_message_callback;
    std::optional<SetRunningCostCallback> set_running_cost_callback;
    boost::asio::io_service& io_service;

private: // Functions
    // Functional Block I: TariffAndCost
    void handle_costupdated_req(const Call<CostUpdatedRequest> call);

    ///
    /// \brief Check if multilanguage setting (variable) is enabled.
    /// \return True if enabled.
    ///
    bool is_multilanguage_enabled() const;

    ///
    /// \brief Check if tariff setting (variable) is enabled.
    /// \return True if enabled.
    ///
    bool is_tariff_enabled() const;

    ///
    /// \brief Check if cost setting (variable) is enabled.
    /// \return True if enabled.
    ///
    bool is_cost_enabled() const;
};
} // namespace ocpp::v2
