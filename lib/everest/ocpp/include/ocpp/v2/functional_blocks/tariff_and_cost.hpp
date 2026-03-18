// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#pragma once

#include <ocpp/v2/message_handler.hpp>
#include <ocpp/v2/ocpp_types.hpp>

#include <ocpp/v2/functional_blocks/display_message.hpp>

namespace ocpp::v2 {
struct FunctionalBlockContext;
class MeterValuesInterface;

struct CostUpdatedRequest;

using SetRunningCostCallback = std::function<void(
    const RunningCost& running_cost, const std::uint32_t number_of_decimals, std::optional<std::string> currency_code)>;
using TariffMessageCallback = std::function<void(const TariffMessage& message)>;

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

    ///
    /// \brief Publish the TotalCostFallbackMessage via tariff_message_callback when the CS is offline at
    ///        transaction end and cannot retrieve the actual total cost (I05.FR.02).
    /// \param transaction_id  The id of the transaction that just ended.
    ///
    virtual void send_total_cost_fallback_message(const std::string& transaction_id) = 0;

    ///
    /// \brief Ensure that id_token_info.personalMessage is set when tariff is enabled.
    ///        If personalMessage is already set by the CSMS, this is a no-op.
    ///        Otherwise, the TariffFallbackMessage (or OfflineTariffFallbackMessage when offline) is injected:
    ///          - The default-language entry goes into id_token_info.personalMessage.
    ///          - Up to 4 additional language entries go into
    ///            id_token_info.customData.personalMessageExtra[] per California Pricing spec 4.3.4.
    ///        The caller is responsible for publishing the result (e.g. via tariff_message_callback).
    /// \param id_token_info  The IdTokenInfo to modify in place.
    /// \param offline        When true, reads OfflineTariffFallbackMessage; otherwise TariffFallbackMessage.
    ///
    virtual void ensure_personal_message(IdTokenInfo& id_token_info, bool offline) = 0;
};

class TariffAndCost : public TariffAndCostInterface {
public:
    TariffAndCost(const FunctionalBlockContext& functional_block_context, MeterValuesInterface& meter_values,
                  std::optional<TariffMessageCallback>& tariff_message_callback,
                  std::optional<SetRunningCostCallback>& set_running_cost_callback,
                  boost::asio::io_context& io_context);
    void handle_message(const ocpp::EnhancedMessage<MessageType>& message) override;

    void handle_cost_and_tariff(const TransactionEventResponse& response,
                                const TransactionEventRequest& original_message,
                                const json& original_transaction_event_response) override;

    void send_total_cost_fallback_message(const std::string& transaction_id) override;
    void ensure_personal_message(IdTokenInfo& id_token_info, bool offline) override;

private:
    // Members
    const FunctionalBlockContext& context;
    MeterValuesInterface& meter_values;
    std::optional<TariffMessageCallback> tariff_message_callback;
    std::optional<SetRunningCostCallback> set_running_cost_callback;
    boost::asio::io_context& io_context;

    // Functions
    // Functional Block I: TariffAndCost
    void handle_costupdated_req(const Call<CostUpdatedRequest> call);

    ///
    /// \brief Get the fallback tariff message. Returns the configured TariffFallbackMessage (or
    ///        OfflineTariffFallbackMessage when offline=true, falling back to TariffFallbackMessage if not configured).
    ///        Returns std::nullopt when tariff is not enabled or no fallback messages are configured.
    /// \param offline  When true, reads OfflineTariffFallbackMessage first; otherwise reads TariffFallbackMessage.
    ///
    std::optional<TariffMessage> get_fallback_tariff_message(bool offline) const;

    ///
    /// \brief Read all configured messages for the given ComponentVariable.
    ///        Tries the default (no-instance) entry and, for every supported language, the
    ///        language-specific instance.  Missing or empty values are silently skipped.
    /// \param component_variable  e.g. ControllerComponentVariables::TariffFallbackMessage,
    ///                            OfflineTariffFallbackMessage, or TotalCostFallbackMessage.
    /// \return Vector of DisplayMessageContent, possibly empty when nothing is configured.
    ///
    std::vector<DisplayMessageContent> get_fallback_messages(const ComponentVariable& component_variable) const;

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
