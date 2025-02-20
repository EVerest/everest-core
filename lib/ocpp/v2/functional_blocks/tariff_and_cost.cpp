// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include <ocpp/v2/functional_blocks/tariff_and_cost.hpp>

#include <ocpp/v2/ctrlr_component_variables.hpp>
#include <ocpp/v2/device_model.hpp>
#include <ocpp/v2/evse_manager.hpp>
#include <ocpp/v2/functional_blocks/meter_values.hpp>

#include <ocpp/v2/messages/CostUpdated.hpp>

const auto DEFAULT_PRICE_NUMBER_OF_DECIMALS = 3;

namespace ocpp::v2 {
TariffAndCost::TariffAndCost(MessageDispatcherInterface<MessageType>& message_dispatcher, DeviceModel& device_model,
                             EvseManagerInterface& evse_manager, MeterValuesInterface& meter_values,
                             std::optional<SetDisplayMessageCallback>& set_display_message_callback,
                             std::optional<SetRunningCostCallback>& set_running_cost_callback,
                             boost::asio::io_service& io_service) :
    message_dispatcher(message_dispatcher),
    device_model(device_model),
    evse_manager(evse_manager),
    meter_values(meter_values),
    set_display_message_callback(set_display_message_callback),
    set_running_cost_callback(set_running_cost_callback),
    io_service(io_service) {
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
        MessageContent personal_message = response.updatedPersonalMessage.value();
        DisplayMessageContent message = message_content_to_display_message_content(personal_message);
        cost_messages.push_back(message);

        // If cost is enabled, the message will be sent to the running cost callback. But if it is not enabled, the
        // tariff message will be sent using the display message callback.
        if (!cost_enabled and this->set_display_message_callback.has_value() and
            this->set_display_message_callback != nullptr) {
            DisplayMessage display_message;
            display_message.message = message;
            display_message.identifier_id = original_message.transactionInfo.transactionId;
            display_message.identifier_type = IdentifierType::TransactionId;
            this->set_display_message_callback.value()({display_message});
        }
    }

    // Check if cost is available and enabled, and if there is a totalcost message.
    if (cost_enabled and response.totalCost.has_value() and this->set_running_cost_callback.has_value()) {
        RunningCost running_cost;
        // We use the original string and convert it to a double ourselves, as the nlohmann library converts it to a
        // float first and then multiply by 10^5 for example (5 decimals) will give some rounding errors. With a initial
        // double instead of float, we have (a bit) more accuracy.
        if (original_transaction_event_response.contains("totalCost")) {
            std::string total_cost = original_transaction_event_response.at("totalCost").dump();
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
                running_cost.meter_value = static_cast<int32_t>(max_meter_value.value());
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
                device_model.get_optional_value<bool>(ControllerComponentVariables::DisplayMessageQRCodeDisplayCapable)
                    .value_or(false)) {
                running_cost.qr_code_text = custom_data.at("qrCodeText");
            }

            // Add multilanguage messages
            if (custom_data.contains("updatedPersonalMessageExtra") and is_multilanguage_enabled()) {
                // Get supported languages, which is stored in the values list of "Language" of
                // "DisplayMessageCtrlr"
                std::optional<VariableMetaData> metadata = device_model.get_variable_meta_data(
                    ControllerComponentVariables::DisplayMessageLanguage.component,
                    ControllerComponentVariables::DisplayMessageLanguage.variable.value());

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
            this->device_model.get_optional_value<int>(ControllerComponentVariables::NumberOfDecimalsForCostValues)
                .value_or(DEFAULT_PRICE_NUMBER_OF_DECIMALS);
        uint32_t decimals =
            (number_of_decimals < 0 ? DEFAULT_PRICE_NUMBER_OF_DECIMALS : static_cast<uint32_t>(number_of_decimals));
        const std::optional<std::string> currency =
            this->device_model.get_value<std::string>(ControllerComponentVariables::TariffCostCtrlrCurrency);
        this->set_running_cost_callback.value()(running_cost, decimals, currency);
    }
}

void TariffAndCost::handle_costupdated_req(const Call<CostUpdatedRequest> call) {
    CostUpdatedResponse response;
    ocpp::CallResult<CostUpdatedResponse> call_result(response, call.uniqueId);

    if (!is_cost_enabled() or !this->set_running_cost_callback.has_value()) {
        this->message_dispatcher.dispatch_call_result(call_result);
        return;
    }

    RunningCost running_cost;
    TriggerMeterValue triggers;

    if (device_model
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

    std::optional<int32_t> transaction_evse_id = this->evse_manager.get_transaction_evseid(running_cost.transaction_id);
    if (!transaction_evse_id.has_value()) {
        // We just put an error in the log as the spec does not define what to do here. It is not possible to return
        // a 'Rejected' or something in that manner.
        EVLOG_error << "Received CostUpdatedRequest, but transaction id is not a valid transaction id.";
    }

    const int number_of_decimals =
        this->device_model.get_optional_value<int>(ControllerComponentVariables::NumberOfDecimalsForCostValues)
            .value_or(DEFAULT_PRICE_NUMBER_OF_DECIMALS);
    uint32_t decimals =
        (number_of_decimals < 0 ? DEFAULT_PRICE_NUMBER_OF_DECIMALS : static_cast<uint32_t>(number_of_decimals));
    const std::optional<std::string> currency =
        this->device_model.get_value<std::string>(ControllerComponentVariables::TariffCostCtrlrCurrency);
    this->set_running_cost_callback.value()(running_cost, decimals, currency);

    this->message_dispatcher.dispatch_call_result(call_result);

    // In OCPP 2.0.1, the chargepoint status trigger is not used.
    if (!triggers.at_energy_kwh.has_value() and !triggers.at_power_kw.has_value() and !triggers.at_time.has_value()) {
        return;
    }

    const std::optional<int32_t> evse_id_opt = this->evse_manager.get_transaction_evseid(running_cost.transaction_id);
    if (!evse_id_opt.has_value()) {
        EVLOG_warning << "Can not set running cost triggers as there is no evse id found with the transaction id from "
                         "the incoming CostUpdatedRequest";
        return;
    }

    const int32_t evse_id = evse_id_opt.value();
    auto& evse = this->evse_manager.get_evse(evse_id);
    evse.set_meter_value_pricing_triggers(
        triggers.at_power_kw, triggers.at_energy_kwh, triggers.at_time,
        [this, evse_id](const std::vector<MeterValue>& meter_values) {
            this->meter_values.meter_values_req(evse_id, meter_values, false);
        },
        this->io_service);
}

bool TariffAndCost::is_multilanguage_enabled() const {
    return this->device_model
        .get_optional_value<bool>(ControllerComponentVariables::CustomImplementationMultiLanguageEnabled)
        .value_or(false);
}

bool TariffAndCost::is_tariff_enabled() const {
    return this->device_model.get_optional_value<bool>(ControllerComponentVariables::TariffCostCtrlrAvailableTariff)
               .value_or(false) and
           this->device_model.get_optional_value<bool>(ControllerComponentVariables::TariffCostCtrlrEnabledTariff)
               .value_or(false);
}

bool TariffAndCost::is_cost_enabled() const {
    return this->device_model.get_optional_value<bool>(ControllerComponentVariables::TariffCostCtrlrAvailableCost)
               .value_or(false) and
           this->device_model.get_optional_value<bool>(ControllerComponentVariables::TariffCostCtrlrEnabledCost)
               .value_or(false);
}
} // namespace ocpp::v2
