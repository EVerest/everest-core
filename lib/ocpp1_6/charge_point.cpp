// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest
#include <thread>

#include <everest/logging.hpp>
#include <ocpp1_6/charge_point.hpp>
#include <ocpp1_6/charge_point_configuration.hpp>
#include <ocpp1_6/database_handler.hpp>
#include <ocpp1_6/schemas.hpp>

#include <openssl/rsa.h>

namespace ocpp1_6 {

ChargePoint::ChargePoint(std::shared_ptr<ChargePointConfiguration> configuration, const std::string& database_path,
                         const std::string &sql_init_path) :
    heartbeat_interval(configuration->getHeartbeatInterval()),
    initialized(false),
    registration_status(RegistrationStatus::Pending),
    diagnostics_status(DiagnosticsStatus::Idle),
    firmware_status(FirmwareStatus::Idle),
    log_status(UploadLogStatusEnumType::Idle),
    switch_security_profile_callback(nullptr) {
    this->configuration = configuration;
    this->connection_state = ChargePointConnectionState::Disconnected;
    this->database_handler = std::make_unique<DatabaseHandler>(this->configuration->getChargePointId(),
                                                               boost::filesystem::path(database_path),
                                                               boost::filesystem::path(sql_init_path));
    this->database_handler->open_db_connection(this->configuration->getNumberOfConnectors());
    this->transaction_handler = std::make_unique<TransactionHandler>(this->configuration->getNumberOfConnectors());
    this->message_queue = std::make_unique<MessageQueue>(
        this->configuration, [this](json message) -> bool { return this->websocket->send(message.dump()); });

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

    this->websocket->register_message_callback([this](const std::string& message) { this->message_callback(message); });

    if (security_profile == 3) {
        EVLOG_debug << "Registerung certificate timer";
        this->websocket->register_sign_certificate_callback([this]() { this->sign_certificate(); });
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
            if (this->transaction_handler->transaction_active(connector)) {
                this->transaction_handler->get_transaction(connector)->add_meter_value(meter_value);
                this->send_meter_value(connector, meter_value);
            }
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
    this->transaction_handler->change_meter_values_sample_intervals(interval);
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
                 std::chrono::seconds(clock_aligned_data_interval + date::get_tzdb().leap_seconds.size());
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

void ChargePoint::stop_pending_transactions() {
    const auto transactions = this->database_handler->get_transactions(true);
    for (const auto& transaction_entry : transactions) {
        StopTransactionRequest req;
        req.meterStop = transaction_entry.meter_start; // FIXME(piet): Get latest meter value here
        req.timestamp = DateTime();
        req.transactionId = transaction_entry.transaction_id.value();
        req.reason = Reason::PowerLoss;

        auto message_id = this->message_queue->createMessageId();
        Call<StopTransactionRequest> call(req, message_id);

        {
            std::lock_guard<std::mutex> lock(this->stop_transaction_mutex);
            this->send<StopTransactionRequest>(call);
        }
        this->database_handler->update_transaction(transaction_entry.session_id, req.meterStop, req.timestamp,
                                                   boost::none, req.reason);
    }
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

MeterValue ChargePoint::get_signed_meter_value(const std::string& signed_value, const ReadingContext& context,
                                               const DateTime& timestamp) {
    MeterValue meter_value;
    meter_value.timestamp = timestamp;
    SampledValue sampled_value;
    sampled_value.context = context;
    sampled_value.value = signed_value;
    sampled_value.format = ValueFormat::SignedData;

    meter_value.sampledValue.push_back(sampled_value);
    return meter_value;
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
        auto transaction = this->transaction_handler->get_transaction(connector);
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
    // deny all charging services until BootNotification.conf with status Accepted
    for (int connector = 1; connector < this->configuration->getNumberOfConnectors(); connector++) {
        this->disable_evse_callback(connector);
    }
    this->init_websocket(this->configuration->getSecurityProfile());
    this->websocket->connect(this->configuration->getSecurityProfile());
    this->boot_notification();
    this->stop_pending_transactions();
    this->stopped = false;
    return true;
}

bool ChargePoint::restart() {
    if (this->stopped) {
        EVLOG_info << "Restarting OCPP Chargepoint";
        this->database_handler->open_db_connection(this->configuration->getNumberOfConnectors());
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
        if (this->transaction_handler->transaction_active(connector)) {
            this->transaction_handler->get_transaction(connector)->add_stop_energy_wh(
                std::make_shared<StampedEnergyWh>(DateTime(), this->get_meter_wh(connector)));
            this->stop_transaction_callback(connector, reason);
            this->transaction_handler->remove_active_transaction(connector);
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

        this->database_handler->close_db_connection();
        this->websocket->disconnect(websocketpp::close::status::going_away);
        this->message_queue->stop();
        this->work.reset();
        this->io_service.stop();
        this->io_service_thread.join();

        this->stopped = true;
        EVLOG_info << "Terminating...";
        return true;
    } else {
        EVLOG_warning << "Attempting to stop Chargepoint while it has been stopped before";
        return false;
    }

    EVLOG_info << "Terminating...";
}

void ChargePoint::connected_callback() {
    this->switch_security_profile_callback = nullptr;
    this->configuration->getPkiHandler()->removeCentralSystemFallbackCa();
    switch (this->connection_state) {
    case ChargePointConnectionState::Disconnected: {
        this->connection_state = ChargePointConnectionState::Connected;
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
        // enable all evse on Accepted
        for (int connector = 1; connector < this->configuration->getNumberOfConnectors(); connector++) {
            this->enable_evse_callback(connector);
        }
        this->connection_state = ChargePointConnectionState::Booted;
        // we are allowed to send messages to the central system
        // activate heartbeat
        this->update_heartbeat_interval();

        // activate clock aligned sampling of meter values
        this->update_clock_aligned_meter_values_interval();

        auto connector_availability = this->database_handler->get_connector_availability();
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
    if (call.msg.connectorId <= this->configuration->getNumberOfConnectors() && call.msg.connectorId >= 0) {
        std::vector<int32_t> connectors;
        bool transaction_running = false;

        if (call.msg.connectorId == 0) {
            int32_t number_of_connectors = this->configuration->getNumberOfConnectors();
            for (int32_t connector = 1; connector <= number_of_connectors; connector++) {
                if (this->transaction_handler->transaction_active(connector)) {
                    transaction_running = true;
                    std::lock_guard<std::mutex> change_availability_lock(change_availability_mutex);
                    this->change_availability_queue[connector] = call.msg.type;
                } else {
                    connectors.push_back(connector);
                }
            }
        } else {
            if (this->transaction_handler->transaction_active(call.msg.connectorId)) {
                transaction_running = true;
            } else {
                connectors.push_back(call.msg.connectorId);
            }
        }

        if (transaction_running) {
            response.status = AvailabilityStatus::Scheduled;
        } else {
            this->database_handler->insert_or_update_connector_availability(connectors, call.msg.type);
            for (auto connector : connectors) {
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
                    this->switch_security_profile_callback = [this, security_profile]() {
                        this->switchSecurityProfile(security_profile);
                    };
                    // disconnected_callback will trigger security_profile_callback when it is set
                    this->websocket->disconnect(websocketpp::close::status::service_restart);
                }
                if (call.msg.key == "ConnectionTimeout") {
                    this->set_connection_timeout_callback(this->configuration->getConnectionTimeOut());
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
    this->switch_security_profile_callback = [this]() {
        EVLOG_warning << "Switching security profile back to fallback because new profile couldnt connect";
        this->switchSecurityProfile(this->configuration->getSecurityProfile());
    };

    this->websocket->connect(new_security_profile);
}

void ChargePoint::handleClearCacheRequest(Call<ClearCacheRequest> call) {
    EVLOG_debug << "Received ClearCacheRequest: " << call.msg << "\nwith messageId: " << call.uniqueId;

    ClearCacheResponse response;

    if (this->configuration->getAuthorizationCacheEnabled()) {
        this->database_handler->clear_authorization_cache();
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
    if (this->database_handler->get_connector_availability(connector) == AvailabilityType::Inoperative) {
        EVLOG_warning << "Received RemoteStartTransactionRequest for inoperative connector";
        response.status = RemoteStartStopStatus::Rejected;
        CallResult<RemoteStartTransactionResponse> call_result(response, call.uniqueId);
        this->send<RemoteStartTransactionResponse>(call_result);
        return;
    }
    if (this->transaction_handler->get_transaction(connector) != nullptr) {
        EVLOG_debug << "Received RemoteStartTransactionRequest for a connector with an active transaction.";
        response.status = RemoteStartStopStatus::Rejected;
        CallResult<RemoteStartTransactionResponse> call_result(response, call.uniqueId);
        this->send<RemoteStartTransactionResponse>(call_result);
        return;
    }

    if (call.msg.chargingProfile) {
        // TODO(kai): A charging profile was provided, forward to the charger
        if (call.msg.chargingProfile.get().chargingProfilePurpose != ChargingProfilePurposeType::TxProfile) {
            response.status = RemoteStartStopStatus::Rejected;
            CallResult<RemoteStartTransactionResponse> call_result(response, call.uniqueId);
            this->send<RemoteStartTransactionResponse>(call_result);
            return;
        }
    }

    {
        std::lock_guard<std::mutex> lock(remote_start_transaction_mutex);

        int32_t connector = call.msg.connectorId.value();
        IdTagInfo id_tag_info;
        id_tag_info.status = AuthorizationStatus::Accepted;
        // check if connector is reserved and tagId matches the reserved tag
        response.status = RemoteStartStopStatus::Accepted;
        CallResult<RemoteStartTransactionResponse> call_result(response, call.uniqueId);
        this->send<RemoteStartTransactionResponse>(call_result);

        if (this->configuration->getAuthorizeRemoteTxRequests()) {
            bool prevalidated = false;
            if (this->configuration->getAuthorizationCacheEnabled()) {
                if (this->validate_against_cache_entries(call.msg.idTag) &&
                    this->status->get_state(connector) != ChargePointStatus::Reserved) {
                    prevalidated = true;
                }
            }
            this->provide_token_callback(call.msg.idTag.get(), connector, prevalidated);
        } else {
            this->provide_token_callback(call.msg.idTag.get(), connector, true); // prevalidated
        }
    };
}

bool ChargePoint::validate_against_cache_entries(CiString20Type id_tag) {
    const auto cache_entry_opt = this->database_handler->get_authorization_cache_entry(id_tag);
    if (cache_entry_opt.has_value()) {
        auto cache_entry = cache_entry_opt.value();
        const auto expiry_date_opt = cache_entry.expiryDate;

        if (cache_entry.status == AuthorizationStatus::Accepted) {
            if (expiry_date_opt.has_value()) {
                const auto expiry_date = expiry_date_opt.value();
                if (expiry_date < DateTime()) {
                    cache_entry.status = AuthorizationStatus::Expired;
                    this->database_handler->insert_or_update_authorization_cache_entry(id_tag, cache_entry);
                    return false;
                } else {
                    return true;
                }
            } else {
                return true;
            }
        } else {
            return false;
        }
    } else {
        return false;
    }
}

void ChargePoint::handleRemoteStopTransactionRequest(Call<RemoteStopTransactionRequest> call) {
    EVLOG_debug << "Received RemoteStopTransactionRequest: " << call.msg << "\nwith messageId: " << call.uniqueId;

    RemoteStopTransactionResponse response;
    response.status = RemoteStartStopStatus::Rejected;

    auto connector = this->transaction_handler->get_connector_from_transaction_id(call.msg.transactionId);
    if (connector > 0) {
        response.status = RemoteStartStopStatus::Accepted;
    }

    CallResult<RemoteStopTransactionResponse> call_result(response, call.uniqueId);
    this->send<RemoteStopTransactionResponse>(call_result);

    if (connector > 0) {
        this->stop_transaction_callback(connector, Reason::Remote);
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

    const auto transaction = this->transaction_handler->get_transaction(call_result.uniqueId);
    int32_t connector = transaction->get_connector();
    transaction->set_transaction_id(start_transaction_response.transactionId);

    this->database_handler->update_transaction(transaction->get_session_id(), start_transaction_response.transactionId,
                                               call_result.msg.idTagInfo.parentIdTag);

    if (transaction->is_finished()) {
        this->message_queue->add_stopped_transaction_id(transaction->get_stop_transaction_message_id(),
                                                        start_transaction_response.transactionId);
    }

    auto idTag = transaction->get_id_tag();
    this->database_handler->insert_or_update_authorization_cache_entry(idTag, start_transaction_response.idTagInfo);

    if (start_transaction_response.idTagInfo.status == AuthorizationStatus::ConcurrentTx) {
        return;
    }

    if (this->resume_charging_callback != nullptr) {
        this->resume_charging_callback(connector);
    }

    if (start_transaction_response.idTagInfo.status != AuthorizationStatus::Accepted) {
        // FIXME(kai): libfsm        this->status_notification(connector, ChargePointErrorCode::NoError,
        // this->status[connector]->suspended_evse());
        this->pause_charging_callback(connector);
        if (this->configuration->getStopTransactionOnInvalidId()) {
            this->stop_transaction_callback(connector, Reason::DeAuthorized);
        }
    }
}

void ChargePoint::handleStopTransactionResponse(CallResult<StopTransactionResponse> call_result) {

    StopTransactionResponse stop_transaction_response = call_result.msg;

    // TODO(piet): Fix this for multiple connectors;
    int32_t connector = 1;

    if (stop_transaction_response.idTagInfo) {
        auto id_tag = this->transaction_handler->get_authorized_id_tag(call_result.uniqueId.get());
        if (id_tag) {
            this->database_handler->insert_or_update_authorization_cache_entry(
                id_tag.value(), stop_transaction_response.idTagInfo.value());
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
        this->database_handler->insert_or_update_connector_availability(connector, connector_availability);
        EVLOG_debug << "Queued availability change of connector " << connector << " to "
                    << conversions::availability_type_to_string(connector_availability);

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
    }
    this->transaction_handler->remove_stopped_transaction(call_result.uniqueId.get());
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
        if (this->transaction_handler->transaction_active(connector)) {
            EVLOG_info << "Received unlock connector request with active session for this connector.";
            this->stop_transaction_callback(connector, Reason::UnlockCommand);
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
    const auto supported_purpose_types = this->configuration->getSupportedChargingProfilePurposeTypes();
    if (call.msg.connectorId > number_of_connectors || call.msg.connectorId < 0 ||
        call.msg.csChargingProfiles.stackLevel < 0 ||
        call.msg.csChargingProfiles.stackLevel > this->configuration->getChargeProfileMaxStackLevel()) {
        response.status = ChargingProfileStatus::Rejected;
    } else if (std::find(supported_purpose_types.begin(), supported_purpose_types.end(),
                         call.msg.csChargingProfiles.chargingProfilePurpose) == supported_purpose_types.end()) {
        EVLOG_debug << "Rejecting SetChargingProfileRequest because purpose type is not supported: " << call.msg.csChargingProfiles.chargingProfilePurpose;
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
                auto transaction = this->transaction_handler->get_transaction(call.msg.connectorId);
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

    if (response.status == ChargingProfileStatus::Accepted) {
        this->signal_set_charging_profiles_callback();
    }
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

std::vector<ChargingProfile> ChargePoint::get_all_valid_profiles(const date::utc_clock::time_point& start_time,
                                                                 const date::utc_clock::time_point& end_time,
                                                                 const int32_t connector_id) {
    // charge point max profiles
    std::unique_lock<std::mutex> charge_point_max_profiles_lock(charge_point_max_profiles_mutex);
    std::vector<ChargingProfile> valid_profiles =
        this->get_valid_profiles(this->charge_point_max_profiles, start_time, end_time);
    charge_point_max_profiles_lock.unlock();

    if (connector_id > 0) {
        // TODO: this should probably be done additionally to the composite schedule == 0
        // see if there's a transaction running and a tx_charging_profile installed
        auto transaction = this->transaction_handler->get_transaction(connector_id);
        if (transaction != nullptr) {
            auto start_energy_stamped = transaction->get_start_energy_wh();
            auto start_timestamp = start_energy_stamped->timestamp;
            auto tx_charging_profiles = transaction->get_charging_profiles();

            if (tx_charging_profiles.size() > 0) {
                auto valid_tx_charging_profiles = this->get_valid_profiles(tx_charging_profiles, start_time, end_time);
                valid_profiles.insert(valid_profiles.end(), valid_tx_charging_profiles.begin(),
                                      valid_tx_charging_profiles.end());
            } else {
                std::map<int32_t, ocpp1_6::ChargingProfile> connector_tx_default_profiles;
                {
                    std::lock_guard<std::mutex> tx_default_profiles_lock(tx_default_profiles_mutex);
                    if (this->tx_default_profiles.count(connector_id) > 0) {
                        connector_tx_default_profiles = tx_default_profiles[connector_id];
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
    return valid_profiles;
}

std::map<int32_t, ChargingSchedule> ChargePoint::get_all_composite_charging_schedules(const int32_t duration_s) {

    std::map<int32_t, ChargingSchedule> charging_schedules;

    for (int connector_id = 0; connector_id < this->configuration->getNumberOfConnectors(); connector_id++) {
        auto start_time = date::utc_clock::now();
        auto duration = std::chrono::seconds(duration_s);
        auto end_time = start_time + duration;

        const auto valid_profiles = this->get_all_valid_profiles(start_time, end_time, connector_id);

        auto composite_schedule = this->create_composite_schedule(valid_profiles, start_time, end_time, duration_s);
        charging_schedules[connector_id] = composite_schedule;
    }

    return charging_schedules;
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

        const auto valid_profiles = this->get_all_valid_profiles(start_time, end_time, call.msg.connectorId);

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
            auto transaction = this->transaction_handler->get_transaction(connector);
            if (transaction != nullptr) {
                transaction->remove_charging_profiles();
            }
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
        this->diagnostic_status_notification(this->diagnostics_status);
        break;
    case MessageTrigger::FirmwareStatusNotification:
        this->firmware_status_notification(this->firmware_status);
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
        const auto get_log_response = this->upload_diagnostics_callback(call.msg);
        if (get_log_response.filename.has_value()) {
            response.fileName = get_log_response.filename.value();
        }
    }
    CallResult<GetDiagnosticsResponse> call_result(response, call.uniqueId);
    this->send<GetDiagnosticsResponse>(call_result);
}

void ChargePoint::handleUpdateFirmwareRequest(Call<UpdateFirmwareRequest> call) {
    EVLOG_debug << "Received UpdateFirmwareRequest: " << call.msg << "\nwith messageId: " << call.uniqueId;
    UpdateFirmwareResponse response;
    if (this->update_firmware_callback) {
        this->update_firmware_callback(call.msg);
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
        this->signed_firmware_update_status_notification(this->signed_firmware_status,
                                                         this->signed_firmware_status_request_id);
        break;
    case MessageTriggerEnumType::Heartbeat:
        this->heartbeat();
        break;
    case MessageTriggerEnumType::LogStatusNotification:
        this->log_status_notification(this->log_status, this->log_status_request_id);
        break;
    case MessageTriggerEnumType::MeterValues:
        this->send_meter_value(
            connector, this->get_latest_meter_value(connector, this->configuration->getMeterValuesSampledDataVector(),
                                                    ReadingContext::Trigger));
        break;
    case MessageTriggerEnumType::SignChargePointCertificate:
        this->sign_certificate();
        break;
    case MessageTriggerEnumType::StatusNotification:
        this->status_notification(connector, ChargePointErrorCode::NoError, this->status->get_state(connector));
        break;
    }
}

void ChargePoint::sign_certificate() {

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

    if (this->upload_logs_callback) {

        const auto get_log_response = this->upload_logs_callback(call.msg);
        response.status = get_log_response.status;
        response.filename = get_log_response.filename;
    }

    CallResult<GetLogResponse> call_result(response, call.uniqueId);
    this->send<GetLogResponse>(call_result);
}

void ChargePoint::handleSignedUpdateFirmware(Call<SignedUpdateFirmwareRequest> call) {
    EVLOG_debug << "Received SignedUpdateFirmwareRequest: " << call.msg << "\nwith messageId: " << call.uniqueId;
    SignedUpdateFirmwareResponse response;

    if (!this->configuration->getPkiHandler()->verifyFirmwareCertificate(call.msg.firmware.signingCertificate.get())) {
        response.status = UpdateFirmwareStatusEnumType::InvalidCertificate;
        CallResult<SignedUpdateFirmwareResponse> call_result(response, call.uniqueId);
        this->send<SignedUpdateFirmwareResponse>(call_result);
    } else {
        response.status = this->signed_update_firmware_callback(call.msg);
        CallResult<SignedUpdateFirmwareResponse> call_result(response, call.uniqueId);
        this->send<SignedUpdateFirmwareResponse>(call_result);
    }

    if (response.status == UpdateFirmwareStatusEnumType::InvalidCertificate) {
        this->securityEventNotification(SecurityEvent::InvalidFirmwareSigningCertificate, "Certificate is invalid.");
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

void ChargePoint::log_status_notification(UploadLogStatusEnumType status, int requestId) {

    EVLOG_debug << "Sending log_status_notification with status: " << status << ", requestId: " << requestId;

    LogStatusNotificationRequest req;
    req.status = status;
    req.requestId = requestId;

    this->log_status = status;
    this->log_status_request_id = requestId;

    Call<LogStatusNotificationRequest> call(req, this->message_queue->createMessageId());
    this->send<LogStatusNotificationRequest>(call);
}

void ChargePoint::signed_firmware_update_status_notification(FirmwareStatusEnumType status, int requestId) {
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
}

void ChargePoint::handleCancelReservationRequest(Call<CancelReservationRequest> call) {
    CancelReservationResponse response;
    response.status = CancelReservationStatus::Rejected;

    if (this->cancel_reservation_callback != nullptr) {
        if (this->cancel_reservation_callback(call.msg.reservationId)) {
            response.status = CancelReservationStatus::Accepted;
        }
    }
    CallResult<CancelReservationResponse> call_result(response, call.uniqueId);
    this->send<CancelReservationResponse>(call_result);
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
            this->database_handler->clear_local_authorization_list();
            this->database_handler->insert_or_update_local_list_version(call.msg.listVersion);
            this->database_handler->insert_or_update_local_authorization_list(local_auth_list);
        } else {
            this->database_handler->insert_or_update_local_list_version(call.msg.listVersion);
            this->database_handler->clear_local_authorization_list();
        }
        response.status = UpdateStatus::Accepted;
    } else if (call.msg.updateType == UpdateType::Differential) {
        if (call.msg.localAuthorizationList) {
            auto local_auth_list = call.msg.localAuthorizationList.get();
            if (this->database_handler->get_local_list_version() < call.msg.listVersion) {
                this->database_handler->insert_or_update_local_list_version(call.msg.listVersion);
                this->database_handler->insert_or_update_local_authorization_list(local_auth_list);

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
        response.listVersion = this->database_handler->get_local_list_version();
    }

    CallResult<GetLocalListVersionResponse> call_result(response, call.uniqueId);
    this->send<GetLocalListVersionResponse>(call_result);
}

bool ChargePoint::allowed_to_send_message(json::array_t message) {
    auto message_type = conversions::string_to_messagetype(message.at(CALL_ACTION));

    if (!this->initialized) {
        // BootNotification and StopTransaction messages can be queued before receiving a BootNotification.conf
        if (message_type == MessageType::BootNotification || message_type == MessageType::StopTransaction) {
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
        // BootNotification and StopTransaction messages can be queued before receiving a BootNotification.conf
        if (message_type == MessageType::BootNotification || message_type == MessageType::StopTransaction) {
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

IdTagInfo ChargePoint::authorize_id_token(CiString20Type idTag) {
    // only do authorize req when authorization locally not enabled or fails
    // proritize auth list over auth cache for same idTags

    if (this->configuration->getLocalAuthListEnabled()) {
        const auto auth_list_entry_opt = this->database_handler->get_local_authorization_list_entry(idTag);
        if (auth_list_entry_opt.has_value()) {
            EVLOG_info << "Found id_tag " << idTag.get() << " in AuthorizationList";
            return auth_list_entry_opt.value();
        }
    }
    if (this->configuration->getAuthorizationCacheEnabled()) {
        if (this->validate_against_cache_entries(idTag)) {
            EVLOG_info << "Found vlaid id_tag " << idTag.get() << " in AuthorizationCache";
            return this->database_handler->get_authorization_cache_entry(idTag).value();
        }
    }

    AuthorizeRequest req;
    req.idTag = idTag;

    Call<AuthorizeRequest> call(req, this->message_queue->createMessageId());

    auto authorize_future = this->send_async<AuthorizeRequest>(call);
    auto enhanced_message = authorize_future.get();

    IdTagInfo id_tag_info;
    if (enhanced_message.messageType == MessageType::AuthorizeResponse) {
        CallResult<AuthorizeResponse> call_result = enhanced_message.message;
        if (call_result.msg.idTagInfo.status == AuthorizationStatus::Accepted) {
            this->database_handler->insert_or_update_authorization_cache_entry(idTag, call_result.msg.idTagInfo);
        }
        return call_result.msg.idTagInfo;
    } else if (enhanced_message.offline) {
        if (this->configuration->getAllowOfflineTxForUnknownId() != boost::none &&
            this->configuration->getAllowOfflineTxForUnknownId().value()) {
            id_tag_info = {AuthorizationStatus::Accepted, boost::none, boost::none};
            return id_tag_info;
        }
    }
    id_tag_info = {AuthorizationStatus::Invalid, boost::none, boost::none};
    return id_tag_info;
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

void ChargePoint::on_meter_values(int32_t connector, json powermeter) {
    EVLOG_debug << "updating power meter for connector: " << connector;
    std::lock_guard<std::mutex> lock(power_meter_mutex);
    this->power_meter[connector] = powermeter;
}

void ChargePoint::on_max_current_offered(int32_t connector, int32_t max_current) {
    std::lock_guard<std::mutex> lock(power_meter_mutex);
    // TODO(kai): uses power meter mutex because the reading context is similar, think about storing
    // this information in a unified struct
    this->max_current_offered[connector] = max_current;
}

void ChargePoint::start_transaction(std::shared_ptr<Transaction> transaction) {

    StartTransactionRequest req;
    req.connectorId = transaction->get_connector();
    req.idTag = transaction->get_id_tag();
    req.meterStart = std::round(transaction->get_start_energy_wh()->energy_Wh);
    req.timestamp = transaction->get_start_energy_wh()->timestamp;
    const auto message_id = this->message_queue->createMessageId();
    Call<StartTransactionRequest> call(req, message_id);

    if (transaction->get_reservation_id()) {
        call.msg.reservationId = transaction->get_reservation_id().value();
    }

    transaction->set_start_transaction_message_id(message_id.get());
    transaction->change_meter_values_sample_interval(this->configuration->getMeterValueSampleInterval());

    this->send<StartTransactionRequest>(call);
}

void ChargePoint::on_session_started(int32_t connector, const std::string& session_id, const std::string& reason) {

    EVLOG_debug << "Session on connector#" << connector << " started with reason " << reason;

    const auto session_started_reason = conversions::string_to_session_started_reason(reason);

    // dont change to preparing when in reserved
    if ((this->status->get_state(connector) == ChargePointStatus::Reserved &&
         session_started_reason == SessionStartedReason::Authorized) ||
        this->status->get_state(connector) != ChargePointStatus::Reserved) {
        this->status->submit_event(connector, Event_UsageInitiated());
    }
}

void ChargePoint::on_session_stopped(const int32_t connector) {
    EVLOG_debug << "Session stopped on connector " << connector;
    // TODO(piet) fix this when evse manager signals clearance of an error
    if (this->status->get_state(connector) == ChargePointStatus::Faulted) {
        this->status->submit_event(connector, Event_I1_ReturnToAvailable());
    } else if (this->status->get_state(connector) != ChargePointStatus::Reserved &&
               this->status->get_state(connector) != ChargePointStatus::Unavailable) {
        this->status->submit_event(connector, Event_BecomeAvailable());
    }
}

void ChargePoint::on_transaction_started(const int32_t& connector, const std::string& session_id,
                                         const std::string& id_token, const int32_t& meter_start,
                                         boost::optional<int32_t> reservation_id, const DateTime& timestamp,
                                         boost::optional<std::string> signed_meter_value) {
    if (this->status->get_state(connector) == ChargePointStatus::Reserved) {
        this->status->submit_event(connector, Event_UsageInitiated());
    }

    auto meter_values_sample_timer = std::make_unique<Everest::SteadyTimer>(&this->io_service, [this, connector]() {
        auto meter_value = this->get_latest_meter_value(
            connector, this->configuration->getMeterValuesSampledDataVector(), ReadingContext::Sample_Periodic);
        this->transaction_handler->add_meter_value(connector, meter_value);
        this->send_meter_value(connector, meter_value);
    });
    std::shared_ptr<Transaction> transaction =
        std::make_shared<Transaction>(connector, session_id, CiString20Type(id_token), meter_start, reservation_id,
                                      timestamp, std::move(meter_values_sample_timer));
    if (signed_meter_value) {
        const auto meter_value =
            this->get_signed_meter_value(signed_meter_value.value(), ReadingContext::Transaction_Begin, timestamp);
        transaction->add_meter_value(meter_value);
    }

    this->database_handler->insert_transaction(session_id, connector, id_token, timestamp.to_rfc3339(), meter_start,
                                               reservation_id);
    this->transaction_handler->add_transaction(transaction);
    this->start_transaction(transaction);
}

void ChargePoint::on_transaction_stopped(const int32_t connector, const std::string& session_id, const Reason& reason,
                                         DateTime timestamp, float energy_wh_import,
                                         boost::optional<CiString20Type> id_tag_end,
                                         boost::optional<std::string> signed_meter_value) {
    if (signed_meter_value) {
        const auto meter_value =
            this->get_signed_meter_value(signed_meter_value.value(), ReadingContext::Transaction_End, timestamp);
        this->transaction_handler->get_transaction(connector)->add_meter_value(meter_value);
    }
    const auto stop_energy_wh = std::make_shared<StampedEnergyWh>(timestamp, energy_wh_import);
    this->transaction_handler->get_transaction(connector)->add_stop_energy_wh(stop_energy_wh);

    this->status->submit_event(connector, Event_TransactionStoppedAndUserActionRequired());
    this->stop_transaction(connector, reason, id_tag_end);
    this->database_handler->update_transaction(session_id, energy_wh_import, timestamp.to_rfc3339(), id_tag_end,
                                               reason);
    this->transaction_handler->remove_active_transaction(connector);
}

void ChargePoint::stop_transaction(int32_t connector, Reason reason, boost::optional<CiString20Type> id_tag_end) {
    EVLOG_debug << "Called stop transaction with reason: " << conversions::reason_to_string(reason);
    StopTransactionRequest req;

    auto transaction = this->transaction_handler->get_transaction(connector);
    auto energyWhStamped = transaction->get_stop_energy_wh();

    if (reason == Reason::EVDisconnected) {
        // unlock connector
        if (this->configuration->getUnlockConnectorOnEVSideDisconnect() && this->unlock_connector_callback != nullptr) {
            this->unlock_connector_callback(connector);
        }
    }

    req.meterStop = std::round(energyWhStamped->energy_Wh);
    req.timestamp = energyWhStamped->timestamp;
    req.reason.emplace(reason);
    req.transactionId = transaction->get_transaction_id();

    if (id_tag_end) {
        req.idTag.emplace(id_tag_end.value());
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
    this->transaction_handler->add_stopped_transaction(transaction->get_connector());
}

void ChargePoint::on_suspend_charging_ev(int32_t connector) {
    this->status->submit_event(connector, Event_PauseChargingEV());
}

void ChargePoint::on_suspend_charging_evse(int32_t connector) {
    this->status->submit_event(connector, Event_PauseChargingEVSE());
}

void ChargePoint::on_resume_charging(int32_t connector) {
    this->status->submit_event(connector, Event_StartCharging());
}

void ChargePoint::on_error(int32_t connector, const ChargePointErrorCode& error) {
    this->status->submit_event(connector, Event_FaultDetected(ChargePointErrorCode(error)));
}

void ChargePoint::on_log_status_notification(int32_t request_id, std::string log_status) {
    // request id of -1 indicates a diagnostics status notification, else log status notification
    if (request_id != -1) {
        this->log_status_notification(conversions::string_to_upload_log_status_enum_type(log_status), request_id);
    } else {
        // In OCPP enum DiagnosticsStatus it is called UploadFailed, in UploadLogStatusEnumType of Security Whitepaper
        // it is called UploadFailure
        if (log_status == "UploadFailure") {
            log_status = "UploadFailed";
        }
        this->diagnostic_status_notification(conversions::string_to_diagnostics_status(log_status));
    }
}

void ChargePoint::on_firmware_update_status_notification(int32_t request_id, std::string firmware_update_status) {
    if (request_id != -1) {
        this->signed_firmware_update_status_notification(
            conversions::string_to_firmware_status_enum_type(firmware_update_status), request_id);
    } else {
        this->firmware_status_notification(conversions::string_to_firmware_status(firmware_update_status));
    }
}

void ChargePoint::diagnostic_status_notification(DiagnosticsStatus status) {
    DiagnosticsStatusNotificationRequest req;
    req.status = status;
    this->diagnostics_status = status;

    Call<DiagnosticsStatusNotificationRequest> call(req, this->message_queue->createMessageId());
    this->send_async<DiagnosticsStatusNotificationRequest>(call);
}

void ChargePoint::firmware_status_notification(FirmwareStatus status) {
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

void ChargePoint::register_provide_token_callback(
    const std::function<void(const std::string& id_token, int32_t connector, bool prevalidated)>& callback) {
    this->provide_token_callback = callback;
}

void ChargePoint::register_stop_transaction_callback(
    const std::function<bool(int32_t connector, ocpp1_6::Reason reason)>& callback) {
    this->stop_transaction_callback = callback;
}

void ChargePoint::register_reserve_now_callback(
    const std::function<ReservationStatus(int32_t reservation_id, int32_t connector, ocpp1_6::DateTime expiryDate,
                                          ocpp1_6::CiString20Type idTag,
                                          boost::optional<ocpp1_6::CiString20Type> parent_id)>& callback) {
    this->reserve_now_callback = callback;
}

void ChargePoint::register_cancel_reservation_callback(const std::function<bool(int32_t connector)>& callback) {
    this->cancel_reservation_callback = callback;
}

void ChargePoint::register_unlock_connector_callback(const std::function<bool(int32_t connector)>& callback) {
    this->unlock_connector_callback = callback;
}

void ChargePoint::register_set_max_current_callback(
    const std::function<bool(int32_t connector, double max_current)>& callback) {
    this->set_max_current_callback = callback;
}

void ChargePoint::register_reset_callback(const std::function<bool(const ResetType& reset_type)>& callback) {
    this->reset_callback = callback;
}

void ChargePoint::register_set_system_time_callback(
    const std::function<void(const std::string& system_time)>& callback) {
    this->set_system_time_callback = callback;
}

void ChargePoint::register_signal_set_charging_profiles_callback(const std::function<void()>& callback) {
    this->signal_set_charging_profiles_callback = callback;
}

void ChargePoint::register_upload_diagnostics_callback(
    const std::function<ocpp1_6::GetLogResponse(const ocpp1_6::GetDiagnosticsRequest& request)>& callback) {
    this->upload_diagnostics_callback = callback;
}

void ChargePoint::register_update_firmware_callback(
    const std::function<void(const ocpp1_6::UpdateFirmwareRequest msg)>& callback) {
    this->update_firmware_callback = callback;
}

void ChargePoint::register_signed_update_firmware_callback(
    const std::function<ocpp1_6::UpdateFirmwareStatusEnumType(const ocpp1_6::SignedUpdateFirmwareRequest msg)>&
        callback) {
    this->signed_update_firmware_callback = callback;
}

void ChargePoint::register_upload_logs_callback(
    const std::function<ocpp1_6::GetLogResponse(GetLogRequest req)>& callback) {
    this->upload_logs_callback = callback;
}

void ChargePoint::register_set_connection_timeout_callback(
    const std::function<void(int32_t connection_timeout)>& callback) {
    this->set_connection_timeout_callback = callback;
}

void ChargePoint::on_reservation_start(int32_t connector) {
    this->status->submit_event(connector, Event_ReserveConnector());
}

void ChargePoint::on_reservation_end(int32_t connector) {
    this->status->submit_event(connector, Event_BecomeAvailable());
}

} // namespace ocpp1_6
