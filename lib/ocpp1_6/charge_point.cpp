// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest
#include <thread>

#include <everest/logging.hpp>
#include <ocpp1_6/charge_point.hpp>
#include <ocpp1_6/charge_point_configuration.hpp>
#include <ocpp1_6/schemas.hpp>

#include <openssl/rsa.h>

namespace ocpp1_6 {

ChargePoint::ChargePoint(std::shared_ptr<ChargePointConfiguration> configuration) :
    heartbeat_interval(configuration->getHeartbeatInterval()),
    initialized(false),
    registration_status(RegistrationStatus::Pending),
    diagnostics_status(DiagnosticsStatus::Idle),
    firmware_status(FirmwareStatus::Idle),
    log_status(UploadLogStatusEnumType::Idle),
    signed_firmware_update_running(false),
    switch_security_profile_callback(nullptr),
    interrupt_log_upload(false) {
    this->configuration = configuration;
    // TODO(piet): this->register_scheduled_callbacks();
    this->connection_state = ChargePointConnectionState::Disconnected;
    this->charging_session_handler =
        std::make_unique<ChargingSessionHandler>(this->configuration->getNumberOfConnectors());
    for (int32_t connector = 0; connector < this->configuration->getNumberOfConnectors() + 1; connector++) {
        this->connection_timeout_timer.push_back(
            std::make_unique<Everest::SteadyTimer>(&this->io_service, [this, connector]() {
                EVLOG_info << "Removing session at connector: " << connector
                           << " because the car was not plugged-in in time.";
                this->charging_session_handler->remove_active_session(connector);
                this->status->submit_event(connector, Event_BecomeAvailable());
            }));
    }
    this->message_queue = std::make_unique<MessageQueue>(
        this->configuration, [this](json message) -> bool { return this->websocket->send(message.dump()); });

    this->boot_notification_timer =
        std::make_unique<Everest::SteadyTimer>(&this->io_service, [this]() { this->boot_notification(); });

    this->heartbeat_timer = std::make_unique<Everest::SteadyTimer>(&this->io_service, [this]() { this->heartbeat(); });

    this->clock_aligned_meter_values_timer = std::make_unique<Everest::SystemTimer>(
        &this->io_service, [this]() { this->clock_aligned_meter_values_sample(); });

    this->signed_firmware_update_download_timer =
        std::make_unique<Everest::SteadyTimer>(&this->io_service, [this]() {});

    this->signed_firmware_update_install_timer = std::make_unique<Everest::SteadyTimer>(&this->io_service, [this]() {});

    this->work = boost::make_shared<boost::asio::io_service::work>(this->io_service);

    this->io_service_thread = std::thread([this]() { this->io_service.run(); });

    this->status = std::make_unique<ChargePointStates>(
        this->configuration->getNumberOfConnectors(),
        [this](int32_t connector, ChargePointErrorCode errorCode, ChargePointStatus status) {
            this->status_notification(connector, errorCode, status);
        });
}

void ChargePoint::register_scheduled_callbacks() {

    // std::vector<ScheduledCallback> callbacks = this->configuration->getScheduledCallbacks();

    // SignedFirmwareUpdateRequest req = json::parse(args[0]);
    // json j = req;
    // j.dump();

    // iterate over callbacks and register them
}

void ChargePoint::init_websocket(int32_t security_profile) {
    this->websocket = std::make_unique<Websocket>(this->configuration, security_profile);
    this->websocket->register_connected_callback([this]() {
        this->message_queue->resume(); //
        this->connected_callback();    //
    });
    this->websocket->register_disconnected_callback([this]() {
        this->message_queue->pause(); //
        if (this->switch_security_profile_callback != nullptr) {
            this->switch_security_profile_callback();
        }
    });

    this->websocket->register_message_callback([this](const std::string& message) {
        this->message_callback(message); //
    });

    if (security_profile == 3) {
        EVLOG_debug << "Registerung certificate timer";
        this->websocket->register_sign_certificate_callback([this]() { this->signCertificate(); });
    }
}

void ChargePoint::connect_websocket() {
    if (!this->websocket->is_connected()) {
        this->init_websocket(this->configuration->getSecurityProfile());
        this->websocket->connect(this->configuration->getSecurityProfile());
    }
}

void ChargePoint::disconnect_websocket() {
    if (this->websocket->is_connected()) {
        this->websocket->disconnect(websocketpp::close::status::going_away);
    }
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
    req.chargeBoxSerialNumber.emplace(this->configuration->getChargeBoxSerialNumber());
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
            this->charging_session_handler->add_meter_value(connector, meter_value);
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
    this->charging_session_handler->change_meter_values_sample_intervals(interval);
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
                        sample.value = conversions::double_to_string((double)power_meter_value["current_A"]["N"]);
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
                    EVLOG_error << "No max current offered for connector " << connector << " yet, skipping meter value";
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

    if (meter_value.sampledValue.size() == 0) {
        return;
    }

    MeterValuesRequest req;
    // connector = 0 designates the main powermeter
    // connector > 0 designates a connector of the charge point
    req.connectorId = connector;
    std::ostringstream oss;
    oss << "Gathering measurands of connector: " << connector;
    if (connector > 0) {
        auto transaction = this->charging_session_handler->get_transaction(connector);
        if (transaction != nullptr && transaction->get_transaction_id() != -1) {
            auto transaction_id = transaction->get_transaction_id();
            req.transactionId.emplace(transaction_id);
        }
    }

    EVLOG_debug << oss.str();

    req.meterValue.push_back(meter_value);

    Call<MeterValuesRequest> call(req, this->message_queue->createMessageId());
    this->send<MeterValuesRequest>(call);
}

bool ChargePoint::start() {
    this->init_websocket(this->configuration->getSecurityProfile());
    this->websocket->connect(this->configuration->getSecurityProfile());
    this->stopped = false;
    return true;
}

bool ChargePoint::restart() {
    if (this->stopped) {
        EVLOG_info << "Restarting OCPP Chargepoint";
        this->configuration->restart();
        // instantiating new message queue on restart
        this->message_queue = std::make_unique<MessageQueue>(
            this->configuration, [this](json message) -> bool { return this->websocket->send(message.dump()); });
        this->work = boost::make_shared<boost::asio::io_service::work>(this->io_service);
        this->io_service_thread = std::thread([this]() { this->io_service.run(); });
        this->initialized = true;
        return this->start();
    } else {
        EVLOG_warning << "Attempting to restart Chargepoint while it has not been stopped before";
        return false;
    }
}

void ChargePoint::stop_all_transactions() {
    this->stop_all_transactions(Reason::Other);
}

void ChargePoint::stop_all_transactions(Reason reason) {
    int32_t number_of_connectors = this->configuration->getNumberOfConnectors();
    for (int32_t connector = 1; connector <= number_of_connectors; connector++) {
        if (this->charging_session_handler->transaction_active(connector)) {
            this->charging_session_handler->add_stop_energy_wh(
                connector, std::make_shared<StampedEnergyWh>(DateTime(), this->get_meter_wh(connector)));
            this->stop_transaction(connector, reason);
            this->charging_session_handler->remove_active_session(connector);
        }
    }
}

bool ChargePoint::stop() {
    if (!this->stopped) {
        EVLOG_info << "Stopping OCPP Chargepoint";
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

        this->websocket->disconnect(websocketpp::close::status::going_away);
        this->message_queue->stop();
        this->work.reset();
        this->io_service.stop();
        this->io_service_thread.join();

        this->configuration->close();
        this->stopped = true;
        EVLOG_info << "Terminating...";
        return true;
    } else {
        EVLOG_warning << "Attempting to stop Chargepoint while it has been stopped before";
        return false;
    }
}

void ChargePoint::connected_callback() {
    this->switch_security_profile_callback = nullptr;
    this->configuration->getPkiHandler()->removeCentralSystemFallbackCa();
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

    case MessageType::CertificateSigned:
        this->handleCertificateSignedRequest(json_message);
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
        this->handleStartTransactionResponse(json_message);
        break;

    case MessageType::StopTransactionResponse:
        this->handleStopTransactionResponse(json_message);
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

    case MessageType::GetInstalledCertificateIds:
        this->handleGetInstalledCertificateIdsRequest(json_message);
        break;

    case MessageType::DeleteCertificate:
        this->handleDeleteCertificateRequest(json_message);
        break;

    case MessageType::InstallCertificate:
        this->handleInstallCertificateRequest(json_message);
        break;

    case MessageType::GetLog:
        this->handleGetLogRequest(json_message);
        break;

    case MessageType::SignedUpdateFirmware:
        this->handleSignedUpdateFirmware(json_message);
        break;

    case MessageType::ReserveNow:
        this->handleReserveNowRequest(json_message);
        break;

    case MessageType::CancelReservation:
        this->handleCancelReservationRequest(json_message);
        break;

    case MessageType::ExtendedTriggerMessage:
        this->handleExtendedTriggerMessageRequest(json_message);
        break;

    case MessageType::SendLocalList:
        this->handleSendLocalListRequest(json_message);
        break;

    case MessageType::GetLocalListVersion:
        this->handleGetLocalListVersionRequest(json_message);
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

        EVLOG_debug << "BootNotification response is pending.";
        this->boot_notification_timer->timeout(std::chrono::seconds(call_result.msg.interval));
        break;
    default:
        this->connection_state = ChargePointConnectionState::Rejected;
        // In this state we are not allowed to send any messages to the central system, even when
        // requested. The first time we are allowed to send a message (a BootNotification) is
        // after boot_time + heartbeat_interval if the msg.interval is 0, or after boot_timer + msg.interval
        EVLOG_debug << "BootNotification was rejected, trying again in " << this->configuration->getHeartbeatInterval()
                    << "s";

        this->boot_notification_timer->timeout(std::chrono::seconds(call_result.msg.interval));

        break;
    }
}
void ChargePoint::handleChangeAvailabilityRequest(Call<ChangeAvailabilityRequest> call) {
    EVLOG_debug << "Received ChangeAvailabilityRequest: " << call.msg << "\nwith messageId: " << call.uniqueId;

    ChangeAvailabilityResponse response;
    // we can only change the connector availability if there is no active transaction on this
    // connector. is that case this change must be scheduled and we should report an availability status
    // of "Scheduled"

    // check if connector exists
    if (call.msg.connectorId <= this->configuration->getNumberOfConnectors() || call.msg.connectorId < 0) {
        std::vector<int32_t> connectors;
        bool transaction_running = false;

        if (call.msg.connectorId == 0) {
            int32_t number_of_connectors = this->configuration->getNumberOfConnectors();
            for (int32_t connector = 1; connector <= number_of_connectors; connector++) {
                if (this->charging_session_handler->transaction_active(connector)) {
                    transaction_running = true;
                    std::lock_guard<std::mutex> change_availability_lock(change_availability_mutex);
                    this->change_availability_queue[connector] = call.msg.type;
                } else {
                    connectors.push_back(connector);
                }
            }
        } else {
            if (this->charging_session_handler->transaction_active(call.msg.connectorId)) {
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
    } else {
        // Reject if given connector id doesnt exist
        response.status = AvailabilityStatus::Rejected;
    }

    CallResult<ChangeAvailabilityResponse> call_result(response, call.uniqueId);
    this->send<ChangeAvailabilityResponse>(call_result);
}

void ChargePoint::handleChangeConfigurationRequest(Call<ChangeConfigurationRequest> call) {
    EVLOG_debug << "Received ChangeConfigurationRequest: " << call.msg << "\nwith messageId: " << call.uniqueId;

    ChangeConfigurationResponse response;
    // when reconnect or switching security profile the response has to be sent before that
    bool responded = false;

    auto kv = this->configuration->get(call.msg.key);
    if (kv || call.msg.key == "AuthorizationKey") {
        if (call.msg.key != "AuthorizationKey" && kv.value().readonly) {
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
                if (call.msg.key == "AuthorizationKey") {
                    /*SECURITYLOG*/ EVLOG_debug << "AuthorizationKey was changed by central system";
                    if (this->configuration->getSecurityProfile() == 0) {
                        EVLOG_debug << "AuthorizationKey was changed while on security profile 0.";
                    } else if (this->configuration->getSecurityProfile() == 1 ||
                               this->configuration->getSecurityProfile() == 2) {
                        EVLOG_debug
                            << "AuthorizationKey was changed while on security profile 1 or 2. Reconnect Websocket.";
                        CallResult<ChangeConfigurationResponse> call_result(response, call.uniqueId);
                        this->send<ChangeConfigurationResponse>(call_result);
                        responded = true;
                        this->websocket->reconnect(std::error_code(), 1000);
                    } else {
                        EVLOG_debug << "AuthorizationKey was changed while on security profile 3. Nothing to do.";
                    }
                    // what if basic auth is not in use? what if client side certificates are in use?
                    // log change in security log - if we have one yet?!
                }
                if (call.msg.key == "SecurityProfile") {
                    CallResult<ChangeConfigurationResponse> call_result(response, call.uniqueId);
                    this->send<ChangeConfigurationResponse>(call_result);
                    int32_t security_profile = std::stoi(call.msg.value);
                    responded = true;
                    this->registerSwitchSecurityProfileCallback(
                        [this, security_profile]() { this->switchSecurityProfile(security_profile); });
                    // disconnected_callback will trigger security_profile_callback when it is set
                    this->websocket->disconnect(websocketpp::close::status::service_restart);
                }
            }
        }
    } else {
        response.status = ConfigurationStatus::NotSupported;
    }

    if (!responded) {
        CallResult<ChangeConfigurationResponse> call_result(response, call.uniqueId);
        this->send<ChangeConfigurationResponse>(call_result);
    }
}

void ChargePoint::switchSecurityProfile(int32_t new_security_profile) {
    EVLOG_debug << "Switching security profile from " << this->configuration->getSecurityProfile() << " to "
                << new_security_profile;

    this->init_websocket(new_security_profile);
    this->registerSwitchSecurityProfileCallback([this]() {
        EVLOG_warning << "Switching security profile back to fallback because new profile couldnt connect";
        this->switchSecurityProfile(this->configuration->getSecurityProfile());
    });

    this->websocket->connect(new_security_profile);
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
    if (!call.msg.connectorId || call.msg.connectorId.get() == 0) {
        EVLOG_warning << "RemoteStartTransactionRequest without a connector id or connector id = 0 is not supported "
                         "at the moment.";
        response.status = RemoteStartStopStatus::Rejected;
        CallResult<RemoteStartTransactionResponse> call_result(response, call.uniqueId);
        this->send<RemoteStartTransactionResponse>(call_result);
        return;
    }
    int32_t connector = call.msg.connectorId.value();
    if (this->configuration->getConnectorAvailability(connector) == AvailabilityType::Inoperative) {
        EVLOG_warning << "Received RemoteStartTransactionRequest for inoperative connector";
        response.status = RemoteStartStopStatus::Rejected;
        CallResult<RemoteStartTransactionResponse> call_result(response, call.uniqueId);
        this->send<RemoteStartTransactionResponse>(call_result);
        return;
    }
    if (this->charging_session_handler->get_transaction(connector) != nullptr) {
        EVLOG_debug << "Received RemoteStartTransactionRequest for a connector with an active session.";
        response.status = RemoteStartStopStatus::Rejected;
        CallResult<RemoteStartTransactionResponse> call_result(response, call.uniqueId);
        this->send<RemoteStartTransactionResponse>(call_result);
        return;
    }

    // check if connector is reserved and tagId matches the reserved tag
    if (this->reserved_id_tag_map.find(connector) != this->reserved_id_tag_map.end() &&
        call.msg.idTag.get() != this->reserved_id_tag_map[connector]) {
        EVLOG_warning << "Received RemoteStartTransactionRequest for a reserved connector with wrong idTag.";
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
            int32_t connector = 0;
            if (!call.msg.connectorId) {
                connector = 0;
            } else {
                connector = call.msg.connectorId.value();
            }

            if (this->configuration->getAuthorizeConnectorZeroOnConnectorOne() && connector == 0) {
                connector = 1;
            }
            if (this->configuration->getAuthorizeRemoteTxRequests()) {
                // need to authorize first
                authorized = (this->authorize_id_tag(call.msg.idTag) == AuthorizationStatus::Accepted);
            } else {
                // no explicit authorization is requested, implicitly authorizing this idTag internally
                IdTagInfo idTagInfo;
                idTagInfo.status = AuthorizationStatus::Accepted;
                this->charging_session_handler->add_authorized_token(connector, call.msg.idTag, idTagInfo);
                this->connection_timeout_timer.at(connector)->timeout(
                    std::chrono::seconds(this->configuration->getConnectionTimeOut()));
            }
            if (authorized) {
                // FIXME(kai): this probably needs to be signalled to the evse_manager in some way? we at least need the
                // start_energy and start_timestamp from the evse manager to properly start the transaction
                if (this->remote_start_transaction_callback != nullptr) {
                    this->remote_start_transaction_callback(connector, call.msg.idTag);
                    this->start_transaction(connector);
                } else {
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

    auto connector = this->charging_session_handler->get_connector_from_transaction_id(call.msg.transactionId);
    if (connector > 0) {
        response.status = RemoteStartStopStatus::Accepted;
    }

    CallResult<RemoteStopTransactionResponse> call_result(response, call.uniqueId);
    this->send<RemoteStopTransactionResponse>(call_result);

    if (connector > 0) {
        this->cancel_charging_callback(connector, Reason::Remote);
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
            this->stop_all_transactions(Reason::HardReset);
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

void ChargePoint::handleStartTransactionResponse(CallResult<StartTransactionResponse> call_result) {

    StartTransactionResponse start_transaction_response = call_result.msg;
    // TODO(piet): Fix this for multiple connectors;

    auto transaction = this->charging_session_handler->get_transaction(call_result.uniqueId);
    int32_t connector = transaction->get_connector();
    transaction->set_transaction_id(start_transaction_response.transactionId);

    if (transaction->is_finished()) {
        this->message_queue->add_stopped_transaction_id(transaction->get_stop_transaction_message_id(),
                                                        start_transaction_response.transactionId);
    }

    auto idTag = transaction->get_id_tag();
    this->configuration->updateAuthorizationCacheEntry(idTag, start_transaction_response.idTagInfo);
    this->charging_session_handler->add_authorized_token(connector, idTag, start_transaction_response.idTagInfo);

    if (start_transaction_response.idTagInfo.status == AuthorizationStatus::ConcurrentTx) {
        return;
    }

    if (this->resume_charging_callback != nullptr) {
        this->resume_charging_callback(connector);
    }

    if (start_transaction_response.idTagInfo.status != AuthorizationStatus::Accepted) {
        // FIXME(kai): libfsm        this->status_notification(connector, ChargePointErrorCode::NoError,
        // this->status[connector]->suspended_evse());
        this->cancel_charging_callback(connector, Reason::DeAuthorized);
    }
}

void ChargePoint::handleStopTransactionResponse(CallResult<StopTransactionResponse> call_result) {

    StopTransactionResponse stop_transaction_response = call_result.msg;

    // TODO(piet): Fix this for multiple connectors;
    int32_t connector = 1;

    if (stop_transaction_response.idTagInfo) {
        auto idTag = this->charging_session_handler->get_authorized_id_tag(call_result.uniqueId.get());
        if (idTag) {
            this->configuration->updateAuthorizationCacheEntry(idTag.value(),
                                                               stop_transaction_response.idTagInfo.value());
        }
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
                this->status->submit_event(connector, Event_BecomeAvailable());
            } else {
                if (this->disable_evse_callback != nullptr) {
                    // TODO(kai): check return value
                    this->disable_evse_callback(connector);
                }
                this->status->submit_event(connector, Event_ChangeAvailabilityToUnavailable());
            }
        } else {
            oss << " failed" << std::endl;
            EVLOG_error << oss.str();
        }
    }
    this->charging_session_handler->remove_stopped_transaction(call_result.uniqueId.get());
}

void ChargePoint::handleUnlockConnectorRequest(Call<UnlockConnectorRequest> call) {
    EVLOG_debug << "Received UnlockConnectorRequest: " << call.msg << "\nwith messageId: " << call.uniqueId;
    std::lock_guard<std::mutex> lock(this->stop_transaction_mutex);

    UnlockConnectorResponse response;
    auto connector = call.msg.connectorId;
    if (connector == 0 || connector > this->configuration->getNumberOfConnectors()) {
        response.status = UnlockStatus::NotSupported;
    } else {
        // this message is not intended to remotely stop a transaction, but if a transaction is still ongoing it is
        // advised to stop it first
        CiString20Type idTag;
        int32_t transactionId;
        if (this->charging_session_handler->transaction_active(connector)) {
            EVLOG_info << "Received unlock connector request with active session for this connector.";
            this->cancel_charging_callback(connector, Reason::UnlockCommand);
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

    // FIXME(kai): after a new profile has been installed we must notify interested parties (energy manager?)

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
                                EVLOG_debug << "No startSchedule provided for a recurring charging profile, setting to "
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
                for (int32_t connector = 1; connector <= number_of_connectors; connector++) {
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
                auto transaction = this->charging_session_handler->get_transaction(call.msg.connectorId);
                if (transaction != nullptr) {
                    if (call.msg.csChargingProfiles.transactionId) {
                        auto transaction_id = call.msg.csChargingProfiles.transactionId.get();
                        if (transaction_id != transaction->get_transaction_id()) {
                            response.status = ChargingProfileStatus::Rejected;
                        } else {
                            transaction->set_charging_profile(call.msg.csChargingProfiles);
                            response.status = ChargingProfileStatus::Accepted;
                        }
                    } else {
                        transaction->set_charging_profile(call.msg.csChargingProfiles);
                        response.status = ChargingProfileStatus::Accepted;
                    }
                } else {
                    response.status = ChargingProfileStatus::Rejected;
                }
            }
        }
    }

    CallResult<SetChargingProfileResponse> call_result(response, call.uniqueId);
    this->send<SetChargingProfileResponse>(call_result);
}

std::vector<ChargingProfile> ChargePoint::get_valid_profiles(std::map<int32_t, ocpp1_6::ChargingProfile> profiles,
                                                             date::utc_clock::time_point start_time,
                                                             date::utc_clock::time_point end_time) {
    auto start_datetime = DateTime(start_time);
    auto end_datetime = DateTime(end_time);

    std::vector<ChargingProfile> valid_profiles;

    for (auto profile : profiles) {
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
                // profile only valid before the requested duration
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
    return valid_profiles;
}

ChargingSchedule ChargePoint::create_composite_schedule(std::vector<ChargingProfile> charging_profiles,
                                                        date::utc_clock::time_point start_time,
                                                        date::utc_clock::time_point end_time, int32_t duration_s) {
    auto start_datetime = DateTime(start_time);
    auto midnight_of_start_time = date::floor<date::days>(start_time);

    ChargingSchedule composite_schedule; // = get_composite_schedule(start_time, duration,
                                         // call.msg.chargingRateUnit); // TODO
    composite_schedule.duration.emplace(duration_s);
    composite_schedule.startSchedule.emplace(start_datetime);
    // FIXME: conversions!
    // if (call.msg.chargingRateUnit) {
    //     composite_schedule.chargingRateUnit = call.msg.chargingRateUnit.value();
    // } else {
    //     composite_schedule.chargingRateUnit = ChargingRateUnit::A; // TODO default & conversion
    // }
    composite_schedule.chargingRateUnit = ChargingRateUnit::A; // TODO default & conversion

    // FIXME: a default limit of 0 is very conservative but there should always be a profile installed
    const int32_t default_limit = 0;
    for (int32_t delta = 0; delta < duration_s; delta++) {
        auto delta_duration = std::chrono::seconds(delta);
        // assemble all profiles that are valid at start_time + delta:
        ChargingSchedulePeriod delta_period;
        delta_period.startPeriod = delta;
        delta_period.limit = default_limit; // FIXME?
        int32_t current_stack_level = 0;

        auto sample_time = start_time + delta_duration;
        auto sample_time_datetime = DateTime(sample_time);

        ChargingSchedulePeriod max_delta_period;
        max_delta_period.startPeriod = delta;
        max_delta_period.limit = default_limit; // FIXME?
        bool max_charging_profile_exists = false;
        // first pass to establish a baseline with charge point max profiles:
        for (auto p : charging_profiles) {
            if (p.chargingProfilePurpose == ChargingProfilePurposeType::ChargePointMaxProfile &&
                p.stackLevel >= current_stack_level) {
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
                if (p.chargingProfileKind == ChargingProfileKindType::Absolute && !p.chargingSchedule.startSchedule) {
                    EVLOG_error << "ERROR we do not know when the schedule should start, this should not be "
                                   "possible...";
                    continue;
                }

                auto start_schedule = p.chargingSchedule.startSchedule.value().to_time_point();
                if (p.chargingProfileKind == ChargingProfileKindType::Recurring) {
                    // TODO(kai): special handling of recurring charging profiles!
                    if (!p.recurrencyKind) {
                        EVLOG_warning << "Recurring charging profile without a recurreny kind is not supported";
                        continue;
                    }
                    if (!p.chargingSchedule.startSchedule) {
                        EVLOG_warning << "Recurring charging profile without a start schedule is not supported";
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
                    if (p.chargingProfilePurpose == ChargingProfilePurposeType::ChargePointMaxProfile) {
                        EVLOG_error << "ERROR relative charging profiles are not supported as ChargePointMaxProfile "
                                       "(at least for now)";
                        continue;
                    } else {
                        // FIXME: relative charging profiles are possible here
                    }
                }

                // FIXME: fix this for recurring charging profiles!
                if (p.chargingSchedule.duration &&
                    sample_time > p.chargingSchedule.startSchedule.value().to_time_point() +
                                      std::chrono::seconds(p.chargingSchedule.duration.value())) {
                    continue;
                }
                for (auto period : p.chargingSchedule.chargingSchedulePeriod) {
                    auto period_time = std::chrono::seconds(period.startPeriod) + start_schedule;
                    if (period_time <= sample_time) {
                        max_delta_period.limit = period.limit;
                        max_delta_period.numberPhases = period.numberPhases;
                        max_charging_profile_exists = true;
                    } else {
                        break; // this should speed things up a little bit...
                    }
                }
            }
        }
        current_stack_level = 0;
        delta_period.limit = max_delta_period.limit; // FIXME?
        for (auto p : charging_profiles) {
            if (p.stackLevel >= current_stack_level) {
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
                if (p.chargingProfileKind == ChargingProfileKindType::Absolute && !p.chargingSchedule.startSchedule) {
                    EVLOG_error << "ERROR we do not know when the schedule should start, this should not be "
                                   "possible...";
                    continue;
                }

                auto start_schedule = p.chargingSchedule.startSchedule.value().to_time_point();
                if (p.chargingProfileKind == ChargingProfileKindType::Recurring) {
                    // TODO(kai): special handling of recurring charging profiles!
                    if (!p.recurrencyKind) {
                        EVLOG_warning << "Recurring charging profile without a recurreny kind is not supported";
                        continue;
                    }
                    if (!p.chargingSchedule.startSchedule) {
                        EVLOG_warning << "Recurring charging profile without a start schedule is not supported";
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
                    if (p.chargingProfilePurpose == ChargingProfilePurposeType::ChargePointMaxProfile) {
                        EVLOG_error << "ERROR relative charging profiles are not supported as ChargePointMaxProfile "
                                       "(at least for now)";
                        continue;
                    } else {
                        // FIXME: relative charging profiles are possible here
                    }
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
                        if (max_charging_profile_exists) {
                            if (period.limit < max_delta_period.limit && period.limit < delta_period.limit) {
                                delta_period.limit = period.limit;
                                delta_period.numberPhases = period.numberPhases;
                            } else {
                                delta_period.limit = max_delta_period.limit;
                            }
                        } else {
                            delta_period.limit = period.limit;
                            delta_period.numberPhases = period.numberPhases;
                        }
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

    return composite_schedule;
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
        auto duration = std::chrono::seconds(call.msg.duration);
        auto end_time = start_time + duration;
        auto end_datetime = DateTime(end_time);

        using date::operator<<;
        std::ostringstream oss;
        oss << "Calculating composite schedule from " << start_time << " to " << end_time;
        EVLOG_debug << oss.str();
        EVLOG_debug << "Calculate expected consumption for the grid connection";
        response.scheduleStart.emplace(start_datetime);

        // charge point max profiles
        std::unique_lock<std::mutex> charge_point_max_profiles_lock(charge_point_max_profiles_mutex);
        std::vector<ChargingProfile> valid_profiles =
            this->get_valid_profiles(this->charge_point_max_profiles, start_time, end_time);
        charge_point_max_profiles_lock.unlock();

        if (call.msg.connectorId > 0) {
            // TODO: this should probably be done additionally to the composite schedule == 0
            // see if there's a transaction running and a tx_charging_profile installed
            auto transaction = this->charging_session_handler->get_transaction(call.msg.connectorId);
            if (transaction != nullptr) {
                auto start_energy_stamped = this->charging_session_handler->get_start_energy_wh(call.msg.connectorId);
                auto start_timestamp = start_energy_stamped->timestamp;
                auto tx_charging_profiles = transaction->get_charging_profiles();

                if (tx_charging_profiles.size() > 0) {
                    auto valid_tx_charging_profiles =
                        this->get_valid_profiles(tx_charging_profiles, start_time, end_time);
                    valid_profiles.insert(valid_profiles.end(), valid_tx_charging_profiles.begin(),
                                          valid_tx_charging_profiles.end());
                } else {
                    std::map<int32_t, ocpp1_6::ChargingProfile> connector_tx_default_profiles;
                    {
                        std::lock_guard<std::mutex> tx_default_profiles_lock(tx_default_profiles_mutex);
                        if (this->tx_default_profiles.count(call.msg.connectorId) > 0) {
                            connector_tx_default_profiles = tx_default_profiles[call.msg.connectorId];
                        }
                    }
                    auto valid_tx_default_profiles =
                        this->get_valid_profiles(connector_tx_default_profiles, start_time, end_time);
                    valid_profiles.insert(valid_profiles.end(), valid_tx_default_profiles.begin(),
                                          valid_tx_default_profiles.end());
                }
            } else {
                // TODO: is it needed to check for the tx_default_profiles when no transaction is running?
            }
        }

        // sort by start time
        // TODO: is this really needed or just useful for some future optimization?
        std::sort(valid_profiles.begin(), valid_profiles.end(), [](ChargingProfile a, ChargingProfile b) {
            // FIXME(kai): at the moment we only expect Absolute charging profiles
            // that means a startSchedule is always present
            return a.chargingSchedule.startSchedule.value() < b.chargingSchedule.startSchedule.value();
        });

        ChargingSchedule composite_schedule =
            this->create_composite_schedule(valid_profiles, start_time, end_time, call.msg.duration);

        response.status = GetCompositeScheduleStatus::Accepted;
        response.scheduleStart.emplace(start_datetime);
        response.connectorId = 0;
        response.chargingSchedule.emplace(composite_schedule);
    }

    CallResult<GetCompositeScheduleResponse> call_result(response, call.uniqueId);
    this->send<GetCompositeScheduleResponse>(call_result);
}

void ChargePoint::handleClearChargingProfileRequest(Call<ClearChargingProfileRequest> call) {
    EVLOG_debug << "Received ClearChargingProfileRequest: " << call.msg << "\nwith messageId: " << call.uniqueId;

    // FIXME(kai): after a profile has been deleted we must notify interested parties (energy manager?)

    ClearChargingProfileResponse response;
    response.status = ClearChargingProfileStatus::Unknown;

    // clear all charging profiles
    if (!call.msg.id && !call.msg.connectorId && !call.msg.chargingProfilePurpose && !call.msg.stackLevel) {
        {
            std::lock_guard<std::mutex> charge_point_max_profiles_lock(charge_point_max_profiles_mutex);
            this->charge_point_max_profiles.clear();
        }
        {
            std::lock_guard<std::mutex> tx_default_profiles_lock(tx_default_profiles_mutex);
            this->tx_default_profiles.clear();
        }
        // clear tx_profile of every charging session
        auto number_of_connectors = this->configuration->getNumberOfConnectors();
        for (int32_t connector = 1; connector <= number_of_connectors; connector++) {
            auto transaction = this->charging_session_handler->get_transaction(connector);
            if (transaction == nullptr) {
                continue;
            }

            transaction->remove_charging_profiles();
        }

        response.status = ClearChargingProfileStatus::Accepted;
    }

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
    if (this->update_firmware_callback) {
        // FIXME(kai): respect call.msg.retrieveDate and only then trigger this callback
        this->update_firmware_callback(call.msg.location);
    }
    CallResult<UpdateFirmwareResponse> call_result(response, call.uniqueId);
    this->send<UpdateFirmwareResponse>(call_result);
}

void ChargePoint::handleExtendedTriggerMessageRequest(Call<ExtendedTriggerMessageRequest> call) {
    EVLOG_debug << "Received ExtendedTriggerMessageRequest: " << call.msg << "\nwith messageId: " << call.uniqueId;

    ExtendedTriggerMessageResponse response;
    response.status = TriggerMessageStatusEnumType::Rejected;
    switch (call.msg.requestedMessage) {
    case MessageTriggerEnumType::BootNotification:
        response.status = TriggerMessageStatusEnumType::Accepted;
        break;
    case MessageTriggerEnumType::FirmwareStatusNotification:
        response.status = TriggerMessageStatusEnumType::Accepted;
        break;
    case MessageTriggerEnumType::Heartbeat:
        response.status = TriggerMessageStatusEnumType::Accepted;
        break;
    case MessageTriggerEnumType::LogStatusNotification:
        response.status = TriggerMessageStatusEnumType::Accepted;
        break;
    case MessageTriggerEnumType::MeterValues:
        response.status = TriggerMessageStatusEnumType::Accepted;
        break;
    case MessageTriggerEnumType::SignChargePointCertificate:
        if (this->configuration->getCpoName() != boost::none) {
            response.status = TriggerMessageStatusEnumType::Accepted;
        } else {
            EVLOG_warning << "Received ExtendedTriggerMessage with SignChargePointCertificate but no "
                             "CpoName is set.";
        }
        break;
    case MessageTriggerEnumType::StatusNotification:
        response.status = TriggerMessageStatusEnumType::Accepted;
        break;
    }

    auto connector = call.msg.connectorId.value_or(0);
    bool valid = true;
    if (response.status == TriggerMessageStatusEnumType::Rejected || connector < 0 ||
        connector > this->configuration->getNumberOfConnectors()) {
        response.status = TriggerMessageStatusEnumType::Rejected;
        valid = false;
    }

    CallResult<ExtendedTriggerMessageResponse> call_result(response, call.uniqueId);
    this->send<ExtendedTriggerMessageResponse>(call_result);

    if (!valid) {
        return;
    }

    switch (call.msg.requestedMessage) {
    case MessageTriggerEnumType::BootNotification:
        this->boot_notification();
        break;
    case MessageTriggerEnumType::FirmwareStatusNotification:
        this->signedFirmwareUpdateStatusNotification(this->signed_firmware_status,
                                                     this->signed_firmware_status_request_id);
        break;
    case MessageTriggerEnumType::Heartbeat:
        this->heartbeat();
        break;
    case MessageTriggerEnumType::LogStatusNotification:
        this->logStatusNotification(this->log_status, this->log_status_request_id);
        break;
    case MessageTriggerEnumType::MeterValues:
        this->send_meter_value(
            connector, this->get_latest_meter_value(connector, this->configuration->getMeterValuesSampledDataVector(),
                                                    ReadingContext::Trigger));
        break;
    case MessageTriggerEnumType::SignChargePointCertificate:
        this->signCertificate();
        break;
    case MessageTriggerEnumType::StatusNotification:
        this->status_notification(connector, ChargePointErrorCode::NoError, this->status->get_state(connector));
        break;
    }
}

void ChargePoint::signCertificate() {

    SignCertificateRequest req;

    std::string csr = this->configuration->getPkiHandler()->generateCsr(
        "DE", "BW", "Bad Schoenborn", this->configuration->getCpoName().get().c_str(),
        this->configuration->getChargeBoxSerialNumber().c_str());

    req.csr = csr;
    Call<SignCertificateRequest> call(req, this->message_queue->createMessageId());
    this->send<SignCertificateRequest>(call);
}

void ChargePoint::handleCertificateSignedRequest(Call<CertificateSignedRequest> call) {
    EVLOG_debug << "Received CertificateSignedRequest: " << call.msg << "\nwith messageId: " << call.uniqueId;

    CertificateSignedResponse response;
    response.status = CertificateSignedStatusEnumType::Rejected;

    std::string certificateChain = call.msg.certificateChain;
    std::shared_ptr<PkiHandler> pkiHandler = this->configuration->getPkiHandler();

    CertificateVerificationResult certificateVerificationResult =
        pkiHandler->verifyChargepointCertificate(certificateChain, this->configuration->getChargeBoxSerialNumber());

    if (certificateVerificationResult == CertificateVerificationResult::Valid) {
        response.status = CertificateSignedStatusEnumType::Accepted;
        // FIXME(piet): dont just override, store other one for at least one month according to spec
        pkiHandler->writeClientCertificate(certificateChain);
    }

    CallResult<CertificateSignedResponse> call_result(response, call.uniqueId);
    this->send<CertificateSignedResponse>(call_result);

    if (response.status == CertificateSignedStatusEnumType::Rejected) {
        this->securityEventNotification(
            SecurityEvent::InvalidChargePointCertificate,
            conversions::certificate_verification_result_to_string(certificateVerificationResult));
    }

    // reconnect with new certificate if valid and security profile is 3
    if (response.status == CertificateSignedStatusEnumType::Accepted &&
        this->configuration->getSecurityProfile() == 3) {
        if (pkiHandler->validIn(certificateChain) < 0) {
            this->websocket->reconnect(std::error_code(), 1000);
        } else {
            this->websocket->reconnect(std::error_code(), pkiHandler->validIn(certificateChain));
        }
    }
}

void ChargePoint::handleGetInstalledCertificateIdsRequest(Call<GetInstalledCertificateIdsRequest> call) {
    EVLOG_debug << "Received GetInstalledCertificatesRequest: " << call.msg << "\nwith messageId: " << call.uniqueId;
    GetInstalledCertificateIdsResponse response;
    response.status = GetInstalledCertificateStatusEnumType::NotFound;

    auto certificateHashData =
        this->configuration->getPkiHandler()->getRootCertificateHashData(call.msg.certificateType);
    if (certificateHashData != boost::none) {
        response.certificateHashData = certificateHashData;
        response.status = GetInstalledCertificateStatusEnumType::Accepted;
    }

    CallResult<GetInstalledCertificateIdsResponse> call_result(response, call.uniqueId);
    this->send<GetInstalledCertificateIdsResponse>(call_result);
}

void ChargePoint::handleDeleteCertificateRequest(Call<DeleteCertificateRequest> call) {
    DeleteCertificateResponse response;
    response.status = this->configuration->getPkiHandler()->deleteRootCertificate(
        call.msg.certificateHashData, this->configuration->getSecurityProfile());

    CallResult<DeleteCertificateResponse> call_result(response, call.uniqueId);
    this->send<DeleteCertificateResponse>(call_result);
}

void ChargePoint::handleInstallCertificateRequest(Call<InstallCertificateRequest> call) {
    InstallCertificateResponse response;
    response.status = InstallCertificateStatusEnumType::Rejected;

    InstallCertificateResult installCertificateResult = this->configuration->getPkiHandler()->installRootCertificate(
        call.msg, this->configuration->getCertificateStoreMaxLength(),
        this->configuration->getAdditionalRootCertificateCheck());

    if (installCertificateResult == InstallCertificateResult::Ok ||
        installCertificateResult == InstallCertificateResult::Valid) {
        response.status = InstallCertificateStatusEnumType::Accepted;
    } else if (installCertificateResult == InstallCertificateResult::WriteError) {
        response.status = InstallCertificateStatusEnumType::Failed;
    }

    CallResult<InstallCertificateResponse> call_result(response, call.uniqueId);
    this->send<InstallCertificateResponse>(call_result);

    if (response.status == InstallCertificateStatusEnumType::Rejected) {
        this->securityEventNotification(SecurityEvent::InvalidCentralSystemCertificate,
                                        conversions::install_certificate_result_to_string(installCertificateResult));
    }
}

void ChargePoint::handleGetLogRequest(Call<GetLogRequest> call) {
    GetLogResponse response;

    if (this->log_status == UploadLogStatusEnumType::Idle) {
        response.status = LogStatusEnumType::Accepted;
    } else {
        response.status = LogStatusEnumType::AcceptedCanceled;
        this->interrupt_log_upload = true;
    }

    if (this->upload_logs_callback) {
        // wait for current log upload to finish if running
        std::unique_lock<std::mutex> lock(this->log_upload_mutex);
        this->cv.wait(lock, [this]() { return !this->interrupt_log_upload.load(); });
        response.filename.emplace(this->upload_logs_callback(call.msg));
    }

    CallResult<GetLogResponse> call_result(response, call.uniqueId);
    this->send<GetLogResponse>(call_result);
}

void ChargePoint::handleSignedUpdateFirmware(Call<SignedUpdateFirmwareRequest> call) {
    EVLOG_debug << "Received SignedUpdateFirmwareRequest: " << call.msg << "\nwith messageId: " << call.uniqueId;
    SignedUpdateFirmwareResponse response;

    if (this->configuration->getPkiHandler()->verifyFirmwareCertificate(call.msg.firmware.signingCertificate)) {
        response.status = UpdateFirmwareStatusEnumType::Accepted;
    } else {
        EVLOG_warning << "Certificate of firmware update is not valid";
        response.status = UpdateFirmwareStatusEnumType::InvalidCertificate;
    }

    {
        std::lock_guard<std::mutex> lock(this->signed_firmware_update_mutex);

        if (this->signed_firmware_update_running) {
            EVLOG_debug << "Rejecting signed firmware update request because other update is in progress";
            response.status = UpdateFirmwareStatusEnumType::Rejected;
        }

        CallResult<SignedUpdateFirmwareResponse> call_result(response, call.uniqueId);
        this->send<SignedUpdateFirmwareResponse>(call_result);

        if (response.status == UpdateFirmwareStatusEnumType::InvalidCertificate) {
            this->securityEventNotification(SecurityEvent::InvalidFirmwareSigningCertificate,
                                            "Certificate is invalid.");
        }

        if (response.status == UpdateFirmwareStatusEnumType::Accepted) {
            this->signed_firmware_update_running = true;
            if (call.msg.firmware.retrieveDateTime.to_time_point() > date::utc_clock::now()) {
                this->signed_firmware_update_download_timer->at(
                    [this, call]() { this->signed_update_firmware_download_callback(call.msg); },
                    call.msg.firmware.retrieveDateTime.to_time_point());
                EVLOG_debug << "DownloadScheduled for: " << call.msg.firmware.retrieveDateTime.to_rfc3339();
                this->signedFirmwareUpdateStatusNotification(FirmwareStatusEnumType::DownloadScheduled,
                                                             call.msg.requestId);
            } else {
                this->signed_update_firmware_download_callback(call.msg);
            }
        }
    }
}

void ChargePoint::securityEventNotification(const SecurityEvent& type, const std::string& tech_info) {

    SecurityEventNotificationRequest req;
    req.type = conversions::security_event_to_string(type);
    req.techInfo.emplace(tech_info);
    req.timestamp = ocpp1_6::DateTime();

    Call<SecurityEventNotificationRequest> call(req, this->message_queue->createMessageId());
    this->send<SecurityEventNotificationRequest>(call);
}

void ChargePoint::logStatusNotification(UploadLogStatusEnumType status, int requestId) {

    EVLOG_debug << "Sending logStatusNotification with status: " << status << ", requestId: " << requestId;

    LogStatusNotificationRequest req;
    req.status = status;
    req.requestId = requestId;

    this->log_status = status;
    this->log_status_request_id = requestId;

    Call<LogStatusNotificationRequest> call(req, this->message_queue->createMessageId());
    this->send<LogStatusNotificationRequest>(call);
}

void ChargePoint::signedFirmwareUpdateStatusNotification(FirmwareStatusEnumType status, int requestId) {
    EVLOG_debug << "Sending FirmwareUpdateStatusNotification";
    SignedFirmwareStatusNotificationRequest req;
    req.status = status;
    req.requestId = requestId;

    this->signed_firmware_status = status;
    this->signed_firmware_status_request_id = requestId;

    Call<SignedFirmwareStatusNotificationRequest> call(req, this->message_queue->createMessageId());
    this->send<SignedFirmwareStatusNotificationRequest>(call);

    if (status == FirmwareStatusEnumType::InvalidSignature) {
        this->securityEventNotification(SecurityEvent::InvalidFirmwareSignature, "techinfo");
    }
}

void ChargePoint::handleReserveNowRequest(Call<ReserveNowRequest> call) {
    ReserveNowResponse response;
    response.status = ReservationStatus::Rejected;

    if (this->status->get_state(call.msg.connectorId) == ChargePointStatus::Faulted) {
        response.status = ReservationStatus::Faulted;
    } else if (this->reserve_now_callback != nullptr &&
               this->configuration->getSupportedFeatureProfiles().find("Reservation") != std::string::npos) {
        response.status = this->reserve_now_callback(call.msg.reservationId, call.msg.connectorId, call.msg.expiryDate,
                                                     call.msg.idTag, call.msg.parentIdTag);
    }

    CallResult<ReserveNowResponse> call_result(response, call.uniqueId);
    this->send<ReserveNowResponse>(call_result);

    if (response.status == ReservationStatus::Accepted) {
        this->status->submit_event(call.msg.connectorId, Event_ReserveConnector());
    }
}

void ChargePoint::handleCancelReservationRequest(Call<CancelReservationRequest> call) {
    CancelReservationResponse response;
    response.status = CancelReservationStatus::Rejected;

    int32_t connector = this->res_conn_map[call.msg.reservationId];
    if (this->cancel_reservation_callback != nullptr) {
        response.status = this->cancel_reservation_callback(connector);
    }
    CallResult<CancelReservationResponse> call_result(response, call.uniqueId);
    this->send<CancelReservationResponse>(call_result);

    if (response.status == CancelReservationStatus::Accepted) {
        this->status->submit_event(connector, Event_BecomeAvailable());
    }
}

void ChargePoint::handleSendLocalListRequest(Call<SendLocalListRequest> call) {
    EVLOG_debug << "Received SendLocalListRequest: " << call.msg << "\nwith messageId: " << call.uniqueId;

    SendLocalListResponse response;
    response.status = UpdateStatus::Failed;

    if (!this->configuration->getLocalAuthListEnabled()) {
        response.status = UpdateStatus::NotSupported;
    }

    else if (call.msg.updateType == UpdateType::Full) {
        if (call.msg.localAuthorizationList) {
            auto local_auth_list = call.msg.localAuthorizationList.get();
            this->configuration->clearLocalAuthorizationList();
            this->configuration->updateLocalAuthorizationListVersion(call.msg.listVersion);
            this->configuration->updateLocalAuthorizationList(local_auth_list);
        } else {
            this->configuration->updateLocalAuthorizationListVersion(call.msg.listVersion);
            this->configuration->clearLocalAuthorizationList();
        }
        response.status = UpdateStatus::Accepted;
    } else if (call.msg.updateType == UpdateType::Differential) {
        if (call.msg.localAuthorizationList) {
            auto local_auth_list = call.msg.localAuthorizationList.get();
            if (this->configuration->getLocalListVersion() < call.msg.listVersion) {
                this->configuration->updateLocalAuthorizationListVersion(call.msg.listVersion);
                this->configuration->updateLocalAuthorizationList(local_auth_list);

                response.status = UpdateStatus::Accepted;
            } else {
                response.status = UpdateStatus::VersionMismatch;
            }
        }
    }

    CallResult<SendLocalListResponse> call_result(response, call.uniqueId);
    this->send<SendLocalListResponse>(call_result);
}

void ChargePoint::handleGetLocalListVersionRequest(Call<GetLocalListVersionRequest> call) {
    EVLOG_debug << "Received GetLocalListVersionRequest: " << call.msg << "\nwith messageId: " << call.uniqueId;

    GetLocalListVersionResponse response;
    if (!this->configuration->getSupportedFeatureProfilesSet().count(
            SupportedFeatureProfiles::LocalAuthListManagement)) {
        // if Local Authorization List is not supported, report back -1 as list version
        response.listVersion = -1;
    } else {
        response.listVersion = this->configuration->getLocalListVersion();
    }

    CallResult<GetLocalListVersionResponse> call_result(response, call.uniqueId);
    this->send<GetLocalListVersionResponse>(call_result);
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

    // check if a session is running with this idTag associated
    for (int32_t connector = 1; connector < this->configuration->getNumberOfConnectors() + 1; connector++) {
        auto authorized_id_tag = this->charging_session_handler->get_authorized_id_tag(connector);
        if (authorized_id_tag != boost::none) {
            if (authorized_id_tag.get() == idTag) {
                // get transaction
                auto transaction = this->charging_session_handler->get_transaction(connector);
                if (transaction != nullptr) {
                    // stop charging, this will stop the session as well
                    this->cancel_charging_callback(connector, Reason::Local);
                    return AuthorizationStatus::Invalid;
                }
            }
        }
    }

    // check if all connectors have active transactions
    if (this->charging_session_handler->all_connectors_have_active_transaction()) {
        return AuthorizationStatus::Invalid;
    }

    int32_t connector = 1;

    // dont authorize if state is Unavailable
    if (this->status->get_state(connector) == ChargePointStatus::Unavailable) {
        return AuthorizationStatus::Invalid;
    }

    // check if connector is reserved and tagId matches the reserved tag
    if (this->reserved_id_tag_map.find(connector) != this->reserved_id_tag_map.end()) {
        if (idTag.get() == this->reserved_id_tag_map[connector]) {
            auth_status = AuthorizationStatus::Accepted;
        } else {
            return AuthorizationStatus::Invalid;
        }
    }

    AuthorizeRequest req;
    req.idTag = idTag;

    Call<AuthorizeRequest> call(req, this->message_queue->createMessageId());

    auto authorize_future = this->send_async<AuthorizeRequest>(call);

    auto enhanced_message = authorize_future.get();

    if (enhanced_message.messageType == MessageType::AuthorizeResponse) {
        CallResult<AuthorizeResponse> call_result = enhanced_message.message;
        AuthorizeResponse authorize_response = call_result.msg;

        auth_status = authorize_response.idTagInfo.status;

        if (auth_status == AuthorizationStatus::Accepted) {
            this->configuration->updateAuthorizationCacheEntry(idTag, call_result.msg.idTagInfo);
            if (!this->configuration->getAuthorizeConnectorZeroOnConnectorOne()) {
                connector = this->charging_session_handler->add_authorized_token(idTag, authorize_response.idTagInfo);
            } else {
                EVLOG_info << "Automatically authorizing idTag on connector 1 because there is only "
                              "one connector";
                connector = this->charging_session_handler->add_authorized_token(connector, idTag,
                                                                                 authorize_response.idTagInfo);
            }
            this->connection_timeout_timer.at(connector)->timeout(
                std::chrono::seconds(this->configuration->getConnectionTimeOut()));
            this->status->submit_event(connector, Event_UsageInitiated());
        } else {
            return auth_status;
        }
    }
    if (enhanced_message.offline) {
        // if chargepoint is offline and allowing transactions for unknown id
        if (this->configuration->getAllowOfflineTxForUnknownId() != boost::none &&
            this->configuration->getAllowOfflineTxForUnknownId().value()) {
            IdTagInfo id_tag_info = {AuthorizationStatus::Accepted, boost::none, boost::none};
            this->charging_session_handler->add_authorized_token(connector, idTag, id_tag_info);
            auth_status = AuthorizationStatus::Accepted;
        }

        // The charge point is offline or has a bad connection.
        // Check if local authorization via the authorization cache is allowed:
        if (this->configuration->getAuthorizationCacheEnabled() && this->configuration->getLocalAuthorizeOffline()) {
            EVLOG_info << "Charge point appears to be offline, using authorization cache to check if "
                          "IdTag is valid";
            auto idTagInfo = this->configuration->getAuthorizationCacheEntry(idTag);
            if (idTagInfo) {
                this->charging_session_handler->add_authorized_token(connector, idTag, idTagInfo.get());
                auth_status = idTagInfo.get().status;
            }
        }
    }

    if (connector > 0 && auth_status == AuthorizationStatus::Accepted) {
        this->start_transaction(connector);
    }

    return auth_status;
}

boost::optional<CiString20Type> ChargePoint::get_authorized_id_tag(int32_t connector) {
    return this->charging_session_handler->get_authorized_id_tag(connector);
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
        response.status = DataTransferStatus::Rejected; // Rejected is not completely correct, but the
                                                        // best we have to indicate an error
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
    // TODO(kai): uses power meter mutex because the reading context is similar, think about storing
    // this information in a unified struct
    this->max_current_offered[connector] = max_current;
}

void ChargePoint::receive_number_of_phases_available(int32_t connector, double number_of_phases) {
    std::lock_guard<std::mutex> lock(power_meter_mutex);
    // TODO(kai): uses power meter mutex because the reading context is similar, think about storing
    // this information in a unified struct
    this->number_of_phases_available[connector] = number_of_phases;
}

bool ChargePoint::start_transaction(int32_t connector) {
    AvailabilityType connector_availability = this->configuration->getConnectorAvailability(connector);
    if (connector_availability == AvailabilityType::Inoperative) {
        EVLOG_error << "Connector " << connector << " is inoperative.";
        return false;
    }

    if (this->charging_session_handler->get_transaction(connector) != nullptr) {
        EVLOG_debug << "Transaction has already been started";
        return false;
    }

    if (!this->charging_session_handler->ready(connector)) {
        EVLOG_debug << "Could not start charging session on connector '" << connector << "', session not yet ready.";
        return false;
    }

    auto idTag_option = this->charging_session_handler->get_authorized_id_tag(connector);
    auto idTag = idTag_option.get();
    auto energyWhStamped = this->charging_session_handler->get_start_energy_wh(connector);

    // stop connection timeout timer, car is obviously plugged-in now
    this->connection_timeout_timer.at(connector)->stop();

    StartTransactionRequest req;
    req.connectorId = connector;
    req.idTag = idTag;
    // rounding is necessary here
    req.meterStart = std::round(energyWhStamped->energy_Wh);
    req.timestamp = energyWhStamped->timestamp;

    auto message_id = this->message_queue->createMessageId();
    Call<StartTransactionRequest> call(req, message_id);

    if (this->charging_session_handler->get_reservation_id(connector) != boost::none) {
        call.msg.reservationId = this->charging_session_handler->get_reservation_id(connector).value();
    }

    auto meter_values_sample_timer = std::make_unique<Everest::SteadyTimer>(&this->io_service, [this, connector]() {
        auto meter_value = this->get_latest_meter_value(
            connector, this->configuration->getMeterValuesSampledDataVector(), ReadingContext::Sample_Periodic);
        this->charging_session_handler->add_meter_value(connector, meter_value);
        this->send_meter_value(connector, meter_value);
    });
    auto transaction =
        std::make_unique<Transaction>(-1, connector, std::move(meter_values_sample_timer), idTag, message_id.get());
    if (!this->charging_session_handler->add_transaction(connector, std::move(transaction))) {
        EVLOG_error << "could not add_transaction";
    }

    this->charging_session_handler->change_meter_values_sample_interval(
        connector, this->configuration->getMeterValueSampleInterval());

    this->send<StartTransactionRequest>(call);
    return true;
}

bool ChargePoint::start_session(int32_t connector, DateTime timestamp, double energy_Wh_import,
                                boost::optional<int32_t> reservation_id) {

    // dont start session if connector is unavailable
    if (this->status->get_state(connector) == ChargePointStatus::Unavailable) {
        EVLOG_warning << "Connector is inoperative and can't start a session";
        return false;
    }

    if (!this->charging_session_handler->initiate_session(connector)) {
        EVLOG_error << "Could not initiate charging session on connector '" << connector << "'";
        return false;
    }

    // dont change to preparing when in reserved
    if (this->status->get_state(connector) != ChargePointStatus::Reserved) {
        this->status->submit_event(connector, Event_UsageInitiated());
    }

    this->charging_session_handler->add_start_energy_wh(connector,
                                                        std::make_shared<StampedEnergyWh>(timestamp, energy_Wh_import));
    if (reservation_id != boost::none) {
        this->charging_session_handler->add_reservation_id(connector, reservation_id.value());
    }

    return this->start_transaction(connector);
}

bool ChargePoint::stop_transaction(int32_t connector, Reason reason) {
    EVLOG_debug << "Called stop transaction with reason: " << conversions::reason_to_string(reason);
    StopTransactionRequest req;
    // TODO(kai): allow a new id tag to be presented
    // req.idTag-emplace(idTag);
    // rounding is necessary here
    auto transaction = this->charging_session_handler->get_transaction(connector);
    auto energyWhStamped = this->charging_session_handler->get_stop_energy_wh(connector);

    // TODO(kai): are there other status notifications that should be sent here?

    if (reason == Reason::EVDisconnected) {
        // unlock connector
        if (this->configuration->getUnlockConnectorOnEVSideDisconnect() && this->unlock_connector_callback != nullptr) {
            this->unlock_connector_callback(connector);
        }
        // FIXME(kai): libfsm
        // this->status_notification(connector, ChargePointErrorCode::NoError, std::string("EV side
        // disconnected"),
        //                           this->status[connector]->finishing(), DateTime());
    }

    req.meterStop = std::round(energyWhStamped->energy_Wh); // TODO(kai):: maybe store this in the session/transaction
                                                            // object to unify this function a bit...
    req.timestamp = energyWhStamped->timestamp;
    req.reason.emplace(reason);
    req.transactionId = transaction->get_transaction_id();

    if (this->charging_session_handler->get_authorized_id_tag(connector) != boost::none) {
        req.idTag = this->charging_session_handler->get_authorized_id_tag(connector).value();
    }

    std::vector<TransactionData> transaction_data_vec = transaction->get_transaction_data();
    if (!transaction_data_vec.empty()) {
        req.transactionData.emplace(transaction_data_vec);
    }

    auto message_id = this->message_queue->createMessageId();
    Call<StopTransactionRequest> call(req, message_id);

    {
        std::lock_guard<std::mutex> lock(this->stop_transaction_mutex);
        this->send<StopTransactionRequest>(call);
    }

    transaction->set_finished();
    transaction->set_stop_transaction_message_id(message_id.get());
    this->charging_session_handler->add_stopped_transaction(transaction);
    return true;
}

bool ChargePoint::cancel_session(int32_t connector, DateTime timestamp, double energy_Wh_import, Reason reason) {
    this->charging_session_handler->add_stop_energy_wh(connector,
                                                       std::make_shared<StampedEnergyWh>(timestamp, energy_Wh_import));

    EVLOG_debug << "Cancelling Session on connector " << connector;

    if (this->charging_session_handler->get_transaction(connector) == nullptr) {
        EVLOG_info << "Cancelled a session without a transaction";
    }

    if (reason != Reason::DeAuthorized || this->configuration->getStopTransactionOnInvalidId()) {
        this->status->submit_event(connector, Event_TransactionStoppedAndUserActionRequired());
        bool success = this->stop_transaction(connector, reason);
        if (!success) {
            EVLOG_error << "Something went wrong stopping the transaction a connector " << connector;
            return false;
        }
    }
    return true; // FIXME
}

bool ChargePoint::stop_session(int32_t connector, DateTime timestamp, double energy_Wh_import) {
    EVLOG_debug << "Stopping session on connector " << connector;
    this->charging_session_handler->add_stop_energy_wh(connector,
                                                       std::make_shared<StampedEnergyWh>(timestamp, energy_Wh_import));

    auto transaction = this->charging_session_handler->get_transaction(connector);
    if (transaction == nullptr || transaction->is_finished()) {
        EVLOG_debug << "Transaction has already finished, not sending stop_transaction";
    } else {
        if (this->stop_transaction(connector, Reason::EVDisconnected)) {
        } else {
            EVLOG_error << "Something went wrong stopping the transaction at connector " << connector;
        }
    }

    this->charging_session_handler->remove_active_session(connector);
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
    // TODO(piet) fix this when evse manager signals clearance of an error
    if (this->status->get_state(connector) == ChargePointStatus::Faulted) {
        this->status->submit_event(connector, Event_I1_ReturnToAvailable());
    } else if (this->status->get_state(connector) != ChargePointStatus::Reserved &&
               this->status->get_state(connector) != ChargePointStatus::Unavailable) {
        this->status->submit_event(connector, Event_BecomeAvailable());
    }
    return true;
}

bool ChargePoint::error(int32_t connector, const ChargePointErrorCode& error) {

    this->status->submit_event(connector, Event_FaultDetected(ChargePointErrorCode(error)));
    return true;
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

void ChargePoint::register_cancel_charging_callback(
    const std::function<bool(int32_t connector, ocpp1_6::Reason reason)>& callback) {
    this->cancel_charging_callback = callback;
}

void ChargePoint::register_remote_start_transaction_callback(
    const std::function<void(int32_t connector, ocpp1_6::CiString20Type idTag)>& callback) {
    this->remote_start_transaction_callback = callback;
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

void ChargePoint::registerSwitchSecurityProfileCallback(const std::function<void()>& callback) {
    this->switch_security_profile_callback = callback;
}

void ChargePoint::register_upload_logs_callback(const std::function<std::string(GetLogRequest req)>& callback) {
    this->upload_logs_callback = callback;
}

void ChargePoint::register_signed_update_firmware_download_callback(
    const std::function<void(SignedUpdateFirmwareRequest req)>& callback) {
    this->signed_update_firmware_download_callback = callback;
}

void ChargePoint::register_signed_update_firmware_install_callback(
    const std::function<void(SignedUpdateFirmwareRequest req, boost::filesystem::path file_path)>& callback) {
    this->signed_update_firmware_install_callback = callback;
}

void ChargePoint::notify_signed_firmware_update_downloaded(SignedUpdateFirmwareRequest req,
                                                           boost::filesystem::path file_path) {
    if (req.firmware.installDateTime != boost::none &&
        req.firmware.installDateTime.value().to_time_point() > date::utc_clock::now()) {
        // TODO(piet): put cb into database
        this->signed_firmware_update_install_timer->at(
            [this, req, file_path]() { this->signed_update_firmware_install_callback(req, file_path); },
            req.firmware.installDateTime.value().to_time_point());
        EVLOG_debug << "InstallScheduled for: " << req.firmware.installDateTime.value().to_rfc3339();
        this->signedFirmwareUpdateStatusNotification(ocpp1_6::FirmwareStatusEnumType::InstallScheduled, req.requestId);
    } else {
        this->signed_update_firmware_install_callback(req, file_path);
    }
}

void ChargePoint::trigger_boot_notification() {
    this->boot_notification();
}

bool ChargePoint::is_signed_firmware_update_running() {
    return this->signed_firmware_update_running;
}

void ChargePoint::set_signed_firmware_update_running(bool b) {
    this->signed_firmware_update_running = b;
}

void ChargePoint::reservation_start(int32_t connector, int32_t reservation_id, std::string id_tag) {
    this->reserved_id_tag_map[connector] = id_tag;

    if (res_conn_map.count(reservation_id) == 0) {
        this->res_conn_map[reservation_id] = connector;
    } else if (res_conn_map.count(reservation_id) == 1) {
        std::map<int32_t, int32_t>::iterator it;
        it = this->res_conn_map.find(reservation_id);
        this->res_conn_map.erase(it);
        this->res_conn_map[reservation_id] = connector;
    }
}

void ChargePoint::reservation_end(int32_t connector, int32_t reservation_id, std::string reason) {
    if (reason == "Expired") {
        this->status->submit_event(connector, Event_BecomeAvailable());
    }
    // delete reservation in res con map
    std::map<int32_t, int32_t>::iterator res_con_map_it;
    res_con_map_it = this->res_conn_map.find(reservation_id);
    this->res_conn_map.erase(res_con_map_it);

    this->reserved_id_tag_map.erase(connector);
}

} // namespace ocpp1_6
