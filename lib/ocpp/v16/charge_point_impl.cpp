// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#include "ocpp/common/types.hpp"

#include <stdexcept>
#include <thread>

#include <everest/logging.hpp>
#include <ocpp/common/constants.hpp>
#include <ocpp/common/websocket/websocket.hpp>
#include <ocpp/v16/charge_point.hpp>
#include <ocpp/v16/charge_point_configuration.hpp>
#include <ocpp/v16/charge_point_impl.hpp>
#include <ocpp/v16/utils.hpp>
#include <ocpp/v2/messages/CostUpdated.hpp>
#include <ocpp/v2/messages/SetDisplayMessage.hpp>
#include <ocpp/v2/utils.hpp>

#include <optional>

using QueryExecutionException = everest::db::QueryExecutionException;
using RequiredEntryNotFoundException = everest::db::RequiredEntryNotFoundException;

namespace ocpp {
namespace v16 {

const auto ISO15118_PNC_VENDOR_ID = "org.openchargealliance.iso15118pnc";
const auto CALIFORNIA_PRICING_VENDOR_ID = "org.openchargealliance.costmsg";
const auto CLIENT_CERTIFICATE_TIMER_INTERVAL = std::chrono::hours(12);
const auto V2G_CERTIFICATE_TIMER_INTERVAL = std::chrono::hours(12);
const auto OCSP_REQUEST_TIMER_INTERVAL = std::chrono::hours(12);
const auto INITIAL_CERTIFICATE_REQUESTS_DELAY = std::chrono::seconds(60);
const auto WEBSOCKET_INIT_DELAY = std::chrono::seconds(2);
const auto DEFAULT_MESSAGE_QUEUE_SIZE_THRESHOLD = 2E5;
const auto DEFAULT_BOOT_NOTIFICATION_INTERVAL_S = 60; // fallback interval if BootNotification returns interval of 0.
const auto DEFAULT_PRICE_NUMBER_OF_DECIMALS = 3;
const auto DEFAULT_WAIT_FOR_SET_USER_PRICE_TIMEOUT_MS = 0;

ChargePointImpl::ChargePointImpl(const std::string& config, const fs::path& share_path,
                                 const fs::path& user_config_path, const fs::path& database_path,
                                 const fs::path& sql_init_path, const fs::path& message_log_path,
                                 const std::shared_ptr<EvseSecurity> evse_security,
                                 const std::optional<SecurityConfiguration> security_configuration) :
    ocpp::ChargingStationBase(evse_security, security_configuration),
    bootreason(BootReasonEnum::PowerUp),
    initialized(false),
    connection_state(ChargePointConnectionState::Disconnected),
    registration_status(RegistrationStatus::Pending),
    diagnostics_status(DiagnosticsStatus::Idle),
    firmware_status(FirmwareStatus::Idle),
    log_status(UploadLogStatusEnumType::Idle),
    message_log_path(message_log_path.string()), // .string() for compatibility with boost::filesystem
    switch_security_profile_callback(nullptr) {
    this->configuration = std::make_shared<ocpp::v16::ChargePointConfiguration>(config, share_path, user_config_path);
    this->heartbeat_timer = std::make_unique<Everest::SteadyTimer>(&this->io_context, [this]() { this->heartbeat(); });
    this->heartbeat_interval = this->configuration->getHeartbeatInterval();
    auto database_connection = std::make_unique<everest::db::sqlite::Connection>(
        database_path / (this->configuration->getChargePointId() + ".db"));
    this->database_handler = std::make_shared<DatabaseHandler>(std::move(database_connection), sql_init_path,
                                                               this->configuration->getNumberOfConnectors());
    this->database_handler->open_connection();
    this->transaction_handler = std::make_unique<TransactionHandler>(this->configuration->getNumberOfConnectors());
    this->external_notify = {v16::MessageType::StartTransactionResponse};
    this->message_queue = this->create_message_queue();
    this->message_dispatcher =
        std::make_unique<MessageDispatcher>(*this->message_queue, *this->configuration, this->registration_status);
    auto log_formats = this->configuration->getLogMessagesFormat();
    bool log_to_console = std::find(log_formats.begin(), log_formats.end(), "console") != log_formats.end();
    bool detailed_log_to_console =
        std::find(log_formats.begin(), log_formats.end(), "console_detailed") != log_formats.end();
    bool log_to_file = std::find(log_formats.begin(), log_formats.end(), "log") != log_formats.end();
    bool log_to_html = std::find(log_formats.begin(), log_formats.end(), "html") != log_formats.end();
    bool log_security = std::find(log_formats.begin(), log_formats.end(), "security") != log_formats.end();
    bool session_logging = std::find(log_formats.begin(), log_formats.end(), "session_logging") != log_formats.end();

    if (this->configuration->getLogRotation()) {
        this->logging = std::make_shared<ocpp::MessageLogging>(
            this->configuration->getLogMessages(), this->message_log_path, "libocpp_16", log_to_console,
            detailed_log_to_console, log_to_file, log_to_html, log_security, session_logging, nullptr,
            ocpp::LogRotationConfig(this->configuration->getLogRotationDateSuffix(),
                                    this->configuration->getLogRotationMaximumFileSize(),
                                    this->configuration->getLogRotationMaximumFileCount()),
            [this](ocpp::LogRotationStatus status) {
                if (status == ocpp::LogRotationStatus::RotatedWithDeletion) {
                    this->securityEventNotification(
                        CiString<50>(ocpp::security_events::SECURITYLOGWASCLEARED),
                        CiString<255>("Security log was rotated and an old log was deleted in the process"), true);
                }
            });
    } else {
        this->logging = std::make_shared<ocpp::MessageLogging>(
            this->configuration->getLogMessages(), this->message_log_path, DateTime().to_rfc3339(), log_to_console,
            detailed_log_to_console, log_to_file, log_to_html, log_security, session_logging, nullptr);
    }

    this->boot_notification_timer =
        std::make_unique<Everest::SteadyTimer>(&this->io_context, [this]() { this->boot_notification(); });

    for (int32_t connector = 0; connector < this->configuration->getNumberOfConnectors() + 1; connector++) {
        this->status_notification_timers.push_back(std::make_unique<Everest::SteadyTimer>(&this->io_context));
    }

    this->clock_aligned_meter_values_timer =
        std::make_unique<ClockAlignedTimer>(&this->io_context, [this]() { this->clock_aligned_meter_values_sample(); });

    this->client_certificate_timer = std::make_unique<Everest::SteadyTimer>(&this->io_context, [this]() {
        EVLOG_info << "Checking if CSMS client certificate has expired";
        int expiry_days_count = this->evse_security->get_leaf_expiry_days_count(
            ocpp::CertificateSigningUseEnum::ChargingStationCertificate);
        if (expiry_days_count < 30) {
            EVLOG_info << "CSMS client certificate is invalid in " << expiry_days_count
                       << " days. Requesting new certificate with certificate signing request";
            this->sign_certificate(ocpp::CertificateSigningUseEnum::ChargingStationCertificate);
        } else {
            EVLOG_info << "CSMS client certificate is still valid.";
        }
        this->client_certificate_timer->interval(CLIENT_CERTIFICATE_TIMER_INTERVAL);
    });

    this->v2g_certificate_timer = std::make_unique<Everest::SteadyTimer>(&this->io_context, [this]() {
        EVLOG_info << "Checking if V2GCertificate has expired";
        int expiry_days_count =
            this->evse_security->get_leaf_expiry_days_count(ocpp::CertificateSigningUseEnum::V2GCertificate);
        if (expiry_days_count < 30) {
            EVLOG_info << "V2GCertificate is invalid in " << expiry_days_count
                       << " days. Requesting new certificate with certificate signing request";
            this->data_transfer_pnc_sign_certificate();
        } else {
            EVLOG_info << "V2GCertificate is still valid.";
        }
        this->v2g_certificate_timer->interval(V2G_CERTIFICATE_TIMER_INTERVAL);
    });

    this->status = std::make_unique<ChargePointStates>(
        [this](const int32_t connector, const ChargePointErrorCode errorCode, const ChargePointStatus status,
               const ocpp::DateTime& timestamp, const std::optional<CiString<50>>& info,
               const std::optional<CiString<255>>& vendor_id, const std::optional<CiString<50>>& vendor_error_code) {
            if (connector >= this->status_notification_timers.size() or connector >= this->connectors.size()) {
                EVLOG_error << "Attempting to stop status notification timer for connector " << connector
                            << " when there are only " << this->status_notification_timers.size() << " timers and "
                            << this->connectors.size() << " connectors including connector 0.";
                return;
            }
            this->status_notification_timers.at(connector)->stop();
            this->status_notification_timers.at(connector)->timeout(
                [this, connector, errorCode, status, timestamp, info, vendor_id, vendor_error_code]() {
                    this->status_notification(connector, errorCode, status, timestamp, info, vendor_id,
                                              vendor_error_code);
                },
                std::chrono::seconds(this->configuration->getMinimumStatusDuration().value_or(0)));

            // Check if the changed status should trigger to send a metervalue.
            const std::shared_ptr<Connector>& c = this->connectors.at(connector);
            if (!c->trigger_metervalue_on_status.has_value()) {
                return;
            }

            for (const auto& cp_status : c->trigger_metervalue_on_status.value()) {
                if (status == cp_status && (!c->previous_status.has_value() || c->previous_status.value() != status)) {
                    const std::optional<MeterValue>& meter_value = get_latest_meter_value(
                        connector, {{Measurand::Energy_Active_Import_Register, std::nullopt}}, ReadingContext::Other);
                    if (!meter_value.has_value()) {
                        EVLOG_error << "Send latest meter value because of chargepoint status trigger failed";
                    } else {
                        send_meter_value(connector, meter_value.value());
                        break;
                    }
                }
            }

            c->previous_status = status;
        });

    for (int id = 0; id <= this->configuration->getNumberOfConnectors(); id++) {
        this->connectors.insert(std::make_pair(id, std::make_shared<Connector>(id)));
    }

    this->smart_charging_handler =
        std::make_unique<SmartChargingHandler>(this->connectors, this->database_handler, *this->configuration);
    this->load_charging_profiles();

    // ISO15118 PnC handlers
    if (this->configuration->getSupportedFeatureProfilesSet().count(SupportedFeatureProfiles::PnC)) {
        this->data_transfer_pnc_callbacks[conversions::messagetype_to_string(MessageType::TriggerMessage)] =
            [this](ocpp::Call<ocpp::v16::DataTransferRequest> call) {
                this->handle_data_transfer_pnc_trigger_message(call);
            };
        this->data_transfer_pnc_callbacks[conversions::messagetype_to_string(MessageType::CertificateSigned)] =
            [this](ocpp::Call<ocpp::v16::DataTransferRequest> call) {
                this->handle_data_transfer_pnc_certificate_signed(call);
            };
        this->data_transfer_pnc_callbacks[conversions::messagetype_to_string(MessageType::GetInstalledCertificateIds)] =
            [this](ocpp::Call<ocpp::v16::DataTransferRequest> call) {
                this->handle_data_transfer_pnc_get_installed_certificates(call);
            };
        this->data_transfer_pnc_callbacks[conversions::messagetype_to_string(MessageType::DeleteCertificate)] =
            [this](ocpp::Call<ocpp::v16::DataTransferRequest> call) {
                this->handle_data_transfer_delete_certificate(call);
            };
        this->data_transfer_pnc_callbacks[conversions::messagetype_to_string(MessageType::InstallCertificate)] =
            [this](ocpp::Call<ocpp::v16::DataTransferRequest> call) {
                this->handle_data_transfer_install_certificate(call);
            };
        this->ocsp_request_timer = std::make_unique<Everest::SteadyTimer>(&this->io_context, [this]() {
            this->update_ocsp_cache();
            this->ocsp_request_timer->interval(OCSP_REQUEST_TIMER_INTERVAL);
        });
    }

    // California pricing requirements
    // Only enable the callbacks if display cost and price is enabled in the configuration.
    if (this->configuration->getCustomDisplayCostAndPriceEnabled()) {
        this->register_data_transfer_callback(
            CALIFORNIA_PRICING_VENDOR_ID, "SetUserPrice",
            [this](const std::optional<std::string>& message) -> DataTransferResponse {
                return handle_set_user_price(message);
            });

        this->register_data_transfer_callback(
            CALIFORNIA_PRICING_VENDOR_ID, "FinalCost",
            [this](const std::optional<std::string>& message) -> DataTransferResponse {
                return handle_set_session_cost(RunningCostState::Finished, message);
            });

        this->register_data_transfer_callback(
            CALIFORNIA_PRICING_VENDOR_ID, "RunningCost",
            [this](const std::optional<std::string>& message) -> DataTransferResponse {
                return handle_set_session_cost(RunningCostState::Charging, message);
            });

        std::optional<std::string> time_offset_transition_date_time =
            this->configuration->getNextTimeOffsetTransitionDateTime();
        if (time_offset_transition_date_time.has_value()) {
            set_time_offset_timer(time_offset_transition_date_time.value());
        }
    }
}

std::unique_ptr<ocpp::MessageQueue<v16::MessageType>> ChargePointImpl::create_message_queue() {

    // The StartTransaction.conf handler attempts to get the transaction based on the message id. The message id changes
    // in case of a message retry attempt, so we need to update it for the transaction as well
    const auto start_transaction_message_retry_callback = [this](const std::string& new_message_id,
                                                                 const std::string& old_message_id) {
        auto transaction = this->transaction_handler->get_transaction(old_message_id);
        if (transaction != nullptr) {
            transaction->set_start_transaction_message_id(new_message_id);
            try {
                this->database_handler->update_start_transaction_message_id(transaction->get_session_id(),
                                                                            new_message_id);
            } catch (const QueryExecutionException& e) {
                EVLOG_warning << "Could not update start transaction message id";
            }
        } else {
            EVLOG_warning << "Could not find transaction with start_transaction_message_id: " << old_message_id
                          << " and could therefore not replace it with: " << new_message_id;
        }
    };

    std::set<v16::MessageType> message_types_discard_for_queueing;

    if (this->configuration->getMessageTypesDiscardForQueueing().has_value()) {
        try {
            const auto message_types_discard_for_queueing_csl =
                ocpp::split_string(this->configuration->getMessageTypesDiscardForQueueing().value(), ',');
            std::transform(message_types_discard_for_queueing_csl.begin(), message_types_discard_for_queueing_csl.end(),
                           std::inserter(message_types_discard_for_queueing, message_types_discard_for_queueing.end()),
                           [](const std::string element) { return conversions::string_to_messagetype(element); });
        } catch (const StringToEnumException& e) {
            EVLOG_warning << "Could not convert configured MessageType value of MessageTypesDiscardForQueueing. Please "
                             "check you configurationMessageTypesDiscardForQueueing: "
                          << e.what();
        } catch (...) {
            EVLOG_warning << "Could not apply MessageTypesDiscardForQueueing configuration";
        }
    }

    return std::make_unique<ocpp::MessageQueue<v16::MessageType>>(
        [this](json message) -> bool { return this->websocket->send(message.dump()); },
        MessageQueueConfig<v16::MessageType>{
            this->configuration->getTransactionMessageAttempts(),
            this->configuration->getTransactionMessageRetryInterval(),
            this->configuration->getMessageQueueSizeThreshold().value_or(DEFAULT_MESSAGE_QUEUE_SIZE_THRESHOLD),
            this->configuration->getQueueAllMessages().value_or(false), message_types_discard_for_queueing},
        this->external_notify, this->database_handler, start_transaction_message_retry_callback);
}

void ChargePointImpl::init_websocket() {

    auto connection_options = this->get_ws_connection_options();

    this->websocket = std::make_unique<Websocket>(connection_options, this->evse_security, this->logging);
    this->websocket->register_connected_callback([this](OcppProtocolVersion protocol) {
        if (this->connection_state_changed_callback != nullptr) {
            this->connection_state_changed_callback(true);
        }
        this->message_queue->resume(this->message_queue_resume_delay);
        this->connected_callback();

        // signal_set_charging_profiles_callback since composite schedule could have changed if
        // IgnoredProfilePurposesOffline are configured when becoming online
        if (this->signal_set_charging_profiles_callback != nullptr and
            not this->configuration->getIgnoredProfilePurposesOffline().empty()) {
            this->signal_set_charging_profiles_callback();
        }
    });
    this->websocket->register_disconnected_callback([this]() {
        if (this->connection_state_changed_callback != nullptr) {
            this->connection_state_changed_callback(false);
        }
        this->message_queue->pause();
        if (this->ocsp_request_timer != nullptr) {
            this->ocsp_request_timer->stop();
        }
        if (this->client_certificate_timer != nullptr) {
            this->client_certificate_timer->stop();
        }
        if (this->v2g_certificate_timer != nullptr) {
            this->v2g_certificate_timer->stop();
        }
        // signal_set_charging_profiles_callback since composite schedule could have changed if
        // IgnoredProfilePurposesOffline are configured when becoming offline
        if (this->signal_set_charging_profiles_callback != nullptr and
            not this->configuration->getIgnoredProfilePurposesOffline().empty()) {
            this->signal_set_charging_profiles_callback();
        }
    });
    this->websocket->register_stopped_connecting_callback([this](const WebsocketCloseReason reason) {
        if (this->switch_security_profile_callback != nullptr) {
            this->switch_security_profile_callback();
        }
    });
    this->websocket->register_connection_failed_callback([this](const ocpp::ConnectionFailedReason reason) {
        if (reason == ocpp::ConnectionFailedReason::FailedToAuthenticateAtCsms) {
            this->securityEventNotification(CiString<50>(ocpp::security_events::FAILEDTOAUTHENTICATEATCSMS),
                                            std::nullopt, true);
        }
        if (reason == ocpp::ConnectionFailedReason::InvalidCSMSCertificate) {
            // This event is forced to accomodate for for TC_078_CS even if this event is not critical
            this->securityEventNotification(CiString<50>(ocpp::security_events::INVALIDCENTRALSYSTEMCERTIFICATE),
                                            std::nullopt, true, true);
        }
    });

    this->websocket->register_message_callback([this](const std::string& message) { this->message_callback(message); });
}
void ChargePointImpl::init_state_machine(const std::map<int, ChargePointStatus>& connector_status_map) {
    // if connector_status_map empty it retrieves the last availablity states from the database
    if (connector_status_map.empty()) {
        auto connector_availability = this->database_handler->get_connector_availability();
        std::map<int, ChargePointStatus> _connector_status_map;
        for (const auto& [connector, availability] : connector_availability) {
            if (availability == AvailabilityType::Operative) {
                _connector_status_map[connector] = ChargePointStatus::Available;
                if (this->enable_evse_callback != nullptr) {
                    this->enable_evse_callback(connector);
                }
            } else {
                _connector_status_map[connector] = ChargePointStatus::Unavailable;
                if (this->disable_evse_callback != nullptr) {
                    this->disable_evse_callback(connector);
                }
            }
        }
        this->status->reset(_connector_status_map);
    } else {
        if (static_cast<size_t>(this->configuration->getNumberOfConnectors()) + 1 != connector_status_map.size()) {
            throw std::runtime_error(
                "Number of configured connectors doesn't match number of connectors in the database.");
        }
        this->status->reset(connector_status_map);
    }
}

WebsocketConnectionOptions ChargePointImpl::get_ws_connection_options() {
    auto security_profile = this->configuration->getSecurityProfile();
    auto uri = Uri::parse_and_validate(this->configuration->getCentralSystemURI(),
                                       this->configuration->getChargePointId(), security_profile);

    WebsocketConnectionOptions connection_options{{OcppProtocolVersion::v16},
                                                  uri,
                                                  security_profile,
                                                  this->configuration->getAuthorizationKey(),
                                                  this->configuration->getRetryBackoffRandomRange(),
                                                  this->configuration->getRetryBackoffRepeatTimes(),
                                                  this->configuration->getRetryBackoffWaitMinimum(),
                                                  -1,
                                                  this->configuration->getSupportedCiphers12(),
                                                  this->configuration->getSupportedCiphers13(),
                                                  this->configuration->getWebsocketPingInterval().value_or(0),
                                                  this->configuration->getWebsocketPingPayload(),
                                                  this->configuration->getWebsocketPongTimeout(),
                                                  this->configuration->getUseSslDefaultVerifyPaths(),
                                                  this->configuration->getAdditionalRootCertificateCheck(),
                                                  this->configuration->getHostName(),
                                                  this->configuration->getVerifyCsmsCommonName(),
                                                  this->configuration->getUseTPM(),
                                                  this->configuration->getVerifyCsmsAllowWildcards(),
                                                  this->configuration->getIFace(),
                                                  this->configuration->getEnableTLSKeylog(),
                                                  this->configuration->getTLSKeylogFile()};
    return connection_options;
}

void ChargePointImpl::connect_websocket() {
    if (!this->websocket->is_connected()) {
        this->websocket->start_connecting();
    }
}

void ChargePointImpl::disconnect_websocket() {
    if (this->websocket->is_connected()) {
        this->websocket->disconnect(WebsocketCloseReason::Normal);
    }
}

void ChargePointImpl::call_set_connection_timeout() {
    if (this->set_connection_timeout_callback != nullptr) {
        this->set_connection_timeout_callback(this->configuration->getConnectionTimeOut());
    }
}

void ChargePointImpl::heartbeat(bool initiated_by_trigger_message) {
    EVLOG_debug << "Sending heartbeat";
    HeartbeatRequest req;

    ocpp::Call<HeartbeatRequest> call(req);
    this->message_dispatcher->dispatch_call(call, initiated_by_trigger_message);
}

void ChargePointImpl::boot_notification(bool initiated_by_trigger_message) {
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

    ocpp::Call<BootNotificationRequest> call(req);
    this->message_dispatcher->dispatch_call(call, initiated_by_trigger_message);
}

void ChargePointImpl::clock_aligned_meter_values_sample() {
    EVLOG_debug << "Sending clock aligned meter values";
    for (int32_t connector = 1; connector < this->configuration->getNumberOfConnectors() + 1; connector++) {
        auto meter_value = this->get_latest_meter_value(
            connector, this->configuration->getMeterValuesAlignedDataVector(), ReadingContext::Sample_Clock);
        if (meter_value.has_value()) {
            if (this->transaction_handler->transaction_active(connector)) {
                this->transaction_handler->get_transaction(connector)->add_meter_value(meter_value.value());
            }
            this->send_meter_value(connector, meter_value.value());
        } else {
            EVLOG_warning << "Could not send clock aligned meter value for uninitialized measurement at connector#"
                          << connector;
        }
    }
}

void ChargePointImpl::update_heartbeat_interval() {
    this->heartbeat_timer->interval(std::chrono::seconds(this->configuration->getHeartbeatInterval()));
}

void ChargePointImpl::update_meter_values_sample_interval() {
    // TODO(kai): should we update the meter values for continuous monitoring here too?
    int32_t interval = this->configuration->getMeterValueSampleInterval();
    this->transaction_handler->change_meter_values_sample_intervals(interval);
}

void ChargePointImpl::update_clock_aligned_meter_values_interval() {
    const auto clock_aligned_data_interval = this->configuration->getClockAlignedDataInterval();

    if (clock_aligned_data_interval > 0) {
        this->clock_aligned_meter_values_timer->interval_starting_from(
            std::chrono::seconds(clock_aligned_data_interval),
            std::chrono::floor<date::days>(date::utc_clock::to_sys(date::utc_clock::now())));
    } else {
        this->clock_aligned_meter_values_timer->stop();
    }
}

void ChargePointImpl::try_resume_transactions(const std::set<std::string>& resuming_session_ids) {
    std::vector<ocpp::v16::TransactionEntry> transactions;
    try {
        transactions = this->database_handler->get_transactions(true);
    } catch (const QueryExecutionException& e) {
        EVLOG_warning << "Could not query transactions that haven't been acknowledged by the CSMS";
        return;
    }
    for (const auto& transaction_entry : transactions) {
        std::shared_ptr<Transaction> transaction = std::make_shared<Transaction>(
            this->transaction_handler->get_negative_random_transaction_id(), transaction_entry.connector,
            transaction_entry.session_id, CiString<20>(transaction_entry.id_tag_start), transaction_entry.meter_start,
            transaction_entry.reservation_id, ocpp::DateTime(transaction_entry.time_start), nullptr);
        ocpp::DateTime timestamp;
        int meter_stop = 0;
        if (transaction_entry.time_end.has_value() and transaction_entry.meter_stop.has_value()) {
            timestamp = ocpp::DateTime(transaction_entry.time_end.value());
            meter_stop = transaction_entry.meter_stop.value();
        } else {
            timestamp = ocpp::DateTime(transaction_entry.meter_last_time);
            meter_stop = transaction_entry.meter_last;
        }

        const auto stop_energy_wh = std::make_shared<StampedEnergyWh>(timestamp, meter_stop);
        transaction->set_transaction_id(transaction_entry.transaction_id);
        // we need this in order to handle a StartTransaction.conf
        transaction->set_start_transaction_message_id(transaction_entry.start_transaction_message_id);
        if (transaction_entry.stop_transaction_message_id.has_value()) {
            // we need this in order to handle a StopTransaction.conf
            transaction->set_stop_transaction_message_id(transaction_entry.stop_transaction_message_id.value());
        }

        this->transaction_handler->add_transaction(transaction);

        EVLOG_info << "Trying to resume transaction with session_id: " << transaction_entry.session_id
                   << " and transaction_id: " << transaction_entry.transaction_id;

        if (this->message_queue->contains_stop_transaction_message(transaction_entry.transaction_id)) {
            // StopTransaction.req is already queued for the transaction in the database, so we mark the transaction as
            // stopped and wait for the StopTransaction.conf
            transaction->set_finished();
            this->transaction_handler->add_stopped_transaction(transaction->get_connector());
        } else {
            if (std::find(resuming_session_ids.begin(), resuming_session_ids.end(), transaction_entry.session_id) ==
                resuming_session_ids.end()) {
                EVLOG_info << "Queuing StopTransaction.req for transaction with id: "
                           << transaction_entry.transaction_id
                           << " because it hasn't been acknowledged by CSMS and shall not be resumed.";
                transaction->add_stop_energy_wh(stop_energy_wh);
                this->stop_transaction(transaction_entry.connector, Reason::PowerLoss, std::nullopt);
            } else {
                EVLOG_info << "Resuming transaction with transaction id: " << transaction_entry.transaction_id;
            }
        }
    }
}

void ChargePointImpl::load_charging_profiles() {
    try {
        auto profiles = this->database_handler->get_charging_profiles();
        EVLOG_info << "Found " << profiles.size() << " charging profile(s) in the database";
        const auto supported_purpose_types = this->configuration->getSupportedChargingProfilePurposeTypes();
        for (auto& profile : profiles) {
            try {
                const auto connector_id = this->database_handler->get_connector_id(profile.chargingProfileId);
                if (this->smart_charging_handler->validate_profile(
                        profile, connector_id, false, this->configuration->getChargeProfileMaxStackLevel(),
                        this->configuration->getMaxChargingProfilesInstalled(),
                        this->configuration->getChargingScheduleMaxPeriods(),
                        this->configuration->getChargingScheduleAllowedChargingRateUnitVector()) and
                    std::find(supported_purpose_types.begin(), supported_purpose_types.end(),
                              profile.chargingProfilePurpose) != supported_purpose_types.end()) {

                    if (profile.chargingProfilePurpose == ChargingProfilePurposeType::ChargePointMaxProfile) {
                        this->smart_charging_handler->add_charge_point_max_profile(profile);
                    } else if (profile.chargingProfilePurpose == ChargingProfilePurposeType::TxDefaultProfile) {
                        this->smart_charging_handler->add_tx_default_profile(profile, connector_id);
                    } else if (profile.chargingProfilePurpose == ChargingProfilePurposeType::TxProfile) {
                        this->smart_charging_handler->add_tx_profile(profile, connector_id);
                    }
                } else {
                    // delete if not valid anymore
                    this->database_handler->delete_charging_profile(profile.chargingProfileId);
                }
            } catch (RequiredEntryNotFoundException& e) {
                EVLOG_warning << "Could not get connector id from database: " << e.what();
            } catch (const QueryExecutionException& e) {
                EVLOG_warning << "Could not get connector id from database: " << e.what();
            }
        }
    } catch (const QueryExecutionException& e) {
        EVLOG_warning << "Could not load charging profiles from database: " << e.what();
    } catch (const std::exception& e) {
        EVLOG_warning << "Unknown error while loading charging profiles from database: " << e.what();
    }
}

std::optional<MeterValue> ChargePointImpl::get_latest_meter_value(int32_t connector,
                                                                  std::vector<MeasurandWithPhase> values_of_interest,
                                                                  ReadingContext context) {
    std::lock_guard<std::mutex> lock(measurement_mutex);
    std::optional<MeterValue> filtered_meter_value_opt;
    // TODO(kai): also support readings from the charge point measurement at "connector 0"
    if (this->connectors.find(connector) != this->connectors.end() &&
        this->connectors.at(connector)->measurement.has_value()) {
        MeterValue filtered_meter_value;
        const auto measurement = this->connectors.at(connector)->measurement.value();
        const auto power_meter = measurement.power_meter;
        const auto timestamp = power_meter.timestamp;
        filtered_meter_value.timestamp = ocpp::DateTime(timestamp);
        EVLOG_debug << "Measurement value for connector: " << connector << ": " << measurement;
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
            case Measurand::Energy_Active_Import_Register: {
                const auto energy_Wh_import = power_meter.energy_Wh_import;

                // Imported energy in Wh (from grid)
                sample.unit.emplace(UnitOfMeasure::Wh);
                sample.location.emplace(Location::Outlet);

                if (configured_measurand.phase) {
                    // phase available and it makes sense here
                    auto phase = configured_measurand.phase.value();
                    sample.phase.emplace(phase);
                    if (phase == Phase::L1) {
                        if (energy_Wh_import.L1) {
                            sample.value = ocpp::conversions::double_to_string((double)energy_Wh_import.L1.value());
                        } else {
                            EVLOG_debug
                                << "Power meter does not contain energy_Wh_import configured measurand for phase L1";
                        }
                    } else if (phase == Phase::L2) {
                        if (energy_Wh_import.L2) {
                            sample.value = ocpp::conversions::double_to_string((double)energy_Wh_import.L2.value());
                        } else {
                            EVLOG_debug
                                << "Power meter does not contain energy_Wh_import configured measurand for phase L2";
                        }
                    } else if (phase == Phase::L3) {
                        if (energy_Wh_import.L3) {
                            sample.value = ocpp::conversions::double_to_string((double)energy_Wh_import.L3.value());
                        } else {
                            EVLOG_debug
                                << "Power meter does not contain energy_Wh_import configured measurand for phase L3";
                        }
                    }
                } else {
                    // store total value
                    sample.value = ocpp::conversions::double_to_string((double)energy_Wh_import.total);
                }
                break;
            }
            case Measurand::Energy_Active_Export_Register: {
                const auto energy_Wh_export = power_meter.energy_Wh_export;
                // Exported energy in Wh (to grid)
                sample.unit.emplace(UnitOfMeasure::Wh);
                // TODO: which location is appropriate here? Inlet?
                // sample.location.emplace(Location::Outlet);
                if (energy_Wh_export) {
                    if (configured_measurand.phase) {
                        // phase available and it makes sense here
                        auto phase = configured_measurand.phase.value();
                        sample.phase.emplace(phase);
                        if (phase == Phase::L1) {
                            if (energy_Wh_export.value().L1) {
                                sample.value =
                                    ocpp::conversions::double_to_string((double)energy_Wh_export.value().L1.value());
                            } else {
                                EVLOG_debug << "Power meter does not contain energy_Wh_export configured measurand "
                                               "for phase L1";
                            }
                        } else if (phase == Phase::L2) {
                            if (energy_Wh_export.value().L2) {
                                sample.value =
                                    ocpp::conversions::double_to_string((double)energy_Wh_export.value().L2.value());
                            } else {
                                EVLOG_debug << "Power meter does not contain energy_Wh_export configured measurand "
                                               "for phase L2";
                            }
                        } else if (phase == Phase::L3) {
                            if (energy_Wh_export.value().L3) {
                                sample.value =
                                    ocpp::conversions::double_to_string((double)energy_Wh_export.value().L3.value());
                            } else {
                                EVLOG_debug << "Power meter does not contain energy_Wh_export configured measurand "
                                               "for phase L3";
                            }
                        }
                    } else {
                        // store total value
                        sample.value = ocpp::conversions::double_to_string((double)energy_Wh_export.value().total);
                    }
                } else {
                    EVLOG_debug << "Power meter does not contain energy_Wh_export configured measurand";
                }
                break;
            }
            case Measurand::Power_Active_Import: {
                const auto power_W = power_meter.power_W;
                // power flow to EV, Instantaneous power in Watt
                sample.unit.emplace(UnitOfMeasure::W);
                sample.location.emplace(Location::Outlet);
                if (power_W) {
                    if (configured_measurand.phase) {
                        // phase available and it makes sense here
                        auto phase = configured_measurand.phase.value();
                        sample.phase.emplace(phase);
                        if (phase == Phase::L1) {
                            if (power_W.value().L1) {
                                sample.value = ocpp::conversions::double_to_string((double)power_W.value().L1.value());
                            } else {
                                EVLOG_debug << "Power meter does not contain power_W configured measurand for phase L1";
                            }
                        } else if (phase == Phase::L2) {
                            if (power_W.value().L2) {
                                sample.value = ocpp::conversions::double_to_string((double)power_W.value().L2.value());
                            } else {
                                EVLOG_debug << "Power meter does not contain power_W configured measurand for phase L2";
                            }
                        } else if (phase == Phase::L3) {
                            if (power_W.value().L3) {
                                sample.value = ocpp::conversions::double_to_string((double)power_W.value().L3.value());
                            } else {
                                EVLOG_debug << "Power meter does not contain power_W configured measurand for phase L3";
                            }
                        }
                    } else {
                        // store total value
                        sample.value = ocpp::conversions::double_to_string((double)power_W.value().total);
                    }
                } else {
                    EVLOG_debug << "Power meter does not contain power_W configured measurand";
                }
                break;
            }
            case Measurand::Voltage: {
                const auto voltage_V = power_meter.voltage_V;
                // AC supply voltage, Voltage in Volts
                sample.unit.emplace(UnitOfMeasure::V);
                sample.location.emplace(Location::Outlet);
                if (voltage_V) {
                    if (configured_measurand.phase) {
                        // phase available and it makes sense here
                        auto phase = configured_measurand.phase.value();
                        sample.phase.emplace(phase);
                        if (phase == Phase::L1) {
                            if (voltage_V.value().L1) {
                                sample.value =
                                    ocpp::conversions::double_to_string((double)voltage_V.value().L1.value());
                            } else {
                                EVLOG_debug
                                    << "Power meter does not contain voltage_V configured measurand for phase L1";
                            }
                        } else if (phase == Phase::L2) {
                            if (voltage_V.value().L2) {
                                sample.value =
                                    ocpp::conversions::double_to_string((double)voltage_V.value().L2.value());
                            } else {
                                EVLOG_debug
                                    << "Power meter does not contain voltage_V configured measurand for phase L2";
                            }
                        } else if (phase == Phase::L3) {
                            if (voltage_V.value().L3) {
                                sample.value =
                                    ocpp::conversions::double_to_string((double)voltage_V.value().L3.value());
                            } else {
                                EVLOG_debug
                                    << "Power meter does not contain voltage_V configured measurand for phase L3";
                            }
                        }
                    }
                    // report DC value if set. This is a workaround for the fact that the power meter does not report
                    // AC (Dc charging)
                    else if (voltage_V.value().DC) {
                        sample.value = ocpp::conversions::double_to_string((double)voltage_V.value().DC.value());
                    }

                } else {
                    EVLOG_debug << "Power meter does not contain voltage_V configured measurand";
                }
                break;
            }
            case Measurand::Current_Import: {
                const auto current_A = power_meter.current_A;
                // current flow to EV in A
                sample.unit.emplace(UnitOfMeasure::A);
                sample.location.emplace(Location::Outlet);
                if (current_A) {
                    if (configured_measurand.phase) {
                        // phase available and it makes sense here
                        auto phase = configured_measurand.phase.value();
                        sample.phase.emplace(phase);
                        if (phase == Phase::L1) {
                            if (current_A.value().L1) {
                                sample.value =
                                    ocpp::conversions::double_to_string((double)current_A.value().L1.value());
                            } else {
                                EVLOG_debug
                                    << "Power meter does not contain current_A configured measurand for phase L1";
                            }
                        } else if (phase == Phase::L2) {
                            if (current_A.value().L2) {
                                sample.value =
                                    ocpp::conversions::double_to_string((double)current_A.value().L2.value());
                            } else {
                                EVLOG_debug
                                    << "Power meter does not contain current_A configured measurand for phase L2";
                            }
                        } else if (phase == Phase::L3) {
                            if (current_A.value().L3) {
                                sample.value =
                                    ocpp::conversions::double_to_string((double)current_A.value().L3.value());
                            } else {
                                EVLOG_debug
                                    << "Power meter does not contain current_A configured measurand for phase L3";
                            }
                        }
                    }
                    // report DC value if set. This is a workaround for the fact that the power meter does not report
                    // AC (DC charging)
                    else if (current_A.value().DC) {
                        sample.value = ocpp::conversions::double_to_string((double)current_A.value().DC.value());
                    }
                } else {
                    EVLOG_debug << "Power meter does not contain current_A configured measurand";
                }

                break;
            }
            case Measurand::Frequency: {
                const auto frequency_Hz = power_meter.frequency_Hz;
                // Grid frequency in Hertz
                // TODO: which location is appropriate here? Inlet?
                // sample.location.emplace(Location::Outlet);
                if (frequency_Hz) {
                    if (configured_measurand.phase) {
                        // phase available and it makes sense here
                        auto phase = configured_measurand.phase.value();
                        sample.phase.emplace(phase);
                        if (phase == Phase::L1) {
                            sample.value = ocpp::conversions::double_to_string((double)frequency_Hz.value().L1);
                        } else if (phase == Phase::L2) {
                            if (frequency_Hz.value().L2) {
                                sample.value =
                                    ocpp::conversions::double_to_string((double)frequency_Hz.value().L2.value());
                            } else {
                                EVLOG_debug
                                    << "Power meter does not contain frequency_Hz configured measurand for phase L2";
                            }
                        } else if (phase == Phase::L3) {
                            if (frequency_Hz.value().L3) {
                                sample.value =
                                    ocpp::conversions::double_to_string((double)frequency_Hz.value().L3.value());
                            } else {
                                EVLOG_debug
                                    << "Power meter does not contain frequency_Hz configured measurand for phase L3";
                            }
                        }
                    }
                } else {
                    EVLOG_debug << "Power meter does not contain frequency_Hz configured measurand";
                }
                break;
            }
            case Measurand::Current_Offered: {
                // current offered to EV
                sample.unit.emplace(UnitOfMeasure::A);
                sample.location.emplace(Location::Outlet);

                sample.value = ocpp::conversions::double_to_string(this->connectors.at(connector)->max_current_offered);
                break;
            }
            case Measurand::Power_Offered: {
                // power offered to EV
                sample.unit.emplace(UnitOfMeasure::W);
                sample.location.emplace(Location::Outlet);

                sample.value = ocpp::conversions::double_to_string(this->connectors.at(connector)->max_power_offered);
                break;
            }
            case Measurand::SoC: {
                // state of charge
                const auto soc = measurement.soc_Percent;
                if (soc) {
                    sample.unit.emplace(UnitOfMeasure::Percent);
                    sample.value = ocpp::conversions::double_to_string(soc.value().value);

                    if (soc.value().location.has_value()) {
                        sample.location.emplace(conversions::string_to_location(soc.value().location.value()));
                    } else {
                        sample.location.emplace(Location::EV);
                    }
                } else {
                    EVLOG_debug << "Measurement does not contain soc_Percent configured measurand";
                }
                break;
            }

            case Measurand::Temperature: {
                // temperature
                for (const auto temperature : measurement.temperature_C) {
                    sample.unit.emplace(UnitOfMeasure::Celsius);
                    if (temperature.location.has_value()) {
                        try {
                            sample.location.emplace(conversions::string_to_location(temperature.location.value()));
                        } catch (const StringToEnumException& e) {
                            EVLOG_debug << "Could not convert string: " << temperature.location.value()
                                        << " to Location";
                        }
                    } else {
                        sample.location = std::nullopt;
                    }
                    sample.value = ocpp::conversions::double_to_string(temperature.value);
                    filtered_meter_value.sampledValue.push_back(sample);
                    sample.value.clear();
                }
                if (measurement.temperature_C.empty()) {
                    EVLOG_debug << "Measurement does not contain temperature_C configured measurand";
                }
                break;
            }

            case Measurand::RPM: {
                // RPM
                const auto rpm = measurement.rpm;
                if (rpm) {
                    if (rpm.value().location.has_value()) {
                        sample.location.emplace(conversions::string_to_location(rpm.value().location.value()));
                    } else {
                        sample.location.emplace(Location::EV);
                    }
                    sample.value = ocpp::conversions::double_to_string(rpm.value().value);
                } else {
                    EVLOG_debug << "Measurement does not contain rpm configured measurand";
                }
                break;
            }
            case Measurand::Energy_Reactive_Export_Register:
            case Measurand::Energy_Reactive_Import_Register:
            case Measurand::Energy_Active_Export_Interval:
            case Measurand::Energy_Active_Import_Interval:
            case Measurand::Energy_Reactive_Export_Interval:
            case Measurand::Energy_Reactive_Import_Interval:
            case Measurand::Power_Active_Export:
            case Measurand::Power_Reactive_Export:
            case Measurand::Power_Reactive_Import:
            case Measurand::Power_Factor:
            case Measurand::Current_Export:
                break;
            }
            // only add if value is set
            if (!sample.value.empty()) {
                filtered_meter_value.sampledValue.push_back(sample);
            }
        }
        filtered_meter_value_opt.emplace(filtered_meter_value);
    }
    return filtered_meter_value_opt;
}

MeterValue ChargePointImpl::get_signed_meter_value(const std::string& signed_value, const ReadingContext& context,
                                                   const ocpp::DateTime& timestamp) {
    MeterValue meter_value;
    meter_value.timestamp = timestamp;
    SampledValue sampled_value;
    sampled_value.context = context;
    sampled_value.value = signed_value;
    sampled_value.format = ValueFormat::SignedData;

    meter_value.sampledValue.push_back(sampled_value);
    return meter_value;
}

void ChargePointImpl::send_meter_value(int32_t connector, MeterValue meter_value, bool initiated_by_trigger_message) {

    if (meter_value.sampledValue.size() == 0) {
        return;
    }

    MeterValuesRequest req;
    const auto message_id = ocpp::create_message_id();
    // connector = 0 designates the main measurement
    // connector > 0 designates a connector of the charge point
    req.connectorId = connector;
    std::ostringstream oss;
    oss << "Gathering measurands of connector: " << connector;

    if (connector > 0) {
        auto transaction = this->transaction_handler->get_transaction(connector);
        if (transaction != nullptr and transaction->get_transaction_id().has_value()) {
            auto transaction_id = transaction->get_transaction_id().value();
            req.transactionId.emplace(transaction_id);
        } else if (transaction != nullptr) {
            // this means a transaction is active but we have not received a transaction_id from CSMS yet
            this->message_queue->add_meter_value_message_id(transaction->get_start_transaction_message_id(),
                                                            message_id.get());
        }
    }

    EVLOG_debug << oss.str();

    req.meterValue.push_back(meter_value);

    ocpp::Call<MeterValuesRequest> call(req, message_id);
    this->message_dispatcher->dispatch_call(call, initiated_by_trigger_message);
}

void ChargePointImpl::send_meter_value_on_pricing_trigger(const int32_t connector_number,
                                                          std::shared_ptr<Connector> connector,
                                                          const Measurement& measurement) {
    if (connector->trigger_metervalue_on_energy_kwh.has_value()) {
        if (static_cast<double>(measurement.power_meter.energy_Wh_import.total) >=
            (connector->trigger_metervalue_on_energy_kwh.value() * 1000)) {

            MeasurandWithPhase measurand;
            measurand.measurand = Measurand::Energy_Active_Import_Register;
            const auto& meter_value = get_latest_meter_value(connector_number, {measurand}, ReadingContext::Other);

            if (meter_value.has_value()) {
                EVLOG_debug << "Sending meter value because of kWh pricing trigger";

                send_meter_value(connector_number, meter_value.value());
                // Metervalue sent, should not be sent again.
                connector->trigger_metervalue_on_energy_kwh = std::nullopt;
                return;
            } else {
                EVLOG_error << "Send latest meter value because of energy (kWh) trigger failed";
            }
        }
    }

    if (not connector->trigger_metervalue_on_power_kw.has_value() or not measurement.power_meter.power_W.has_value()) {
        return;
    }

    // Store current power kW from measurement.
    const double current_power_kw = static_cast<double>(measurement.power_meter.power_W.value().total) / 1000.0;

    if (connector->last_triggered_metervalue_power_kw.has_value()) {
        const double triggered_power_kw = connector->last_triggered_metervalue_power_kw.value();
        const double trigger_metervalue_kw = connector->trigger_metervalue_on_power_kw.value();
        // Hysteresis of 5% to avoid repetitive triggers when the power fluctuates around the trigger level.
        const double hysteresis_kw = trigger_metervalue_kw * 0.05;

        if ( // Check if trigger value is crossed in upward direction.
            (triggered_power_kw < trigger_metervalue_kw &&
             current_power_kw >= (trigger_metervalue_kw + hysteresis_kw)) ||
            // Check if trigger value is crossed in downward direction.
            (triggered_power_kw > trigger_metervalue_kw &&
             current_power_kw <= (trigger_metervalue_kw - hysteresis_kw))) {

            // Power threshold is crossed, send metervalues.
            const auto& meter_value = get_latest_meter_value(connector_number,
                                                             {{Measurand::Energy_Active_Import_Register, std::nullopt},
                                                              {Measurand::Power_Active_Import, std::nullopt}},
                                                             ReadingContext::Other);
            if (!meter_value.has_value()) {
                EVLOG_error << "Send latest meter value because of power (Wh) trigger failed";
            } else {
                send_meter_value(connector_number, meter_value.value());
                // Store just sent power value to check next time.
                connector->last_triggered_metervalue_power_kw = current_power_kw;
            }
        }
    } else {
        // Send metervalue anyway since we have no previous metervalue stored and don't know if we should send any
        const auto& meter_value = get_latest_meter_value(
            connector_number,
            {{Measurand::Energy_Active_Import_Register, std::nullopt}, {Measurand::Power_Active_Import, std::nullopt}},
            ReadingContext::Other);
        if (!meter_value.has_value()) {
            EVLOG_error << "Send latest meter value because of power (Wh) trigger failed";
        } else {
            send_meter_value(connector_number, meter_value.value());
            // Store just sent power value to check for crossing next time.
            connector->last_triggered_metervalue_power_kw = current_power_kw;
        }
    }
}

void ChargePointImpl::reset_pricing_triggers(const int32_t connector_number) {
    std::shared_ptr<Connector> c = this->connectors.at(connector_number);
    c->last_triggered_metervalue_power_kw = std::nullopt;
    c->trigger_metervalue_on_power_kw = std::nullopt;
    c->trigger_metervalue_on_energy_kwh = std::nullopt;
    c->trigger_metervalue_on_status = std::nullopt;
    c->previous_status = std::nullopt;

    if (c->trigger_metervalue_at_time_timer != nullptr) {
        c->trigger_metervalue_at_time_timer->stop();
        c->trigger_metervalue_at_time_timer = nullptr;
    }
}

bool ChargePointImpl::init(const std::map<int, ChargePointStatus>& connector_status_map,
                           const std::set<std::string>& resuming_session_ids) {
    // push transaction messages including SecurityEventNotification.req onto the message queue
    this->message_queue->get_persisted_messages_from_db(this->configuration->getDisableSecurityEventNotifications());
    this->try_resume_transactions(
        resuming_session_ids); // calling resume transactions before message_queue.start() ensures that
                               // no message handlers (e.g. for StopTransaction.conf) are
                               // yet received that could interfere with try_resume_transactions
    this->message_queue->start();
    this->init_state_machine(connector_status_map);
    this->initialized = true;
    return true;
}

bool ChargePointImpl::start(const std::map<int, ChargePointStatus>& connector_status_map, BootReasonEnum bootreason,
                            const std::set<std::string>& resuming_session_ids) {
    if (!this->initialized) {
        init(connector_status_map, resuming_session_ids);
    }
    this->bootreason = bootreason;
    this->init_websocket();
    this->websocket->start_connecting();
    this->boot_notification();
    this->call_set_connection_timeout();

    switch (bootreason) {
    case BootReasonEnum::RemoteReset:
        this->securityEventNotification(
            CiString<50>(ocpp::security_events::RESET_OR_REBOOT),
            std::optional<CiString<255>>("Charging Station rebooted due to requested remote reset!"), true);
        break;
    case BootReasonEnum::ScheduledReset:
        this->securityEventNotification(
            CiString<50>(ocpp::security_events::RESET_OR_REBOOT),
            std::optional<CiString<255>>("Charging Station rebooted due to a scheduled reset!"), true);
        break;
    case BootReasonEnum::FirmwareUpdate:
        this->securityEventNotification(
            CiString<50>(ocpp::security_events::FIRMWARE_UPDATED),
            std::optional<CiString<255>>("Charging Station rebooted due to firmware update!"), true);
        break;
    case BootReasonEnum::ApplicationReset:
    case BootReasonEnum::LocalReset:
    case BootReasonEnum::PowerUp:
    case BootReasonEnum::Triggered:
    case BootReasonEnum::Unknown:
    case BootReasonEnum::Watchdog:
        this->securityEventNotification(CiString<50>(ocpp::security_events::STARTUP_OF_THE_DEVICE),
                                        std::optional<CiString<255>>("The Charge Point has booted"), true);
        break;
    }

    this->stopped = false;
    return true;
}

bool ChargePointImpl::restart(const std::map<int, ChargePointStatus>& connector_status_map, BootReasonEnum bootreason) {
    if (this->stopped) {
        EVLOG_info << "Restarting OCPP Chargepoint";
        this->database_handler->open_connection();
        // instantiating new message queue on restart
        this->message_queue = this->create_message_queue();
        this->message_dispatcher =
            std::make_unique<MessageDispatcher>(*this->message_queue, *this->configuration, this->registration_status);
        return this->start(connector_status_map, bootreason, {});
    } else {
        EVLOG_warning << "Attempting to restart Chargepoint while it has not been stopped before";
        return false;
    }
}

void ChargePointImpl::reset_state_machine(const std::map<int, ChargePointStatus>& connector_status_map) {
    this->status->reset(connector_status_map);
}

void ChargePointImpl::change_all_connectors_to_unavailable_for_firmware_update() {
    std::map<int32_t, AvailabilityStatus> connector_availability_status;

    bool transaction_running = false;

    int32_t number_of_connectors = this->configuration->getNumberOfConnectors();
    for (int32_t connector = 1; connector <= number_of_connectors; connector++) {
        if (this->transaction_handler->transaction_active(connector)) {
            EVLOG_debug << "change_all_connectors_to_unavailable_for_firmware_update: detected running Transaction on "
                           "connector "
                        << connector;
            transaction_running = true;
            std::lock_guard<std::mutex> change_availability_lock(change_availability_mutex);
            this->change_availability_queue[connector] = {AvailabilityType::Inoperative, false};
        } else {
            connector_availability_status[connector] = AvailabilityStatus::Accepted;
        }
    }

    for (const auto& [connector, availability_status] : connector_availability_status) {
        if (connector == 0) {
            this->status->submit_event(0, FSMEvent::ChangeAvailabilityToUnavailable, ocpp::DateTime());
        } else if (this->disable_evse_callback != nullptr and availability_status == AvailabilityStatus::Accepted) {
            this->disable_evse_callback(connector);
        }
    }

    if (!transaction_running) {
        if (this->all_connectors_unavailable_callback) {
            this->all_connectors_unavailable_callback();
        }
    }
}

void ChargePointImpl::stop_all_transactions() {
    this->stop_all_transactions(Reason::Other);
}

void ChargePointImpl::stop_all_transactions(Reason reason) {
    int32_t number_of_connectors = this->configuration->getNumberOfConnectors();
    for (int32_t connector = 1; connector <= number_of_connectors; connector++) {
        if (this->transaction_handler->transaction_active(connector)) {
            this->stop_transaction_callback(connector, reason);
        }
    }
}

bool ChargePointImpl::stop() {
    if (!this->stopped) {
        EVLOG_info << "Stopping OCPP Chargepoint";
        if (this->boot_notification_timer != nullptr) {
            this->boot_notification_timer->stop();
        }
        if (this->heartbeat_timer != nullptr) {
            this->heartbeat_timer->stop();
        }
        if (this->clock_aligned_meter_values_timer != nullptr) {
            this->clock_aligned_meter_values_timer->stop();
        }
        if (this->ocsp_request_timer != nullptr) {
            this->ocsp_request_timer->stop();
        }
        if (this->client_certificate_timer != nullptr) {
            this->client_certificate_timer->stop();
        }
        if (this->v2g_certificate_timer != nullptr) {
            this->v2g_certificate_timer->stop();
        }
        if (this->change_time_offset_timer != nullptr) {
            this->change_time_offset_timer->stop();
        }

        for (const auto& [id, connector] : this->connectors) {
            if (connector->trigger_metervalue_at_time_timer != nullptr) {
                connector->trigger_metervalue_at_time_timer->stop();
            }
        }

        this->websocket_timer.stop();

        this->stop_all_transactions();

        this->database_handler->close_connection();
        this->websocket->disconnect(WebsocketCloseReason::Normal);
        this->message_queue->stop();

        this->stopped = true;
        this->initialized = false;
        EVLOG_info << "Terminating...";
        return true;
    } else {
        EVLOG_warning << "Attempting to stop Chargepoint while it has been stopped before";
        return false;
    }

    EVLOG_info << "Terminating...";
}

void ChargePointImpl::connected_callback() {
    this->switch_security_profile_callback = nullptr;
    switch (this->connection_state) {
    case ChargePointConnectionState::Disconnected: {
        this->connection_state = ChargePointConnectionState::Connected;
        break;
    }
    case ChargePointConnectionState::Booted: {
        // on_open in a Booted state can happen after a successful reconnect.
        // according to spec, a charge point should not send a BootNotification after a reconnect
        // still we send StatusNotification.req for all connectors after a reconnect
        this->status->trigger_status_notifications();
        break;
    }
    case ChargePointConnectionState::Pending: {
        // in Pending state this can happen when we reconnected while the BootNotification had not been yet accepted
        break;
    }
    case ChargePointConnectionState::Rejected:
    case ChargePointConnectionState::Connected:
        EVLOG_error << "Connected but not in state 'Disconnected' or 'Booted'. This can happen when the CSMS does not "
                       "respond to the initial BootNotification.req at all or with a CALLERROR";
        break;
    }
}

void ChargePointImpl::message_callback(const std::string& message) {
    EVLOG_debug << "Received Message: " << message;

    EnhancedMessage<v16::MessageType> enhanced_message;
    try {
        enhanced_message = this->message_queue->receive(message);
    } catch (const TimePointParseException& e) {
        EVLOG_error << "Exception during handling of message: " << e.what();
        this->message_dispatcher->dispatch_call_error(
            CallError(enhanced_message.uniqueId, "FormationViolation", e.what(), json({})));
        return;
    } catch (const json::exception& e) {
        EVLOG_error << "JSON exception during reception of message: " << e.what();
        this->message_dispatcher->dispatch_call_error(CallError(MessageId("-1"), "GenericError", e.what(), json({})));
        return;
    } catch (const std::runtime_error& e) {
        EVLOG_error << "runtime_error during reception of message: " << e.what();
        this->message_dispatcher->dispatch_call_error(CallError(MessageId("-1"), "GenericError", e.what(), json({})));
        return;
    } catch (const std::exception& e) {
        EVLOG_error << "Exception during reception of message: " << e.what();
        this->message_dispatcher->dispatch_call_error(CallError(MessageId("-1"), "GenericError", e.what(), json({})));
        return;
    }

    auto json_message = enhanced_message.message;
    this->logging->central_system(conversions::messagetype_to_string(enhanced_message.messageType), message);
    try {
        // reject unsupported messages
        if (this->configuration->getSupportedMessageTypesReceiving().count(enhanced_message.messageType) == 0) {
            EVLOG_warning << "Received an unsupported message: " << enhanced_message.messageType;
            // FIXME(kai): however, only send a CALLERROR when it is a CALL message we just received
            if (enhanced_message.messageTypeId == MessageTypeId::CALL) {
                auto call_error = CallError(enhanced_message.uniqueId, "NotSupported", "", json({}, true));
                this->message_dispatcher->dispatch_call_error(call_error);
            } else if (enhanced_message.messageTypeId == MessageTypeId::CALLERROR) {
                EVLOG_error << "Received a CALLERROR in response to a "
                            << conversions::messagetype_to_string(enhanced_message.messageType) << ": " << message;
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
            } else if (enhanced_message.messageTypeId == MessageTypeId::CALL) {
                // we dont want to reply to this message
                this->message_queue->reset_next_message_to_send();
            }
            break;
        }
        case ChargePointConnectionState::Rejected: {
            if (this->registration_status == RegistrationStatus::Rejected) {
                if (enhanced_message.messageType == MessageType::BootNotificationResponse) {
                    this->handleBootNotificationResponse(json_message);
                } else if (enhanced_message.messageTypeId == MessageTypeId::CALL) {
                    // we dont want to reply to this message
                    this->message_queue->reset_next_message_to_send();
                }
            }
            break;
        }
        case ChargePointConnectionState::Pending: {
            if (this->registration_status == RegistrationStatus::Pending) {
                if (enhanced_message.messageType == MessageType::BootNotificationResponse) {
                    this->handleBootNotificationResponse(json_message);
                } else if (enhanced_message.messageType == MessageType::RemoteStartTransaction) {
                    RemoteStartTransactionResponse response;
                    response.status = RemoteStartStopStatus::Rejected;
                    const ocpp::CallResult<RemoteStartTransactionResponse> call_result(response,
                                                                                       enhanced_message.uniqueId);
                    this->message_dispatcher->dispatch_call_result(call_result);
                } else if (enhanced_message.messageType == MessageType::RemoteStopTransaction) {
                    RemoteStopTransactionResponse response;
                    response.status = RemoteStartStopStatus::Rejected;
                    const ocpp::CallResult<RemoteStopTransactionResponse> call_result(response,
                                                                                      enhanced_message.uniqueId);
                    this->message_dispatcher->dispatch_call_result(call_result);
                } else {
                    this->handle_message(enhanced_message);
                }
            }
            break;
        }
        case ChargePointConnectionState::Booted: {
            this->handle_message(enhanced_message);
            break;
        }

        default:
            EVLOG_error << "Reached default statement in on_message, this should not be possible";
            break;
        }
    } catch (json::exception& e) {
        EVLOG_error << "JSON exception during handling of message: " << e.what();
        this->securityEventNotification(ocpp::security_events::INVALIDMESSAGES,
                                        CiString<255>(message, StringTooLarge::Truncate), true);
        if (enhanced_message.messageTypeId != MessageTypeId::CALL) {
            return; // CALLERROR shall only follow on a CALL message
        }
        if (json_message.is_array() && json_message.size() > MESSAGE_ID) {
            auto call_error = CallError(enhanced_message.uniqueId, "FormationViolation", e.what(), json({}, true));
            this->message_dispatcher->dispatch_call_error(call_error);
        }
    } catch (const EnumConversionException& e) {
        EVLOG_error << "EnumConversionException during handling of message: " << e.what();
        this->securityEventNotification(ocpp::security_events::INVALIDMESSAGES,
                                        CiString<255>(message, StringTooLarge::Truncate), true);
        if (enhanced_message.messageTypeId != MessageTypeId::CALL) {
            return; // CALLERROR shall only follow on a CALL message
        }
        auto call_error = CallError(enhanced_message.uniqueId, "FormationViolation", e.what(), json({}, true));
        this->message_dispatcher->dispatch_call_error(call_error);
    } catch (const StringConversionException& e) {
        EVLOG_error << "StringConversionException during handling of message: " << e.what();
        this->securityEventNotification(ocpp::security_events::INVALIDMESSAGES,
                                        CiString<255>(message, StringTooLarge::Truncate), true);
        if (enhanced_message.messageTypeId != MessageTypeId::CALL) {
            return; // CALLERROR shall only follow on a CALL message
        }
        auto call_error = CallError(enhanced_message.uniqueId, "FormationViolation", e.what(), json({}, true));
        this->message_dispatcher->dispatch_call_error(call_error);
    } catch (const std::exception& e) {
        EVLOG_error << "Exception during handling of message: " << e.what();
        if (enhanced_message.messageTypeId != MessageTypeId::CALL) {
            return; // CALLERROR shall only follow on a CALL message
        }
        auto call_error = CallError(enhanced_message.uniqueId, "GenericError", e.what(), json({}, true));
        this->message_dispatcher->dispatch_call_error(call_error);
    }
}

void ChargePointImpl::handle_message(const EnhancedMessage<v16::MessageType>& message) {
    const auto& json_message = message.message;
    // lots of messages are allowed here
    switch (message.messageType) {

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
        this->handleStopTransactionResponse(message);
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

    case MessageType::HeartbeatResponse:
        this->handleHeartbeatResponse(json_message);
        break;

    case MessageType::Authorize:
    case MessageType::BootNotification:
    case MessageType::BootNotificationResponse:
    case MessageType::CancelReservationResponse:
    case MessageType::CertificateSignedResponse:
    case MessageType::ChangeAvailabilityResponse:
    case MessageType::ChangeConfigurationResponse:
    case MessageType::ClearCacheResponse:
    case MessageType::ClearChargingProfileResponse:
    case MessageType::DeleteCertificateResponse:
    case MessageType::DiagnosticsStatusNotification:
    case MessageType::DiagnosticsStatusNotificationResponse:
    case MessageType::ExtendedTriggerMessageResponse:
    case MessageType::FirmwareStatusNotification:
    case MessageType::FirmwareStatusNotificationResponse:
    case MessageType::GetCompositeScheduleResponse:
    case MessageType::GetConfigurationResponse:
    case MessageType::GetDiagnosticsResponse:
    case MessageType::GetInstalledCertificateIdsResponse:
    case MessageType::GetLocalListVersionResponse:
    case MessageType::GetLogResponse:
    case MessageType::Heartbeat:
    case MessageType::InstallCertificateResponse:
    case MessageType::LogStatusNotification:
    case MessageType::LogStatusNotificationResponse:
    case MessageType::MeterValues:
    case MessageType::MeterValuesResponse:
    case MessageType::RemoteStartTransactionResponse:
    case MessageType::RemoteStopTransactionResponse:
    case MessageType::ReserveNowResponse:
    case MessageType::ResetResponse:
    case MessageType::SecurityEventNotification:
    case MessageType::SecurityEventNotificationResponse:
    case MessageType::SendLocalListResponse:
    case MessageType::SetChargingProfileResponse:
    case MessageType::SignCertificate:
    case MessageType::SignCertificateResponse:
    case MessageType::SignedFirmwareStatusNotification:
    case MessageType::SignedFirmwareStatusNotificationResponse:
    case MessageType::SignedUpdateFirmwareResponse:
    case MessageType::StartTransaction:
    case MessageType::StatusNotification:
    case MessageType::StatusNotificationResponse:
    case MessageType::StopTransaction:
    case MessageType::TriggerMessageResponse:
    case MessageType::UnlockConnectorResponse:
    case MessageType::UpdateFirmwareResponse:
    case MessageType::InternalError:
        // TODO(kai): not implemented error?
        break;
    }
}

void ChargePointImpl::handleBootNotificationResponse(ocpp::CallResult<BootNotificationResponse> call_result) {
    EVLOG_debug << "Received BootNotificationResponse: " << call_result.msg
                << "\nwith messageId: " << call_result.uniqueId;

    this->registration_status = call_result.msg.status;
    this->boot_time = date::utc_clock::now();
    if (call_result.msg.interval > 0) {
        this->configuration->setHeartbeatInterval(call_result.msg.interval);
    }

    // If interval value is zero, the Charge Point chooses a waiting interval on its own
    auto boot_notification_retry_interval = DEFAULT_BOOT_NOTIFICATION_INTERVAL_S;
    if (call_result.msg.interval > 0) {
        boot_notification_retry_interval = call_result.msg.interval;
    }

    if (call_result.msg.status == RegistrationStatus::Accepted) {
        this->connection_state = ChargePointConnectionState::Booted;
        this->message_queue->set_registration_status_accepted();

        if (this->set_system_time_callback != nullptr) {
            this->set_system_time_callback(call_result.msg.currentTime.to_rfc3339());
        }

        // in case the BootNotification.req was triggered by a TriggerMessage.req the timer might still run
        this->boot_notification_timer->stop();

        // we are allowed to send messages to the central system
        // activate heartbeat
        this->update_heartbeat_interval();

        // activate clock aligned sampling of meter values
        this->update_clock_aligned_meter_values_interval();

        // send initial StatusNotification.req
        this->status->trigger_status_notifications();

        if (this->is_pnc_enabled()) {
            this->ocsp_request_timer->timeout(INITIAL_CERTIFICATE_REQUESTS_DELAY);
            this->v2g_certificate_timer->timeout(INITIAL_CERTIFICATE_REQUESTS_DELAY);
        }

        if (this->configuration->getSecurityProfile() == 3) {
            this->client_certificate_timer->stop();
            this->client_certificate_timer->timeout(INITIAL_CERTIFICATE_REQUESTS_DELAY);
        }

        if (this->is_pnc_enabled()) {
            this->ocsp_request_timer->timeout(INITIAL_CERTIFICATE_REQUESTS_DELAY);
        }

    } else if (call_result.msg.status == RegistrationStatus::Pending) {
        this->connection_state = ChargePointConnectionState::Pending;
        EVLOG_info << "BootNotification response is pending.";
        this->boot_notification_timer->timeout(std::chrono::seconds(boot_notification_retry_interval));
    } else {
        this->connection_state = ChargePointConnectionState::Rejected;
        // In this state we are not allowed to send any messages to the central system, even when
        // requested. The first time we are allowed to send a message (a BootNotification) is
        // after boot_time + heartbeat_interval if the msg.interval is 0, or after boot_timer + msg.interval
        EVLOG_info << "BootNotification was rejected, trying again in " << this->configuration->getHeartbeatInterval()
                   << "s";

        this->boot_notification_timer->timeout(std::chrono::seconds(boot_notification_retry_interval));
    }

    if (this->boot_notification_response_callback != nullptr) {
        this->boot_notification_response_callback(call_result.msg);
    }
}

void ChargePointImpl::preprocess_change_availability_request(
    const ChangeAvailabilityRequest& request, ChangeAvailabilityResponse& response,
    std::vector<int32_t>& accepted_connector_availability_changes) {

    // we can only change the connector availability if there is no active transaction on this
    // connector or the connector is not in Preparing state. If this is the case this change must be scheduled and we
    // should report an availability status of "Scheduled"
    auto should_be_scheduled_func = [this](int32_t connector) {
        return this->transaction_handler->transaction_active(connector) or
               this->status->get_state(connector) == ChargePointStatus::Preparing;
    };

    // check if connector exists
    if (request.connectorId <= this->configuration->getNumberOfConnectors() && request.connectorId >= 0) {
        bool should_be_scheduled = false;

        if (request.connectorId == 0) {
            accepted_connector_availability_changes.push_back(0);
            int32_t number_of_connectors = this->configuration->getNumberOfConnectors();
            for (int32_t connector = 1; connector <= number_of_connectors; connector++) {
                if (should_be_scheduled_func(connector)) {
                    should_be_scheduled = true;
                    std::lock_guard<std::mutex> change_availability_lock(change_availability_mutex);
                    this->change_availability_queue[connector] = {request.type, true};
                } else {
                    accepted_connector_availability_changes.push_back(connector);
                }
            }
        } else {
            if (should_be_scheduled_func(request.connectorId)) {
                should_be_scheduled = true;
                std::lock_guard<std::mutex> change_availability_lock(change_availability_mutex);
                this->change_availability_queue[request.connectorId] = {request.type, true};
            } else {
                accepted_connector_availability_changes.push_back(request.connectorId);
            }
        }

        // We store the queued request in the database, so in case of a powerloss the operational state is persisted.
        for (const auto& [connector, availabilityChange] : this->change_availability_queue) {
            this->database_handler->insert_or_update_connector_availability(connector, availabilityChange.availability);
        }

        if (should_be_scheduled) {
            response.status = AvailabilityStatus::Scheduled;
        } else {
            response.status = AvailabilityStatus::Accepted;
        }
    } else {
        // Reject if given connector id doesnt exist
        response.status = AvailabilityStatus::Rejected;
    }
}

void ChargePointImpl::execute_connectors_availability_change(const std::vector<int32_t>& changed_connectors,
                                                             const ocpp::v16::AvailabilityType availability,
                                                             bool persist) {
    for (const auto& connector : changed_connectors) {
        if (persist) {
            try {
                this->database_handler->insert_or_update_connector_availability(connector, availability);
            } catch (const QueryExecutionException& e) {
                EVLOG_warning << "Could not update availability of connector << " << connector
                              << " in the database: " << e.what();
            }
        }
        if (availability == AvailabilityType::Operative) {
            if (connector == 0) {
                this->status->submit_event(connector, FSMEvent::BecomeAvailable, ocpp::DateTime());
            }

            if (this->enable_evse_callback != nullptr) {
                this->enable_evse_callback(connector);
            }
        } else {
            if (connector == 0) {
                this->status->submit_event(connector, FSMEvent::ChangeAvailabilityToUnavailable, ocpp::DateTime());
            }
            if (this->disable_evse_callback != nullptr) {
                this->disable_evse_callback(connector);
            }
        }
    }
}

void ChargePointImpl::execute_queued_availability_change(const int32_t connector) {
    bool change_queued = false;
    AvailabilityType connector_availability;
    bool persist = false;
    {
        std::lock_guard<std::mutex> change_availability_lock(change_availability_mutex);
        change_queued = this->change_availability_queue.count(connector) != 0;
        connector_availability = this->change_availability_queue[connector].availability;
        persist = this->change_availability_queue[connector].persist;
        this->change_availability_queue.erase(connector);
    }

    if (change_queued) {
        EVLOG_debug << "Queued availability change of connector " << connector << " to "
                    << conversions::availability_type_to_string(connector_availability);
        execute_connectors_availability_change({connector}, connector_availability, persist);
    }
}

void ChargePointImpl::handleChangeAvailabilityRequest(ocpp::Call<ChangeAvailabilityRequest> call) {
    EVLOG_debug << "Received ChangeAvailabilityRequest: " << call.msg << "\nwith messageId: " << call.uniqueId;

    const auto& request = call.msg;

    ChangeAvailabilityResponse response{};
    std::vector<int32_t> accepted_connector_availability_changes{};
    preprocess_change_availability_request(request, response, accepted_connector_availability_changes);

    // respond first
    ocpp::CallResult<ChangeAvailabilityResponse> call_result(response, call.uniqueId);
    this->message_dispatcher->dispatch_call_result(call_result);

    // if scheduled: execute status transition for connector 0
    // if accepted: execute status transition for connector 0 and other connectors
    if (response.status != AvailabilityStatus::Rejected) {
        execute_connectors_availability_change(accepted_connector_availability_changes, request.type, true);
    }
}

void ChargePointImpl::handleChangeConfigurationRequest(ocpp::Call<ChangeConfigurationRequest> call) {
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
                } else if (call.msg.key == "MeterValueSampleInterval") {
                    this->update_meter_values_sample_interval();
                } else if (call.msg.key == "ClockAlignedDataInterval") {
                    this->update_clock_aligned_meter_values_interval();
                } else if (call.msg.key == "AuthorizationKey") {
                    EVLOG_info << "AuthorizationKey was changed by central system";
                    this->websocket->set_authorization_key(this->configuration->getAuthorizationKey().value());
                    if (this->configuration->getSecurityProfile() == 0) {
                        EVLOG_info << "AuthorizationKey was changed while on security profile 0.";
                    } else if (this->configuration->getSecurityProfile() == 1 ||
                               this->configuration->getSecurityProfile() == 2) {
                        EVLOG_info
                            << "AuthorizationKey was changed while on security profile 1 or 2. Reconnect Websocket.";
                        ocpp::CallResult<ChangeConfigurationResponse> call_result(response, call.uniqueId);
                        this->message_dispatcher->dispatch_call_result(call_result);
                        responded = true;
                        this->websocket->reconnect(1000);
                    } else {
                        EVLOG_info << "AuthorizationKey was changed while on security profile 3. Nothing to do.";
                    }
                } else if (call.msg.key == "SecurityProfile") {
                    try {
                        const auto security_profile = std::stoi(call.msg.value.get());
                        const auto current_security_profile = this->configuration->getSecurityProfile();
                        if (security_profile <= current_security_profile) {
                            EVLOG_warning << "New security profile is <= current security profile. Rejecting request.";
                            response.status = ConfigurationStatus::Rejected;
                        } else if ((security_profile == 1 || security_profile == 2) &&
                                   this->configuration->getAuthorizationKey() == std::nullopt) {
                            EVLOG_warning << "New security level set to 1 or 2 but no authorization key is set. "
                                             "Rejecting request.";
                            response.status = ConfigurationStatus::Rejected;
                        } else if ((security_profile == 2 || security_profile == 3) &&
                                   !this->evse_security->is_ca_certificate_installed(ocpp::CaCertificateType::CSMS)) {
                            EVLOG_warning
                                << "New security level set to 2 or 3 but no CentralSystemRootCertificateInstalled";
                            response.status = ConfigurationStatus::Rejected;
                        } else if (security_profile == 3 &&
                                   this->evse_security
                                           ->get_leaf_certificate_info(
                                               ocpp::CertificateSigningUseEnum::ChargingStationCertificate)
                                           .status != ocpp::GetCertificateInfoStatus::Accepted) {
                            EVLOG_warning << "New security level set to 3 but no Client Certificate is installed";
                            response.status = ConfigurationStatus::Rejected;
                        } else if (security_profile > 3) {
                            response.status = ConfigurationStatus::Rejected;
                        } else {
                            // valid set of security profile
                            ocpp::CallResult<ChangeConfigurationResponse> call_result(response, call.uniqueId);
                            this->message_dispatcher->dispatch_call_result(call_result);
                            int32_t security_profile = std::stoi(call.msg.value);
                            responded = true;
                            this->switch_security_profile_callback = [this, security_profile]() {
                                this->switchSecurityProfile(security_profile, 1);
                            };
                            // disconnected_callback will trigger security_profile_callback when it is set
                            this->websocket->disconnect(WebsocketCloseReason::Normal);
                        }
                    } catch (const std::invalid_argument& e) {
                        response.status = ConfigurationStatus::Rejected;
                    }
                } else if (call.msg.key == "ConnectionTimeout") {
                    this->call_set_connection_timeout();
                } else if (call.msg.key == "TransactionMessageAttempts") {
                    this->message_queue->update_transaction_message_attempts(
                        this->configuration->getTransactionMessageAttempts());
                } else if (call.msg.key == "TransactionMessageRetryInterval") {
                    this->message_queue->update_transaction_message_retry_interval(
                        this->configuration->getTransactionMessageRetryInterval());
                } else if (call.msg.key == "WebsocketPingInterval") {
                    auto websocket_ping_interval_option = this->configuration->getWebsocketPingInterval();
                    if (websocket_ping_interval_option.has_value()) {
                        auto websocket_ping_interval = websocket_ping_interval_option.value();
                        this->websocket->set_websocket_ping_interval(websocket_ping_interval);
                    }
                } else if (call.msg.key == "ISO15118PnCEnabled") {
                    if (ocpp::conversions::string_to_bool(call.msg.value.get())) {
                        this->ocsp_request_timer->stop();
                        this->ocsp_request_timer->timeout(INITIAL_CERTIFICATE_REQUESTS_DELAY);
                        this->v2g_certificate_timer->stop();
                        this->v2g_certificate_timer->timeout(INITIAL_CERTIFICATE_REQUESTS_DELAY);
                    } else {
                        this->ocsp_request_timer->stop();
                        this->v2g_certificate_timer->stop();
                    }
                } else if (call.msg.key == "OcspRequestInterval") {
                    if (this->is_pnc_enabled()) {
                        this->ocsp_request_timer->stop();
                        this->ocsp_request_timer->interval(
                            std::chrono::seconds(this->configuration->getOcspRequestInterval()));
                    }
                } else if (call.msg.key == "NextTimeOffsetTransitionDateTime") {
                    if (this->configuration->getNextTimeOffsetTransitionDateTime().has_value()) {
                        set_time_offset_timer(this->configuration->getNextTimeOffsetTransitionDateTime().value());
                    }
                }
            }
        }
    } else {
        response.status = ConfigurationStatus::NotSupported;
    }

    if (!responded) {
        ocpp::CallResult<ChangeConfigurationResponse> call_result(response, call.uniqueId);
        this->message_dispatcher->dispatch_call_result(call_result);
    }

    if (this->configuration_key_changed_callbacks.count(call.msg.key) and
        this->configuration_key_changed_callbacks[call.msg.key] != nullptr and
        response.status == ConfigurationStatus::Accepted) {
        kv.value().value = call.msg.value;
        this->configuration_key_changed_callbacks[call.msg.key](kv.value());
    } else if (this->generic_configuration_key_changed_callback != nullptr and
               response.status == ConfigurationStatus::Accepted) {
        kv.value().value = call.msg.value;
        this->generic_configuration_key_changed_callback(kv.value());
    }
}

void ChargePointImpl::switchSecurityProfile(int32_t new_security_profile, int32_t max_connection_attempts) {
    EVLOG_info << "Switching security profile from " << this->configuration->getSecurityProfile() << " to "
               << new_security_profile;
    const auto old_security_profile = this->configuration->getSecurityProfile();
    this->configuration->setSecurityProfile(new_security_profile);

    this->switch_security_profile_callback = [this, old_security_profile]() {
        EVLOG_warning << "Switching security profile back to fallback because new profile couldnt connect";
        this->switchSecurityProfile(old_security_profile, -1);
    };

    // we need to reinitialize because it could be plain or tls websocket
    this->websocket_timer.timeout(
        [this, max_connection_attempts, new_security_profile]() {
            auto connection_options = this->get_ws_connection_options();
            connection_options.security_profile = new_security_profile;
            connection_options.max_connection_attempts = max_connection_attempts;
            this->websocket->set_connection_options(connection_options);
            this->websocket->start_connecting();
        },
        WEBSOCKET_INIT_DELAY);
}

void ChargePointImpl::handleClearCacheRequest(ocpp::Call<ClearCacheRequest> call) {
    EVLOG_debug << "Received ClearCacheRequest: " << call.msg << "\nwith messageId: " << call.uniqueId;

    ClearCacheResponse response;

    if (this->configuration->getAuthorizationCacheEnabled()) {
        try {
            this->database_handler->clear_authorization_cache();
            response.status = ClearCacheStatus::Accepted;
        } catch (QueryExecutionException& e) {
            auto call_error = CallError(call.uniqueId, "InternalError",
                                        "Database error while clearing authorization cache", json({}, true));
            this->message_dispatcher->dispatch_call_error(call_error);
            return;
        }
    } else {
        response.status = ClearCacheStatus::Rejected;
    }

    ocpp::CallResult<ClearCacheResponse> call_result(response, call.uniqueId);
    this->message_dispatcher->dispatch_call_result(call_result);
}

void ChargePointImpl::handleDataTransferRequest(ocpp::Call<DataTransferRequest> call) {
    EVLOG_debug << "Received DataTransferRequest: " << call.msg << "\nwith messageId: " << call.uniqueId;

    DataTransferResponse response;

    auto vendorId = call.msg.vendorId.get();
    auto messageId = call.msg.messageId.value_or(CiString<50>()).get();

    // first try the callbacks that are explicitly registered for a vendorId or messageId
    {
        std::lock_guard<std::mutex> lock(data_transfer_callbacks_mutex);
        if (vendorId == ISO15118_PNC_VENDOR_ID and !this->is_pnc_enabled()) {
            response.status = DataTransferStatus::UnknownVendorId;
        } else if (vendorId == ISO15118_PNC_VENDOR_ID and this->is_pnc_enabled()) {
            if (this->data_transfer_pnc_callbacks.count(messageId)) {
                // there is a ISO15118 PnC callback registered for this vendorId and messageId
                const auto callback = this->data_transfer_pnc_callbacks[messageId];
                callback(call); // DataTransfer PnC callback is responsible to send DataTransfer.conf
                return;
            } else {
                EVLOG_warning
                    << "Received DataTransfer.req for ISO15118 PnC while PnC is enabled but no handler found for : "
                    << messageId;
                response.status = DataTransferStatus::UnknownMessageId;
            }
        } else if (this->data_transfer_callbacks.count(vendorId) == 0) {
            response.status = DataTransferStatus::UnknownVendorId;
        } else if (this->data_transfer_callbacks.count(vendorId) and
                   this->data_transfer_callbacks[vendorId].count(messageId) == 0) {
            response.status = DataTransferStatus::UnknownMessageId;
        } else {
            // there is a callback registered for this vendorId and messageId
            response = this->data_transfer_callbacks[vendorId][messageId](call.msg.data);
        }
    }

    // only try the general data_transfer_callback if a explicity registered callback was not found
    if (response.status == DataTransferStatus::UnknownVendorId or
        response.status == DataTransferStatus::UnknownMessageId) {
        if (this->data_transfer_callback != nullptr) {
            response = this->data_transfer_callback(call.msg);
        }
    }

    ocpp::CallResult<DataTransferResponse> call_result(response, call.uniqueId);
    this->message_dispatcher->dispatch_call_result(call_result);
}

void ChargePointImpl::handleGetConfigurationRequest(ocpp::Call<GetConfigurationRequest> call) {
    EVLOG_debug << "Received GetConfigurationRequest: " << call.msg << "\nwith messageId: " << call.uniqueId;

    const auto response = this->get_configuration_key(call.msg);
    ocpp::CallResult<GetConfigurationResponse> call_result(response, call.uniqueId);
    this->message_dispatcher->dispatch_call_result(call_result);
}

void ChargePointImpl::handleRemoteStartTransactionRequest(ocpp::Call<RemoteStartTransactionRequest> call) {
    EVLOG_debug << "Received RemoteStartTransactionRequest: " << call.msg << "\nwith messageId: " << call.uniqueId;

    // a charge point may reject a remote start transaction request without a connectorId
    // TODO(kai): what is our policy here? reject for now
    RemoteStartTransactionResponse response;
    std::vector<int32_t> referenced_connectors;

    if (call.msg.connectorId) {
        if (call.msg.connectorId.value() <= 0 or
            call.msg.connectorId.value() > this->configuration->getNumberOfConnectors()) {
            EVLOG_warning << "Received RemoteStartTransactionRequest with connector id <= 0 or > "
                          << this->configuration->getNumberOfConnectors();
            response.status = RemoteStartStopStatus::Rejected;
            ocpp::CallResult<RemoteStartTransactionResponse> call_result(response, call.uniqueId);
            this->message_dispatcher->dispatch_call_result(call_result);
            return;
        }
        referenced_connectors.push_back(call.msg.connectorId.value());
    } else {
        for (int connector = 1; connector <= this->configuration->getNumberOfConnectors(); connector++) {
            referenced_connectors.push_back(connector);
        }
    }

    // Check if at least one conenctor is able to execute RemoteStart (obtainable == true).
    bool obtainable = true;
    for (const auto connector : referenced_connectors) {
        obtainable = true;

        if (this->status->get_state(connector) == ChargePointStatus::Unavailable or
            this->status->get_state(connector) == ChargePointStatus::Faulted) {
            obtainable = false;
            continue;
        }

        if (this->transaction_handler->get_transaction(connector) != nullptr ||
            this->status->get_state(connector) == ChargePointStatus::Finishing) {
            obtainable = false;
            continue;
        }

        if (this->is_token_reserved_for_connector_callback != nullptr) {
            const ocpp::ReservationCheckStatus reservation_status =
                is_token_reserved_for_connector_callback(connector, call.msg.idTag.get());

            const bool is_reserved = (reservation_status == ocpp::ReservationCheckStatus::ReservedForOtherToken);

            if (this->status->get_state(connector) == ChargePointStatus::Reserved && is_reserved) {
                obtainable = false;
                continue;
            }
        }

        if (obtainable) {
            // at least one connector can do the remote start
            break;
        }
    }

    if (!obtainable) {
        EVLOG_debug << "Received RemoteStartTransactionRequest for reserved connector and rejected";
        response.status = RemoteStartStopStatus::Rejected;
        ocpp::CallResult<RemoteStartTransactionResponse> call_result(response, call.uniqueId);
        this->message_dispatcher->dispatch_call_result(call_result);
        return;
    }

    if (call.msg.chargingProfile) {
        // TODO(kai): A charging profile was provided, forward to the charger
        if (call.msg.connectorId &&
            call.msg.chargingProfile.value().chargingProfilePurpose == ChargingProfilePurposeType::TxProfile &&
            this->smart_charging_handler->validate_profile(
                call.msg.chargingProfile.value(), call.msg.connectorId.value(), true,
                this->configuration->getChargeProfileMaxStackLevel(),
                this->configuration->getMaxChargingProfilesInstalled(),
                this->configuration->getChargingScheduleMaxPeriods(),
                this->configuration->getChargingScheduleAllowedChargingRateUnitVector())) {
            this->smart_charging_handler->add_tx_profile(call.msg.chargingProfile.value(),
                                                         call.msg.connectorId.value());
        } else {
            response.status = RemoteStartStopStatus::Rejected;
            ocpp::CallResult<RemoteStartTransactionResponse> call_result(response, call.uniqueId);
            this->message_dispatcher->dispatch_call_result(call_result);
            return;
        }
    }

    {
        std::vector<int32_t> referenced_connectors;

        if (!call.msg.connectorId) {
            for (int connector = 1; connector <= this->configuration->getNumberOfConnectors(); connector++) {
                referenced_connectors.push_back(connector);
            }
        } else {
            referenced_connectors.push_back(call.msg.connectorId.value());
        }

        response.status = RemoteStartStopStatus::Accepted;
        ocpp::CallResult<RemoteStartTransactionResponse> call_result(response, call.uniqueId);
        this->message_dispatcher->dispatch_call_result(call_result);

        if (this->configuration->getAuthorizeRemoteTxRequests()) {
            this->provide_token_callback(call.msg.idTag.get(), referenced_connectors, false);
        } else {
            this->provide_token_callback(call.msg.idTag.get(), referenced_connectors, true); // prevalidated
        }
    }
}

bool ChargePointImpl::validate_against_cache_entries(CiString<20> id_tag) {
    const auto cache_entry_opt = this->database_handler->get_authorization_cache_entry(id_tag);
    if (cache_entry_opt.has_value()) {
        auto cache_entry = cache_entry_opt.value();
        const auto expiry_date_opt = cache_entry.expiryDate;

        if (cache_entry.status == AuthorizationStatus::Accepted) {
            if (expiry_date_opt.has_value()) {
                const auto expiry_date = expiry_date_opt.value();
                if (expiry_date < ocpp::DateTime()) {
                    cache_entry.status = AuthorizationStatus::Expired;
                    try {
                        this->database_handler->insert_or_update_authorization_cache_entry(id_tag, cache_entry);
                    } catch (const QueryExecutionException& e) {
                        EVLOG_warning << "Could not insert or update authorization cache entry: " << e.what();
                    }
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

void ChargePointImpl::handleRemoteStopTransactionRequest(ocpp::Call<RemoteStopTransactionRequest> call) {
    EVLOG_debug << "Received RemoteStopTransactionRequest: " << call.msg << "\nwith messageId: " << call.uniqueId;

    RemoteStopTransactionResponse response;
    response.status = RemoteStartStopStatus::Rejected;

    auto connector = this->transaction_handler->get_connector_from_transaction_id(call.msg.transactionId);
    if (connector > 0) {
        response.status = RemoteStartStopStatus::Accepted;
    }

    ocpp::CallResult<RemoteStopTransactionResponse> call_result(response, call.uniqueId);
    this->message_dispatcher->dispatch_call_result(call_result);

    if (connector > 0) {
        this->stop_transaction_callback(connector, Reason::Remote);
    }
}

void ChargePointImpl::handleResetRequest(ocpp::Call<ResetRequest> call) {
    EVLOG_debug << "Received ResetRequest: " << call.msg << "\nwith messageId: " << call.uniqueId;

    const auto reset_type = call.msg.type;
    ResetResponse response;

    if (this->is_reset_allowed_callback == nullptr || this->reset_callback == nullptr ||
        !this->is_reset_allowed_callback(reset_type)) {
        response.status = ResetStatus::Rejected;
    } else {
        // reset is allowed
        response.status = ResetStatus::Accepted;
    }

    // send response
    ocpp::CallResult<ResetResponse> call_result(response, call.uniqueId);
    this->message_dispatcher->dispatch_call_result(call_result);

    if (response.status == ResetStatus::Accepted) {
        // gracefully stop all transactions and send StopTransaction. Restart software afterwards
        this->reset_thread = std::thread([this, reset_type]() {
            EVLOG_debug << "Waiting until all transactions are stopped...";
            std::unique_lock lk(this->stop_transaction_mutex);
            this->stop_transaction_cv.wait_for(
                lk, std::chrono::seconds(this->configuration->getWaitForStopTransactionsOnResetTimeout()), [this] {
                    for (int32_t connector = 1; connector <= this->configuration->getNumberOfConnectors();
                         connector++) {
                        if (this->transaction_handler->transaction_active(connector) or
                            this->status->get_state(connector) == ChargePointStatus::Charging) {
                            return false;
                        }
                    }
                    return true;
                });
            // this is executed after all transactions have been stopped
            // it is expected that the user properly shuts down the software, including calling stop()
            this->reset_callback(reset_type);
        });
        if (call.msg.type == ResetType::Soft) {
            this->stop_all_transactions(Reason::SoftReset);
        } else {
            this->stop_all_transactions(Reason::HardReset);
        }
    }
}

void ChargePointImpl::handleStartTransactionResponse(ocpp::CallResult<StartTransactionResponse> call_result) {

    StartTransactionResponse start_transaction_response = call_result.msg;

    const auto transaction = this->transaction_handler->get_transaction(call_result.uniqueId);

    if (transaction != nullptr) {
        // this can happen when a chargepoint was offline during transaction and StopTransaction.req is already queued
        if (transaction->is_finished()) {
            this->message_queue->add_stopped_transaction_id(transaction->get_stop_transaction_message_id(),
                                                            start_transaction_response.transactionId);
        }
        this->message_queue->notify_start_transaction_handled(call_result.uniqueId.get(),
                                                              start_transaction_response.transactionId);
        int32_t connector = transaction->get_connector();
        transaction->set_transaction_id(start_transaction_response.transactionId);
        auto idTag = transaction->get_id_tag();

        try {
            this->database_handler->update_transaction(transaction->get_session_id(),
                                                       start_transaction_response.transactionId,
                                                       call_result.msg.idTagInfo.parentIdTag);
        } catch (const QueryExecutionException& e) {
            EVLOG_warning << "Could not update transaction with session_id " << transaction->get_session_id()
                          << " in the database: " << e.what();
        }

        try {
            this->database_handler->insert_or_update_authorization_cache_entry(idTag,
                                                                               start_transaction_response.idTagInfo);
        } catch (const QueryExecutionException& e) {
            EVLOG_warning << "Could not insert or update authorization cache entry: " << e.what();
        }

        if (start_transaction_response.idTagInfo.status != AuthorizationStatus::Accepted) {
            this->pause_charging_callback(connector);
            if (this->configuration->getStopTransactionOnInvalidId()) {
                this->stop_transaction_callback(connector, Reason::DeAuthorized);
            }
        }

        if (this->transaction_updated_callback != nullptr) {
            this->transaction_updated_callback(connector, transaction->get_session_id(),
                                               start_transaction_response.transactionId,
                                               start_transaction_response.idTagInfo);
        }
    } else {
        EVLOG_warning << "Received StartTransaction.conf for transaction that is not known to transaction_handler";
    }
}

void ChargePointImpl::handleStopTransactionResponse(const EnhancedMessage<v16::MessageType>& message) {

    CallResult<StopTransactionResponse> call_result = message.message;
    const Call<StopTransactionRequest>& original_call = message.call_message;

    StopTransactionResponse stop_transaction_response = call_result.msg;
    const auto transaction = this->transaction_handler->get_transaction(call_result.uniqueId);

    if (transaction != nullptr) {
        int32_t connector = transaction->get_connector();

        if (stop_transaction_response.idTagInfo) {
            auto id_tag = this->transaction_handler->get_authorized_id_tag(call_result.uniqueId.get());
            if (id_tag.has_value()) {
                try {
                    this->database_handler->insert_or_update_authorization_cache_entry(
                        id_tag.value(), stop_transaction_response.idTagInfo.value());
                } catch (const QueryExecutionException& e) {
                    EVLOG_warning << "Could not insert or update authorization cache entry";
                }
            }
        }

        // perform a queued connector availability change
        this->execute_queued_availability_change(connector);
    } else {
        EVLOG_warning << "Received StopTransaction.conf for transaction that is not known to transaction_handler";
    }

    try {
        this->database_handler->update_transaction_csms_ack(original_call.msg.transactionId);
    } catch (const QueryExecutionException& e) {
        EVLOG_warning << "Could not update CSMS_ACK of transaction with transactionId "
                      << original_call.msg.transactionId << " in the database: " << e.what();
    }

    this->transaction_handler->erase_stopped_transaction(call_result.uniqueId.get());
    // when this transaction was stopped because of a Reset.req this signals that StopTransaction.conf has been received
    this->stop_transaction_cv.notify_one();

    if (this->firmware_update_is_pending) {
        this->change_all_connectors_to_unavailable_for_firmware_update();
    }
}

void ChargePointImpl::handleUnlockConnectorRequest(ocpp::Call<UnlockConnectorRequest> call) {
    EVLOG_debug << "Received UnlockConnectorRequest: " << call.msg << "\nwith messageId: " << call.uniqueId;
    std::lock_guard<std::mutex> lock(this->stop_transaction_mutex);

    UnlockConnectorResponse response;
    auto connector = call.msg.connectorId;
    if (connector <= 0 or connector > this->configuration->getNumberOfConnectors()) {
        response.status = UnlockStatus::NotSupported;
    } else {

        if (this->unlock_connector_callback != nullptr) {
            response.status = this->unlock_connector_callback(call.msg.connectorId);
        } else {
            response.status = UnlockStatus::NotSupported;
        }
    }

    ocpp::CallResult<UnlockConnectorResponse> call_result(response, call.uniqueId);
    this->message_dispatcher->dispatch_call_result(call_result);

    if (response.status == UnlockStatus::NotSupported and this->transaction_handler->transaction_active(connector) and
        this->configuration->getStopTransactionIfUnlockNotSupported()) {
        this->stop_transaction_callback(connector, Reason::UnlockCommand);
    }
}

void ChargePointImpl::handleHeartbeatResponse(CallResult<HeartbeatResponse> call_result) {
    if (this->set_system_time_callback != nullptr) {
        this->set_system_time_callback(call_result.msg.currentTime.to_rfc3339());
    }
}

void ChargePointImpl::handleSetChargingProfileRequest(ocpp::Call<SetChargingProfileRequest> call) {
    EVLOG_debug << "Received SetChargingProfileRequest: " << call.msg << "\nwith messageId: " << call.uniqueId;

    // FIXME(kai): after a new profile has been installed we must notify interested parties (energy manager?)

    SetChargingProfileResponse response;
    response.status = ChargingProfileStatus::Rejected;

    auto profile = call.msg.csChargingProfiles;
    const int connector_id = call.msg.connectorId;

    const auto supported_purpose_types = this->configuration->getSupportedChargingProfilePurposeTypes();
    if (std::find(supported_purpose_types.begin(), supported_purpose_types.end(),
                  call.msg.csChargingProfiles.chargingProfilePurpose) == supported_purpose_types.end()) {
        EVLOG_warning << "Rejecting SetChargingProfileRequest because purpose type is not supported: "
                      << call.msg.csChargingProfiles.chargingProfilePurpose;
        response.status = ChargingProfileStatus::Rejected;
    } else if (this->smart_charging_handler->validate_profile(
                   profile, connector_id, false, this->configuration->getChargeProfileMaxStackLevel(),
                   this->configuration->getMaxChargingProfilesInstalled(),
                   this->configuration->getChargingScheduleMaxPeriods(),
                   this->configuration->getChargingScheduleAllowedChargingRateUnitVector())) {
        response.status = ChargingProfileStatus::Accepted;
        // If a charging profile with the same chargingProfileId, or the same combination of stackLevel /
        // ChargingProfilePurpose, exists on the Charge Point, the new charging profile SHALL replace the
        // existing charging profile, otherwise it SHALL be added.
        this->smart_charging_handler->clear_all_profiles_with_filter(profile.chargingProfileId, std::nullopt,
                                                                     std::nullopt, std::nullopt, true);
        this->smart_charging_handler->clear_all_profiles_with_filter(std::nullopt, connector_id, profile.stackLevel,
                                                                     profile.chargingProfilePurpose, false);
        if (profile.chargingProfilePurpose == ChargingProfilePurposeType::ChargePointMaxProfile) {
            this->smart_charging_handler->add_charge_point_max_profile(profile);
        } else if (profile.chargingProfilePurpose == ChargingProfilePurposeType::TxDefaultProfile) {
            this->smart_charging_handler->add_tx_default_profile(profile, connector_id);
        } else if (profile.chargingProfilePurpose == ChargingProfilePurposeType::TxProfile) {
            this->smart_charging_handler->add_tx_profile(profile, connector_id);
        }
        response.status = ChargingProfileStatus::Accepted;
    } else {
        response.status = ChargingProfileStatus::Rejected;
    }

    ocpp::CallResult<SetChargingProfileResponse> call_result(response, call.uniqueId);
    this->message_dispatcher->dispatch_call_result(call_result);

    if (response.status == ChargingProfileStatus::Accepted) {
        if (this->signal_set_charging_profiles_callback != nullptr) {
            this->signal_set_charging_profiles_callback();
        } else {
            EVLOG_warning << "No callback registered for signaling that Charging Profiles have been set. Profiles have "
                             "been accepted but will not be applied";
        }
    }
}

void ChargePointImpl::handleGetCompositeScheduleRequest(ocpp::Call<GetCompositeScheduleRequest> call) {
    EVLOG_debug << "Received GetCompositeScheduleRequest: " << call.msg << "\nwith messageId: " << call.uniqueId;

    GetCompositeScheduleResponse response;

    const auto connector_id = call.msg.connectorId;
    const auto allowed_charging_rate_units = this->configuration->getChargingScheduleAllowedChargingRateUnitVector();
    const auto is_offline = this->websocket == nullptr or not this->websocket->is_connected();

    if (connector_id > this->configuration->getNumberOfConnectors() or connector_id < 0) {
        response.status = GetCompositeScheduleStatus::Rejected;
    } else if (call.msg.chargingRateUnit and
               std::find(allowed_charging_rate_units.begin(), allowed_charging_rate_units.end(),
                         call.msg.chargingRateUnit.value()) == allowed_charging_rate_units.end()) {
        EVLOG_warning << "GetCompositeScheduleRequest: ChargingRateUnit not allowed";
        response.status = GetCompositeScheduleStatus::Rejected;
    } else {
        const auto start_time = ocpp::DateTime(std::chrono::floor<std::chrono::seconds>(date::utc_clock::now()));
        if (call.msg.duration > this->configuration->getMaxCompositeScheduleDuration()) {
            EVLOG_warning << "GetCompositeScheduleRequest: Requested duration of " << call.msg.duration << "s"
                          << " is bigger than configured maximum value of "
                          << this->configuration->getMaxCompositeScheduleDuration() << "s";
        }
        const auto duration = std::min(this->configuration->getMaxCompositeScheduleDuration(), call.msg.duration);
        const auto end_time = ocpp::DateTime(start_time.to_time_point() + std::chrono::seconds(duration));

        const auto composite_schedule = this->smart_charging_handler->calculate_composite_schedule(
            start_time, end_time, connector_id, call.msg.chargingRateUnit.value_or(allowed_charging_rate_units.at(0)),
            is_offline, true);
        response.status = GetCompositeScheduleStatus::Accepted;
        response.connectorId = connector_id;
        response.scheduleStart = start_time;
        response.chargingSchedule = composite_schedule;
    }

    ocpp::CallResult<GetCompositeScheduleResponse> call_result(response, call.uniqueId);
    this->message_dispatcher->dispatch_call_result(call_result);
}

void ChargePointImpl::handleClearChargingProfileRequest(ocpp::Call<ClearChargingProfileRequest> call) {
    EVLOG_debug << "Received ClearChargingProfileRequest: " << call.msg << "\nwith messageId: " << call.uniqueId;

    ClearChargingProfileResponse response;
    response.status = ClearChargingProfileStatus::Unknown;

    // clear all charging profiles
    if (!call.msg.id && !call.msg.connectorId && !call.msg.chargingProfilePurpose && !call.msg.stackLevel) {
        this->smart_charging_handler->clear_all_profiles();
        response.status = ClearChargingProfileStatus::Accepted;
    } else if (call.msg.id &&
               this->smart_charging_handler->clear_all_profiles_with_filter(
                   call.msg.id, call.msg.connectorId, call.msg.stackLevel, call.msg.chargingProfilePurpose, true)) {
        response.status = ClearChargingProfileStatus::Accepted;
    } else if (!call.msg.id and
               this->smart_charging_handler->clear_all_profiles_with_filter(
                   std::nullopt, call.msg.connectorId, call.msg.stackLevel, call.msg.chargingProfilePurpose, false)) {
        response.status = ClearChargingProfileStatus::Accepted;
    }

    ocpp::CallResult<ClearChargingProfileResponse> call_result(response, call.uniqueId);
    this->message_dispatcher->dispatch_call_result(call_result);

    if (response.status == ClearChargingProfileStatus::Accepted and
        this->signal_set_charging_profiles_callback != nullptr) {
        this->signal_set_charging_profiles_callback();
    }
}

void ChargePointImpl::handleTriggerMessageRequest(ocpp::Call<TriggerMessageRequest> call) {
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
    bool trigger_message = true;
    if (connector < 0 || connector > this->configuration->getNumberOfConnectors()) {
        response.status = TriggerMessageStatus::Rejected;
        trigger_message = false;
    }

    if (trigger_message and call.msg.requestedMessage == MessageTrigger::MeterValues) {
        if (call.msg.connectorId.has_value()) {
            trigger_message = this->connectors.at(call.msg.connectorId.value())->measurement.has_value();
        } else {
            trigger_message = std::any_of(this->connectors.begin(), this->connectors.end(), [](const auto& connector) {
                return connector.second->measurement.has_value();
            });
        }
        if (not trigger_message) {
            response.status = TriggerMessageStatus::Rejected;
        }
    }

    ocpp::CallResult<TriggerMessageResponse> call_result(response, call.uniqueId);
    this->message_dispatcher->dispatch_call_result(call_result);

    if (not trigger_message) {
        return;
    }

    switch (call.msg.requestedMessage) {
    case MessageTrigger::BootNotification:
        this->boot_notification(true);
        break;
    case MessageTrigger::DiagnosticsStatusNotification:
        this->diagnostic_status_notification(this->diagnostics_status, true);
        break;
    case MessageTrigger::FirmwareStatusNotification:
        this->firmware_status_notification(this->firmware_status, true);
        break;
    case MessageTrigger::Heartbeat:
        this->heartbeat(true);
        break;
    case MessageTrigger::MeterValues: {
        const auto send_meter_value_func = [this](const int32_t connector_id) {
            auto meter_value = this->get_latest_meter_value(
                connector_id, this->configuration->getMeterValuesSampledDataVector(), ReadingContext::Trigger);
            if (meter_value.has_value()) {
                this->send_meter_value(connector_id, meter_value.value(), true);
            } else {
                EVLOG_warning << "Could not send triggered meter value for uninitialized measurement at connector#"
                              << connector_id;
            }
        };

        if (!call.msg.connectorId.has_value()) {
            // send a MeterValue.req for every connector
            for (int32_t c = 0; c <= this->configuration->getNumberOfConnectors(); c++) {
                send_meter_value_func(c);
            }
        } else {
            send_meter_value_func(call.msg.connectorId.value());
        }
        break;
    }
    case MessageTrigger::StatusNotification:
        if (!call.msg.connectorId.has_value()) {
            // send a status notification for every connector
            for (int32_t c = 0; c <= this->configuration->getNumberOfConnectors(); c++) {
                ErrorInfo error_info =
                    this->status->get_latest_error(c).value_or(ErrorInfo("", ChargePointErrorCode::NoError, false));
                this->status_notification(c, error_info.error_code, this->status->get_state(c), ocpp::DateTime(),
                                          error_info.info, error_info.vendor_id, error_info.vendor_error_code, true);
            }
        } else {
            ErrorInfo error_info =
                this->status->get_latest_error(connector).value_or(ErrorInfo("", ChargePointErrorCode::NoError, false));
            this->status_notification(connector, error_info.error_code, this->status->get_state(connector),
                                      ocpp::DateTime(), error_info.info, error_info.vendor_id,
                                      error_info.vendor_error_code, true);
        }
        break;
    }
}

void ChargePointImpl::handleGetDiagnosticsRequest(ocpp::Call<GetDiagnosticsRequest> call) {
    EVLOG_debug << "Received GetDiagnosticsRequest: " << call.msg << "\nwith messageId: " << call.uniqueId;
    GetDiagnosticsResponse response;
    if (this->upload_diagnostics_callback) {
        const auto get_log_response = this->upload_diagnostics_callback(call.msg);
        if (get_log_response.filename.has_value()) {
            response.fileName = get_log_response.filename.value();
        }
    }
    ocpp::CallResult<GetDiagnosticsResponse> call_result(response, call.uniqueId);
    this->message_dispatcher->dispatch_call_result(call_result);
}

void ChargePointImpl::handleUpdateFirmwareRequest(ocpp::Call<UpdateFirmwareRequest> call) {
    EVLOG_debug << "Received UpdateFirmwareRequest: " << call.msg << "\nwith messageId: " << call.uniqueId;
    UpdateFirmwareResponse response;
    if (this->update_firmware_callback) {
        this->update_firmware_callback(call.msg);
    }
    ocpp::CallResult<UpdateFirmwareResponse> call_result(response, call.uniqueId);
    this->message_dispatcher->dispatch_call_result(call_result);
}

void ChargePointImpl::handleExtendedTriggerMessageRequest(ocpp::Call<ExtendedTriggerMessageRequest> call) {
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
        if (this->configuration->getCpoName() != std::nullopt) {
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

    ocpp::CallResult<ExtendedTriggerMessageResponse> call_result(response, call.uniqueId);
    this->message_dispatcher->dispatch_call_result(call_result);

    if (!valid) {
        return;
    }

    switch (call.msg.requestedMessage) {
    case MessageTriggerEnumType::BootNotification:
        this->boot_notification(true);
        break;
    case MessageTriggerEnumType::FirmwareStatusNotification:
        this->signed_firmware_update_status_notification(this->signed_firmware_status,
                                                         this->signed_firmware_status_request_id, true);
        break;
    case MessageTriggerEnumType::Heartbeat:
        this->heartbeat(true);
        break;
    case MessageTriggerEnumType::LogStatusNotification:
        this->log_status_notification(this->log_status, this->log_status_request_id, true);
        break;
    case MessageTriggerEnumType::MeterValues: {
        const auto meter_value = this->get_latest_meter_value(
            connector, this->configuration->getMeterValuesSampledDataVector(), ReadingContext::Trigger);
        if (meter_value.has_value()) {
            this->send_meter_value(connector, meter_value.value(), true);
        } else {
            EVLOG_warning << "Could not send triggered meter value for uninitialized measurement at connector#"
                          << connector;
        }
        break;
    }
    case MessageTriggerEnumType::SignChargePointCertificate:
        this->sign_certificate(ocpp::CertificateSigningUseEnum::ChargingStationCertificate, true);
        break;
    case MessageTriggerEnumType::StatusNotification:
        if (!call.msg.connectorId.has_value()) {
            // send a status notification for every connector
            for (int32_t c = 0; c <= this->configuration->getNumberOfConnectors(); c++) {
                ErrorInfo error_info =
                    this->status->get_latest_error(c).value_or(ErrorInfo("", ChargePointErrorCode::NoError, false));
                this->status_notification(c, error_info.error_code, this->status->get_state(c), ocpp::DateTime(),
                                          error_info.info, error_info.vendor_id, error_info.vendor_error_code, true);
            }
        } else {
            ErrorInfo error_info =
                this->status->get_latest_error(connector).value_or(ErrorInfo("", ChargePointErrorCode::NoError, false));
            this->status_notification(connector, error_info.error_code, this->status->get_state(connector),
                                      ocpp::DateTime(), error_info.info, error_info.vendor_id,
                                      error_info.vendor_error_code, true);
        }
        break;
    }
}

void ChargePointImpl::sign_certificate(const ocpp::CertificateSigningUseEnum& certificate_signing_use,
                                       bool initiated_by_trigger_message) {

    bool use_tpm = false;

    if (certificate_signing_use == CertificateSigningUseEnum::ChargingStationCertificate) {
        use_tpm = this->configuration->getUseTPM();
    } else if (certificate_signing_use == CertificateSigningUseEnum::V2GCertificate) {
        use_tpm = this->configuration->getUseTPMSeccLeafCertificate();
    }

    EVLOG_info << "Create CSR (TPM=" << use_tpm << ")";
    SignCertificateRequest req;

    const auto response = this->evse_security->generate_certificate_signing_request(
        certificate_signing_use, this->configuration->getSeccLeafSubjectCountry().value_or("DE"),
        this->configuration->getCpoName().value(), this->configuration->getChargeBoxSerialNumber(), use_tpm);

    if (response.status != GetCertificateSignRequestStatus::Accepted || !response.csr.has_value()) {
        EVLOG_error << "Create CSR (TPM=" << use_tpm << ")"
                    << " failed for:"
                    << ocpp::conversions::certificate_signing_use_enum_to_string(certificate_signing_use);

        std::string gen_error =
            "Sign certificate failed due to:" +
            ocpp::conversions::generate_certificate_signing_request_status_to_string(response.status);
        this->securityEventNotification(ocpp::security_events::CSRGENERATIONFAILED,
                                        std::optional<CiString<255>>(gen_error), true);

        return;
    }

    req.csr = response.csr.value();

    ocpp::Call<SignCertificateRequest> call(req);
    this->message_dispatcher->dispatch_call(call, initiated_by_trigger_message);
}

void ChargePointImpl::update_ocsp_cache() {
    try {
        EVLOG_info << "Updating OCSP cache for V2G leaf certificates";
        const auto ocsp_request_data = this->evse_security->get_v2g_ocsp_request_data();
        for (const auto& ocsp_request_entry : ocsp_request_data) {
            ocpp::v2::OCSPRequestData ocsp_request = ocpp::evse_security_conversions::to_ocpp_v2(ocsp_request_entry);
            this->data_transfer_pnc_get_certificate_status(ocsp_request);
        }
    } catch (const std::exception& e) {
        EVLOG_warning << "Unknown Error while updating OCSP: " << e.what();
    }
}

void ChargePointImpl::handleCertificateSignedRequest(ocpp::Call<CertificateSignedRequest> call) {
    EVLOG_debug << "Received CertificateSignedRequest: " << call.msg << "\nwith messageId: " << call.uniqueId;

    CertificateSignedResponse response;
    response.status = CertificateSignedStatusEnumType::Rejected;

    const auto certificateChain = call.msg.certificateChain.get();

    // TODO(piet): Choose the right sign use enum!
    const auto result = this->evse_security->update_leaf_certificate(
        certificateChain, ocpp::CertificateSigningUseEnum::ChargingStationCertificate);
    if (result == ocpp::InstallCertificateResult::Accepted) {
        response.status = CertificateSignedStatusEnumType::Accepted;
    }

    ocpp::CallResult<CertificateSignedResponse> call_result(response, call.uniqueId);
    this->message_dispatcher->dispatch_call_result(call_result);

    if (response.status == CertificateSignedStatusEnumType::Rejected) {
        // This event is forced to accomodate for for TC_077_CS even if this event is not critical
        this->securityEventNotification(
            ocpp::security_events::INVALIDCHARGEPOINTCERTIFICATE,
            std::optional<CiString<255>>(ocpp::conversions::install_certificate_result_to_string(result)), true, true);
    }

    // reconnect with new certificate if valid and security profile is 3
    if (response.status == CertificateSignedStatusEnumType::Accepted &&
        this->configuration->getSecurityProfile() == 3) {
        this->websocket->reconnect(1000);
    }
}

void ChargePointImpl::handleGetInstalledCertificateIdsRequest(ocpp::Call<GetInstalledCertificateIdsRequest> call) {
    EVLOG_debug << "Received GetInstalledCertificatesRequest: " << call.msg << "\nwith messageId: " << call.uniqueId;
    GetInstalledCertificateIdsResponse response;
    response.status = GetInstalledCertificateStatusEnumType::NotFound;

    // add v16::CertificateUseEnumType to std::vector<ocpp::CaCertificateType>
    std::vector<ocpp::CertificateType> certificate_types;

    if (call.msg.certificateType == CertificateUseEnumType::CentralSystemRootCertificate) {
        certificate_types.push_back(ocpp::CertificateType::CSMSRootCertificate);
    }
    if (call.msg.certificateType == CertificateUseEnumType::ManufacturerRootCertificate) {
        certificate_types.push_back(ocpp::CertificateType::MFRootCertificate);
    }
    // this is common CertificateHashDataChain
    const auto certificate_hash_data_chains = this->evse_security->get_installed_certificates(certificate_types);
    if (!certificate_hash_data_chains.empty()) {
        // convert ocpp::CertificateHashData to v16::CertificateHashData
        std::optional<std::vector<CertificateHashDataType>> certificate_hash_data_16_vec_opt;
        std::vector<CertificateHashDataType> certificate_hash_data_16_vec;
        for (const auto certificate_hash_data_chain_entry : certificate_hash_data_chains) {
            certificate_hash_data_16_vec.push_back(
                CertificateHashDataType(json(certificate_hash_data_chain_entry.certificateHashData)));
        }
        certificate_hash_data_16_vec_opt.emplace(certificate_hash_data_16_vec);
        response.certificateHashData = certificate_hash_data_16_vec_opt;
        response.status = GetInstalledCertificateStatusEnumType::Accepted;
    }

    ocpp::CallResult<GetInstalledCertificateIdsResponse> call_result(response, call.uniqueId);
    this->message_dispatcher->dispatch_call_result(call_result);
}

void ChargePointImpl::handleDeleteCertificateRequest(ocpp::Call<DeleteCertificateRequest> call) {
    DeleteCertificateResponse response;

    // convert 1.6 CertificateHashData to common CertificateHashData
    ocpp::CertificateHashDataType certificate_hash_data(json(call.msg.certificateHashData));

    const auto result = this->evse_security->delete_certificate(certificate_hash_data);

    response.status = conversions::string_to_delete_certificate_status_enum_type(
        ocpp::conversions::delete_certificate_result_to_string(result));

    ocpp::CallResult<DeleteCertificateResponse> call_result(response, call.uniqueId);
    this->message_dispatcher->dispatch_call_result(call_result);
}

void ChargePointImpl::handleInstallCertificateRequest(ocpp::Call<InstallCertificateRequest> call) {
    InstallCertificateResponse response;
    response.status = InstallCertificateStatusEnumType::Rejected;

    ocpp::CaCertificateType ca_certificate_type;
    if (call.msg.certificateType == CertificateUseEnumType::CentralSystemRootCertificate) {
        ca_certificate_type = CaCertificateType::CSMS;
    } else {
        ca_certificate_type = CaCertificateType::MF;
    }

    const auto result = this->evse_security->install_ca_certificate(call.msg.certificate.get(), ca_certificate_type);

    if (result == ocpp::InstallCertificateResult::Accepted) {
        response.status = InstallCertificateStatusEnumType::Accepted;
    } else if (result == ocpp::InstallCertificateResult::WriteError) {
        response.status = InstallCertificateStatusEnumType::Failed;
    } else {
        response.status = InstallCertificateStatusEnumType::Rejected;
    }

    ocpp::CallResult<InstallCertificateResponse> call_result(response, call.uniqueId);
    this->message_dispatcher->dispatch_call_result(call_result);

    if (response.status == InstallCertificateStatusEnumType::Rejected) {
        this->securityEventNotification(
            ocpp::security_events::INVALIDCENTRALSYSTEMCERTIFICATE,
            std::optional<CiString<255>>(ocpp::conversions::install_certificate_result_to_string(result)), true, false);
    }
}

void ChargePointImpl::handleGetLogRequest(ocpp::Call<GetLogRequest> call) {
    GetLogResponse response;

    if (this->upload_logs_callback) {

        const auto get_log_response = this->upload_logs_callback(call.msg);
        response.status = get_log_response.status;
        response.filename = get_log_response.filename;
    }

    ocpp::CallResult<GetLogResponse> call_result(response, call.uniqueId);
    this->message_dispatcher->dispatch_call_result(call_result);
}

void ChargePointImpl::handleSignedUpdateFirmware(ocpp::Call<SignedUpdateFirmwareRequest> call) {
    EVLOG_debug << "Received SignedUpdateFirmwareRequest: " << call.msg << "\nwith messageId: " << call.uniqueId;
    SignedUpdateFirmwareResponse response;

    if (this->evse_security->verify_certificate(call.msg.firmware.signingCertificate.get(),
                                                ocpp::LeafCertificateType::MF) !=
        ocpp::CertificateValidationResult::Valid) {
        response.status = UpdateFirmwareStatusEnumType::InvalidCertificate;
        ocpp::CallResult<SignedUpdateFirmwareResponse> call_result(response, call.uniqueId);
        this->message_dispatcher->dispatch_call_result(call_result);
    } else {
        response.status = this->signed_update_firmware_callback(call.msg);
        ocpp::CallResult<SignedUpdateFirmwareResponse> call_result(response, call.uniqueId);
        this->message_dispatcher->dispatch_call_result(call_result);
    }

    if (response.status == UpdateFirmwareStatusEnumType::InvalidCertificate) {
        this->securityEventNotification(ocpp::security_events::INVALIDFIRMWARESIGNINGCERTIFICATE,
                                        std::optional<CiString<255>>("Certificate is invalid."), true);
    }
}

void ChargePointImpl::securityEventNotification(const CiString<50>& event_type,
                                                const std::optional<CiString<255>>& tech_info,
                                                const bool triggered_internally, const std::optional<bool>& critical,
                                                const std::optional<DateTime>& timestamp) {
    SecurityEventNotificationRequest req;
    req.type = event_type;
    req.techInfo = tech_info;
    if (timestamp.has_value()) {
        req.timestamp = timestamp.value();
    } else {
        req.timestamp = ocpp::DateTime();
    }

    this->logging->security(json(req).dump());

    auto critical_security_event = true;
    if (critical.has_value()) {
        critical_security_event = critical.value();
    } else {
        critical_security_event = utils::is_critical(event_type);
    }

    if (critical_security_event and !this->configuration->getDisableSecurityEventNotifications()) {
        ocpp::Call<SecurityEventNotificationRequest> call(req);
        this->message_dispatcher->dispatch_call(call);
    }

    if (triggered_internally and this->security_event_callback != nullptr) {
        this->security_event_callback(event_type.get(), tech_info.value_or(CiString<255>("")).get());
    }
}

void ChargePointImpl::log_status_notification(UploadLogStatusEnumType status, int requestId,
                                              bool initiated_by_trigger_message) {

    EVLOG_debug << "Sending log_status_notification with status: " << status << ", requestId: " << requestId;

    LogStatusNotificationRequest req;
    req.status = status;
    req.requestId = requestId;

    this->log_status = status;
    this->log_status_request_id = requestId;

    ocpp::Call<LogStatusNotificationRequest> call(req);
    this->message_dispatcher->dispatch_call(call, initiated_by_trigger_message);
    if (status != UploadLogStatusEnumType::Uploading) {
        // Reset status to idle to avoid on trigger message sending an incorrect status
        // Even if we have to retry the log upload resetting to Idle won't cause an issue since we do not trigger a
        // status notification and we don't have a state machine to block certain state transitions
        this->log_status = UploadLogStatusEnumType::Idle;
    }
}

void ChargePointImpl::signed_firmware_update_status_notification(FirmwareStatusEnumType status, int requestId,
                                                                 bool initiated_by_trigger_message) {
    EVLOG_debug << "Sending FirmwareUpdateStatusNotification with status"
                << conversions::firmware_status_enum_type_to_string(status);
    SignedFirmwareStatusNotificationRequest req;
    req.status = status;
    req.requestId = requestId;

    // The "SignatureVerified" status signals a firmware update is pending which will cause connectors to be set
    // inoperative (now or after pending transactions are stopped); in case of a status that signals a failed firmware
    // update this is revoked
    if (status == FirmwareStatusEnumType::SignatureVerified) {
        this->firmware_update_is_pending = true;
    } else if (status == FirmwareStatusEnumType::InstallationFailed ||
               status == FirmwareStatusEnumType::DownloadFailed ||
               status == FirmwareStatusEnumType::InstallVerificationFailed ||
               status == FirmwareStatusEnumType::InvalidSignature) {
        this->firmware_update_is_pending = false;
    }

    this->signed_firmware_status = status;
    this->signed_firmware_status_request_id = requestId;

    ocpp::Call<SignedFirmwareStatusNotificationRequest> call(req);
    this->message_dispatcher->dispatch_call(call, initiated_by_trigger_message);

    if (status == FirmwareStatusEnumType::InvalidSignature) {
        this->securityEventNotification(ocpp::security_events::INVALIDFIRMWARESIGNATURE, std::nullopt, true);
    }

    if (this->firmware_update_is_pending) {
        this->change_all_connectors_to_unavailable_for_firmware_update();
    }
}

void ChargePointImpl::handleReserveNowRequest(ocpp::Call<ReserveNowRequest> call) {
    ReserveNowResponse response;
    response.status = ReservationStatus::Rejected;

    if (this->status->get_state(call.msg.connectorId) == ChargePointStatus::Faulted) {
        response.status = ReservationStatus::Faulted;
    } else if (this->status->get_state(call.msg.connectorId) == ChargePointStatus::Preparing) {
        response.status = ReservationStatus::Occupied;
    } else if (this->reserve_now_callback != nullptr &&
               this->configuration->getSupportedFeatureProfiles().find("Reservation") != std::string::npos) {
        if (call.msg.connectorId != 0 || this->configuration->getReserveConnectorZeroSupported().value_or(false)) {
            response.status = this->reserve_now_callback(call.msg.reservationId, call.msg.connectorId,
                                                         call.msg.expiryDate, call.msg.idTag, call.msg.parentIdTag);
        }
    }

    ocpp::CallResult<ReserveNowResponse> call_result(response, call.uniqueId);
    this->message_dispatcher->dispatch_call_result(call_result);
}

void ChargePointImpl::handleCancelReservationRequest(ocpp::Call<CancelReservationRequest> call) {
    CancelReservationResponse response;
    response.status = CancelReservationStatus::Rejected;

    if (this->cancel_reservation_callback != nullptr) {
        if (this->cancel_reservation_callback(call.msg.reservationId)) {
            response.status = CancelReservationStatus::Accepted;
        }
    }
    ocpp::CallResult<CancelReservationResponse> call_result(response, call.uniqueId);
    this->message_dispatcher->dispatch_call_result(call_result);
}

void ChargePointImpl::handleSendLocalListRequest(ocpp::Call<SendLocalListRequest> call) {
    EVLOG_debug << "Received SendLocalListRequest: " << call.msg << "\nwith messageId: " << call.uniqueId;

    SendLocalListResponse response;
    response.status = UpdateStatus::Failed;

    try {
        if (!this->configuration->getSupportedFeatureProfilesSet().count(
                SupportedFeatureProfiles::LocalAuthListManagement) or
            !this->configuration->getLocalAuthListEnabled()) {
            response.status = UpdateStatus::NotSupported;
        } else if (call.msg.updateType == UpdateType::Full) {
            if (call.msg.localAuthorizationList) {
                auto local_auth_list = call.msg.localAuthorizationList.value();
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
                auto local_auth_list = call.msg.localAuthorizationList.value();
                try {
                    if (this->database_handler->get_local_list_version() < call.msg.listVersion) {
                        this->database_handler->insert_or_update_local_list_version(call.msg.listVersion);
                        this->database_handler->insert_or_update_local_authorization_list(local_auth_list);

                        response.status = UpdateStatus::Accepted;
                    } else {
                        response.status = UpdateStatus::VersionMismatch;
                    }
                } catch (const RequiredEntryNotFoundException& e) {
                    try {
                        // try to recover if no entry for local list version is found
                        this->database_handler->insert_or_ignore_local_list_version(0);
                    } catch (QueryExecutionException& e) {
                        EVLOG_warning << "Could not restore local list version in database";
                    }
                    response.status = UpdateStatus::Failed;
                }
            }
        }
    } catch (const QueryExecutionException& e) {
        EVLOG_warning << "Insert or update of local authorization list failed (at least partially): " << e.what();
        response.status = UpdateStatus::Failed;
    }

    ocpp::CallResult<SendLocalListResponse> call_result(response, call.uniqueId);
    this->message_dispatcher->dispatch_call_result(call_result);
}

void ChargePointImpl::handleGetLocalListVersionRequest(ocpp::Call<GetLocalListVersionRequest> call) {
    EVLOG_debug << "Received GetLocalListVersionRequest: " << call.msg << "\nwith messageId: " << call.uniqueId;

    GetLocalListVersionResponse response;
    if (!this->configuration->getSupportedFeatureProfilesSet().count(
            SupportedFeatureProfiles::LocalAuthListManagement) or
        !this->configuration->getLocalAuthListEnabled()) {
        // if Local Authorization List is not supported, report back -1 as list version
        response.listVersion = -1;
    } else {
        try {
            if (this->database_handler->get_local_authorization_list_number_of_entries() == 0) {
                response.listVersion = 0;
            } else {
                response.listVersion = this->database_handler->get_local_list_version();
            }
        } catch (QueryExecutionException& e) {
            auto call_error = CallError(call.uniqueId, "InternalError", "Could not retrieve listVersion from database",
                                        json({}, true));
            this->message_dispatcher->dispatch_call_error(call_error);
            return;
        } catch (RequiredEntryNotFoundException& e) {
            try {
                // try to recover if not entry is found
                this->database_handler->insert_or_ignore_local_list_version(0);
            } catch (QueryExecutionException& e) {
                EVLOG_warning << "Could not restore local list version in database";
            }
            auto call_error = CallError(call.uniqueId, "InternalError", "Could not retrieve listVersion from database",
                                        json({}, true));
            this->message_dispatcher->dispatch_call_error(call_error);
            return;
        }
    }

    ocpp::CallResult<GetLocalListVersionResponse> call_result(response, call.uniqueId);
    this->message_dispatcher->dispatch_call_result(call_result);
}

DataTransferResponse ChargePointImpl::handle_set_user_price(const std::optional<std::string>& msg) {
    std::optional<std::string> id_token = std::nullopt;
    DataTransferResponse response;
    response.status = DataTransferStatus::Rejected;
    if (!msg.has_value()) {
        EVLOG_error << "Data transfer for set user price does not contain any data.";
        return response;
    }

    if (tariff_message_callback == nullptr) {
        EVLOG_error << "Received data transfer for set user price, but no callback is registered.";
        return response;
    }

    json data;
    try {
        data = json(msg.value());
        if (data.is_string()) {
            data = json::parse(msg.value());
        }

        if (data.contains("idToken")) {
            id_token = data.at("idToken");
        } else {
            EVLOG_error << "No id token received, can not set user price.";
            return response;
        }
    } catch (const std::exception& e) {
        EVLOG_error << "Could not parse set user price datatransfer message: " << e.what();
        return response;
    }

    TariffMessage tariff_message;
    const auto t = this->transaction_handler->get_transaction_from_id_tag(id_token.value());
    std::string identifier_id;
    IdentifierType identifier_type;

    if (t == nullptr) {
        identifier_id = id_token.value();
        identifier_type = IdentifierType::IdToken;
    } else {
        identifier_id = t->get_session_id();
        identifier_type = IdentifierType::SessionId;
        const std::optional<int32_t> transaction_id = t->get_transaction_id();
        if (transaction_id != std::nullopt) {
            tariff_message.ocpp_transaction_id = std::to_string(transaction_id.value());
        }
    }

    tariff_message.identifier_id = identifier_id;
    tariff_message.identifier_type = identifier_type;

    if (data.contains("priceText")) {
        DisplayMessageContent m;
        m.message = data.at("priceText");
        if (this->configuration->getLanguage().has_value()) {
            m.language = this->configuration->getLanguage().value();
        }
        tariff_message.message.push_back(m);
    }
    if (this->configuration->getCustomMultiLanguageMessagesEnabled() && data.contains("priceTextExtra") &&
        data.at("priceTextExtra").is_array()) {
        for (const json& j : data.at("priceTextExtra")) {
            DisplayMessageContent message;
            message = j;

            tariff_message.message.push_back(message);
        }
    }

    {
        std::lock_guard<std::mutex> lock(this->user_price_map_mutex);

        if (this->user_price_cvs.find(id_token.value()) != this->user_price_cvs.end()) {
            this->tariff_messages_by_id_token[id_token.value()] = tariff_message;
            this->user_price_cvs[id_token.value()].notify_all();
        }
    }
    response = this->tariff_message_callback(tariff_message);
    return response;
}

DataTransferResponse ChargePointImpl::handle_set_session_cost(const RunningCostState& type,
                                                              const std::optional<std::string>& message) {
    DataTransferResponse response;
    response.status = DataTransferStatus::Rejected;

    if (!message.has_value()) {
        EVLOG_error << "Data transfer for RunningCost / FinalCost does not contain any data.";
        return response;
    }

    if (session_cost_callback == nullptr) {
        EVLOG_error << "Received data transfer for " << ocpp::conversions::running_cost_state_to_string(type)
                    << ", but no callback is registered.";
        return response;
    }

    RunningCost cost;
    json data;

    TriggerMeterValue trigger_meter_value;

    try {
        data = json::parse(message.value());
        cost = data;

        // Libocpp will handle those triggers, so they are not stored in the 'RunningCost' object.
        if (data.contains("triggerMeterValue")) {
            trigger_meter_value = data.at("triggerMeterValue");
        }
    } catch (const std::exception& e) {
        EVLOG_warning << "Set session cost: Parsing failed! " << e.what();
        response.data = "json parsing failed";
        return response;
    }

    int32_t connector_id = this->transaction_handler->get_connector_from_transaction_id(std::stoi(cost.transaction_id));
    std::string session_id = cost.transaction_id;
    if (connector_id == -1) {
        EVLOG_warning << "Set session cost failed: Could not set session id because transaction with transaction id "
                      << cost.transaction_id << " is not found.";
        return response;
    }

    std::shared_ptr<Connector> connector = this->connectors.at(connector_id);
    std::shared_ptr<Transaction> transaction = this->transaction_handler->get_transaction(connector_id);
    if (transaction == nullptr) {
        EVLOG_warning << "Set session cost failed: Could not set session id because transaction with transaction id "
                      << cost.transaction_id << " for connector " << connector_id << " is not found.";
        return response;
    }

    session_id = transaction->get_session_id();

    cost.transaction_id = session_id;
    cost.state = type;
    static const uint32_t number_of_decimals =
        this->configuration->getPriceNumberOfDecimalsForCostValues().value_or(DEFAULT_PRICE_NUMBER_OF_DECIMALS);

    if (connector != nullptr) {
        connector->trigger_metervalue_on_energy_kwh = trigger_meter_value.at_energy_kwh;
        connector->trigger_metervalue_on_power_kw = trigger_meter_value.at_power_kw;
        connector->trigger_metervalue_on_status = trigger_meter_value.at_chargepoint_status;

        // Set timer to trigger sending a metervalue at a specific time.
        if (trigger_meter_value.at_time.has_value()) {
            set_connector_trigger_metervalue_timer(trigger_meter_value.at_time.value(), connector);
        }
    }

    return session_cost_callback(cost, number_of_decimals);
}

void ChargePointImpl::set_connector_trigger_metervalue_timer(const DateTime& date_time,
                                                             std::shared_ptr<Connector> connector) {
    if (connector == nullptr) {
        EVLOG_error << "Could not set display time offset: could not find connector";
        return;
    }

    std::chrono::time_point<date::utc_clock> trigger_timepoint = date_time.to_time_point();
    const std::chrono::time_point<date::utc_clock> now = date::utc_clock::now();

    if (trigger_timepoint < now) {
        EVLOG_error << "Could not set trigger metervalue because trigger time is in the past.";
        return;
    }

    std::optional<std::string> time_offset = this->configuration->getDisplayTimeOffset();
    if (time_offset.has_value()) {
        const std::vector<std::string> times = split_string(time_offset.value(), ':');
        if (times.size() != 2) {
            EVLOG_error << "Could not set display time offset: format not correct (should be something like "
                           "\"-05:00\", but is "
                        << time_offset.value() << ")";
        } else {
            try {
                trigger_timepoint += std::chrono::hours(std::stoi(times.at(0)));
                trigger_timepoint += std::chrono::minutes(std::stoi(times.at(1)));
            } catch (const std::exception& e) {
                EVLOG_error << "Could not set display time offset: format not correct (should be something "
                               "like \"-19:15\", but is "
                            << time_offset.value() << "): " << e.what();
            }
        }
    }

    DateTime d(now);
    int32_t connector_id = connector->id;

    connector->trigger_metervalue_at_time_timer =
        std::make_unique<Everest::SystemTimer>(&this->io_context, [this, connector_id]() {
            const std::optional<MeterValue>& meter_value = get_latest_meter_value(
                connector_id, {{Measurand::Energy_Active_Import_Register, std::nullopt}}, ReadingContext::Other);
            if (!meter_value.has_value()) {
                EVLOG_error << "Send latest meter value because of chargepoint time trigger failed";
            } else {
                send_meter_value(connector_id, meter_value.value());
            }
        });
    connector->trigger_metervalue_at_time_timer->at(trigger_timepoint);
}

void ChargePointImpl::set_time_offset_timer(const std::string& date_time) {
    if (this->change_time_offset_timer != nullptr) {
        this->change_time_offset_timer->stop();
        this->change_time_offset_timer = nullptr;
    }

    DateTime d(date_time);
    if (d.to_time_point() < date::utc_clock::now()) {
        // Not setting offset timer, because the date is in the past.
        return;
    }

    this->change_time_offset_timer = std::make_unique<Everest::SystemTimer>(&this->io_context, [this]() {
        const std::optional<std::string> next_offset = this->configuration->getTimeOffsetNextTransition();
        if (next_offset.has_value()) {
            this->configuration->setDisplayTimeOffset(next_offset.value());
        }
    });

    this->change_time_offset_timer->at(d.to_time_point());
}

void ChargePointImpl::status_notification(const int32_t connector, const ChargePointErrorCode errorCode,
                                          const ChargePointStatus status, const ocpp::DateTime& timestamp,
                                          const std::optional<CiString<50>>& info,
                                          const std::optional<CiString<255>>& vendor_id,
                                          const std::optional<CiString<50>>& vendor_error_code,
                                          bool initiated_by_trigger_message) {
    StatusNotificationRequest request;
    request.connectorId = connector;
    request.errorCode = errorCode;
    request.status = status;
    request.timestamp = timestamp;
    request.info = info;
    request.vendorId = vendor_id;
    request.vendorErrorCode = vendor_error_code;
    ocpp::Call<StatusNotificationRequest> call(request);
    this->message_dispatcher->dispatch_call(call, initiated_by_trigger_message);
}

// public API for Core profile

EnhancedIdTagInfo ChargePointImpl::authorize_id_token(CiString<20> idTag, const bool authorize_only_locally) {
    EnhancedIdTagInfo enhanced_id_tag_info;

    // only do authorize req when authorization locally not enabled or fails
    // proritize auth list over auth cache for same idTags

    // Authorize locally (cache or local list) if
    // - LocalPreAuthorize is true and CP is online
    // OR
    // - LocalAuthorizeOffline is true and CP is offline
    if ((this->configuration->getLocalPreAuthorize() &&
         (this->websocket != nullptr && this->websocket->is_connected())) ||
        (this->configuration->getLocalAuthorizeOffline() &&
         (this->websocket == nullptr || !this->websocket->is_connected()))) {

        const auto update_tariff_message_if_eligible = [this](EnhancedIdTagInfo& enhanced_id_tag_info) {
            if (enhanced_id_tag_info.id_tag_info.status == AuthorizationStatus::Accepted &&
                this->configuration->getCustomDisplayCostAndPriceEnabled()) {
                enhanced_id_tag_info.tariff_message =
                    this->websocket->is_connected()
                        ? this->configuration->getTariffMessageWithDefaultPriceText()
                        : this->configuration->getTariffMessageWithDefaultPriceTextOffline();
            }
        };

        if (this->configuration->getLocalAuthListEnabled()) {
            try {
                const auto auth_list_entry_opt = this->database_handler->get_local_authorization_list_entry(idTag);
                if (auth_list_entry_opt.has_value()) {
                    EVLOG_info << "Found id_tag " << idTag.get() << " in AuthorizationList";
                    enhanced_id_tag_info.id_tag_info = auth_list_entry_opt.value();

                    update_tariff_message_if_eligible(enhanced_id_tag_info);
                    return enhanced_id_tag_info;
                }
            } catch (const QueryExecutionException& e) {
                EVLOG_warning << "Could not request local authorization list entry: " << e.what();
            } catch (const std::exception& e) {
                EVLOG_warning << "Unknown Error while requesting local authorization list entry: " << e.what();
            }
        }

        if (this->configuration->getAuthorizationCacheEnabled()) {
            if (this->validate_against_cache_entries(idTag)) {
                EVLOG_info << "Found valid id_tag " << idTag.get() << " in AuthorizationCache";
                enhanced_id_tag_info.id_tag_info = this->database_handler->get_authorization_cache_entry(idTag).value();
                update_tariff_message_if_eligible(enhanced_id_tag_info);
                return enhanced_id_tag_info;
            }
        }
    }

    if (authorize_only_locally) {
        enhanced_id_tag_info.id_tag_info = {AuthorizationStatus::Invalid};
        return enhanced_id_tag_info;
    }

    std::unique_lock<std::mutex> lock(this->user_price_map_mutex);
    this->tariff_messages_by_id_token.erase(idTag.get()); // reset any prior data

    AuthorizeRequest req;
    req.idTag = idTag;
    ocpp::Call<AuthorizeRequest> call(req);

    auto authorize_future = this->message_dispatcher->dispatch_call_async(call);

    if (authorize_future.wait_for(DEFAULT_WAIT_FOR_FUTURE_TIMEOUT) == std::future_status::timeout) {
        EVLOG_warning << "Waiting for Authorize.conf future timed out!";
        enhanced_id_tag_info.id_tag_info = {AuthorizationStatus::Invalid, std::nullopt, std::nullopt};
        return enhanced_id_tag_info;
    }

    auto enhanced_message = authorize_future.get();

    if (enhanced_message.messageType == MessageType::AuthorizeResponse) {
        try {
            ocpp::CallResult<AuthorizeResponse> call_result = enhanced_message.message;
            this->database_handler->insert_or_update_authorization_cache_entry(idTag, call_result.msg.idTagInfo);
            enhanced_id_tag_info.id_tag_info = call_result.msg.idTagInfo;

            if (this->configuration->getCustomDisplayCostAndPriceEnabled() &&
                call_result.msg.idTagInfo.status == AuthorizationStatus::Accepted) {
                EVLOG_info << "California pricing is active, waiting for set user price.";

                this->user_price_cvs[idTag.get()].wait_for(
                    lock,
                    std::chrono::milliseconds(this->configuration->getWaitForSetUserPriceTimeout().value_or(
                        DEFAULT_WAIT_FOR_SET_USER_PRICE_TIMEOUT_MS)),
                    [&] {
                        return this->tariff_messages_by_id_token.find(idTag.get()) !=
                               this->tariff_messages_by_id_token.end();
                    });

                auto tariff_it = this->tariff_messages_by_id_token.find(idTag.get());
                if (tariff_it != this->tariff_messages_by_id_token.end()) {
                    EVLOG_info << "Tariff message received for idToken " << idTag.get();
                    enhanced_id_tag_info.tariff_message = tariff_it->second;
                    this->tariff_messages_by_id_token.erase(tariff_it);
                } else {
                    EVLOG_warning << "Tariff message was not received within timeout for idToken " << idTag.get();
                    enhanced_id_tag_info.tariff_message = this->configuration->getTariffMessageWithDefaultPriceText();
                }
                this->user_price_cvs.erase(idTag.get());
            }
            return enhanced_id_tag_info;
        } catch (const std::exception& e) {
            EVLOG_warning << "Error processing AuthorizeResponse: " << e.what();
            enhanced_id_tag_info.id_tag_info = {AuthorizationStatus::Invalid, std::nullopt, std::nullopt};
            return enhanced_id_tag_info;
        }
    } else if (enhanced_message.offline) {
        if (this->configuration->getAllowOfflineTxForUnknownId().value_or(false)) {
            enhanced_id_tag_info.id_tag_info = {AuthorizationStatus::Accepted, std::nullopt, std::nullopt};
            return enhanced_id_tag_info;
        }
    }

    enhanced_id_tag_info.id_tag_info = {AuthorizationStatus::Invalid, std::nullopt, std::nullopt};
    return enhanced_id_tag_info;
}

std::map<int32_t, ChargingSchedule> ChargePointImpl::get_all_composite_charging_schedules(const int32_t duration_s,
                                                                                          const ChargingRateUnit unit) {

    std::map<int32_t, ChargingSchedule> charging_schedules;
    std::set<ChargingProfilePurposeType> purposes_to_ignore;
    const auto is_offline = this->websocket == nullptr or not this->websocket->is_connected();

    if (not is_offline) {
        const auto purposes_to_ignore_vec = this->configuration->getIgnoredProfilePurposesOffline();
        purposes_to_ignore.insert(purposes_to_ignore_vec.begin(), purposes_to_ignore_vec.end());
    }

    for (int connector_id = 0; connector_id <= this->configuration->getNumberOfConnectors(); connector_id++) {
        const auto start_time = ocpp::DateTime();
        const auto duration = std::chrono::seconds(duration_s);
        const auto end_time = ocpp::DateTime(start_time.to_time_point() + duration);

        const auto valid_profiles =
            this->smart_charging_handler->get_valid_profiles(start_time, end_time, connector_id, purposes_to_ignore);
        const auto composite_schedule = this->smart_charging_handler->calculate_composite_schedule(
            start_time, end_time, connector_id, unit, is_offline, true);
        charging_schedules[connector_id] = composite_schedule;
    }

    return charging_schedules;
}

std::map<int32_t, EnhancedChargingSchedule>
ChargePointImpl::get_all_enhanced_composite_charging_schedules(const int32_t duration_s, const ChargingRateUnit unit) {

    std::map<int32_t, EnhancedChargingSchedule> charging_schedules;
    std::set<ChargingProfilePurposeType> purposes_to_ignore;

    if (this->websocket == nullptr or not this->websocket->is_connected()) {
        const auto purposes_to_ignore_vec = this->configuration->getIgnoredProfilePurposesOffline();
        purposes_to_ignore.insert(purposes_to_ignore_vec.begin(), purposes_to_ignore_vec.end());
    }

    for (int connector_id = 0; connector_id <= this->configuration->getNumberOfConnectors(); connector_id++) {
        const auto start_time = ocpp::DateTime();
        const auto duration = std::chrono::seconds(duration_s);
        const auto end_time = ocpp::DateTime(start_time.to_time_point() + duration);

        const auto composite_schedule = this->smart_charging_handler->calculate_enhanced_composite_schedule(
            start_time, end_time, connector_id, unit, (this->connection_state != ChargePointConnectionState::Booted),
            true);
        charging_schedules[connector_id] = composite_schedule;
    }

    return charging_schedules;
}

bool ChargePointImpl::is_pnc_enabled() {
    return this->configuration->getSupportedFeatureProfilesSet().count(SupportedFeatureProfiles::PnC) and
           this->configuration->getISO15118PnCEnabled();
}

ocpp::v2::AuthorizeResponse ChargePointImpl::data_transfer_pnc_authorize(

    const std::string& emaid, const std::optional<std::string>& certificate,
    const std::optional<std::vector<ocpp::v2::OCSPRequestData>>& iso15118_certificate_hash_data) {

    ocpp::v2::AuthorizeResponse authorize_response;
    authorize_response.idTokenInfo.status = ocpp::v2::AuthorizationStatusEnum::Invalid;

    if (!this->is_pnc_enabled()) {
        EVLOG_warning << "Received request to send DataTransfer(Authorize.req) while PnC is disabled";
        return authorize_response;
    }

    DataTransferRequest req;
    req.vendorId = ISO15118_PNC_VENDOR_ID;
    req.messageId.emplace(CiString<50>(std::string("Authorize")));

    ocpp::v2::AuthorizeRequest authorize_req;
    ocpp::v2::IdToken id_token;

    id_token.type = v2::IdTokenEnumStringType::eMAID;
    id_token.idToken = emaid;
    authorize_req.idToken = id_token;

    // Temporary variable that is set to true to avoid immediate response to allow the local auth list
    // or auth cache to be tried
    bool try_local_auth_list_or_cache = false;
    bool forward_to_csms = false;

    if (this->websocket->is_connected() and iso15118_certificate_hash_data.has_value()) {
        authorize_req.iso15118CertificateHashData = iso15118_certificate_hash_data;
        forward_to_csms = true;
    } else if (certificate.has_value()) {
        // First try to validate the contract certificate locally
        CertificateValidationResult local_verify_result = this->evse_security->verify_certificate(
            certificate.value(), {ocpp::LeafCertificateType::MO, ocpp::LeafCertificateType::V2G});
        EVLOG_info << "Local contract validation result: " << local_verify_result;
        const auto central_contract_validation_allowed =
            this->configuration->getCentralContractValidationAllowed().value_or(false);
        const auto contract_validation_offline = this->configuration->getContractValidationOffline();
        const auto local_authorize_offline = this->configuration->getLocalAuthorizeOffline();

        // C07.FR.01: When CS is online, it shall send an AuthorizeRequest
        // C07.FR.02: The AuthorizeRequest shall at least contain the OCSP data
        if (this->websocket->is_connected()) {
            if (local_verify_result == CertificateValidationResult::IssuerNotFound) {
                // C07.FR.06: Pass contract validation to CSMS when no contract root is found
                if (central_contract_validation_allowed) {
                    EVLOG_info << "Online: No local contract root found. Pass contract validation to CSMS";
                    authorize_req.certificate = certificate.value();
                    forward_to_csms = true;
                } else {
                    EVLOG_warning << "Online: Central Contract Validation not allowed";
                    authorize_response.idTokenInfo.status = ocpp::v2::AuthorizationStatusEnum::Invalid;
                }
            } else {
                // Try to generate the OCSP data from the certificate chain and use that
                const auto generated_ocsp_request_data_list = ocpp::evse_security_conversions::to_ocpp_v2(
                    this->evse_security->get_mo_ocsp_request_data(certificate.value()));
                if (generated_ocsp_request_data_list.size() > 0) {
                    EVLOG_info << "Online: Pass generated OCSP data to CSMS";
                    authorize_req.iso15118CertificateHashData = generated_ocsp_request_data_list;
                    forward_to_csms = true;
                } else {
                    EVLOG_warning << "Online: OCSP data could not be generated";
                    if (central_contract_validation_allowed) {
                        EVLOG_info << "Online: OCSP data could not be generated. Pass contract validation to CSMS";
                        authorize_req.certificate = certificate.value();
                        forward_to_csms = true;
                    } else {
                        EVLOG_warning
                            << "Online: OCSP data could not be generated and CentralContractValidation not allowed";
                        authorize_response.idTokenInfo.status = ocpp::v2::AuthorizationStatusEnum::Invalid;
                    }
                }
            }
        } else { // Offline
            // C07.FR.08: CS shall try to validate the contract locally
            if (contract_validation_offline) {
                EVLOG_info << "Offline: contract " << local_verify_result;
                switch (local_verify_result) {
                // C07.FR.09: CS shall lookup the eMAID in Local Auth List or Auth Cache when
                // local validation succeeded
                case CertificateValidationResult::Valid:
                    // In C07.FR.09 LocalAuthorizeOffline is mentioned, this seems to be a generic config item
                    // that applies to Local Auth List and Auth Cache, but since there are no requirements about
                    // it, lets check it here
                    if (local_authorize_offline) {
                        try_local_auth_list_or_cache = true;
                    } else {
                        // No requirement states what to do when ContractValidationOffline is true
                        // and LocalAuthorizeOffline is false
                        authorize_response.idTokenInfo.status = ocpp::v2::AuthorizationStatusEnum::Unknown;
                        authorize_response.certificateStatus = ocpp::v2::AuthorizeCertificateStatusEnum::Accepted;
                    }
                    break;
                case CertificateValidationResult::Expired:
                    authorize_response.idTokenInfo.status = ocpp::v2::AuthorizationStatusEnum::Expired;
                    authorize_response.certificateStatus = ocpp::v2::AuthorizeCertificateStatusEnum::CertificateExpired;
                    break;
                case CertificateValidationResult::InvalidSignature:
                case CertificateValidationResult::IssuerNotFound:
                case CertificateValidationResult::InvalidLeafSignature:
                case CertificateValidationResult::InvalidChain:
                case CertificateValidationResult::Unknown:
                    authorize_response.idTokenInfo.status = ocpp::v2::AuthorizationStatusEnum::Unknown;
                    break;
                }
            } else {
                EVLOG_warning << "Offline: ContractValidationOffline is disabled. Validation not allowed";
                // C07.FR.07: CS shall not allow charging
                authorize_response.idTokenInfo.status = ocpp::v2::AuthorizationStatusEnum::NotAtThisTime;
            }
        }
    } else {
        EVLOG_warning << "Can not validate eMAID without certificate chain";
        authorize_response.idTokenInfo.status = ocpp::v2::AuthorizationStatusEnum::Invalid;
    }

    if (forward_to_csms) {
        authorize_response.idTokenInfo.status = ocpp::v2::AuthorizationStatusEnum::Unknown;
        // AuthorizeRequest sent to CSMS, let's show the results
        req.data.emplace(json(authorize_req).dump());

        // Send the DataTransfer(Authorize) to the CSMS
        Call<DataTransferRequest> call(req);
        auto authorize_future = this->message_dispatcher->dispatch_call_async(call);

        if (authorize_future.wait_for(DEFAULT_WAIT_FOR_FUTURE_TIMEOUT) == std::future_status::timeout) {
            EVLOG_warning << "Waiting for DataTransfer.conf(Authorize) future timed out!";
            return authorize_response;
        }

        auto enhanced_message = authorize_future.get();

        if (enhanced_message.messageType == MessageType::DataTransferResponse) {
            try {
                // parse and return authorize response
                ocpp::CallResult<DataTransferResponse> call_result = enhanced_message.message;
                if (call_result.msg.data.has_value()) {
                    authorize_response = json::parse(call_result.msg.data.value());
                } else {
                    EVLOG_warning << "CSMS response of DataTransferRequest(Authorize) did not include data";
                }
            } catch (const json::exception& e) {
                EVLOG_warning << "Could not parse data of DataTransfer message Authorize.conf: " << e.what();
            } catch (const std::exception& e) {
                EVLOG_error << "Unknown Error while handling DataTransfer message Authorize.conf: " << e.what();
            }
        } else if (enhanced_message.offline) {
            EVLOG_warning << "No response received for DataTransfer.req(Authorize) from CSMS";
        } else {
            EVLOG_warning << "CSMS response of DataTransferRequest(Authorize) was not DataTransferResponse";
        }
        return authorize_response;
    }
    // For eMAID, we will respond here, unless the local auth list or auth cache is tried
    if (!try_local_auth_list_or_cache) {
        return authorize_response;
    } else {
        const auto local_authorize_result = this->authorize_id_token(emaid, true);
        if (local_authorize_result.id_tag_info.status == AuthorizationStatus::Accepted) {
            authorize_response.idTokenInfo.status = ocpp::v2::AuthorizationStatusEnum::Accepted;
        } else {
            authorize_response.idTokenInfo.status = ocpp::v2::AuthorizationStatusEnum::Invalid;
        }
        return authorize_response;
    }
}

void ChargePointImpl::data_transfer_pnc_sign_certificate() {

    if (!this->configuration->getCpoName().has_value() and
        !this->configuration->getSeccLeafSubjectOrganization().has_value()) {
        EVLOG_warning
            << "Can not request new V2GCertificate because neither CpoName nor SeccLeafSubjectOrganization is set.";
        return;
    }

    DataTransferRequest req;
    req.vendorId = ISO15118_PNC_VENDOR_ID;
    req.messageId.emplace(CiString<50>(std::string("SignCertificate")));

    ocpp::v2::SignCertificateRequest csr_req;

    const auto result = this->evse_security->generate_certificate_signing_request(
        ocpp::CertificateSigningUseEnum::V2GCertificate,
        this->configuration->getSeccLeafSubjectCountry().value_or("DE"),
        this->configuration->getSeccLeafSubjectOrganization().value_or(
            this->configuration->getCpoName().value_or("DEFAULT")),
        this->configuration->getSeccLeafSubjectCommonName().value_or(this->configuration->getChargeBoxSerialNumber()),
        this->configuration->getUseTPMSeccLeafCertificate());

    if (result.status != GetCertificateSignRequestStatus::Accepted || !result.csr.has_value()) {
        EVLOG_error << "Could not request new V2GCertificate, because the CSR was not successful.";

        std::string gen_error = "Data transfer pnc csr failed due to:" +
                                ocpp::conversions::generate_certificate_signing_request_status_to_string(result.status);
        this->securityEventNotification(ocpp::security_events::CSRGENERATIONFAILED,
                                        std::optional<CiString<255>>(gen_error), true);

        return;
    }

    csr_req.csr = result.csr.value();
    csr_req.certificateType = ocpp::v2::CertificateSigningUseEnum::V2GCertificate;
    req.data.emplace(json(csr_req).dump());

    Call<DataTransferRequest> call(req);
    this->message_dispatcher->dispatch_call(call);
}

void ChargePointImpl::data_transfer_pnc_get_15118_ev_certificate(
    const int32_t connector_id, const std::string& exi_request, const std::string& iso15118_schema_version,
    const ocpp::v2::CertificateActionEnum& certificate_action) {

    if (!this->is_pnc_enabled()) {
        EVLOG_warning
            << "Received request to send DataTransfer(Get15118EVCertificateRequest.req) while PnC is disabled";
        return;
    }

    EVLOG_info << "Sending Get15118EVCertificate DataTransfer.req";
    DataTransferRequest req;
    req.vendorId = ISO15118_PNC_VENDOR_ID;
    req.messageId.emplace(CiString<50>(std::string("Get15118EVCertificate")));

    ocpp::v2::Get15118EVCertificateRequest cert_req;
    cert_req.action = certificate_action;
    cert_req.exiRequest = exi_request;
    cert_req.iso15118SchemaVersion = iso15118_schema_version;

    req.data.emplace(json(cert_req).dump());

    Call<DataTransferRequest> call(req);
    auto future = this->message_dispatcher->dispatch_call_async(call);

    if (future.wait_for(DEFAULT_WAIT_FOR_FUTURE_TIMEOUT) == std::future_status::timeout) {
        EVLOG_warning << "Waiting for DataTransfer.conf(Get15118EVCertificate) future timed out!";
        return;
    }

    auto enhanced_message = future.get();

    if (enhanced_message.messageType == MessageType::DataTransferResponse) {
        // parse and return authorize response
        try {
            ocpp::CallResult<DataTransferResponse> call_result = enhanced_message.message;
            if (call_result.msg.data.has_value() and call_result.msg.status == DataTransferStatus::Accepted) {
                ocpp::v2::Get15118EVCertificateResponse ev_certificate_response =
                    json::parse(call_result.msg.data.value());
                this->get_15118_ev_certificate_response_callback(connector_id, ev_certificate_response,
                                                                 certificate_action);
            } else {
                EVLOG_warning << "DataTransferRequest(Get15118EVCertificate) was not Accepted by CSMS or missed data";
                return;
            }
        } catch (const json::exception& e) {
            EVLOG_warning << "Could not parse data of DataTransfer message Get15118EVCertificate.conf: " << e.what();
            return;
        } catch (const std::exception& e) {
            EVLOG_error << "Unknown Error while handling DataTransfer message Get15118EVCertificate.conf: " << e.what();
            return;
        }
    } else if (enhanced_message.offline) {
        return;
    } else {
        EVLOG_warning << "CSMS response of DataTransferRequest(Get15118EVCertificate) was not DataTransferResponse";
        return;
    }
}

void ChargePointImpl::data_transfer_pnc_get_certificate_status(const ocpp::v2::OCSPRequestData& ocsp_request_data) {
    EVLOG_info << "Requesting OCSP certificate Status";

    DataTransferRequest req;
    req.vendorId = ISO15118_PNC_VENDOR_ID;
    req.messageId.emplace(CiString<50>(std::string("GetCertificateStatus")));

    ocpp::v2::GetCertificateStatusRequest cert_status_req;
    cert_status_req.ocspRequestData = ocsp_request_data;

    req.data.emplace(json(cert_status_req).dump());

    Call<DataTransferRequest> call(req);
    auto future = this->message_dispatcher->dispatch_call_async(call);

    if (future.wait_for(DEFAULT_WAIT_FOR_FUTURE_TIMEOUT) == std::future_status::timeout) {
        EVLOG_warning << "Waiting for DataTransfer.conf(GetCertificateStatus) future timed out!";
        return;
    }

    auto enhanced_message = future.get();

    if (enhanced_message.messageType == MessageType::DataTransferResponse) {
        try {
            ocpp::CallResult<DataTransferResponse> call_result = enhanced_message.message;
            if (call_result.msg.data.has_value()) {
                ocpp::v2::GetCertificateStatusResponse cert_status_response = json::parse(call_result.msg.data.value());
                if (cert_status_response.status == ocpp::v2::GetCertificateStatusEnum::Accepted) {
                    if (cert_status_response.ocspResult.has_value()) {
                        ocpp::CertificateHashDataType certificate_hash_data;
                        certificate_hash_data.hashAlgorithm =
                            ocpp::evse_security_conversions::from_ocpp_v2(ocsp_request_data.hashAlgorithm);
                        certificate_hash_data.issuerKeyHash = ocsp_request_data.issuerKeyHash.get();
                        certificate_hash_data.issuerNameHash = ocsp_request_data.issuerNameHash.get();
                        certificate_hash_data.serialNumber = ocsp_request_data.serialNumber.get();
                        this->evse_security->update_ocsp_cache(certificate_hash_data,
                                                               cert_status_response.ocspResult.value());
                    } else {
                        EVLOG_warning << "GetCertificateStatusResponse status was accepted but no ocspResult is set. "
                                         "This is not allowed by the CSMS";
                        return;
                    }
                }
                // TODO(piet): Update cache and call this once a week (M06)
            } else {
                EVLOG_warning << "CSMS response of DataTransferRequest(GetCertificateStatus) did not include data";
                return;
            }
        } catch (const json::exception& e) {
            EVLOG_warning << "Could not parse data of DataTransfer message GetCertificateStatus.conf: " << e.what();
            return;
        } catch (const std::exception& e) {
            EVLOG_error << "Unknown Error while handling DataTransfer message GetCertificateStatus.conf: " << e.what();
            return;
        }
    } else if (enhanced_message.offline) {
        return;
    } else {
        EVLOG_warning << "CSMS response of DataTransferRequest(GetCertificateStatus) was not DataTransferResponse";
        return;
    }
}

void ChargePointImpl::handle_data_transfer_pnc_trigger_message(Call<DataTransferRequest> call) {
    EVLOG_info << "Received Data Transfer TriggerMessage: " << call.msg << "\nwith messageId: " << call.uniqueId;

    DataTransferResponse response;

    if (this->configuration->getCpoName().has_value() or
        this->configuration->getSeccLeafSubjectOrganization().has_value()) {
        response.status = DataTransferStatus::Accepted;
        ocpp::v2::TriggerMessageResponse trigger_message_response;
        trigger_message_response.status = ocpp::v2::TriggerMessageStatusEnum::Accepted;
        response.data.emplace(json(trigger_message_response).dump());
    } else {
        EVLOG_warning << "Received Data Transfer TriggerMessage to trigger CSR but no "
                         "CpoName or SeccLeafSubjectOrganization is set.";
        response.status = DataTransferStatus::Rejected;
        response.data.emplace("No CpoName or SeccLeafSubjectOrganization is set. Cannot trigger CSR");
    }

    CallResult<DataTransferResponse> call_result(response, call.uniqueId);
    this->message_dispatcher->dispatch_call_result(call_result);

    if (response.status == DataTransferStatus::Accepted) {
        // send sign certificate wrapped in data_transfer
        this->data_transfer_pnc_sign_certificate();
    }
}

void ChargePointImpl::handle_data_transfer_pnc_certificate_signed(Call<DataTransferRequest> call) {
    EVLOG_info << "Received Data Transfer CertificateSignedRequest: " << call.msg
               << "\nwith messageId: " << call.uniqueId;

    DataTransferResponse response;
    response.status = DataTransferStatus::Rejected;

    try {
        const ocpp::v2::CertificateSignedRequest req = json::parse(call.msg.data.value());

        response.status = DataTransferStatus::Accepted;

        CertificateSignedResponse certificate_response;
        certificate_response.status = CertificateSignedStatusEnumType::Rejected;
        std::string tech_info; // in case certificate is rejected this contains human readable information

        if (req.certificateType.has_value() and
            req.certificateType.value() != ocpp::v2::CertificateSigningUseEnum::V2GCertificate) {
            tech_info = "Received DataTransfer.req containing CertificateSigned.req where certificateType is not "
                        "V2GCertificate";
            EVLOG_warning << tech_info;
        } else if (this->configuration->getCertificateSignedMaxChainSize().has_value() and
                   static_cast<size_t>(this->configuration->getCertificateSignedMaxChainSize().value()) <
                       req.certificateChain.get().size()) {
            tech_info = "Received DataTransfer.req containing CertificateSigned.req where chain size is greater "
                        "than configured CertificateSignedMaxChainSize";
            EVLOG_warning << tech_info;
        } else {
            const auto certificate_chain = req.certificateChain.get();
            const auto result = this->evse_security->update_leaf_certificate(
                certificate_chain, ocpp::CertificateSigningUseEnum::V2GCertificate);

            if (result == InstallCertificateResult::Accepted) {
                certificate_response.status = CertificateSignedStatusEnumType::Accepted;
            } else {
                tech_info = ocpp::conversions::install_certificate_result_to_string(result);
            }
        }

        response.data.emplace(json(certificate_response).dump());

        CallResult<DataTransferResponse> call_result(response, call.uniqueId);
        this->message_dispatcher->dispatch_call_result(call_result);

        if (certificate_response.status == CertificateSignedStatusEnumType::Rejected) {
            this->securityEventNotification(ocpp::security_events::INVALIDCHARGEPOINTCERTIFICATE,
                                            std::optional<CiString<255>>(tech_info), true);
        } else {
            // update the OCSP cache in case a new certificate was installed
            this->update_ocsp_cache();
        }
    } catch (const json::exception& e) {
        EVLOG_warning << "Could not parse data of DataTransfer message CertificateSigned.req: " << e.what();
        CallResult<DataTransferResponse> call_result(response, call.uniqueId);
        this->message_dispatcher->dispatch_call_result(call_result);
    } catch (const std::exception& e) {
        EVLOG_error << "Unknown Error while handling DataTransfer message CertificateSigned.req: " << e.what();
        CallResult<DataTransferResponse> call_result(response, call.uniqueId);
        this->message_dispatcher->dispatch_call_result(call_result);
    }
}

void ChargePointImpl::handle_data_transfer_pnc_get_installed_certificates(Call<DataTransferRequest> call) {
    EVLOG_debug << "Received Data Transfer GetInstalledCertificatesRequest: " << call.msg
                << "\nwith messageId: " << call.uniqueId;

    DataTransferResponse response;

    try {
        if (call.msg.data.has_value()) {
            const ocpp::v2::GetInstalledCertificateIdsRequest req = json::parse(call.msg.data.value());

            response.status = DataTransferStatus::Accepted;

            // prepare argument for getRootCertificate
            std::vector<ocpp::CertificateType> certificate_types;
            if (req.certificateType.has_value()) {
                certificate_types = ocpp::evse_security_conversions::from_ocpp_v2(req.certificateType.value());
            }

            ocpp::v2::GetInstalledCertificateIdsResponse get_certificate_ids_response;
            get_certificate_ids_response.status = ocpp::v2::GetInstalledCertificateStatusEnum::NotFound;

            const auto certificate_hash_data_chains =
                this->evse_security->get_installed_certificates(certificate_types);

            if (!certificate_hash_data_chains.empty()) {
                std::optional<std::vector<ocpp::v2::CertificateHashDataChain>> certificate_hash_data_chain_v2_opt;
                std::vector<ocpp::v2::CertificateHashDataChain> certificate_hash_data_chain_v2;
                for (const auto certificate_hash_data_chain_entry : certificate_hash_data_chains) {
                    certificate_hash_data_chain_v2.push_back(
                        ocpp::evse_security_conversions::to_ocpp_v2(certificate_hash_data_chain_entry));
                }
                certificate_hash_data_chain_v2_opt.emplace(certificate_hash_data_chain_v2);
                get_certificate_ids_response.certificateHashDataChain = certificate_hash_data_chain_v2_opt;
                get_certificate_ids_response.status = ocpp::v2::GetInstalledCertificateStatusEnum::Accepted;
            }

            response.data.emplace(json(get_certificate_ids_response).dump());
        } else {
            response.status = DataTransferStatus::Rejected;
        }
    } catch (const json::exception& e) {
        EVLOG_warning << "Could not parse data of DataTransfer message GetInstalledCertificateIds.req: " << e.what();
        response.status = DataTransferStatus::Rejected;
    } catch (const std::exception& e) {
        EVLOG_error << "Unknown Error while handling DataTransfer message GetInstalledCertificateIds.req: " << e.what();
        response.status = DataTransferStatus::Rejected;
    }

    CallResult<DataTransferResponse> call_result(response, call.uniqueId);
    this->message_dispatcher->dispatch_call_result(call_result);
}

void ChargePointImpl::handle_data_transfer_delete_certificate(Call<DataTransferRequest> call) {
    DataTransferResponse response;

    if (call.msg.data.has_value()) {
        try {
            const ocpp::v2::DeleteCertificateRequest req = json::parse(call.msg.data.value());
            response.status = DataTransferStatus::Accepted;

            ocpp::v2::DeleteCertificateResponse delete_cert_response;
            const ocpp::CertificateHashDataType certificate_hash_data(json(req.certificateHashData));

            delete_cert_response.status = ocpp::evse_security_conversions::to_ocpp_v2(
                this->evse_security->delete_certificate(certificate_hash_data));

            response.data.emplace(json(delete_cert_response).dump());
        } catch (const json::exception& e) {
            EVLOG_warning << "Could not parse data of DataTransfer message DeleteCertificate.req: " << e.what();
            response.status = DataTransferStatus::Rejected;
        } catch (const std::exception& e) {
            EVLOG_error << "Unknown Error while handling DataTransfer message DeleteCertificate.req: " << e.what();
            response.status = DataTransferStatus::Rejected;
        }
    } else {
        response.status = DataTransferStatus::Rejected;
    }

    CallResult<DataTransferResponse> call_result(response, call.uniqueId);
    this->message_dispatcher->dispatch_call_result(call_result);
}

void ChargePointImpl::handle_data_transfer_install_certificate(Call<DataTransferRequest> call) {
    DataTransferResponse response;

    if (call.msg.data.has_value()) {
        try {
            const ocpp::v2::InstallCertificateRequest req = json::parse(call.msg.data.value());
            response.status = DataTransferStatus::Accepted;
            ocpp::CaCertificateType ca_certificate_type = evse_security_conversions::from_ocpp_v2(req.certificateType);
            const auto result = this->evse_security->install_ca_certificate(req.certificate.get(), ca_certificate_type);
            ocpp::v2::InstallCertificateResponse install_cert_response;
            install_cert_response.status = ocpp::evse_security_conversions::to_ocpp_v2(result);
            response.data.emplace(json(install_cert_response).dump());
        } catch (const json::exception& e) {
            EVLOG_warning << "Could not parse data of DataTransfer message InstallCertificate.req: " << e.what();
            response.status = DataTransferStatus::Rejected;
        } catch (const std::exception& e) {
            EVLOG_error << "Unknown Error while handling DataTransfer message InstallCertificate.req: " << e.what();
            response.status = DataTransferStatus::Rejected;
        }
    } else {
        response.status = DataTransferStatus::Rejected;
    }

    CallResult<DataTransferResponse> call_result(response, call.uniqueId);
    this->message_dispatcher->dispatch_call_result(call_result);
}

std::optional<DataTransferResponse> ChargePointImpl::data_transfer(const CiString<255>& vendorId,
                                                                   const std::optional<CiString<50>>& messageId,
                                                                   const std::optional<std::string>& data) {
    DataTransferRequest req;
    req.vendorId = vendorId;
    req.messageId = messageId;
    req.data = data;

    DataTransferResponse response;
    response.status = DataTransferStatus::Rejected;
    ocpp::Call<DataTransferRequest> call(req);
    auto data_transfer_future = this->message_dispatcher->dispatch_call_async(call);

    if (this->websocket == nullptr or !this->websocket->is_connected()) {
        EVLOG_warning << "Attempting to send DataTransfer.req but charging station is offline";
        return std::nullopt;
    }

    if (data_transfer_future.wait_for(DEFAULT_WAIT_FOR_FUTURE_TIMEOUT) == std::future_status::timeout) {
        EVLOG_warning << "Waiting for DataTransfer.conf future timed out";
        return std::nullopt;
    }

    auto enhanced_message = data_transfer_future.get();
    if (enhanced_message.messageType == MessageType::DataTransferResponse) {
        try {
            ocpp::CallResult<DataTransferResponse> call_result = enhanced_message.message;
            response = call_result.msg;
        } catch (json::exception& e) {
            EVLOG_warning << "Could not parse DataTransfer.conf message from CSMS";
            // We can not parse the returned message, so we somehow have to indicate an error to the caller
            response.status = DataTransferStatus::Rejected; // Rejected is not completely correct, but the
                                                            // best we have to indicate an error
        }
    }
    if (enhanced_message.offline) {
        EVLOG_warning << "Did not receive DataTransfer.conf from CSMS because we are offline";
        // The charge point is offline or has a bad connection.
        return std::nullopt;
    }

    return response;
}

void ChargePointImpl::register_data_transfer_callback(
    const CiString<255>& vendorId, const CiString<50>& messageId,
    const std::function<DataTransferResponse(const std::optional<std::string>& msg)>& callback) {
    std::lock_guard<std::mutex> lock(data_transfer_callbacks_mutex);
    this->data_transfer_callbacks[vendorId.get()][messageId.get()] = callback;
}

void ChargePointImpl::register_data_transfer_callback(
    const std::function<DataTransferResponse(const DataTransferRequest& request)>& callback) {
    this->data_transfer_callback = callback;
}

void ChargePointImpl::on_meter_values(int32_t connector, const Measurement& measurement) {
    // FIXME: fix measurement to also work with dc
    std::shared_ptr<Connector> c;
    {
        EVLOG_debug << "updating measurement for connector: " << connector;
        std::lock_guard<std::mutex> lock(measurement_mutex);
        c = this->connectors.at(connector);
        c->measurement.emplace(measurement);
    }

    send_meter_value_on_pricing_trigger(connector, c, measurement);
}

void ChargePointImpl::on_max_current_offered(int32_t connector, int32_t max_current) {
    std::lock_guard<std::mutex> lock(measurement_mutex);
    // TODO(kai): uses power meter mutex because the reading context is similar, think about storing
    // this information in a unified struct
    this->connectors.at(connector)->max_current_offered = max_current;
}

void ChargePointImpl::on_max_power_offered(int32_t connector, int32_t max_power) {
    std::lock_guard<std::mutex> lock(measurement_mutex);
    // TODO(kai): uses power meter mutex because the reading context is similar, think about storing
    // this information in a unified struct
    this->connectors.at(connector)->max_power_offered = max_power;
}

void ChargePointImpl::start_transaction(std::shared_ptr<Transaction> transaction) {

    StartTransactionRequest req;
    req.connectorId = transaction->get_connector();
    req.idTag = transaction->get_id_tag();
    req.meterStart = std::round(transaction->get_start_energy_wh()->energy_Wh);
    req.timestamp = transaction->get_start_energy_wh()->timestamp;
    const auto message_id = ocpp::create_message_id();

    try {
        this->database_handler->insert_transaction(
            transaction->get_session_id(), transaction->get_internal_transaction_id(), req.connectorId, req.idTag.get(),
            req.timestamp, req.meterStart, false, transaction->get_reservation_id(), message_id);
    } catch (const QueryExecutionException& e) {
        EVLOG_warning << "Could not insert transaction with session_id " << transaction->get_session_id()
                      << " into database: " << e.what();
    }

    this->transaction_handler->add_transaction(transaction);
    this->connectors.at(req.connectorId)->transaction = transaction;

    ocpp::Call<StartTransactionRequest> call(req, message_id);

    if (transaction->get_reservation_id()) {
        call.msg.reservationId = transaction->get_reservation_id().value();
    }

    transaction->set_start_transaction_message_id(message_id.get());
    transaction->change_meter_values_sample_interval(this->configuration->getMeterValueSampleInterval());

    this->message_dispatcher->dispatch_call(call);

    if (this->transaction_started_callback != nullptr) {
        this->transaction_started_callback(transaction->get_connector(), transaction->get_session_id());
    }
}

void ChargePointImpl::on_session_started(int32_t connector, const std::string& session_id,
                                         const ocpp::SessionStartedReason reason,
                                         const std::optional<std::string>& session_logging_path) {

    EVLOG_debug << "Session on connector#" << connector << " started with reason " << reason;

    if (session_logging_path.has_value() && this->logging->session_logging_active()) {
        this->logging->start_session_logging(session_id, session_logging_path.value());
    }

    // dont change to preparing when in reserved
    if ((this->status->get_state(connector) == ChargePointStatus::Reserved &&
         reason == SessionStartedReason::Authorized) ||
        this->status->get_state(connector) != ChargePointStatus::Reserved) {
        this->status->submit_event(connector, FSMEvent::UsageInitiated, ocpp::DateTime(),
                                   ocpp::conversions::session_started_reason_to_string(reason));
    }
}

void ChargePointImpl::on_session_stopped(const int32_t connector, const std::string& session_id) {
    this->execute_queued_availability_change(connector);

    if (this->status->get_state(connector) != ChargePointStatus::Reserved &&
        this->status->get_state(connector) != ChargePointStatus::Unavailable) {
        this->status->submit_event(connector, FSMEvent::BecomeAvailable, ocpp::DateTime());
    }

    if (this->logging->session_logging_active()) {
        this->logging->stop_session_logging(session_id);
    }
}

void ChargePointImpl::on_transaction_started(const int32_t& connector, const std::string& session_id,
                                             const std::string& id_token, const double meter_start,
                                             std::optional<int32_t> reservation_id, const ocpp::DateTime& timestamp,
                                             std::optional<std::string> signed_meter_value) {
    if (this->status->get_state(connector) == ChargePointStatus::Reserved) {
        this->status->submit_event(connector, FSMEvent::UsageInitiated, ocpp::DateTime());
    }

    auto meter_values_sample_timer = std::make_unique<Everest::SteadyTimer>(&this->io_context, [this, connector]() {
        const auto meter_value = this->get_latest_meter_value(
            connector, this->configuration->getMeterValuesSampledDataVector(), ReadingContext::Sample_Periodic);
        if (meter_value.has_value()) {
            this->transaction_handler->add_meter_value(connector, meter_value.value());
            this->send_meter_value(connector, meter_value.value());

            // this updates the last meter value in the database
            const auto transaction = this->transaction_handler->get_transaction(connector);
            if (transaction != nullptr) {
                for (const auto& entry : meter_value.value().sampledValue) {
                    if (entry.measurand == Measurand::Energy_Active_Import_Register and !entry.phase.has_value()) {
                        // this is the entry for Energy.Active.Import.Register total
                        try {
                            this->database_handler->update_transaction_meter_value(
                                transaction->get_session_id(), std::stoi(entry.value),
                                meter_value.value().timestamp.to_rfc3339());
                        } catch (const std::invalid_argument& e) {
                            EVLOG_warning << "Processed invalid meter value: " << entry.value
                                          << " while updating database";
                        } catch (const QueryExecutionException& e) {
                            EVLOG_warning << "Could not update meter value of transaction with session_id "
                                          << transaction->get_session_id() << " in the database: " << e.what();
                        }
                    }
                }
            }
        } else {
            EVLOG_warning
                << "Could not send and add meter value to transaction for uninitialized measurement at connector#"
                << connector;
        }
    });
    meter_values_sample_timer->interval(std::chrono::seconds(this->configuration->getMeterValueSampleInterval()));
    std::shared_ptr<Transaction> transaction = std::make_shared<Transaction>(
        this->transaction_handler->get_negative_random_transaction_id(), connector, session_id, CiString<20>(id_token),
        meter_start, reservation_id, timestamp, std::move(meter_values_sample_timer));
    if (signed_meter_value) {
        const auto meter_value =
            this->get_signed_meter_value(signed_meter_value.value(), ReadingContext::Transaction_Begin, timestamp);
        transaction->add_meter_value(meter_value);
    }

    this->start_transaction(transaction);
}

void ChargePointImpl::on_transaction_stopped(const int32_t connector, const std::string& session_id,
                                             const Reason& reason, ocpp::DateTime timestamp, float energy_wh_import,
                                             std::optional<CiString<20>> id_tag_end,
                                             std::optional<std::string> signed_meter_value) {
    auto transaction = this->transaction_handler->get_transaction(connector);
    if (transaction == nullptr) {
        EVLOG_error << "Trying to stop a transaction that is unknown on connector: " << connector
                    << ", with session_id: " << session_id;
        return;
    }
    if (connector <= 0 or connector > this->connectors.size()) {
        EVLOG_error << "Attempting to stop transaction for invalid connector id: " << connector;
    }

    if (signed_meter_value) {
        const auto meter_value =
            this->get_signed_meter_value(signed_meter_value.value(), ReadingContext::Transaction_End, timestamp);
        transaction->add_meter_value(meter_value);
    }
    const auto stop_energy_wh = std::make_shared<StampedEnergyWh>(timestamp, energy_wh_import);
    transaction->add_stop_energy_wh(stop_energy_wh);

    this->stop_transaction(connector, reason, id_tag_end);

    if (reason != Reason::EVDisconnected) {
        this->status->submit_event(connector, FSMEvent::TransactionStoppedAndUserActionRequired, ocpp::DateTime());
    }

    this->transaction_handler->remove_active_transaction(connector);
    this->connectors.at(connector)->transaction = nullptr;

    const auto profile_cleared = this->smart_charging_handler->clear_all_profiles_with_filter(
        std::nullopt, connector, std::nullopt, ChargingProfilePurposeType::TxProfile, false);
    if (profile_cleared and this->signal_set_charging_profiles_callback != nullptr) {
        this->signal_set_charging_profiles_callback();
    }
    reset_pricing_triggers(connector);
}

void ChargePointImpl::stop_transaction(int32_t connector, Reason reason, std::optional<CiString<20>> id_tag_end) {
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
    req.transactionId = transaction->get_transaction_id().value_or(transaction->get_internal_transaction_id());

    if (id_tag_end) {
        req.idTag.emplace(id_tag_end.value());
    }

    const auto transaction_data = this->get_filtered_transaction_data(transaction);
    if (!transaction_data.empty()) {
        req.transactionData.emplace(transaction_data);
    }

    auto message_id = ocpp::create_message_id();
    ocpp::Call<StopTransactionRequest> call(req, message_id);

    const auto max_message_size = this->configuration->getMaxMessageSize();
    if (utils::get_message_size(call) > max_message_size) {
        EVLOG_warning << "StopTransaction.req is too large. Dropping transaction data";
        utils::drop_transaction_data(max_message_size, call);
    }

    {
        std::lock_guard<std::mutex> lock(this->stop_transaction_mutex);
        this->message_dispatcher->dispatch_call(call);
    }

    if (this->transaction_stopped_callback != nullptr) {
        this->transaction_stopped_callback(
            connector, transaction->get_session_id(),
            transaction->get_transaction_id().value_or(transaction->get_internal_transaction_id()));
    }

    transaction->set_finished();
    transaction->set_stop_transaction_message_id(message_id.get());
    this->transaction_handler->add_stopped_transaction(transaction->get_connector());

    try {
        this->database_handler->update_transaction(transaction->get_session_id(), req.meterStop,
                                                   req.timestamp.to_rfc3339(), id_tag_end, reason, message_id.get());
    } catch (const QueryExecutionException& e) {
        EVLOG_warning << "Could not update transaction with session_id " << transaction->get_session_id()
                      << " in the database: " << e.what();
    }
}

std::vector<Measurand> ChargePointImpl::get_measurands_vec(const std::string& measurands_csv) {
    std::vector<Measurand> measurands;
    std::vector<std::string> measurands_strings = ocpp::split_string(measurands_csv, ',');

    for (const auto& measurand_string : measurands_strings) {
        try {
            measurands.push_back(conversions::string_to_measurand(measurand_string));
        } catch (const StringToEnumException& e) {
            EVLOG_warning << "Could not convert string: " << measurand_string << " to MeasurandEnum";
        }
    }
    return measurands;
}

std::vector<TransactionData>
ChargePointImpl::get_filtered_transaction_data(const std::shared_ptr<Transaction>& transaction) {
    const auto stop_txn_sampled_data_measurands =
        this->get_measurands_vec(this->configuration->getStopTxnSampledData());
    const auto stop_txn_aligned_data_measurands =
        this->get_measurands_vec(this->configuration->getStopTxnAlignedData());

    std::vector<TransactionData> filtered_transaction_data_vec;

    if (!stop_txn_sampled_data_measurands.empty() or !stop_txn_aligned_data_measurands.empty() or
        transaction->get_has_signed_meter_values()) {
        std::vector<TransactionData> transaction_data_vec = transaction->get_transaction_data();
        for (const auto& entry : transaction_data_vec) {
            std::vector<SampledValue> sampled_values;
            for (const auto& meter_value : entry.sampledValue) {
                if (meter_value.measurand.has_value()) {
                    // if Sample.Clock use StopTxnAlignedData
                    if (meter_value.context.has_value() and meter_value.context == ReadingContext::Sample_Clock) {
                        if (std::find(stop_txn_aligned_data_measurands.begin(), stop_txn_aligned_data_measurands.end(),
                                      meter_value.measurand.value()) != stop_txn_aligned_data_measurands.end()) {
                            sampled_values.push_back(meter_value);
                            continue;
                        }
                    } else {
                        // else use StopTxnSampledData although spec is unclear about how to filter other ReadingContext
                        // values like Transaction.Begin , Trigger , etc.
                        if (std::find(stop_txn_sampled_data_measurands.begin(), stop_txn_sampled_data_measurands.end(),
                                      meter_value.measurand.value()) != stop_txn_sampled_data_measurands.end()) {
                            sampled_values.push_back(meter_value);
                            continue;
                        }
                    }
                } else {
                    sampled_values.push_back(meter_value);
                }
            }
            if (!sampled_values.empty()) {
                TransactionData transaction_data;
                transaction_data.timestamp = entry.timestamp;
                transaction_data.sampledValue = sampled_values;
                filtered_transaction_data_vec.push_back(transaction_data);
            }
        }
    }
    return filtered_transaction_data_vec;
}

void ChargePointImpl::on_suspend_charging_ev(int32_t connector, const std::optional<CiString<50>> info) {
    this->status->submit_event(connector, FSMEvent::PauseChargingEV, ocpp::DateTime(), info);
}

void ChargePointImpl::on_suspend_charging_evse(int32_t connector, const std::optional<CiString<50>> info) {
    this->status->submit_event(connector, FSMEvent::PauseChargingEVSE, ocpp::DateTime(), info);
}

void ChargePointImpl::on_resume_charging(int32_t connector) {
    this->status->submit_event(connector, FSMEvent::StartCharging, ocpp::DateTime());
}

void ChargePointImpl::on_error(int32_t connector, const ErrorInfo& error_info) {
    this->status->submit_error(connector, error_info);
}

void ChargePointImpl::on_error_cleared(int32_t connector, const std::string uuid) {
    this->status->submit_error_cleared(connector, uuid);
}

void ChargePointImpl::on_all_errors_cleared(int32_t connector) {
    this->status->submit_all_errors_cleared(connector);
}

void ChargePointImpl::on_log_status_notification(int32_t request_id, std::string log_status) {
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

void ChargePointImpl::on_firmware_update_status_notification(int32_t request_id,
                                                             const FirmwareStatusNotification firmware_update_status) {
    try {
        if (request_id != -1) {
            this->signed_firmware_update_status_notification(
                ocpp::conversions::firmware_status_notification_to_firmware_status_enum_type(firmware_update_status),
                request_id);
        } else {
            this->firmware_status_notification(
                ocpp::conversions::firmware_status_notification_to_firmware_status(firmware_update_status));
        }
    } catch (const std::out_of_range& e) {
        EVLOG_debug << "Could not convert incoming FirmwareStatusNotification to OCPP type";
    }

    if (firmware_update_status == FirmwareStatusNotification::InstallationFailed or
        firmware_update_status == FirmwareStatusNotification::Installed or
        firmware_update_status == FirmwareStatusNotification::InstallVerificationFailed) {
        // Restore connector status, since we did not save to db the status, we can just get the old status back using
        // it
        auto connector_availability = this->database_handler->get_connector_availability();
        for (const auto& [connector, availability] : connector_availability) {
            if (availability == AvailabilityType::Operative) {
                if (connector == 0) {
                    this->status->submit_event(0, FSMEvent::BecomeAvailable, ocpp::DateTime());
                }
                if (this->enable_evse_callback != nullptr) {
                    this->enable_evse_callback(connector);
                }
            }
        }
    }
    if (firmware_update_status == FirmwareStatusNotification::InstallationFailed or
        firmware_update_status == FirmwareStatusNotification::Installed or
        firmware_update_status == FirmwareStatusNotification::InvalidSignature or
        firmware_update_status == FirmwareStatusNotification::InstallVerificationFailed or
        firmware_update_status == FirmwareStatusNotification::DownloadFailed or
        firmware_update_status == FirmwareStatusNotification::InvalidSignature) {
        // Reset status to idle to avoid on trigger message sending an incorrect status
        // Even if we have to retry the firmware update resetting to Idle won't cause an issue since we do not trigger a
        // status notification and we don't have a state machine to block certain state transitions
        if (request_id != -1) {
            this->signed_firmware_status = FirmwareStatusEnumType::Idle;
        } else {
            this->firmware_status = FirmwareStatus::Idle;
        }
    }
}

void ChargePointImpl::diagnostic_status_notification(DiagnosticsStatus status, bool initiated_by_trigger_message) {
    DiagnosticsStatusNotificationRequest req;
    req.status = status;
    this->diagnostics_status = status;

    ocpp::Call<DiagnosticsStatusNotificationRequest> call(req);
    this->message_dispatcher->dispatch_call_async(call, true);
    if (status != DiagnosticsStatus::Uploading) {
        // Reset status to idle to avoid on trigger message sending an incorrect status
        // Even if we have to retry the log upload resetting to Idle won't cause an issue since we do not trigger a
        // status notification and we don't have a state machine to block certain state transitions
        this->diagnostics_status = DiagnosticsStatus::Idle;
    }
}

void ChargePointImpl::firmware_status_notification(FirmwareStatus status, bool initiated_by_trigger_message) {

    EVLOG_debug << "Received FirmwareUpdateStatusNotification with status"
                << conversions::firmware_status_to_string(status);
    FirmwareStatusNotificationRequest req;
    req.status = status;

    // The "Downloaded" status signals a firmware update is pending which will cause connectors to be set inoperative
    // (now or after pending transactions are stopped); in case of a status that signals a failed firmware update this
    // is revoked
    if (status == FirmwareStatus::Downloaded) {
        this->firmware_update_is_pending = true;
    } else if (status == FirmwareStatus::DownloadFailed || status == FirmwareStatus::InstallationFailed) {
        this->firmware_update_is_pending = false;
    }

    this->firmware_status = status;

    ocpp::Call<FirmwareStatusNotificationRequest> call(req);
    this->message_dispatcher->dispatch_call_async(call, initiated_by_trigger_message);

    if (this->firmware_update_is_pending) {
        this->change_all_connectors_to_unavailable_for_firmware_update();
    }
}

void ChargePointImpl::register_enable_evse_callback(const std::function<bool(int32_t connector)>& callback) {
    this->enable_evse_callback = callback;
}

void ChargePointImpl::register_disable_evse_callback(const std::function<bool(int32_t connector)>& callback) {
    this->disable_evse_callback = callback;
}

void ChargePointImpl::register_pause_charging_callback(const std::function<bool(int32_t connector)>& callback) {
    this->pause_charging_callback = callback;
}

void ChargePointImpl::register_resume_charging_callback(const std::function<bool(int32_t connector)>& callback) {
    this->resume_charging_callback = callback;
}

void ChargePointImpl::register_provide_token_callback(
    const std::function<void(const std::string& id_token, std::vector<int32_t> referenced_connectors,
                             bool prevalidated)>& callback) {
    this->provide_token_callback = callback;
}

void ChargePointImpl::register_stop_transaction_callback(
    const std::function<bool(int32_t connector, Reason reason)>& callback) {
    this->stop_transaction_callback = callback;
}

void ChargePointImpl::register_reserve_now_callback(
    const std::function<ReservationStatus(int32_t reservation_id, int32_t connector, ocpp::DateTime expiryDate,
                                          CiString<20> idTag, std::optional<CiString<20>> parent_id)>& callback) {
    this->reserve_now_callback = callback;
}

void ChargePointImpl::register_cancel_reservation_callback(const std::function<bool(int32_t connector)>& callback) {
    this->cancel_reservation_callback = callback;
}

void ChargePointImpl::register_unlock_connector_callback(
    const std::function<UnlockStatus(int32_t connector)>& callback) {
    this->unlock_connector_callback = callback;
}

void ChargePointImpl::register_is_reset_allowed_callback(
    const std::function<bool(const ResetType& reset_type)>& callback) {
    this->is_reset_allowed_callback = callback;
}

void ChargePointImpl::register_reset_callback(const std::function<void(const ResetType& reset_type)>& callback) {
    this->reset_callback = callback;
}

void ChargePointImpl::register_set_system_time_callback(
    const std::function<void(const std::string& system_time)>& callback) {
    this->set_system_time_callback = callback;
}

void ChargePointImpl::register_boot_notification_response_callback(
    const std::function<void(const BootNotificationResponse& boot_notification_response)>& callback) {
    this->boot_notification_response_callback = callback;
}

void ChargePointImpl::register_signal_set_charging_profiles_callback(const std::function<void()>& callback) {
    this->signal_set_charging_profiles_callback = callback;
}

void ChargePointImpl::register_upload_diagnostics_callback(
    const std::function<GetLogResponse(const GetDiagnosticsRequest& request)>& callback) {
    this->upload_diagnostics_callback = callback;
}

void ChargePointImpl::register_update_firmware_callback(
    const std::function<void(const UpdateFirmwareRequest msg)>& callback) {
    this->update_firmware_callback = callback;
}

void ChargePointImpl::register_signed_update_firmware_callback(
    const std::function<UpdateFirmwareStatusEnumType(const SignedUpdateFirmwareRequest msg)>& callback) {
    this->signed_update_firmware_callback = callback;
}

void ChargePointImpl::register_all_connectors_unavailable_callback(const std::function<void()>& callback) {
    this->all_connectors_unavailable_callback = callback;
}

void ChargePointImpl::register_upload_logs_callback(const std::function<GetLogResponse(GetLogRequest req)>& callback) {
    this->upload_logs_callback = callback;
}

void ChargePointImpl::register_set_connection_timeout_callback(
    const std::function<void(int32_t connection_timeout)>& callback) {
    this->set_connection_timeout_callback = callback;
}

void ChargePointImpl::register_connection_state_changed_callback(
    const std::function<void(bool is_connected)>& callback) {
    this->connection_state_changed_callback = callback;
}

void ChargePointImpl::register_get_15118_ev_certificate_response_callback(
    const std::function<void(const int32_t connector,
                             const ocpp::v2::Get15118EVCertificateResponse& certificate_response,
                             const ocpp::v2::CertificateActionEnum& certificate_action)>& callback) {
    this->get_15118_ev_certificate_response_callback = callback;
}

void ChargePointImpl::register_transaction_started_callback(
    const std::function<void(const int32_t connector, const std::string& session_id)>& callback) {
    this->transaction_started_callback = callback;
}

void ChargePointImpl::register_transaction_stopped_callback(
    const std::function<void(const int32_t connector, const std::string& session_id, const int32_t transaction_id)>&
        callback) {
    this->transaction_stopped_callback = callback;
}

void ChargePointImpl::register_transaction_updated_callback(
    const std::function<void(const int32_t connector, const std::string& session_id, const int32_t transaction_id,
                             const IdTagInfo& id_tag_info)>
        callback) {
    this->transaction_updated_callback = callback;
}

void ChargePointImpl::register_configuration_key_changed_callback(
    const CiString<50>& key, const std::function<void(const KeyValue& key_value)>& callback) {
    this->configuration_key_changed_callbacks[key] = callback;
}

void ChargePointImpl::register_generic_configuration_key_changed_callback(
    const std::function<void(const KeyValue& key_value)>& callback) {
    this->generic_configuration_key_changed_callback = callback;
}

void ChargePointImpl::register_security_event_callback(
    const std::function<void(const std::string& type, const std::string& tech_info)>& callback) {
    this->security_event_callback = callback;
}

void ChargePointImpl::register_is_token_reserved_for_connector_callback(
    const std::function<ocpp::ReservationCheckStatus(const int32_t connector, const std::string& id_token)>& callback) {
    this->is_token_reserved_for_connector_callback = callback;
}

void ChargePointImpl::register_session_cost_callback(
    const std::function<DataTransferResponse(const RunningCost& running_cost, const uint32_t number_of_decimals)>&
        session_cost_callback) {
    this->session_cost_callback = session_cost_callback;
}

void ChargePointImpl::register_tariff_message_callback(
    const std::function<DataTransferResponse(const TariffMessage& message)>& tariff_message_callback) {
    this->tariff_message_callback = tariff_message_callback;
}

void ChargePointImpl::register_set_display_message_callback(
    const std::function<DataTransferResponse(const std::vector<DisplayMessage>& display_message)>
        set_display_message_callback) {
    this->set_display_message_callback = set_display_message_callback;
}

void ChargePointImpl::on_reservation_start(int32_t connector) {
    this->status->submit_event(connector, FSMEvent::ReserveConnector, ocpp::DateTime());
}

void ChargePointImpl::on_reservation_end(int32_t connector) {
    this->status->submit_event(connector, FSMEvent::ReservationEnd, ocpp::DateTime());
}

void ChargePointImpl::on_enabled(int32_t connector) {
    this->status->submit_event(connector, FSMEvent::BecomeAvailable, ocpp::DateTime());
}

void ChargePointImpl::on_disabled(int32_t connector) {
    this->status->submit_event(connector, FSMEvent::ChangeAvailabilityToUnavailable, ocpp::DateTime());
}

void ChargePointImpl::on_plugin_timeout(int32_t connector) {
    this->status->submit_event(connector, FSMEvent::TransactionStoppedAndUserActionRequired, ocpp::DateTime(),
                               "ConnectionTimeout");
}

void ChargePointImpl::on_security_event(const CiString<50>& event_type, const std::optional<CiString<255>>& tech_info,
                                        const std::optional<bool>& critical, const std::optional<DateTime>& timestamp) {
    this->securityEventNotification(event_type, tech_info, false, critical, timestamp);
}

ChangeAvailabilityResponse ChargePointImpl::on_change_availability(const ChangeAvailabilityRequest& request) {
    EVLOG_debug << "Received internal ChangeAvailabilityRequest for connector " << request.connectorId << " to state "
                << conversions::availability_type_to_string(request.type);

    ChangeAvailabilityResponse response{};
    std::vector<int32_t> accepted_connector_availability_changes{};

    preprocess_change_availability_request(request, response, accepted_connector_availability_changes);

    if (response.status != AvailabilityStatus::Rejected) {
        execute_connectors_availability_change(accepted_connector_availability_changes, request.type, true);
    }

    return response;
}

GetConfigurationResponse ChargePointImpl::get_configuration_key(const GetConfigurationRequest& request) {
    GetConfigurationResponse response;
    std::vector<KeyValue> configurationKey;
    std::vector<CiString<50>> unknownKey;

    if (!request.key) {
        EVLOG_debug << "empty request, sending all configuration keys...";
        configurationKey = this->configuration->get_all_key_value();
    } else {
        auto keys = request.key.value();
        if (keys.empty()) {
            EVLOG_debug << "key field is empty, sending all configuration keys...";
            configurationKey = this->configuration->get_all_key_value();
        } else {
            EVLOG_debug << "specific requests for some keys";
            for (const auto& key : request.key.value()) {
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
    return response;
}

ConfigurationStatus ChargePointImpl::set_custom_configuration_key(CiString<50> key, CiString<500> value) {
    // attempt to set the custom key
    const auto result = this->configuration->setCustomKey(key, value, true);
    if (result != ConfigurationStatus::Accepted) {
        // return immediately if not "Accepted"
        return result;
    }

    // notify callback if registered and change was accepted
    if (this->configuration_key_changed_callbacks.count(key) and
        this->configuration_key_changed_callbacks[key] != nullptr and result == ConfigurationStatus::Accepted) {
        KeyValue kv = {key, false, value};
        this->configuration_key_changed_callbacks[key](kv);
    }

    return result;
}

} // namespace v16
} // namespace ocpp
