// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest
#include <thread>

#include <everest/logging.hpp>

#include <ocpp1_6/charge_point.hpp>
#include <ocpp1_6/charge_point_configuration.hpp>
#include <ocpp1_6/schemas.hpp>

namespace ocpp1_6 {

ChargePoint::ChargePoint(std::shared_ptr<ChargePointConfiguration> configuration) :
    heartbeat_interval(configuration->getHeartbeatInterval()),
    initialized(false),
    registration_status(RegistrationStatus::Pending),
    diagnostics_status(DiagnosticsStatus::Idle),
    firmware_status(FirmwareStatus::Idle) {

    this->configuration = configuration;
    this->connection_state = ChargePointConnectionState::Disconnected;
    this->charging_sessions = std::make_unique<ChargingSessions>(this->configuration->getNumberOfConnectors());

    this->message_queue = std::make_unique<MessageQueue>(
        this->configuration, [this](json message) -> bool { return this->websocket->send(message.dump()); });

    this->websocket = std::make_unique<Websocket>(this->configuration);
    this->websocket->register_connected_callback([this]() {
        this->message_queue->resume(); //
        this->connected_callback();    //
    });
    this->websocket->register_disconnected_callback([this]() {
        this->message_queue->pause(); //
    });

    this->websocket->register_message_callback([this](const std::string& message) {
        this->message_callback(message); //
    });

    this->boot_notification_timer =
        std::make_unique<Everest::SteadyTimer>(&this->io_service, [this]() { this->boot_notification(); });

    this->heartbeat_timer = std::make_unique<Everest::SteadyTimer>(&this->io_service, [this]() { this->heartbeat(); });

    this->clock_aligned_meter_values_timer = std::make_unique<Everest::SystemTimer>(
        &this->io_service, [this]() { this->clock_aligned_meter_values_sample(); });

    this->work = boost::make_shared<boost::asio::io_service::work>(this->io_service);

    this->io_service_thread = std::thread([this]() { this->io_service.run(); });

    this->status = std::make_unique<ChargePointStates>(
        this->configuration->getNumberOfConnectors(),
        [this](int32_t connector, ChargePointErrorCode errorCode, ChargePointStatus status) {
            this->status_notification(connector, errorCode, status);
        });
}

void ChargePoint::heartbeat() {
    EVLOG_debug << "Sending heartbeat";
    HeartbeatRequest req;

    Call<HeartbeatRequest> call(req, this->message_queue->createMessageId());
    this->send<HeartbeatRequest>(call);
}

void ChargePoint::boot_notification() {
    EVLOG_debug << "Sending BootNotification";
    BootNotificationRequest req;
    req.chargeBoxSerialNumber = this->configuration->getChargeBoxSerialNumber();
    req.chargePointModel = this->configuration->getChargePointModel();
    req.chargePointSerialNumber = this->configuration->getChargePointSerialNumber();
    req.chargePointVendor = this->configuration->getChargePointVendor();
    req.firmwareVersion.emplace(this->configuration->getFirmwareVersion());
    req.iccid = this->configuration->getICCID();
    req.imsi = this->configuration->getIMSI();
    req.meterSerialNumber = this->configuration->getMeterSerialNumber();
    req.meterType = this->configuration->getMeterType();

    Call<BootNotificationRequest> call(req, this->message_queue->createMessageId());
    this->send<BootNotificationRequest>(call);
}

void ChargePoint::clock_aligned_meter_values_sample() {
    if (this->initialized) {
        EVLOG_debug << "Sending clock aligned meter values";
        for (int32_t connector = 1; connector < this->configuration->getNumberOfConnectors() + 1; connector++) {
            auto meter_value = this->get_latest_meter_value(
                connector, this->configuration->getMeterValuesAlignedDataVector(), ReadingContext::Sample_Clock);
            this->charging_sessions->add_clock_aligned_meter_value(connector, meter_value);
            this->send_meter_value(connector, meter_value);
        }

        this->update_clock_aligned_meter_values_interval();
    }
}

void ChargePoint::connection_timeout(int32_t connector) {
    if (this->initialized) {
        // FIXME(kai): libfsm this->status_notification(connector, ChargePointErrorCode::NoError,
        // this->status[connector]->timeout());
    }
}

void ChargePoint::update_heartbeat_interval() {
    this->heartbeat_timer->interval(std::chrono::seconds(this->configuration->getHeartbeatInterval()));
}

void ChargePoint::update_meter_values_sample_interval() {
    // TODO(kai): should we update the meter values for continuous monitoring here too?
    int32_t interval = this->configuration->getMeterValueSampleInterval();
    this->charging_sessions->change_meter_values_sample_intervals(interval);
}

void ChargePoint::update_clock_aligned_meter_values_interval() {
    int32_t clock_aligned_data_interval = this->configuration->getClockAlignedDataInterval();
    if (clock_aligned_data_interval == 0) {
        return;
    }
    auto seconds_in_a_day = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::hours(24)).count();
    auto now = date::utc_clock::now();
    auto midnight = date::floor<date::days>(now);
    auto diff = now - midnight;
    auto start = std::chrono::duration_cast<std::chrono::seconds>(diff / clock_aligned_data_interval) *
                     clock_aligned_data_interval +
                 std::chrono::seconds(clock_aligned_data_interval);
    this->clock_aligned_meter_values_time_point = midnight + start;
    using date::operator<<;
    std::ostringstream oss;
    oss << "Sending clock aligned meter values every " << clock_aligned_data_interval << " seconds, starting at "
        << DateTime(this->clock_aligned_meter_values_time_point) << ". This amounts to "
        << seconds_in_a_day / clock_aligned_data_interval << " samples per day.";
    EVLOG_debug << oss.str();

    this->clock_aligned_meter_values_timer->at(this->clock_aligned_meter_values_time_point);
}

int32_t ChargePoint::get_meter_wh(int32_t connector) {
    if (this->power_meter.count(connector) == 0) {
        EVLOG_error << "No power meter entry for connector " << connector << " available. Returning 0.";
        return 0;
    }
    auto power_meter_value = this->power_meter[connector];
    return std::round((double)power_meter_value["energy_Wh_import"]["total"]);
}

MeterValue ChargePoint::get_latest_meter_value(int32_t connector, std::vector<MeasurandWithPhase> values_of_interest,
                                               ReadingContext context) {
    std::lock_guard<std::mutex> lock(power_meter_mutex);
    MeterValue filtered_meter_value;
    // TODO(kai): also support readings from the charge point powermeter at "connector 0"
    if (this->power_meter.count(connector) != 0) {
        auto power_meter_value = this->power_meter[connector];
        auto timestamp =
            std::chrono::time_point<date::utc_clock>(std::chrono::seconds(power_meter_value["timestamp"].get<int>()));
        filtered_meter_value.timestamp = DateTime(timestamp);
        EVLOG_debug << "PowerMeter value for connector: " << connector << ": " << power_meter_value;

        for (auto configured_measurand : values_of_interest) {
            EVLOG_debug << "Value of interest: " << conversions::measurand_to_string(configured_measurand.measurand);
            // constructing sampled value
            SampledValue sample;

            sample.context.emplace(context);
            sample.format.emplace(ValueFormat::Raw); // TODO(kai): support signed data as well

            sample.measurand.emplace(configured_measurand.measurand);
            if (configured_measurand.phase) {
                EVLOG_debug << "  there is a phase configured: "
                             << conversions::phase_to_string(configured_measurand.phase.value());
            }
            switch (configured_measurand.measurand) {
            case Measurand::Energy_Active_Import_Register:
                // Imported energy in Wh (from grid)
                sample.unit.emplace(UnitOfMeasure::Wh);
                sample.location.emplace(Location::Outlet);

                if (configured_measurand.phase) {
                    // phase available and it makes sense here
                    auto phase = configured_measurand.phase.value();
                    sample.phase.emplace(phase);
                    if (phase == Phase::L1) {
                        sample.value =
                            conversions::double_to_string((double)power_meter_value["energy_Wh_import"]["L1"]);
                    }
                    if (phase == Phase::L2) {
                        sample.value =
                            conversions::double_to_string((double)power_meter_value["energy_Wh_import"]["L2"]);
                    }
                    if (phase == Phase::L3) {
                        sample.value =
                            conversions::double_to_string((double)power_meter_value["energy_Wh_import"]["L3"]);
                    }
                    filtered_meter_value.sampledValue.push_back(sample);
                } else {
                    // store total value
                    sample.value =
                        conversions::double_to_string((double)power_meter_value["energy_Wh_import"]["total"]);
                    filtered_meter_value.sampledValue.push_back(sample);
                }
                break;
            case Measurand::Energy_Active_Export_Register:
                // Exported energy in Wh (to grid)
                sample.unit.emplace(UnitOfMeasure::Wh);
                // TODO: which location is appropriate here? Inlet?
                // sample.location.emplace(Location::Outlet);

                if (configured_measurand.phase) {
                    // phase available and it makes sense here
                    auto phase = configured_measurand.phase.value();
                    sample.phase.emplace(phase);
                    if (phase == Phase::L1) {
                        sample.value =
                            conversions::double_to_string((double)power_meter_value["energy_Wh_export"]["L1"]);
                    }
                    if (phase == Phase::L2) {
                        sample.value =
                            conversions::double_to_string((double)power_meter_value["energy_Wh_export"]["L2"]);
                    }
                    if (phase == Phase::L3) {
                        sample.value =
                            conversions::double_to_string((double)power_meter_value["energy_Wh_export"]["L3"]);
                    }
                    filtered_meter_value.sampledValue.push_back(sample);
                } else {
                    // store total value
                    sample.value =
                        conversions::double_to_string((double)power_meter_value["energy_Wh_export"]["total"]);
                    filtered_meter_value.sampledValue.push_back(sample);
                }
                break;
            case Measurand::Power_Active_Import:
                // power flow to EV, Instantaneous power in Watt
                sample.unit.emplace(UnitOfMeasure::W);
                sample.location.emplace(Location::Outlet);

                if (configured_measurand.phase) {
                    // phase available and it makes sense here
                    auto phase = configured_measurand.phase.value();
                    sample.phase.emplace(phase);
                    if (phase == Phase::L1) {
                        sample.value = conversions::double_to_string((double)power_meter_value["power_W"]["L1"]);
                    }
                    if (phase == Phase::L2) {
                        sample.value = conversions::double_to_string((double)power_meter_value["power_W"]["L2"]);
                    }
                    if (phase == Phase::L3) {
                        sample.value = conversions::double_to_string((double)power_meter_value["power_W"]["L3"]);
                    }
                    filtered_meter_value.sampledValue.push_back(sample);
                } else {
                    // store total value
                    sample.value = conversions::double_to_string((double)power_meter_value["power_W"]["total"]);
                    filtered_meter_value.sampledValue.push_back(sample);
                }
                break;
            case Measurand::Voltage:
                // AC supply voltage, Voltage in Volts
                sample.unit.emplace(UnitOfMeasure::V);
                sample.location.emplace(Location::Outlet);

                if (configured_measurand.phase) {
                    // phase available and it makes sense here
                    auto phase = configured_measurand.phase.value();
                    sample.phase.emplace(phase);
                    if (phase == Phase::L1) {
                        sample.value = conversions::double_to_string((double)power_meter_value["voltage_V"]["L1"]);
                    }
                    if (phase == Phase::L2) {
                        sample.value = conversions::double_to_string((double)power_meter_value["voltage_V"]["L2"]);
                    }
                    if (phase == Phase::L3) {
                        sample.value = conversions::double_to_string((double)power_meter_value["voltage_V"]["L3"]);
                    }
                    filtered_meter_value.sampledValue.push_back(sample);
                }
                break;
            case Measurand::Current_Import:
                // current flow to EV in A
                sample.unit.emplace(UnitOfMeasure::A);
                sample.location.emplace(Location::Outlet);

                if (configured_measurand.phase) {
                    // phase available and it makes sense here
                    auto phase = configured_measurand.phase.value();
                    sample.phase.emplace(phase);
                    if (phase == Phase::L1) {
                        sample.value = conversions::double_to_string((double)power_meter_value["current_A"]["L1"]);
                    }
                    if (phase == Phase::L2) {
                        sample.value = conversions::double_to_string((double)power_meter_value["current_A"]["L2"]);
                    }
                    if (phase == Phase::L3) {
                        sample.value = conversions::double_to_string((double)power_meter_value["current_A"]["L3"]);
                    }
                    if (phase == Phase::N) {
                        sample.value = conversions::double_to_string((double)power_meter_value["current_A"]["LN3"]);
                    }
                    filtered_meter_value.sampledValue.push_back(sample);
                }
                break;
            case Measurand::Frequency:
                // Grid frequency in Hertz
                // TODO: which location is appropriate here? Inlet?
                // sample.location.emplace(Location::Outlet);

                if (configured_measurand.phase) {
                    // phase available and it makes sense here
                    auto phase = configured_measurand.phase.value();
                    sample.phase.emplace(phase);
                    if (phase == Phase::L1) {
                        sample.value = conversions::double_to_string((double)power_meter_value["frequency_Hz"]["L1"]);
                    }
                    if (phase == Phase::L2) {
                        sample.value = conversions::double_to_string((double)power_meter_value["frequency_Hz"]["L2"]);
                    }
                    if (phase == Phase::L3) {
                        sample.value = conversions::double_to_string((double)power_meter_value["frequency_Hz"]["L3"]);
                    }
                    filtered_meter_value.sampledValue.push_back(sample);
                }
                break;
            case Measurand::Current_Offered:
                // current offered to EV
                sample.unit.emplace(UnitOfMeasure::A);
                sample.location.emplace(Location::Outlet);
                if (this->max_current_offered.count(connector) == 0) {
                    // something went wrong
                    EVLOG_error << "No max current offered for connector " << connector
                                 << " yet, skipping meter value";
                    break;
                }

                sample.value = conversions::double_to_string(this->max_current_offered[connector]);

                filtered_meter_value.sampledValue.push_back(sample);
                break;

            default:
                break;
            }
        }
    }
    return filtered_meter_value;
}

void ChargePoint::send_meter_value(int32_t connector, MeterValue meter_value) {
    MeterValuesRequest req;
    // connector = 0 designates the main powermeter
    // connector > 0 designates a connector of the charge point
    req.connectorId = connector;
    std::ostringstream oss;
    oss << "Gathering measurands of connector: " << connector;
    if (connector > 0) {
        auto transaction = this->charging_sessions->get_transaction(connector);
        if (transaction != nullptr) {
            auto transaction_id = transaction->get_transaction_id();
            oss << " with active transaction: " << transaction_id;
            req.transactionId.emplace(transaction_id);
        }
    }

    EVLOG_debug << oss.str();

    req.meterValue.push_back(meter_value);

    Call<MeterValuesRequest> call(req, this->message_queue->createMessageId());
    this->send<MeterValuesRequest>(call);
}

void ChargePoint::start() {
    this->websocket->connect();
}

void ChargePoint::stop_all_transactions() {
    this->stop_all_transactions(Reason::Other);
}

void ChargePoint::stop_all_transactions(Reason reason) {
    int32_t number_of_connectors = this->configuration->getNumberOfConnectors();
    for (int32_t connector = 1; connector <= number_of_connectors; connector++) {
        if (this->charging_sessions->transaction_active(connector)) {
            this->charging_sessions->add_stop_energy_wh(
                connector, std::make_shared<StampedEnergyWh>(DateTime(), this->get_meter_wh(connector)));
            this->stop_transaction(connector, reason);
            this->charging_sessions->remove_session(connector);
        }
    }
}

void ChargePoint::stop() {
    EVLOG_info << "Closing";
    this->initialized = false;
    if (this->boot_notification_timer != nullptr) {
        this->boot_notification_timer->stop();
    }
    if (this->heartbeat_timer != nullptr) {
        this->heartbeat_timer->stop();
    }
    if (this->clock_aligned_meter_values_timer != nullptr) {
        this->clock_aligned_meter_values_timer->stop();
    }

    this->stop_all_transactions();

    this->message_queue->stop();

    this->websocket->disconnect();
    this->work.reset();
    this->io_service.stop();
    this->io_service_thread.join();

    this->configuration->close();
    EVLOG_info << "Terminating...";
}

void ChargePoint::connected_callback() {
    switch (this->connection_state) {
    case ChargePointConnectionState::Disconnected: {
        this->connection_state = ChargePointConnectionState::Connected;
        this->boot_notification();
        break;
    }
    case ChargePointConnectionState::Booted: {
        // on_open in a Booted state can happen after a successful reconnect.
        // according to spec, a charge point should not send a BootNotification after a reconnect
        break;
    }
    default:
        EVLOG_error << "Connected but not in state 'Disconnected' or 'Booted', something is wrong: "
                     << this->connection_state;
        break;
    }
}

void ChargePoint::message_callback(const std::string& message) {
    EVLOG_debug << "Received Message: " << message;

    // EVLOG_debug << "json message: " << json_message;
    auto enhanced_message = this->message_queue->receive(message);
    auto json_message = enhanced_message.message;
    // reject unsupported messages
    if (this->configuration->getSupportedMessageTypesReceiving().count(enhanced_message.messageType) == 0) {
        EVLOG_warning << "Received an unsupported message: " << enhanced_message.messageType;
        // FIXME(kai): however, only send a CALLERROR when it is a CALL message we just received
        if (enhanced_message.messageTypeId == MessageTypeId::CALL) {
            auto call_error = CallError(enhanced_message.uniqueId, "NotSupported", "", json({}));
            this->send(call_error);
        }

        // in any case stop message handling here:
        return;
    }

    switch (this->connection_state) {
    case ChargePointConnectionState::Disconnected: {
        EVLOG_error << "Received a message in disconnected state, this cannot be correct";
        break;
    }
    case ChargePointConnectionState::Connected: {
        if (enhanced_message.messageType == MessageType::BootNotificationResponse) {
            this->handleBootNotificationResponse(json_message);
        }
        break;
    }
    case ChargePointConnectionState::Rejected: {
        if (this->registration_status == RegistrationStatus::Rejected) {
            if (enhanced_message.messageType == MessageType::BootNotificationResponse) {
                this->handleBootNotificationResponse(json_message);
            }
        }
        break;
    }
    case ChargePointConnectionState::Pending: {
        if (this->registration_status == RegistrationStatus::Pending) {
            if (enhanced_message.messageType == MessageType::BootNotificationResponse) {
                this->handleBootNotificationResponse(json_message);
            } else {
                this->handle_message(json_message, enhanced_message.messageType);
            }
        }
        break;
    }
    case ChargePointConnectionState::Booted: {
        this->handle_message(json_message, enhanced_message.messageType);
        break;
    }

    default:
        EVLOG_error << "Reached default statement in on_message, this should not be possible";
        break;
    }
}

void ChargePoint::handle_message(const json& json_message, MessageType message_type) {
    // lots of messages are allowed here
    switch (message_type) {

    case MessageType::AuthorizeResponse:
        // handled by authorize_id_tag future
        break;

    case MessageType::ChangeAvailability:
        this->handleChangeAvailabilityRequest(json_message);
        break;

    case MessageType::ChangeConfiguration:
        this->handleChangeConfigurationRequest(json_message);
        break;

    case MessageType::ClearCache:
        this->handleClearCacheRequest(json_message);
        break;

    case MessageType::DataTransfer:
        this->handleDataTransferRequest(json_message);
        break;

    case MessageType::DataTransferResponse:
        // handled by data_transfer future
        break;

    case MessageType::GetConfiguration:
        this->handleGetConfigurationRequest(json_message);
        break;

    case MessageType::RemoteStartTransaction:
        this->handleRemoteStartTransactionRequest(json_message);
        break;

    case MessageType::RemoteStopTransaction:
        this->handleRemoteStopTransactionRequest(json_message);
        break;

    case MessageType::Reset:
        this->handleResetRequest(json_message);
        break;

    case MessageType::StartTransactionResponse:
        // handled by start_transaction future
        break;

    case MessageType::StopTransactionResponse:
        // handled by stop_transaction future
        break;

    case MessageType::UnlockConnector:
        this->handleUnlockConnectorRequest(json_message);
        break;

    case MessageType::SetChargingProfile:
        this->handleSetChargingProfileRequest(json_message);
        break;

    case MessageType::GetCompositeSchedule:
        this->handleGetCompositeScheduleRequest(json_message);
        break;

    case MessageType::ClearChargingProfile:
        this->handleClearChargingProfileRequest(json_message);
        break;

    case MessageType::TriggerMessage:
        this->handleTriggerMessageRequest(json_message);
        break;

    case MessageType::GetDiagnostics:
        this->handleGetDiagnosticsRequest(json_message);
        break;

    case MessageType::UpdateFirmware:
        this->handleUpdateFirmwareRequest(json_message);
        break;

    case MessageType::ReserveNow:
        this->handleReserveNowRequest(json_message);
        break;

    case MessageType::CancelReservation:
        this->handleCancelReservationRequest(json_message);
        break;

    default:
        // TODO(kai): not implemented error?
        break;
    }
}

void ChargePoint::handleBootNotificationResponse(CallResult<BootNotificationResponse> call_result) {
    EVLOG_debug << "Received BootNotificationResponse: " << call_result.msg
                 << "\nwith messageId: " << call_result.uniqueId;

    this->registration_status = call_result.msg.status;
    this->initialized = true;
    this->boot_time = date::utc_clock::now();
    if (call_result.msg.interval > 0) {
        this->configuration->setHeartbeatInterval(call_result.msg.interval);
    }
    switch (call_result.msg.status) {
    case RegistrationStatus::Accepted: {
        this->connection_state = ChargePointConnectionState::Booted;
        // we are allowed to send messages to the central system
        // activate heartbeat
        this->update_heartbeat_interval();

        // activate clock aligned sampling of meter values
        this->update_clock_aligned_meter_values_interval();

        auto connector_availability = this->configuration->getConnectorAvailability();
        connector_availability[0] =
            AvailabilityType::Operative; // FIXME(kai): fix internal representation in charge point states, we need a
                                         // different kind of state machine for connector 0 anyway (with reduced states)
        this->status->run(connector_availability);

        break;
    }
    case RegistrationStatus::Pending:
        this->connection_state = ChargePointConnectionState::Pending;

        EVLOG_debug << "BootNotification response is pending, trying again in "
                     << this->configuration->getHeartbeatInterval() << "s";

        this->boot_notification_timer->timeout(std::chrono::seconds(this->configuration->getHeartbeatInterval()));
        break;
    default:
        this->connection_state = ChargePointConnectionState::Rejected;
        // In this state we are not allowed to send any messages to the central system, even when
        // requested. The first time we are allowed to send a message (a BootNotification) is
        // after boot_time + heartbeat_interval if the msg.interval is 0, or after boot_timer + msg.interval
        EVLOG_debug << "BootNotification was rejected, trying again in " << this->configuration->getHeartbeatInterval()
                     << "s";

        this->boot_notification_timer->timeout(std::chrono::seconds(this->configuration->getHeartbeatInterval()));

        break;
    }
}
void ChargePoint::handleChangeAvailabilityRequest(Call<ChangeAvailabilityRequest> call) {
    EVLOG_debug << "Received ChangeAvailabilityRequest: " << call.msg << "\nwith messageId: " << call.uniqueId;

    ChangeAvailabilityResponse response;
    // we can only change the connector availability if there is no active transaction on this
    // connector. is that case this change must be scheduled and we should report an availability status
    // of "Scheduled"

    std::vector<int32_t> connectors;
    bool transaction_running = false;

    if (call.msg.connectorId == 0) {
        int32_t number_of_connectors = this->configuration->getNumberOfConnectors();
        for (int32_t connector = 1; connector <= number_of_connectors; connector++) {
            if (this->charging_sessions->transaction_active(connector)) {
                transaction_running = true;
                std::lock_guard<std::mutex> change_availability_lock(change_availability_mutex);
                this->change_availability_queue[connector] = call.msg.type;
            } else {
                connectors.push_back(connector);
            }
        }
    } else {
        if (this->charging_sessions->transaction_active(call.msg.connectorId)) {
            transaction_running = true;
        } else {
            connectors.push_back(call.msg.connectorId);
        }
    }

    if (transaction_running) {
        response.status = AvailabilityStatus::Scheduled;
    } else {
        for (auto connector : connectors) {
            bool availability_change_succeeded =
                this->configuration->setConnectorAvailability(connector, call.msg.type);
            if (availability_change_succeeded) {
                if (call.msg.type == AvailabilityType::Operative) {
                    if (this->enable_evse_callback != nullptr) {
                        // TODO(kai): check return value
                        this->enable_evse_callback(connector);
                    }

                    this->status->submit_event(connector, Event_BecomeAvailable());
                } else {
                    if (this->disable_evse_callback != nullptr) {
                        // TODO(kai): check return value
                        this->disable_evse_callback(connector);
                    }
                    this->status->submit_event(connector, Event_ChangeAvailabilityToUnavailable());
                }
            }
        }

        response.status = AvailabilityStatus::Accepted;
    }

    CallResult<ChangeAvailabilityResponse> call_result(response, call.uniqueId);
    this->send<ChangeAvailabilityResponse>(call_result);
}

void ChargePoint::handleChangeConfigurationRequest(Call<ChangeConfigurationRequest> call) {
    EVLOG_debug << "Received ChangeConfigurationRequest: " << call.msg << "\nwith messageId: " << call.uniqueId;

    ChangeConfigurationResponse response;

    auto kv = this->configuration->get(call.msg.key);
    if (kv) {
        if (kv.value().readonly) {
            // supported but could not be changed
            response.status = ConfigurationStatus::Rejected;
        } else {
            // TODO(kai): how to signal RebootRequired? or what does need reboot required?
            response.status = this->configuration->set(call.msg.key, call.msg.value);
            if (response.status == ConfigurationStatus::Accepted) {
                if (call.msg.key == "HeartbeatInterval") {
                    this->update_heartbeat_interval();
                }
                if (call.msg.key == "MeterValueSampleInterval") {
                    this->update_meter_values_sample_interval();
                }
                if (call.msg.key == "ClockAlignedDataInterval") {
                    this->update_clock_aligned_meter_values_interval();
                }
            }
        }
    } else {
        response.status = ConfigurationStatus::NotSupported;
    }

    CallResult<ChangeConfigurationResponse> call_result(response, call.uniqueId);
    this->send<ChangeConfigurationResponse>(call_result);
}

void ChargePoint::handleClearCacheRequest(Call<ClearCacheRequest> call) {
    EVLOG_debug << "Received ClearCacheRequest: " << call.msg << "\nwith messageId: " << call.uniqueId;

    ClearCacheResponse response;

    if (this->configuration->clearAuthorizationCache()) {
        response.status = ClearCacheStatus::Accepted;
    } else {
        response.status = ClearCacheStatus::Rejected;
    }

    CallResult<ClearCacheResponse> call_result(response, call.uniqueId);
    this->send<ClearCacheResponse>(call_result);
}

void ChargePoint::handleDataTransferRequest(Call<DataTransferRequest> call) {
    EVLOG_debug << "Received DataTransferRequest: " << call.msg << "\nwith messageId: " << call.uniqueId;

    DataTransferResponse response;

    auto vendorId = call.msg.vendorId.get();
    auto messageId = call.msg.messageId.get_value_or(CiString50Type()).get();
    std::function<void(const std::string data)> callback;
    {
        std::lock_guard<std::mutex> lock(data_transfer_callbacks_mutex);
        if (this->data_transfer_callbacks.count(vendorId) == 0) {
            response.status = DataTransferStatus::UnknownVendorId;
        } else {
            if (this->data_transfer_callbacks[vendorId].count(messageId) == 0) {
                response.status = DataTransferStatus::UnknownMessageId;
            } else {
                response.status = DataTransferStatus::Accepted;
                callback = this->data_transfer_callbacks[vendorId][messageId];
            }
        }
    }

    CallResult<DataTransferResponse> call_result(response, call.uniqueId);
    this->send<DataTransferResponse>(call_result);

    if (response.status == DataTransferStatus::Accepted) {
        callback(call.msg.data.get_value_or(std::string("")));
    }
}

void ChargePoint::handleGetConfigurationRequest(Call<GetConfigurationRequest> call) {
    EVLOG_debug << "Received GetConfigurationRequest: " << call.msg << "\nwith messageId: " << call.uniqueId;

    GetConfigurationResponse response;
    std::vector<KeyValue> configurationKey;
    std::vector<CiString50Type> unknownKey;

    if (!call.msg.key) {
        EVLOG_debug << "empty request, sending all configuration keys...";
        configurationKey = this->configuration->get_all_key_value();
    } else {
        auto keys = call.msg.key.value();
        if (keys.empty()) {
            EVLOG_debug << "key field is empty, sending all configuration keys...";
            configurationKey = this->configuration->get_all_key_value();
        } else {
            EVLOG_debug << "specific requests for some keys";
            for (const auto& key : call.msg.key.value()) {
                EVLOG_debug << "retrieving key: " << key;
                auto kv = this->configuration->get(key);
                if (kv) {
                    configurationKey.push_back(kv.value());
                } else {
                    unknownKey.push_back(key);
                }
            }
        }
    }

    if (!configurationKey.empty()) {
        response.configurationKey.emplace(configurationKey);
    }
    if (!unknownKey.empty()) {
        response.unknownKey.emplace(unknownKey);
    }

    CallResult<GetConfigurationResponse> call_result(response, call.uniqueId);
    this->send<GetConfigurationResponse>(call_result);
}

void ChargePoint::handleRemoteStartTransactionRequest(Call<RemoteStartTransactionRequest> call) {
    EVLOG_debug << "Received RemoteStartTransactionRequest: " << call.msg << "\nwith messageId: " << call.uniqueId;

    // a charge point may reject a remote start transaction request without a connectorId
    // TODO(kai): what is our policy here? reject for now
    RemoteStartTransactionResponse response;
    if (!call.msg.connectorId) {
        EVLOG_warning << "RemoteStartTransactionRequest without a connector id is not supported at the moment.";
        response.status = RemoteStartStopStatus::Rejected;
        CallResult<RemoteStartTransactionResponse> call_result(response, call.uniqueId);
        this->send<RemoteStartTransactionResponse>(call_result);
        return;
    }
    int32_t connector = call.msg.connectorId.value();
    if (this->configuration->getConnectorAvailability(connector) == AvailabilityType::Inoperative) {
        response.status = RemoteStartStopStatus::Rejected;
        CallResult<RemoteStartTransactionResponse> call_result(response, call.uniqueId);
        this->send<RemoteStartTransactionResponse>(call_result);
        return;
    }
    if (call.msg.chargingProfile) {
        // TODO(kai): A charging profile was provided, forward to the charger
    }

    response.status = RemoteStartStopStatus::Accepted;
    CallResult<RemoteStartTransactionResponse> call_result(response, call.uniqueId);
    this->send<RemoteStartTransactionResponse>(call_result);

    this->status->submit_event(connector, Event_UsageInitiated());

    {
        std::lock_guard<std::mutex> lock(remote_start_transaction_mutex);

        this->remote_start_transaction[call.uniqueId] = std::thread([this, call, connector]() {
            bool authorized = true;
            if (this->configuration->getAuthorizeRemoteTxRequests()) {
                // need to authorize first
                authorized = (this->authorize_id_tag(call.msg.idTag) == AuthorizationStatus::Accepted);
            } else {
                // no explicit authorization is requested, implicitly authorizing this idTag internally
                IdTagInfo idTagInfo;
                idTagInfo.status = AuthorizationStatus::Accepted;
                this->charging_sessions->add_authorized_token(call.msg.connectorId.value(), call.msg.idTag, idTagInfo);
            }
            if (authorized) {
                // FIXME(kai): this probably needs to be signalled to the evse_manager in some way? we at least need the
                // start_energy and start_timestamp from the evse manager to properly start the transaction
                if (!this->start_transaction(connector)) {
                    EVLOG_error << "Could not start a remotely requested transaction on connector: " << connector;
                }
            }
        });
    }
}

void ChargePoint::handleRemoteStopTransactionRequest(Call<RemoteStopTransactionRequest> call) {
    EVLOG_debug << "Received RemoteStopTransactionRequest: " << call.msg << "\nwith messageId: " << call.uniqueId;

    RemoteStopTransactionResponse response;
    response.status = RemoteStartStopStatus::Rejected;

    auto connector = this->charging_sessions->get_connector_from_transaction_id(call.msg.transactionId);
    if (connector > 0) {
        response.status = RemoteStartStopStatus::Accepted;
    }

    CallResult<RemoteStopTransactionResponse> call_result(response, call.uniqueId);
    this->send<RemoteStopTransactionResponse>(call_result);

    if (connector > 0) {
        this->cancel_charging_callback(connector);
        this->charging_sessions->add_stop_energy_wh(
            connector, std::make_shared<StampedEnergyWh>(DateTime(), this->get_meter_wh(connector)));
        {
            std::lock_guard<std::mutex> lock(remote_stop_transaction_mutex);
            this->remote_stop_transaction[call.uniqueId] = std::thread([this, connector, call]() {
                this->stop_transaction(connector, Reason::Remote);
                this->charging_sessions->remove_session(connector);
            });
        }
    }
}

void ChargePoint::handleResetRequest(Call<ResetRequest> call) {
    EVLOG_debug << "Received ResetRequest: " << call.msg << "\nwith messageId: " << call.uniqueId;

    ResetResponse response;
    response.status = ResetStatus::Accepted;

    CallResult<ResetResponse> call_result(response, call.uniqueId);
    this->send<ResetResponse>(call_result);

    if (call.msg.type == ResetType::Hard) {
        // TODO(kai): implement hard reset, if possible send StopTransaction for any running
        // transactions This type of reset should restart all hardware!
        this->reset_thread = std::thread([this]() {
            this->stop_all_transactions(Reason::SoftReset);
            kill(getpid(), SIGINT); // FIXME(kai): this leads to a somewhat dirty reset
        });
    }
    if (call.msg.type == ResetType::Soft) {
        // TODO(kai): gracefully stop all transactions and send StopTransaction. Restart software
        // afterwards
        this->reset_thread = std::thread([this]() {
            this->stop_all_transactions(Reason::SoftReset);
            kill(getpid(), SIGINT); // FIXME(kai): this leads to a somewhat dirty reset
        });
    }
}

void ChargePoint::handleUnlockConnectorRequest(Call<UnlockConnectorRequest> call) {
    EVLOG_debug << "Received UnlockConnectorRequest: " << call.msg << "\nwith messageId: " << call.uniqueId;

    UnlockConnectorResponse response;
    auto connector = call.msg.connectorId;
    if (connector == 0 || connector > this->configuration->getNumberOfConnectors()) {
        response.status = UnlockStatus::NotSupported;
    } else {
        // this message is not intended to remotely stop a transaction, but if a transaction is still ongoing it is
        // advised to stop it first
        CiString20Type idTag;
        int32_t transactionId;
        if (this->charging_sessions->transaction_active(connector)) {
            this->stop_transaction(connector, Reason::UnlockCommand);
        }

        if (this->unlock_connector_callback != nullptr) {
            if (this->unlock_connector_callback(call.msg.connectorId)) {
                response.status = UnlockStatus::Unlocked;
            } else {
                response.status = UnlockStatus::UnlockFailed;
            }
        } else {
            response.status = UnlockStatus::NotSupported;
        }
    }

    CallResult<UnlockConnectorResponse> call_result(response, call.uniqueId);
    this->send<UnlockConnectorResponse>(call_result);
}

void ChargePoint::handleSetChargingProfileRequest(Call<SetChargingProfileRequest> call) {
    EVLOG_debug << "Received SetChargingProfileRequest: " << call.msg << "\nwith messageId: " << call.uniqueId;

    SetChargingProfileResponse response;
    response.status = ChargingProfileStatus::Rejected;
    auto number_of_connectors = this->configuration->getNumberOfConnectors();
    if (call.msg.connectorId > number_of_connectors || call.msg.connectorId < 0 ||
        call.msg.csChargingProfiles.stackLevel < 0 ||
        call.msg.csChargingProfiles.stackLevel > this->configuration->getChargeProfileMaxStackLevel()) {
        response.status = ChargingProfileStatus::Rejected;
    } else {
        // to avoid conflicts, the existence of multiple charging profiles with the same purpose and stack level is not
        // allowed, when we receive such a request we have to replace the existing one with the new one
        if (call.msg.csChargingProfiles.chargingProfilePurpose == ChargingProfilePurposeType::ChargePointMaxProfile) {
            if (call.msg.connectorId != 0) {
                response.status = ChargingProfileStatus::Rejected;

            } else {
                // ChargePointMaxProfile can only be set for the whole charge point (connector = 0)
                EVLOG_debug << "Received ChargePointMaxProfile Charging Profile:\n    " << call.msg.csChargingProfiles;
                if (call.msg.csChargingProfiles.chargingProfileKind == ChargingProfileKindType::Relative) {
                    response.status = ChargingProfileStatus::Rejected;
                } else {
                    // FIXME(kai): only allow absolute or recurring profiles here
                    if (call.msg.csChargingProfiles.chargingProfileKind == ChargingProfileKindType::Absolute) {
                        // FIXME(kai): hint: without a start schedule this absolute schedule is relative to start of
                        // charging
                        std::lock_guard<std::mutex> charge_point_max_profiles_lock(charge_point_max_profiles_mutex);
                        this->charge_point_max_profiles[call.msg.csChargingProfiles.stackLevel] =
                            call.msg.csChargingProfiles;
                        response.status = ChargingProfileStatus::Accepted;
                    } else if (call.msg.csChargingProfiles.chargingProfileKind == ChargingProfileKindType::Recurring) {
                        if (!call.msg.csChargingProfiles.recurrencyKind) {
                            response.status = ChargingProfileStatus::Rejected;
                        } else {
                            // we only accept recurring schedules with a recurrency kind
                            // here we don't neccesarily need a startSchedule, but if it is missing we just assume
                            // 0:00:00 UTC today
                            auto midnight = date::floor<date::days>(date::utc_clock::now());
                            auto start_schedule = DateTime(midnight);
                            if (!call.msg.csChargingProfiles.chargingSchedule.startSchedule) {
                                EVLOG_debug
                                    << "No startSchedule provided for a recurring charging profile, setting to "
                                    << start_schedule << " (midnight today)";
                                call.msg.csChargingProfiles.chargingSchedule.startSchedule.emplace(start_schedule);
                            }
                            std::lock_guard<std::mutex> charge_point_max_profiles_lock(charge_point_max_profiles_mutex);
                            this->charge_point_max_profiles[call.msg.csChargingProfiles.stackLevel] =
                                call.msg.csChargingProfiles;
                            response.status = ChargingProfileStatus::Accepted;
                        }
                    }
                }
            }
        }
        if (call.msg.csChargingProfiles.chargingProfilePurpose == ChargingProfilePurposeType::TxDefaultProfile) {
            // can be used for charging policies, like disabling charging during specific times
            // connector = 0 applies to all connectors
            // connector > 1 applies only to that connector, if a default profile for connector = 0 is already installed
            // the one for a specific connector overwrites that one!
            std::lock_guard<std::mutex> tx_default_profiles_lock(tx_default_profiles_mutex);
            if (call.msg.connectorId == 0) {
                for (int32_t connector = 1; connector < number_of_connectors; connector++) {
                    this->tx_default_profiles[connector][call.msg.csChargingProfiles.stackLevel] =
                        call.msg.csChargingProfiles;
                }
            } else {
                this->tx_default_profiles[call.msg.connectorId][call.msg.csChargingProfiles.stackLevel] =
                    call.msg.csChargingProfiles;
            }
            response.status = ChargingProfileStatus::Accepted;
        }
        if (call.msg.csChargingProfiles.chargingProfilePurpose == ChargingProfilePurposeType::TxProfile) {
            if (call.msg.connectorId == 0) {
                response.status = ChargingProfileStatus::Rejected;
            } else {
                // shall overrule a TxDefaultProfile for the duration of the running transaction
                // if there is no running transaction on the specified connector this change shall be discarded
                if (this->charging_sessions->transaction_active(call.msg.connectorId)) {
                    // FIXME(kai): set charging profile for this transaction and notify interested parties (energy
                    // manager?) this->active_transactions[call.msg.connectorId].tx_charging_profile.emplace(
                    //     call.msg.csChargingProfiles);
                    response.status = ChargingProfileStatus::Accepted;
                } else {
                    response.status = ChargingProfileStatus::Rejected;
                }
            }
        }
    }

    CallResult<SetChargingProfileResponse> call_result(response, call.uniqueId);
    this->send<SetChargingProfileResponse>(call_result);
}

void ChargePoint::handleGetCompositeScheduleRequest(Call<GetCompositeScheduleRequest> call) {
    EVLOG_debug << "Received GetCompositeScheduleRequest: " << call.msg << "\nwith messageId: " << call.uniqueId;

    GetCompositeScheduleResponse response;

    auto number_of_connectors = this->configuration->getNumberOfConnectors();
    if (call.msg.connectorId > number_of_connectors || call.msg.connectorId < 0) {
        response.status = GetCompositeScheduleStatus::Rejected;
    } else {
        auto start_time = date::utc_clock::now();
        auto start_datetime = DateTime(start_time);
        auto midnight_of_start_time = date::floor<date::days>(start_time);
        auto duration = std::chrono::seconds(call.msg.duration);
        auto end_time = start_time + duration;
        auto end_datetime = DateTime(end_time);

        using date::operator<<;
        std::ostringstream oss;
        oss << "Calculating composite schedule from " << start_time << " to " << end_time;
        EVLOG_debug << oss.str();
        if (call.msg.connectorId == 0) {
            EVLOG_debug << "Calculate expected consumption for the grid connection";
            response.scheduleStart.emplace(start_datetime);
            ChargingSchedule composite_schedule; // = get_composite_schedule(start_time, duration,
                                                 // call.msg.chargingRateUnit); // TODO
            composite_schedule.duration.emplace(call.msg.duration);
            composite_schedule.startSchedule.emplace(start_datetime);
            if (call.msg.chargingRateUnit) {
                composite_schedule.chargingRateUnit = call.msg.chargingRateUnit.value();
            } else {
                composite_schedule.chargingRateUnit = ChargingRateUnit::A; // TODO default & conversion
            }
            // charge point max profiles
            std::vector<ChargingProfile> valid_profiles;
            std::unique_lock<std::mutex> charge_point_max_profiles_lock(charge_point_max_profiles_mutex);
            for (auto profile : this->charge_point_max_profiles) {
                // profile.second.chargingSchedule.chargingRateUnit // TODO: collect and convert these
                auto profile_start = start_time;
                auto profile_end = end_time;
                if (profile.second.validFrom) {
                    // check if this profile is even valid
                    if (profile.second.validFrom.value() > end_datetime) {
                        // profile only valid after the requested duration
                        continue;
                    } else {
                        if (profile.second.validFrom.value() < start_datetime) {
                            profile_start = start_time;
                        } else {
                            profile_start = profile.second.validFrom.value().to_time_point();
                        }
                    }
                }
                if (profile.second.validTo) {
                    // check if this profile is even valid
                    if (start_time > profile.second.validTo.value().to_time_point()) {
                        // profile only valid before the requested dura>tion
                        continue;
                    } else {
                        if (profile.second.validTo.value() > end_datetime) {
                            profile_end = end_time;
                        } else {
                            profile_end = profile.second.validTo.value().to_time_point();
                        }
                    }
                }

                // now we know the validity range of this profile:
                std::ostringstream validity_oss;
                validity_oss << "charge point max profile is valid from " << profile_start << " to " << profile_end;
                EVLOG_debug << validity_oss.str();

                valid_profiles.push_back(profile.second);
            }
            charge_point_max_profiles_lock.unlock();

            // sort by start time
            // TODO: is this really needed or just useful for some future optimization?
            std::sort(valid_profiles.begin(), valid_profiles.end(), [](ChargingProfile a, ChargingProfile b) {
                // FIXME(kai): at the moment we only expect Absolute charging profiles
                // that means a startSchedule is always present
                return a.chargingSchedule.startSchedule.value() < b.chargingSchedule.startSchedule.value();
            });
            // FIXME: a default limit of 0 is very conservative but there should always be a profile installed
            const int32_t default_limit = 0;
            for (int32_t delta = 0; delta < call.msg.duration; delta++) {
                auto delta_duration = std::chrono::seconds(delta);
                // assemble all profiles that are valid at start_time + delta:
                ChargingSchedulePeriod delta_period;
                delta_period.startPeriod = delta;
                delta_period.limit = default_limit; // FIXME?
                int32_t current_stack_level = 0;

                auto sample_time = start_time + delta_duration;
                auto sample_time_datetime = DateTime(sample_time);

                for (auto p : valid_profiles) {
                    if (p.stackLevel >= current_stack_level) {
                        // EVLOG_debug << "valid: " << p;
                        // get limit at this point in time:
                        if (p.validFrom && p.validFrom.value() > sample_time_datetime) {
                            // only valid in the future
                            continue;
                        }
                        if (p.validTo && sample_time_datetime > p.validTo.value()) {
                            // only valid in the past
                            continue;
                        }
                        // profile is valid right now
                        // one last check for a start schedule:
                        if (p.chargingProfileKind == ChargingProfileKindType::Absolute &&
                            !p.chargingSchedule.startSchedule) {
                            EVLOG_error << "ERROR we do not know when the schedule should start, this should not be "
                                            "possible...";
                            continue;
                        }

                        auto start_schedule = p.chargingSchedule.startSchedule.value().to_time_point();
                        if (p.chargingProfileKind == ChargingProfileKindType::Recurring) {
                            // TODO(kai): special handling of recurring charging profiles!
                            if (!p.recurrencyKind) {
                                EVLOG_warning
                                    << "Recurring charging profile without a recurreny kind is not supported";
                                continue;
                            }
                            if (!p.chargingSchedule.startSchedule) {
                                EVLOG_warning
                                    << "Recurring charging profile without a start schedule is not supported";
                                continue;
                            }

                            if (p.recurrencyKind.value() == RecurrencyKindType::Daily) {
                                auto midnight_of_start_schedule = date::floor<date::days>(start_schedule);
                                auto diff = start_schedule - midnight_of_start_schedule;
                                auto daily_start_schedule = midnight_of_start_time + diff;
                                if (daily_start_schedule > start_time) {
                                    start_schedule = daily_start_schedule - date::days(1);
                                } else {
                                    start_schedule = daily_start_schedule;
                                }
                            } else if (p.recurrencyKind.value() == RecurrencyKindType::Weekly) {
                                // TODO: compute weekly profile
                            }
                        }
                        if (p.chargingProfileKind == ChargingProfileKindType::Relative) {
                            // FIXME
                            EVLOG_error
                                << "ERROR relative charging profiles are not supported as ChargePointMaxProile "
                                   "(at least for now)";
                            continue;
                        }

                        // FIXME: fix this for recurring charging profiles!
                        if (p.chargingSchedule.duration &&
                            sample_time > p.chargingSchedule.startSchedule.value().to_time_point() +
                                              std::chrono::seconds(p.chargingSchedule.duration.value())) {
                            // charging schedule duration expired
                            continue;
                        }
                        for (auto period : p.chargingSchedule.chargingSchedulePeriod) {
                            auto period_time = std::chrono::seconds(period.startPeriod) + start_schedule;
                            if (period_time <= sample_time) {
                                delta_period.limit = period.limit;
                                delta_period.numberPhases = period.numberPhases;
                            } else {
                                break; // this should speed things up a little bit...
                            }
                        }
                    }
                }
                if (composite_schedule.chargingSchedulePeriod.size() == 0 ||
                    (composite_schedule.chargingSchedulePeriod.back().limit != delta_period.limit ||
                     composite_schedule.chargingSchedulePeriod.back().numberPhases != delta_period.numberPhases)) {
                    composite_schedule.chargingSchedulePeriod.push_back(delta_period);
                }
            }

            response.status = GetCompositeScheduleStatus::Accepted;
            response.scheduleStart.emplace(start_datetime);
            response.connectorId = 0;
            response.chargingSchedule.emplace(composite_schedule);
        } else {
        }
    }

    CallResult<GetCompositeScheduleResponse> call_result(response, call.uniqueId);
    this->send<GetCompositeScheduleResponse>(call_result);
}

void ChargePoint::handleClearChargingProfileRequest(Call<ClearChargingProfileRequest> call) {
    EVLOG_debug << "Received ClearChargingProfileRequest: " << call.msg << "\nwith messageId: " << call.uniqueId;

    ClearChargingProfileResponse response;
    response.status = ClearChargingProfileStatus::Accepted;

    CallResult<ClearChargingProfileResponse> call_result(response, call.uniqueId);
    this->send<ClearChargingProfileResponse>(call_result);
}

void ChargePoint::handleTriggerMessageRequest(Call<TriggerMessageRequest> call) {
    EVLOG_debug << "Received TriggerMessageRequest: " << call.msg << "\nwith messageId: " << call.uniqueId;

    TriggerMessageResponse response;
    response.status = TriggerMessageStatus::Rejected;
    switch (call.msg.requestedMessage) {
    case MessageTrigger::BootNotification:
        response.status = TriggerMessageStatus::Accepted;
        break;
    case MessageTrigger::DiagnosticsStatusNotification:
        response.status = TriggerMessageStatus::Accepted;
        break;
    case MessageTrigger::FirmwareStatusNotification:
        response.status = TriggerMessageStatus::Accepted;
        break;
    case MessageTrigger::Heartbeat:
        response.status = TriggerMessageStatus::Accepted;
        break;
    case MessageTrigger::MeterValues:
        response.status = TriggerMessageStatus::Accepted;
        break;
    case MessageTrigger::StatusNotification:
        response.status = TriggerMessageStatus::Accepted;
        break;
    }

    auto connector = call.msg.connectorId.value_or(0);
    bool valid = true;
    if (connector < 0 || connector > this->configuration->getNumberOfConnectors()) {
        response.status = TriggerMessageStatus::Rejected;
        valid = false;
    }

    CallResult<TriggerMessageResponse> call_result(response, call.uniqueId);
    this->send<TriggerMessageResponse>(call_result);

    if (!valid) {
        return;
    }

    switch (call.msg.requestedMessage) {
    case MessageTrigger::BootNotification:
        this->boot_notification();
        break;
    case MessageTrigger::DiagnosticsStatusNotification:
        this->send_diagnostic_status_notification(this->diagnostics_status);
        break;
    case MessageTrigger::FirmwareStatusNotification:
        this->send_firmware_status_notification(this->firmware_status);
        break;
    case MessageTrigger::Heartbeat:
        this->heartbeat();
        break;
    case MessageTrigger::MeterValues:
        this->send_meter_value(
            connector, this->get_latest_meter_value(connector, this->configuration->getMeterValuesSampledDataVector(),
                                                    ReadingContext::Trigger));
        break;

    case MessageTrigger::StatusNotification:
        this->status_notification(connector, ChargePointErrorCode::NoError, this->status->get_state(connector));
        break;
    }
}

void ChargePoint::handleGetDiagnosticsRequest(Call<GetDiagnosticsRequest> call) {
    EVLOG_debug << "Received GetDiagnosticsRequest: " << call.msg << "\nwith messageId: " << call.uniqueId;
    GetDiagnosticsResponse response;
    if (this->upload_diagnostics_callback) {
        response.fileName.emplace(this->upload_diagnostics_callback(call.msg.location));
    }
    CallResult<GetDiagnosticsResponse> call_result(response, call.uniqueId);
    this->send<GetDiagnosticsResponse>(call_result);
}

void ChargePoint::handleUpdateFirmwareRequest(Call<UpdateFirmwareRequest> call) {
    EVLOG_debug << "Received UpdateFirmwareRequest: " << call.msg << "\nwith messageId: " << call.uniqueId;
    UpdateFirmwareResponse response;
    if (this->upload_diagnostics_callback) {
        // FIXME(kai): respect call.msg.retrieveDate and only then trigger this callback
        this->update_firmware_callback(call.msg.location);
    }
    CallResult<UpdateFirmwareResponse> call_result(response, call.uniqueId);
    this->send<UpdateFirmwareResponse>(call_result);
}

void ChargePoint::handleReserveNowRequest(Call<ReserveNowRequest> call) {
    ReserveNowResponse response;
    response.status = ReservationStatus::Rejected;
    if (this->reserve_now_callback != nullptr) {
        response.status = this->reserve_now_callback(call.msg.reservationId, call.msg.connectorId, call.msg.expiryDate,
                                                     call.msg.idTag, call.msg.parentIdTag);
    }
    CallResult<ReserveNowResponse> call_result(response, call.uniqueId);
    this->send<ReserveNowResponse>(call_result);
}

void ChargePoint::handleCancelReservationRequest(Call<CancelReservationRequest> call) {
    CancelReservationResponse response;
    response.status = CancelReservationStatus::Rejected;
    if (this->cancel_reservation_callback != nullptr) {
        response.status = this->cancel_reservation_callback(call.msg.reservationId);
    }
    CallResult<CancelReservationResponse> call_result(response, call.uniqueId);
    this->send<CancelReservationResponse>(call_result);
}

bool ChargePoint::allowed_to_send_message(json::array_t message) {
    auto message_type = conversions::string_to_messagetype(message.at(CALL_ACTION));

    if (!this->initialized) {
        if (message_type == MessageType::BootNotification) {
            return true;
        }
        return false;
    }

    if (this->registration_status == RegistrationStatus::Rejected) {
        std::chrono::time_point<date::utc_clock> retry_time =
            this->boot_time + std::chrono::seconds(this->configuration->getHeartbeatInterval());
        if (date::utc_clock::now() < retry_time) {
            using date::operator<<;
            std::ostringstream oss;
            oss << "status is rejected and retry time not reached. Messages can be sent again at: " << retry_time;
            EVLOG_debug << oss.str();
            return false;
        }
    } else if (this->registration_status == RegistrationStatus::Pending) {
        if (message_type == MessageType::BootNotification) {
            return true;
        }
        return false;
    }
    return true;
}

template <class T> bool ChargePoint::send(Call<T> call) {
    if (this->allowed_to_send_message(json(call))) {
        this->message_queue->push(call);
        return true;
    }
    return false;
}

template <class T> std::future<EnhancedMessage> ChargePoint::send_async(Call<T> call) {
    return this->message_queue->push_async(call);
}

template <class T> bool ChargePoint::send(CallResult<T> call_result) {
    return this->websocket->send(json(call_result).dump());
}

bool ChargePoint::send(CallError call_error) {
    return this->websocket->send(json(call_error).dump());
}

void ChargePoint::status_notification(int32_t connector, ChargePointErrorCode errorCode, CiString50Type info,
                                      ChargePointStatus status, DateTime timestamp) {
    StatusNotificationRequest request;
    request.connectorId = connector;
    request.errorCode = errorCode;
    request.info.emplace(info);
    request.status = status;
    request.timestamp.emplace(timestamp);
    Call<StatusNotificationRequest> call(request, this->message_queue->createMessageId());
    this->send<StatusNotificationRequest>(call);
}

void ChargePoint::status_notification(int32_t connector, ChargePointErrorCode errorCode, ChargePointStatus status) {
    StatusNotificationRequest request;
    request.connectorId = connector;
    request.errorCode = errorCode;
    request.status = status;
    Call<StatusNotificationRequest> call(request, this->message_queue->createMessageId());
    this->send<StatusNotificationRequest>(call);
}

// public API for Core profile

AuthorizationStatus ChargePoint::authorize_id_tag(CiString20Type idTag) {
    auto auth_status = AuthorizationStatus::Invalid;

    // TODO(kai): implement local authorization (this is optional)

    AuthorizeRequest req;
    req.idTag = idTag;

    Call<AuthorizeRequest> call(req, this->message_queue->createMessageId());

    auto authorize_future = this->send_async<AuthorizeRequest>(call);

    auto enhanced_message = authorize_future.get();
    if (enhanced_message.messageType == MessageType::AuthorizeResponse) {
        CallResult<AuthorizeResponse> call_result = enhanced_message.message;
        AuthorizeResponse authorize_response = call_result.msg;

        auth_status = authorize_response.idTagInfo.status;

        this->configuration->updateAuthorizationCacheEntry(idTag, call_result.msg.idTagInfo);
        auto connector = this->charging_sessions->add_authorized_token(idTag, authorize_response.idTagInfo);
        this->status->submit_event(connector, Event_UsageInitiated());
        if (connector > 0) {
            this->start_transaction(connector);
        }
    }
    if (enhanced_message.offline) {
        // The charge point is offline or has a bad connection.
        // Check if local authorization via the authorization cache is allowed:
        if (this->configuration->getAuthorizationCacheEnabled() && this->configuration->getLocalAuthorizeOffline()) {
            EVLOG_info << "Charge point appears to be offline, using authorization cache to check if IdTag is valid";
            auto idTagInfo = this->configuration->getAuthorizationCacheEntry(idTag);
            if (idTagInfo) {
                auto connector = this->charging_sessions->add_authorized_token(idTag, idTagInfo.get());
                if (connector > 0) {
                    this->start_transaction(connector);
                }
                auth_status = idTagInfo.get().status;
            }
        }
    }

    return auth_status;
}

DataTransferResponse ChargePoint::data_transfer(const CiString255Type& vendorId, const CiString50Type& messageId,
                                                const std::string& data) {
    DataTransferRequest req;
    req.vendorId = vendorId;
    req.messageId = messageId;
    req.data.emplace(data);

    DataTransferResponse response;
    Call<DataTransferRequest> call(req, this->message_queue->createMessageId());

    auto data_transfer_future = this->send_async<DataTransferRequest>(call);

    auto enhanced_message = data_transfer_future.get();
    if (enhanced_message.messageType == MessageType::DataTransferResponse) {
        CallResult<DataTransferResponse> call_result = enhanced_message.message;
        response = call_result.msg;
    }
    if (enhanced_message.offline) {
        // The charge point is offline or has a bad connection.
        response.status = DataTransferStatus::Rejected; // Rejected is not completely correct, but the best we have
                                                        // to indicate an error
    }

    return response;
}

void ChargePoint::register_data_transfer_callback(const CiString255Type& vendorId, const CiString50Type& messageId,
                                                  const std::function<void(const std::string data)>& callback) {
    std::lock_guard<std::mutex> lock(data_transfer_callbacks_mutex);
    this->data_transfer_callbacks[vendorId.get()][messageId.get()] = callback;
}

void ChargePoint::receive_power_meter(int32_t connector, json powermeter) {
    EVLOG_debug << "updating power meter for connector: " << connector;
    std::lock_guard<std::mutex> lock(power_meter_mutex);
    this->power_meter[connector] = powermeter;
}

void ChargePoint::receive_max_current_offered(int32_t connector, double max_current) {
    std::lock_guard<std::mutex> lock(power_meter_mutex);
    // TODO(kai): uses power meter mutex because the reading context is similar, think about storing this
    // information in a unified struct
    this->max_current_offered[connector] = max_current;
}

void ChargePoint::receive_number_of_phases_available(int32_t connector, double number_of_phases) {
    std::lock_guard<std::mutex> lock(power_meter_mutex);
    // TODO(kai): uses power meter mutex because the reading context is similar, think about storing this
    // information in a unified struct
    this->number_of_phases_available[connector] = number_of_phases;
}

bool ChargePoint::start_transaction(int32_t connector) {
    AvailabilityType connector_availability = this->configuration->getConnectorAvailability(connector);
    if (connector_availability == AvailabilityType::Inoperative) {
        EVLOG_error << "Connector " << connector << " is inoperative.";
        return false;
    }

    if (!this->charging_sessions->ready(connector)) {
        EVLOG_error << "Could not start charging session on connector '" << connector << "', session not ready.";
        return false;
    }

    auto idTag_option = this->charging_sessions->get_authorized_id_tag(connector);
    if (idTag_option == boost::none) {
        EVLOG_error << "Could not start charging session on connector '" << connector
                     << "', no authorized token available. This should not happen.";
        return false;
    }
    auto idTag = idTag_option.get();

    auto energyWhStamped = this->charging_sessions->get_start_energy_wh(connector);
    if (energyWhStamped == nullptr) {
        EVLOG_error << "No start energy yet"; // TODO(kai): better comment
        return false;
    }

    StartTransactionRequest req;
    req.connectorId = connector;
    req.idTag = idTag;
    // rounding is necessary here
    req.meterStart = std::round(energyWhStamped->energy_Wh);
    req.timestamp = energyWhStamped->timestamp;

    Call<StartTransactionRequest> call(req, this->message_queue->createMessageId());

    auto start_transaction_future = this->send_async<StartTransactionRequest>(call);

    auto enhanced_message = start_transaction_future.get();
    if (enhanced_message.messageType != MessageType::StartTransactionResponse) {
        return false;
    }

    CallResult<StartTransactionResponse> call_result = enhanced_message.message;
    StartTransactionResponse start_transaction_response = call_result.msg;

    // update authorization cache
    this->configuration->updateAuthorizationCacheEntry(idTag, start_transaction_response.idTagInfo);
    this->charging_sessions->add_authorized_token(connector, idTag, start_transaction_response.idTagInfo);

    if (this->configuration->getStopTransactionOnInvalidId() &&
        start_transaction_response.idTagInfo.status != AuthorizationStatus::Accepted) {
        // FIXME(kai): libfsm        this->status_notification(connector, ChargePointErrorCode::NoError,
        // this->status[connector]->suspended_evse());
        return false;
    }
    if (start_transaction_response.idTagInfo.status == AuthorizationStatus::ConcurrentTx) {
        return false;
    }
    auto meter_values_sample_timer = std::make_unique<Everest::SteadyTimer>(&this->io_service, [this, connector]() {
        auto meter_value = this->get_latest_meter_value(
            connector, this->configuration->getMeterValuesSampledDataVector(), ReadingContext::Sample_Periodic);
        this->charging_sessions->add_sampled_meter_value(connector, meter_value);
        this->send_meter_value(connector, meter_value);
    });
    auto transaction =
        std::make_unique<Transaction>(start_transaction_response.transactionId, std::move(meter_values_sample_timer));
    if (!this->charging_sessions->add_transaction(connector, std::move(transaction))) {
        EVLOG_error << "could not add_transaction";
    }
    this->charging_sessions->change_meter_values_sample_interval(connector,
                                                                 this->configuration->getMeterValueSampleInterval());

    if (this->resume_charging_callback != nullptr) {
        this->resume_charging_callback(connector);
    }

    return true;
}

bool ChargePoint::start_session(int32_t connector, DateTime timestamp, double energy_Wh_import) {
    if (!this->charging_sessions->initiate_session(connector)) {
        EVLOG_error << "Could not initiate charging session on connector '" << connector << "'";
        return false;
    }

    this->status->submit_event(connector, Event_UsageInitiated());

    this->charging_sessions->add_start_energy_wh(connector,
                                                 std::make_shared<StampedEnergyWh>(timestamp, energy_Wh_import));

    return this->start_transaction(connector);
}

bool ChargePoint::stop_transaction(int32_t connector, Reason reason) {
    StopTransactionRequest req;
    // TODO(kai): allow a new id tag to be presented
    // req.idTag-emplace(idTag);
    // rounding is necessary here
    auto energyWhStamped = this->charging_sessions->get_stop_energy_wh(connector);
    if (energyWhStamped == nullptr) {
        EVLOG_error << "No stop energy yet"; // TODO(kai): better comment
        return false;
    }

    // TODO(kai): are there other status notifications that should be sent here?
    if (reason == Reason::EVDisconnected) {
        // FIXME(kai): libfsm
        // this->status_notification(connector, ChargePointErrorCode::NoError, std::string("EV side disconnected"),
        //                           this->status[connector]->finishing(), DateTime());
    }

    req.meterStop = std::round(energyWhStamped->energy_Wh); // TODO(kai):: maybe store this in the session/transaction
                                                            // object to unify this function a bit...
    req.timestamp = energyWhStamped->timestamp;
    req.reason.emplace(reason);
    auto transaction = this->charging_sessions->get_transaction(connector);
    if (transaction == nullptr) {
        EVLOG_error << "Trying to stop a transaction on a charging session with no attached transaction";
        return false;
    }
    req.transactionId = transaction->get_transaction_id();
    req.reason.emplace(Reason::Local);

    std::vector<TransactionData> transaction_data_vec = transaction->get_transaction_data();
    if (!transaction_data_vec.empty()) {
        req.transactionData.emplace(transaction_data_vec);
    }
    Call<StopTransactionRequest> call(req, this->message_queue->createMessageId());

    auto stop_transaction_future = this->send_async<StopTransactionRequest>(call);

    auto enhanced_message = stop_transaction_future.get();
    if (enhanced_message.messageType != MessageType::StopTransactionResponse) {
        return false; // TODO(kai):: check if offline here
    }

    CallResult<StopTransactionResponse> call_result = enhanced_message.message;
    StopTransactionResponse stop_transaction_response = call_result.msg;

    if (stop_transaction_response.idTagInfo) {
        auto idTag = this->charging_sessions->get_authorized_id_tag(connector);
        if (idTag) {
            this->configuration->updateAuthorizationCacheEntry(idTag.value(),
                                                               stop_transaction_response.idTagInfo.value());
        }
    }

    // unlock connector
    if (this->unlock_connector_callback != nullptr) {
        this->unlock_connector_callback(connector);
    }

    // perform a queued connector availability change
    bool change_queued = false;
    AvailabilityType connector_availability;
    {
        std::lock_guard<std::mutex> change_availability_lock(change_availability_mutex);
        change_queued = this->change_availability_queue.count(connector) != 0;
        connector_availability = this->change_availability_queue[connector];
        this->change_availability_queue.erase(connector);
    }

    if (change_queued) {
        bool availability_change_succeeded =
            this->configuration->setConnectorAvailability(connector, connector_availability);
        std::ostringstream oss;
        oss << "Queued availability change of connector " << connector << " to "
            << conversions::availability_type_to_string(connector_availability);
        if (availability_change_succeeded) {
            oss << " successful." << std::endl;
            EVLOG_info << oss.str();

            if (connector_availability == AvailabilityType::Operative) {
                if (this->enable_evse_callback != nullptr) {
                    // TODO(kai): check return value
                    this->enable_evse_callback(connector);
                }
                this->status_notification(connector, ChargePointErrorCode::NoError, ChargePointStatus::Available);
            } else {
                if (this->disable_evse_callback != nullptr) {
                    // TODO(kai): check return value
                    this->disable_evse_callback(connector);
                }
                this->status_notification(connector, ChargePointErrorCode::NoError, ChargePointStatus::Unavailable);
            }
        } else {
            oss << " failed" << std::endl;
            EVLOG_error << oss.str();
        }
    }

    return true;
}

bool ChargePoint::stop_session(int32_t connector, DateTime timestamp, double energy_Wh_import) {
    this->charging_sessions->add_stop_energy_wh(connector,
                                                std::make_shared<StampedEnergyWh>(timestamp, energy_Wh_import));
    if (!this->stop_transaction(connector, Reason::Local)) {
        EVLOG_error << "Something went wrong stopping the transaction a connector " << connector;
    }
    this->charging_sessions->remove_session(connector);

    this->status->submit_event(connector, Event_TransactionStoppedAndUserActionRequired());
    return true; // FIXME
}

bool ChargePoint::start_charging(int32_t connector) {
    this->status->submit_event(connector, Event_StartCharging());
    return true;
}

bool ChargePoint::suspend_charging_ev(int32_t connector) {
    this->status->submit_event(connector, Event_PauseChargingEV());
    return true;
}

bool ChargePoint::suspend_charging_evse(int32_t connector) {
    this->status->submit_event(connector, Event_PauseChargingEVSE());
    return true;
}

bool ChargePoint::resume_charging(int32_t connector) {
    this->status->submit_event(connector, Event_StartCharging());
    return true;
}

bool ChargePoint::plug_disconnected(int32_t connector) {
    this->status->submit_event(connector, Event_BecomeAvailable());
    return true;
}

bool ChargePoint::error(int32_t connector, ChargePointErrorCode error_code) {
    // FIXME(kai): implement
    // FIXME(kai): libfsm
    return false;
}

bool ChargePoint::vendor_error(int32_t connector, CiString50Type vendor_error_code) {
    // FIXME(kai): implement
    return false;
}

bool ChargePoint::permanent_fault(int32_t connector) {
    // FIXME(kai): implement

    return false;
}

void ChargePoint::send_diagnostic_status_notification(DiagnosticsStatus status) {
    DiagnosticsStatusNotificationRequest req;
    req.status = status;
    this->diagnostics_status = status;

    Call<DiagnosticsStatusNotificationRequest> call(req, this->message_queue->createMessageId());
    this->send_async<DiagnosticsStatusNotificationRequest>(call);
}

void ChargePoint::send_firmware_status_notification(FirmwareStatus status) {
    FirmwareStatusNotificationRequest req;
    req.status = status;
    this->firmware_status = status;

    Call<FirmwareStatusNotificationRequest> call(req, this->message_queue->createMessageId());
    this->send_async<FirmwareStatusNotificationRequest>(call);
}

void ChargePoint::register_enable_evse_callback(const std::function<bool(int32_t connector)>& callback) {
    this->enable_evse_callback = callback;
}

void ChargePoint::register_disable_evse_callback(const std::function<bool(int32_t connector)>& callback) {
    this->disable_evse_callback = callback;
}

void ChargePoint::register_pause_charging_callback(const std::function<bool(int32_t connector)>& callback) {
    this->pause_charging_callback = callback;
}

void ChargePoint::register_resume_charging_callback(const std::function<bool(int32_t connector)>& callback) {
    this->resume_charging_callback = callback;
}

void ChargePoint::register_cancel_charging_callback(const std::function<bool(int32_t connector)>& callback) {
    this->cancel_charging_callback = callback;
}

void ChargePoint::register_reserve_now_callback(
    const std::function<ReservationStatus(int32_t reservation_id, int32_t connector, ocpp1_6::DateTime expiryDate,
                                          ocpp1_6::CiString20Type idTag,
                                          boost::optional<ocpp1_6::CiString20Type> parent_id)>& callback) {
    this->reserve_now_callback = callback;
}

void ChargePoint::register_cancel_reservation_callback(
    const std::function<CancelReservationStatus(int32_t connector)>& callback) {
    this->cancel_reservation_callback = callback;
}

void ChargePoint::register_unlock_connector_callback(const std::function<bool(int32_t connector)>& callback) {
    this->unlock_connector_callback = callback;
}

void ChargePoint::register_set_max_current_callback(
    const std::function<bool(int32_t connector, double max_current)>& callback) {
    this->set_max_current_callback = callback;
}
void ChargePoint::register_upload_diagnostics_callback(
    const std::function<std::string(std::string location)>& callback) {
    this->upload_diagnostics_callback = callback;
}

void ChargePoint::register_update_firmware_callback(const std::function<void(std::string location)>& callback) {
    this->update_firmware_callback = callback;
}

} // namespace ocpp1_6
