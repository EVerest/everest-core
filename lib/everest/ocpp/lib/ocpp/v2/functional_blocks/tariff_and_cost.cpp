// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include <ocpp/v2/functional_blocks/tariff_and_cost.hpp>

#include <ocpp/v2/ctrlr_component_variables.hpp>
#include <ocpp/v2/device_model.hpp>
#include <ocpp/v2/evse_manager.hpp>
#include <ocpp/v2/functional_blocks/functional_block_context.hpp>
#include <ocpp/v2/functional_blocks/meter_values.hpp>

#include <ocpp/v2/messages/CostUpdated.hpp>

const auto DEFAULT_PRICE_NUMBER_OF_DECIMALS = 3;

namespace ocpp::v2 {
TariffAndCost::TariffAndCost(const FunctionalBlockContext& functional_block_context, MeterValuesInterface& meter_values,
                             std::optional<TariffMessageCallback>& tariff_message_callback,
                             std::optional<SetRunningCostCallback>& set_running_cost_callback,
                             boost::asio::io_context& io_context) :
    context(functional_block_context),
    meter_values(meter_values),
    tariff_message_callback(tariff_message_callback),
    set_running_cost_callback(set_running_cost_callback),
    io_context(io_context) {
}

void TariffAndCost::handle_message(const ocpp::EnhancedMessage<MessageType>& message) {
    if (message.messageType == MessageType::CostUpdated) {
        const auto& json_message = message.message;
        this->handle_costupdated_req(json_message);
    } else {
        throw MessageTypeNotImplementedException(message.messageType);
    }
}

void TariffAndCost::handle_cost_and_tariff(const TransactionEventResponse& response,
                                           const TransactionEventRequest& original_message,
                                           const json& original_transaction_event_response) {
    const bool tariff_enabled = this->is_tariff_enabled();

    const bool cost_enabled = this->is_cost_enabled();

    std::vector<DisplayMessageContent> cost_messages;

    // Check if there is a tariff message and if 'Tariff' is available and enabled
    if (response.updatedPersonalMessage.has_value() and tariff_enabled) {
        const MessageContent personal_message = response.updatedPersonalMessage.value();
        const DisplayMessageContent message = message_content_to_display_message_content(personal_message);
        cost_messages.push_back(message);

        // Always publish the tariff message so the tariff_message variable on the session_cost interface is updated.
        // When cost is also enabled, the message will additionally be included in the running cost callback below
        // (if totalCost is present in the same response).
        if (this->tariff_message_callback.has_value() and this->tariff_message_callback != nullptr) {
            TariffMessage tariff_message;
            tariff_message.message = cost_messages;
            tariff_message.ocpp_transaction_id = original_message.transactionInfo.transactionId;
            tariff_message.identifier_id = original_message.transactionInfo.transactionId;
            tariff_message.identifier_type = IdentifierType::TransactionId;
            this->tariff_message_callback.value()({tariff_message});
        }
    }

    // Check if cost is available and enabled, and if there is a totalcost message.
    if (cost_enabled and response.totalCost.has_value() and this->set_running_cost_callback.has_value()) {
        RunningCost running_cost;
        // We use the original string and convert it to a double ourselves, as the nlohmann library converts it to a
        // float first and then multiply by 10^5 for example (5 decimals) will give some rounding errors. With a initial
        // double instead of float, we have (a bit) more accuracy.
        if (original_transaction_event_response.contains("totalCost")) {
            const std::string total_cost = original_transaction_event_response.at("totalCost").dump();
            running_cost.cost = stod(total_cost);
        } else {
            running_cost.cost = static_cast<double>(response.totalCost.value());
        }

        if (original_message.eventType == TransactionEventEnum::Ended) {
            running_cost.state = RunningCostState::Finished;
        } else {
            running_cost.state = RunningCostState::Charging;
        }

        running_cost.transaction_id = original_message.transactionInfo.transactionId;

        if (original_message.meterValue.has_value()) {
            const auto& meter_value = original_message.meterValue.value();
            std::optional<float> max_meter_value;
            for (const MeterValue& mv : meter_value) {
                auto it = std::find_if(mv.sampledValue.begin(), mv.sampledValue.end(), [](const SampledValue& value) {
                    return value.measurand == MeasurandEnum::Energy_Active_Import_Register and !value.phase.has_value();
                });
                if (it != mv.sampledValue.end()) {
                    // Found a sampled metervalue we are searching for!
                    if (!max_meter_value.has_value() or max_meter_value.value() < it->value) {
                        max_meter_value = it->value;
                    }
                }
            }
            if (max_meter_value.has_value()) {
                running_cost.meter_value = static_cast<std::int32_t>(max_meter_value.value());
            }
        }

        running_cost.timestamp = original_message.timestamp;

        if (response.customData.has_value()) {
            // With the current spec, it is not possible to send a qr code as well as a multi language personal
            // message, because there can only be one vendor id in custom data. If you not check the vendor id, it
            // is just possible for a csms to include them both.
            const json& custom_data = response.customData.value();
            if (/*custom_data.contains("vendorId") and
                (custom_data.at("vendorId").get<std::string>() == "org.openchargealliance.org.qrcode") and */
                custom_data.contains("qrCodeText") and
                this->context.device_model
                    .get_optional_value<bool>(ControllerComponentVariables::DisplayMessageQRCodeDisplayCapable)
                    .value_or(false)) {
                running_cost.qr_code_text = custom_data.at("qrCodeText");
            }

            // Add multilanguage messages
            if (custom_data.contains("updatedPersonalMessageExtra") and is_multilanguage_enabled()) {
                // Get supported languages, which is stored in the values list of "Language" of
                // "DisplayMessageCtrlr"
                std::optional<VariableMetaData> metadata;
                if (ControllerComponentVariables::DisplayMessageLanguage.variable.has_value()) {
                    metadata = this->context.device_model.get_variable_meta_data(
                        ControllerComponentVariables::DisplayMessageLanguage.component,
                        ControllerComponentVariables::DisplayMessageLanguage.variable.value());
                }

                std::vector<std::string> supported_languages;

                if (metadata.has_value() and metadata.value().characteristics.valuesList.has_value()) {
                    supported_languages =
                        ocpp::split_string(metadata.value().characteristics.valuesList.value(), ',', true);
                } else {
                    EVLOG_error << "DisplayMessageCtrlr variable Language should have a valuesList with supported "
                                   "languages";
                }

                for (const auto& m : custom_data.at("updatedPersonalMessageExtra").items()) {
                    DisplayMessageContent c = message_content_to_display_message_content(m.value());
                    if (!c.language.has_value()) {
                        EVLOG_warning
                            << "updated personal message extra sent but language unknown: Can not show message.";
                        continue;
                    }

                    if (supported_languages.empty()) {
                        EVLOG_warning << "Can not show personal message as the supported languages are unknown "
                                         "(please set the `valuesList` of `DisplayMessageCtrlr` variable `Language` to "
                                         "set the supported languages)";
                        // Break loop because the next iteration, the supported languages will also not be there.
                        break;
                    }

                    if (std::find(supported_languages.begin(), supported_languages.end(), c.language.value()) !=
                        supported_languages.end()) {
                        cost_messages.push_back(c);
                    } else {
                        EVLOG_warning << "Can not send a personal message text in language " << c.language.value()
                                      << " as it is not supported by the charging station.";
                    }
                }
            }
        }

        if (tariff_enabled and !cost_messages.empty()) {
            running_cost.cost_messages = cost_messages;
        }

        const int number_of_decimals =
            this->context.device_model
                .get_optional_value<int>(ControllerComponentVariables::NumberOfDecimalsForCostValues)
                .value_or(DEFAULT_PRICE_NUMBER_OF_DECIMALS);
        const std::uint32_t decimals = (number_of_decimals < 0 ? DEFAULT_PRICE_NUMBER_OF_DECIMALS
                                                               : static_cast<std::uint32_t>(number_of_decimals));
        const std::optional<std::string> currency =
            this->context.device_model.get_value<std::string>(ControllerComponentVariables::TariffCostCtrlrCurrency);
        this->set_running_cost_callback.value()(running_cost, decimals, currency);
    }
}

void TariffAndCost::handle_costupdated_req(const Call<CostUpdatedRequest> call) {
    const CostUpdatedResponse response;
    const ocpp::CallResult<CostUpdatedResponse> call_result(response, call.uniqueId);

    if (!is_cost_enabled() or !this->set_running_cost_callback.has_value()) {
        this->context.message_dispatcher.dispatch_call_result(call_result);
        return;
    }

    RunningCost running_cost;
    TriggerMeterValue triggers;

    if (this->context.device_model
            .get_optional_value<bool>(ControllerComponentVariables::CustomImplementationCaliforniaPricingEnabled)
            .value_or(false) and
        call.msg.customData.has_value()) {
        const json running_cost_json = call.msg.customData.value();

        // California pricing is enabled, which means we have to read the custom data.
        running_cost = running_cost_json;

        if (running_cost_json.contains("triggerMeterValue")) {
            triggers = running_cost_json.at("triggerMeterValue");
        }
    } else {
        running_cost.state = RunningCostState::Charging;
    }

    // In 2.0.1, the cost and transaction id are already part of the CostUpdatedRequest, so they need to be added to
    // the 'RunningCost' struct.
    running_cost.cost = static_cast<double>(call.msg.totalCost);
    running_cost.transaction_id = call.msg.transactionId;

    const std::optional<std::int32_t> transaction_evse_id =
        this->context.evse_manager.get_transaction_evseid(running_cost.transaction_id);
    if (!transaction_evse_id.has_value()) {
        // We just put an error in the log as the spec does not define what to do here. It is not possible to return
        // a 'Rejected' or something in that manner.
        EVLOG_error << "Received CostUpdatedRequest, but transaction id is not a valid transaction id.";
    }

    const int number_of_decimals =
        this->context.device_model.get_optional_value<int>(ControllerComponentVariables::NumberOfDecimalsForCostValues)
            .value_or(DEFAULT_PRICE_NUMBER_OF_DECIMALS);
    const std::uint32_t decimals =
        (number_of_decimals < 0 ? DEFAULT_PRICE_NUMBER_OF_DECIMALS : static_cast<std::uint32_t>(number_of_decimals));
    const std::optional<std::string> currency =
        this->context.device_model.get_value<std::string>(ControllerComponentVariables::TariffCostCtrlrCurrency);
    this->set_running_cost_callback.value()(running_cost, decimals, currency);

    this->context.message_dispatcher.dispatch_call_result(call_result);

    // In OCPP 2.0.1, the chargepoint status trigger is not used.
    if (!triggers.at_energy_kwh.has_value() and !triggers.at_power_kw.has_value() and !triggers.at_time.has_value()) {
        return;
    }

    const std::optional<std::int32_t> evse_id_opt =
        this->context.evse_manager.get_transaction_evseid(running_cost.transaction_id);
    if (!evse_id_opt.has_value()) {
        EVLOG_warning << "Can not set running cost triggers as there is no evse id found with the transaction id from "
                         "the incoming CostUpdatedRequest";
        return;
    }

    const std::int32_t evse_id = evse_id_opt.value();
    auto& evse = this->context.evse_manager.get_evse(evse_id);
    evse.set_meter_value_pricing_triggers(
        triggers.at_power_kw, triggers.at_energy_kwh, triggers.at_time,
        [this, evse_id](const std::vector<MeterValue>& meter_values) {
            this->meter_values.meter_values_req(evse_id, meter_values, false);
        },
        this->io_context);
}

std::vector<DisplayMessageContent>
TariffAndCost::get_fallback_messages(const ComponentVariable& component_variable) const {
    std::vector<DisplayMessageContent> messages;

    // Default (no-instance) entry
    const auto default_msg = this->context.device_model.get_optional_value<std::string>(component_variable);
    if (default_msg.has_value() and !default_msg.value().empty()) {
        messages.push_back({default_msg.value(), std::nullopt, std::nullopt});
    }

    // Per-language instances — one entry per supported language in DisplayMessageCtrlr.Language
    const auto metadata = this->context.device_model.get_variable_meta_data(
        ControllerComponentVariables::DisplayMessageLanguage.component,
        ControllerComponentVariables::DisplayMessageLanguage.variable.value());
    if (metadata.has_value() and metadata.value().characteristics.valuesList.has_value()) {
        for (const auto& language :
             ocpp::split_string(metadata.value().characteristics.valuesList.value(), ',', true)) {
            Variable var;
            var.name = component_variable.variable.value().name;
            var.instance = language;
            const auto msg =
                this->context.device_model.get_optional_value<std::string>(component_variable.component, var);
            if (msg.has_value() and !msg.value().empty()) {
                messages.push_back({msg.value(), language, std::nullopt});
            }
        }
    }

    return messages;
}

std::optional<TariffMessage> TariffAndCost::get_fallback_tariff_message(bool offline) const {
    if (!is_tariff_enabled()) {
        return std::nullopt;
    }

    // I04.FR.01: prefer the offline-specific variable when disconnected; fall back to the
    // online TariffFallbackMessage when no offline messages are configured.
    auto messages = offline ? get_fallback_messages(ControllerComponentVariables::OfflineTariffFallbackMessage)
                            : get_fallback_messages(ControllerComponentVariables::TariffFallbackMessage);

    if (offline and messages.empty()) {
        EVLOG_debug << "No OfflineTariffFallbackMessage configured, falling back to TariffFallbackMessage";
        messages = get_fallback_messages(ControllerComponentVariables::TariffFallbackMessage);
    }

    if (messages.empty()) {
        return std::nullopt;
    }

    TariffMessage tariff_message;
    tariff_message.message = messages;
    return tariff_message;
}

void TariffAndCost::send_total_cost_fallback_message(const std::string& transaction_id) {
    if (!is_cost_enabled() or !this->tariff_message_callback.has_value() or this->tariff_message_callback == nullptr) {
        return;
    }

    // I05.FR.02: show TotalCostFallbackMessage when the CS is offline at transaction end.
    // This is a human-readable display message only — actual cost data is not available.
    const auto messages = get_fallback_messages(ControllerComponentVariables::TotalCostFallbackMessage);
    if (messages.empty()) {
        return;
    }

    TariffMessage tariff_message;
    tariff_message.message = messages;
    tariff_message.ocpp_transaction_id = transaction_id;
    tariff_message.identifier_id = transaction_id;
    tariff_message.identifier_type = IdentifierType::TransactionId;
    this->tariff_message_callback.value()(tariff_message);
}

void TariffAndCost::ensure_personal_message(IdTokenInfo& id_token_info, bool offline) {
    if (!is_tariff_enabled()) {
        return;
    }

    if (id_token_info.personalMessage.has_value()) {
        // Personal message already set by CSMS — assume it contains the tariff information.
        return;
    }

    const auto fallback = get_fallback_tariff_message(offline);
    if (!fallback.has_value() or fallback->message.empty()) {
        return;
    }

    // Per California Pricing 4.3.4: personalMessage holds the text in the default language
    // (DisplayMessageCtrlr.Language). All other languages go into customData.personalMessageExtra[] (max 4 additional
    // entries).
    const auto default_language =
        this->context.device_model.get_optional_value<std::string>(ControllerComponentVariables::DisplayMessageLanguage)
            .value_or(std::string{});

    const auto& messages = fallback->message;

    // Find the entry matching the default language; fall back to index 0 if none matches.
    std::size_t primary_idx = 0;
    for (std::size_t i = 0; i < messages.size(); ++i) {
        if (!default_language.empty() and messages[i].language.has_value() and
            messages[i].language.value() == default_language) {
            primary_idx = i;
            break;
        }
    }

    // Set personalMessage to the default-language entry.
    const auto& primary = messages[primary_idx];
    MessageContent personal_msg;
    personal_msg.format = primary.message_format.value_or(MessageFormatEnum::UTF8);
    personal_msg.content = CiString<1024>(primary.message);
    if (primary.language.has_value()) {
        personal_msg.language = CiString<8>(primary.language.value());
    }
    id_token_info.personalMessage = personal_msg;

    // Put remaining language entries into customData.personalMessageExtra.
    constexpr std::size_t max_extra = 4;
    json extra = json::array();
    for (std::size_t i = 0; i < messages.size() and extra.size() < max_extra; ++i) {
        if (i == primary_idx) {
            continue;
        }
        json entry;
        entry["content"] = messages[i].message;
        if (messages[i].language.has_value()) {
            entry["language"] = messages[i].language.value();
        }
        extra.push_back(entry);
    }

    if (!extra.empty()) {
        CustomData custom_data = id_token_info.customData.value_or(json{});
        custom_data["vendorId"] = "org.openchargealliance.multilanguage";
        custom_data["personalMessageExtra"] = extra;
        id_token_info.customData = custom_data;
    }
}

bool TariffAndCost::is_multilanguage_enabled() const {
    return this->context.device_model
        .get_optional_value<bool>(ControllerComponentVariables::CustomImplementationMultiLanguageEnabled)
        .value_or(false);
}

bool TariffAndCost::is_tariff_enabled() const {
    return this->context.device_model
               .get_optional_value<bool>(ControllerComponentVariables::TariffCostCtrlrAvailableTariff)
               .value_or(false) and
           this->context.device_model
               .get_optional_value<bool>(ControllerComponentVariables::TariffCostCtrlrEnabledTariff)
               .value_or(false);
}

bool TariffAndCost::is_cost_enabled() const {
    return this->context.device_model
               .get_optional_value<bool>(ControllerComponentVariables::TariffCostCtrlrAvailableCost)
               .value_or(false) and
           this->context.device_model.get_optional_value<bool>(ControllerComponentVariables::TariffCostCtrlrEnabledCost)
               .value_or(false);
}
} // namespace ocpp::v2
