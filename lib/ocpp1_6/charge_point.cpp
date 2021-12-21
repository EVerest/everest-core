// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2021 Pionix GmbH and Contributors to EVerest
#include <thread>

#include <boost/bind/bind.hpp>
#include <date/date.h>
#include <everest/logging.hpp>

#include <ocpp1_6/schemas.hpp>

#include <ocpp1_6/charge_point.hpp>

namespace ocpp1_6 {

ChargePointStateMachine::ChargePointStateMachine(ChargePointStatus initial_state) :
    current_state(initial_state), previous_state(initial_state) {
    this->status_transitions[ChargePointStatus::Available] = {
        {ChargePointStatusTransition::A2_UsageInitiated, ChargePointStatus::Preparing},
        {ChargePointStatusTransition::A3_UsageInitiatedWithoutAuthorization, ChargePointStatus::Charging},
        {ChargePointStatusTransition::A4_UsageInitiatedEVDoesNotStartCharging, ChargePointStatus::SuspendedEV},
        {ChargePointStatusTransition::A5_UsageInitiatedEVSEDoesNotAllowCharging, ChargePointStatus::SuspendedEVSE},
        {ChargePointStatusTransition::A7_ReserveNowReservesConnector, ChargePointStatus::Reserved},
        {ChargePointStatusTransition::A8_ChangeAvailabilityToUnavailable, ChargePointStatus::Unavailable},
        {ChargePointStatusTransition::A9_FaultDetected, ChargePointStatus::Faulted},
    };
    this->status_transitions[ChargePointStatus::Preparing] = {
        {ChargePointStatusTransition::B1_IntendedUsageIsEnded, ChargePointStatus::Available},
        {ChargePointStatusTransition::B3_PrerequisitesForChargingMetAndChargingStarts, ChargePointStatus::Charging},
        {ChargePointStatusTransition::B4_PrerequisitesForChargingMetEVDoesNotStartCharging,
         ChargePointStatus::SuspendedEV},
        {ChargePointStatusTransition::B5_PrerequisitesForChargingMetEVSEDoesNotAllowCharging,
         ChargePointStatus::SuspendedEVSE},
        {ChargePointStatusTransition::B6_TimedOut, ChargePointStatus::Finishing},
        {ChargePointStatusTransition::B9_FaultDetected, ChargePointStatus::Faulted},
    };
    this->status_transitions[ChargePointStatus::Charging] = {
        {ChargePointStatusTransition::C1_ChargingSessionEndsNoUserActionRequired, ChargePointStatus::Available},
        {ChargePointStatusTransition::C4_ChargingStopsUponEVRequest, ChargePointStatus::SuspendedEV},
        {ChargePointStatusTransition::C5_ChargingStopsUponEVSERequest, ChargePointStatus::SuspendedEVSE},
        {ChargePointStatusTransition::C6_TransactionStoppedAndUserActionRequired, ChargePointStatus::Finishing},
        {ChargePointStatusTransition::C8_ChargingSessionEndsNoUserActionRequiredConnectorScheduledToBecomeUnavailable,
         ChargePointStatus::Unavailable},
        {ChargePointStatusTransition::C9_FaultDetected, ChargePointStatus::Faulted},
    };
    this->status_transitions[ChargePointStatus::SuspendedEV] = {
        {ChargePointStatusTransition::D1_ChargingSessionEndsNoUserActionRequired, ChargePointStatus::Available},
        {ChargePointStatusTransition::D3_ChargingResumesUponEVRequest, ChargePointStatus::Charging},
        {ChargePointStatusTransition::D5_ChargingSuspendedByEVSE, ChargePointStatus::SuspendedEVSE},
        {ChargePointStatusTransition::D6_TransactionStoppedNoUserActionRequired, ChargePointStatus::Finishing},
        {ChargePointStatusTransition::D8_ChargingSessionEndsNoUserActionRequiredConnectorScheduledToBecomeUnavailable,
         ChargePointStatus::Unavailable},
        {ChargePointStatusTransition::D9_FaultDetected, ChargePointStatus::Faulted},
    };
    this->status_transitions[ChargePointStatus::SuspendedEVSE] = {
        {ChargePointStatusTransition::E1_ChargingSessionEndsNoUserActionRequired, ChargePointStatus::Available},
        {ChargePointStatusTransition::E3_ChargingResumesEVSERestrictionLifted, ChargePointStatus::Charging},
        {ChargePointStatusTransition::E4_EVSERestrictionLiftedEVDoesNotStartCharging, ChargePointStatus::SuspendedEV},
        {ChargePointStatusTransition::E6_TransactionStoppedAndUserActionRequired, ChargePointStatus::Finishing},
        {ChargePointStatusTransition::E8_ChargingSessionEndsNoUserActionRequiredConnectorScheduledToBecomeUnavailable,
         ChargePointStatus::Unavailable},
        {ChargePointStatusTransition::E9_FaultDetected, ChargePointStatus::Faulted},
    };
    this->status_transitions[ChargePointStatus::Finishing] = {
        {ChargePointStatusTransition::F1_AllUserActionsCompleted, ChargePointStatus::Available},
        {ChargePointStatusTransition::F2_UsersRestartChargingSession, ChargePointStatus::Preparing},
        {ChargePointStatusTransition::F8_AllUserActionsCompletedConnectorScheduledToBecomeUnavailable,
         ChargePointStatus::Unavailable},
        {ChargePointStatusTransition::F9_FaultDetected, ChargePointStatus::Faulted},
    };
    this->status_transitions[ChargePointStatus::Reserved] = {
        {ChargePointStatusTransition::G1_ReservationExpiresOrCancelReservationReceived, ChargePointStatus::Available},
        {ChargePointStatusTransition::G2_ReservationIdentityPresented, ChargePointStatus::Preparing},
        {ChargePointStatusTransition::
             G8_ReservationExpiresOrCancelReservationReceivedConnectorScheduledToBecomeUnavailable,
         ChargePointStatus::Unavailable},
        {ChargePointStatusTransition::G9_FaultDetected, ChargePointStatus::Faulted},
    };
    this->status_transitions[ChargePointStatus::Unavailable] = {
        {ChargePointStatusTransition::H1_ConnectorSetAvailableByChangeAvailability, ChargePointStatus::Available},
        {ChargePointStatusTransition::H2_ConnectorSetAvailableAfterUserInteractedWithChargePoint,
         ChargePointStatus::Preparing},
        {ChargePointStatusTransition::H3_ConnectorSetAvailableNoUserActionRequiredToStartCharging,
         ChargePointStatus::Charging},
        {ChargePointStatusTransition::H4_ConnectorSetAvailableNoUserActionRequiredEVDoesNotStartCharging,
         ChargePointStatus::SuspendedEV},
        {ChargePointStatusTransition::H5_ConnectorSetAvailableNoUserActionRequiredEVSEDoesNotAllowCharging,
         ChargePointStatus::SuspendedEVSE},
        {ChargePointStatusTransition::H9_FaultDetected, ChargePointStatus::Faulted},
    };
    this->status_transitions[ChargePointStatus::Faulted] = {
        {ChargePointStatusTransition::I1_ReturnToAvailable, ChargePointStatus::Available},
        {ChargePointStatusTransition::I2_ReturnToPreparing, ChargePointStatus::Preparing},
        {ChargePointStatusTransition::I3_ReturnToCharging, ChargePointStatus::Charging},
        {ChargePointStatusTransition::I4_ReturnToSuspendedEV, ChargePointStatus::SuspendedEV},
        {ChargePointStatusTransition::I5_ReturnToSuspendedEVSE, ChargePointStatus::SuspendedEVSE},
        {ChargePointStatusTransition::I6_ReturnToFinishing, ChargePointStatus::Finishing},
        {ChargePointStatusTransition::I7_ReturnToReserved, ChargePointStatus::Reserved},
        {ChargePointStatusTransition::I8_ReturnToUnavailable, ChargePointStatus::Unavailable},
    };
}

ChargePointStatus ChargePointStateMachine::modify_state(ChargePointStatus new_state) {
    std::lock_guard<std::mutex> lock(state_mutex);
    this->previous_state = this->current_state;
    this->current_state = new_state;
    return this->current_state;
}

ChargePointStatus ChargePointStateMachine::change_state(ChargePointStatusTransition transition) {
    if (this->status_transitions[this->current_state].count(transition) == 0) {
        return this->fault();
    }
    return this->modify_state(this->status_transitions[this->current_state][transition]);
}

ChargePointStatus ChargePointStateMachine::fault() {
    auto state = this->current_state;
    if (state == ChargePointStatus::Faulted) {
        return state;
    }
    return this->modify_state(ChargePointStatus::Faulted);
}

ChargePointStatus ChargePointStateMachine::fault_resolved() {
    return this->modify_state(this->previous_state);
}

ChargePointStatus ChargePointStateMachine::finishing() {
    auto state = this->current_state;
    if (state == ChargePointStatus::Preparing) {
        return this->change_state(ChargePointStatusTransition::B6_TimedOut);
    }
    if (state == ChargePointStatus::Charging) {
        return this->change_state(ChargePointStatusTransition::C6_TransactionStoppedAndUserActionRequired);
    }
    if (state == ChargePointStatus::SuspendedEV) {
        return this->change_state(ChargePointStatusTransition::D6_TransactionStoppedNoUserActionRequired);
    }
    if (state == ChargePointStatus::SuspendedEVSE) {
        return this->change_state(ChargePointStatusTransition::E6_TransactionStoppedAndUserActionRequired);
    }

    return this->fault(); // FIXME(kai): this might be a bit drastic
}

ChargePointStatus ChargePointStateMachine::suspended_ev() {
    auto state = this->current_state;
    if (state == ChargePointStatus::Available) {
        return this->change_state(ChargePointStatusTransition::A4_UsageInitiatedEVDoesNotStartCharging);
    }
    if (state == ChargePointStatus::Preparing) {
        return this->change_state(ChargePointStatusTransition::B4_PrerequisitesForChargingMetEVDoesNotStartCharging);
    }
    if (state == ChargePointStatus::Charging) {
        return this->change_state(ChargePointStatusTransition::C4_ChargingStopsUponEVRequest);
    }
    if (state == ChargePointStatus::SuspendedEVSE) {
        return this->change_state(ChargePointStatusTransition::E4_EVSERestrictionLiftedEVDoesNotStartCharging);
    }

    return this->fault(); // FIXME(kai): this might be a bit drastic
}

ChargePointStatus ChargePointStateMachine::suspended_evse() {
    auto state = this->current_state;
    if (state == ChargePointStatus::Available) {
        return this->change_state(ChargePointStatusTransition::A5_UsageInitiatedEVSEDoesNotAllowCharging);
    }
    if (state == ChargePointStatus::Preparing) {
        return this->change_state(ChargePointStatusTransition::B5_PrerequisitesForChargingMetEVSEDoesNotAllowCharging);
    }
    if (state == ChargePointStatus::Charging) {
        return this->change_state(ChargePointStatusTransition::C5_ChargingStopsUponEVSERequest);
    }
    if (state == ChargePointStatus::SuspendedEV) {
        return this->change_state(ChargePointStatusTransition::D5_ChargingSuspendedByEVSE);
    }

    return this->fault(); // FIXME(kai): this might be a bit drastic
}

ChargePointStatus ChargePointStateMachine::resume_ev() {
    auto state = this->current_state;
    if (state == ChargePointStatus::SuspendedEV) {
        return this->change_state(ChargePointStatusTransition::D3_ChargingResumesUponEVRequest);
    }

    return this->current_state; // FIXME(kai): is this the expected behavior?
}

ChargePointStatus ChargePointStateMachine::timeout() {
    auto state = this->current_state;
    if (state == ChargePointStatus::Preparing) {
        return this->change_state(ChargePointStatusTransition::B1_IntendedUsageIsEnded);
    }

    return this->current_state; // FIXME(kai): is this the expected behavior?
}

ChargePointStatus ChargePointStateMachine::get_current_state() {
    return this->current_state;
}

ChargePoint::ChargePoint(ChargePointConfiguration* configuration) :
    heartbeat_interval(configuration->getHeartbeatInterval()),
    initialized(false),
    registration_status(RegistrationStatus::Pending) {

    this->configuration = configuration;
    this->connection.charge_point_id = this->configuration->getChargePointId();
    this->connection.state = ChargePointConnectionState::Disconnected;
    this->authorize_handler = nullptr;

    this->message_queue =
        new MessageQueue(this->configuration, [this](json message) -> bool { return this->send(message); });

    this->websocket = new Websocket(this->configuration);
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
        new Everest::SteadyTimer(&this->io_service, [this]() { this->boot_notification(); });

    this->heartbeat_timer = new Everest::SteadyTimer(&this->io_service, [this]() { this->heartbeat(); });

    this->meter_values_sample_timer =
        new Everest::SteadyTimer(&this->io_service, [this]() { this->meter_values_sample(); });

    this->clock_aligned_meter_values_timer =
        new Everest::SystemTimer(&this->io_service, [this]() { this->clock_aligned_meter_values_sample(); });

    auto connector_availability = this->configuration->getConnectorAvailability();
    for (auto connector : connector_availability) {
        if (connector.second == AvailabilityType::Operative) {
            this->status[connector.first] = new ChargePointStateMachine(ChargePointStatus::Available);
        } else {
            this->status[connector.first] = new ChargePointStateMachine(ChargePointStatus::Unavailable);
        }

        ChargingSession session;
        session.plug_connected = false;
        session.connection_timer = new Everest::SteadyTimer(&this->io_service);
        this->charging_session[connector.first] = session;
    }
    this->work = boost::make_shared<boost::asio::io_service::work>(this->io_service);

    this->io_service_thread = std::thread([this]() { this->io_service.run(); });
}

void ChargePoint::heartbeat() {
    EVLOG(debug) << "Sending heartbeat";
    HeartbeatRequest req;

    Call<HeartbeatRequest> call(req, this->message_queue->createMessageId());
    this->send(call);
}

void ChargePoint::boot_notification() {
    EVLOG(debug) << "Sending BootNotification";
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
    this->send(call);
}

void ChargePoint::meter_values_sample() {
    if (this->initialized) {
        EVLOG(debug) << "Sending sampled meter values";
        this->send_meter_values(this->configuration->getMeterValuesSampledDataVector(),
                                ReadingContext::Sample_Periodic);
    }
}

void ChargePoint::meter_values_transaction_sample(int32_t connector) {
    if (this->initialized) {

        std::unique_lock<std::mutex> active_transaction_lock(active_transactions_mutex);

        int32_t interval = this->configuration->getMeterValueSampleInterval();
        if (interval != 0) {
            auto meter_value =
                this->get_latest_meter_value(connector, this->configuration->getMeterValuesSampledDataVector(),
                                             ReadingContext::Sample_Periodic); // FIXME?
            auto sampled_meter_value =
                this->get_latest_meter_value(connector, this->configuration->getStopTxnSampledDataVector(),
                                             ReadingContext::Sample_Periodic); // FIXME?
            this->active_transactions[connector].sampled_meter_values.push_back(sampled_meter_value);
            MeterValuesRequest req;
            req.connectorId = connector;
            req.transactionId.emplace(this->active_transactions[connector].transactionId);
            req.meterValue.push_back(meter_value);

            EVLOG(debug) << "Sending sampled meter values of transaction " << req.transactionId.value()
                         << " at connector " << connector;

            Call<MeterValuesRequest> call(req, this->message_queue->createMessageId());
            this->send(call);
        }
        active_transaction_lock.unlock();
    }
}

void ChargePoint::clock_aligned_meter_values_sample() {
    if (this->initialized) {
        EVLOG(debug) << "Sending clock aligned meter values";
        this->send_meter_values(this->configuration->getMeterValuesAlignedDataVector(), ReadingContext::Sample_Clock);

        std::unique_lock<std::mutex> active_transaction_lock(active_transactions_mutex);
        for (auto active_transaction : this->active_transactions) {
            // TODO(kai): only sample the meter values once and then access & filter these afterwards
            active_transaction.second.clock_aligned_meter_values.push_back(this->get_latest_meter_value(
                active_transaction.first, this->configuration->getStopTxnAlignedDataVector(),
                ReadingContext::Sample_Clock));
        }
        active_transaction_lock.unlock();

        this->update_clock_aligned_meter_values_interval();
    }
}

void ChargePoint::connection_timeout(int32_t connector) {
    if (this->initialized) {
        this->status_notification(connector, ChargePointErrorCode::NoError, this->status[connector]->timeout());
    }
}

void ChargePoint::update_heartbeat_interval() {
    this->heartbeat_timer->interval(std::chrono::seconds(this->configuration->getHeartbeatInterval()));
}

void ChargePoint::update_meter_values_sample_interval() {
    // TODO(kai): should we update the meter values sample interval for running transactions and their associated timers
    // as well?
    int32_t interval = this->configuration->getMeterValueSampleInterval();
    this->meter_values_sample_timer->interval(std::chrono::seconds(interval));
}

void ChargePoint::update_clock_aligned_meter_values_interval() {
    int32_t clock_aligned_data_interval = this->configuration->getClockAlignedDataInterval();
    if (clock_aligned_data_interval == 0) {
        return;
    }
    auto seconds_in_a_day = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::hours(24)).count();
    auto now = std::chrono::system_clock::now();
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
    EVLOG(debug) << oss.str();

    this->clock_aligned_meter_values_timer->at(this->clock_aligned_meter_values_time_point);
}

int32_t ChargePoint::get_meter_wh(int32_t connector) {
    if (this->get_meter_wh_callback == nullptr) {
        throw std::runtime_error(
            "The get_meter_wh_callback has not been registered but is needed to start a transaction");
    }
    return this->get_meter_wh_callback(connector);
}

std::vector<MeterValue> ChargePoint::get_meter_values(int32_t connector,
                                                      std::vector<MeasurandWithPhase> values_of_interest) {
    std::unique_lock<std::mutex> lock(meter_values_mutex);
    std::vector<MeterValue> filtered_meter_values;
    for (auto meter_value : this->meter_values[connector]) {
        MeterValue filtered_meter_value;
        filtered_meter_value.timestamp = meter_value.timestamp;
        // TODO(kai): check if this filtering is too restrictive, should not be the case if the measurands are
        // provided properly
        for (auto sample : meter_value.sampledValue) {
            if (sample.measurand) {
                for (auto configured_measurand : values_of_interest) {
                    if (sample.measurand == configured_measurand.measurand) {
                        if (sample.phase) {
                            if (sample.phase.value() == configured_measurand.phase) {
                                filtered_meter_value.sampledValue.push_back(sample);
                            }
                        } else {
                            filtered_meter_value.sampledValue.push_back(sample);
                        }
                    }
                }
            }
        }
        filtered_meter_values.push_back(filtered_meter_value);
    }
    lock.unlock();
    return filtered_meter_values;
}

MeterValue ChargePoint::get_latest_meter_value(int32_t connector, std::vector<MeasurandWithPhase> values_of_interest,
                                               ReadingContext context) {
    std::unique_lock<std::mutex> lock(power_meter_mutex);
    MeterValue filtered_meter_value;
    if (this->power_meter.count(connector) != 0) {
        auto power_meter_value = this->power_meter[connector];
        auto timestamp =
            std::chrono::system_clock::time_point(std::chrono::seconds(power_meter_value["timestamp"].get<int>()));
        filtered_meter_value.timestamp = DateTime(timestamp);
        EVLOG(debug) << "PowerMeter value for connector: " << connector << ": " << power_meter_value;

        for (auto configured_measurand : values_of_interest) {
            EVLOG(debug) << "Value of interest: " << conversions::measurand_to_string(configured_measurand.measurand);
            // constructing sampled value
            SampledValue sample;

            sample.context.emplace(context);
            sample.format.emplace(ValueFormat::Raw); // TODO(kai): support signed data as well

            sample.measurand.emplace(configured_measurand.measurand);
            if (configured_measurand.phase) {
                EVLOG(debug) << "  there is a phase configured: "
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
                            Conversions::double_to_string((double)power_meter_value["energy_Wh_import"]["L1"]);
                    }
                    if (phase == Phase::L2) {
                        sample.value =
                            Conversions::double_to_string((double)power_meter_value["energy_Wh_import"]["L2"]);
                    }
                    if (phase == Phase::L3) {
                        sample.value =
                            Conversions::double_to_string((double)power_meter_value["energy_Wh_import"]["L3"]);
                    }
                    filtered_meter_value.sampledValue.push_back(sample);
                } else {
                    // store total value
                    sample.value =
                        Conversions::double_to_string((double)power_meter_value["energy_Wh_import"]["total"]);
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
                            Conversions::double_to_string((double)power_meter_value["energy_Wh_export"]["L1"]);
                    }
                    if (phase == Phase::L2) {
                        sample.value =
                            Conversions::double_to_string((double)power_meter_value["energy_Wh_export"]["L2"]);
                    }
                    if (phase == Phase::L3) {
                        sample.value =
                            Conversions::double_to_string((double)power_meter_value["energy_Wh_export"]["L3"]);
                    }
                    filtered_meter_value.sampledValue.push_back(sample);
                } else {
                    // store total value
                    sample.value =
                        Conversions::double_to_string((double)power_meter_value["energy_Wh_export"]["total"]);
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
                        sample.value = Conversions::double_to_string((double)power_meter_value["power_W"]["L1"]);
                    }
                    if (phase == Phase::L2) {
                        sample.value = Conversions::double_to_string((double)power_meter_value["power_W"]["L2"]);
                    }
                    if (phase == Phase::L3) {
                        sample.value = Conversions::double_to_string((double)power_meter_value["power_W"]["L3"]);
                    }
                    filtered_meter_value.sampledValue.push_back(sample);
                } else {
                    // store total value
                    sample.value = Conversions::double_to_string((double)power_meter_value["power_W"]["total"]);
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
                        sample.value = Conversions::double_to_string((double)power_meter_value["voltage_V"]["L1"]);
                    }
                    if (phase == Phase::L2) {
                        sample.value = Conversions::double_to_string((double)power_meter_value["voltage_V"]["L2"]);
                    }
                    if (phase == Phase::L3) {
                        sample.value = Conversions::double_to_string((double)power_meter_value["voltage_V"]["L3"]);
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
                        sample.value = Conversions::double_to_string((double)power_meter_value["current_A"]["L1"]);
                    }
                    if (phase == Phase::L2) {
                        sample.value = Conversions::double_to_string((double)power_meter_value["current_A"]["L2"]);
                    }
                    if (phase == Phase::L3) {
                        sample.value = Conversions::double_to_string((double)power_meter_value["current_A"]["L3"]);
                    }
                    if (phase == Phase::N) {
                        sample.value = Conversions::double_to_string((double)power_meter_value["current_A"]["LN3"]);
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
                        sample.value = Conversions::double_to_string((double)power_meter_value["frequency_Hz"]["L1"]);
                    }
                    if (phase == Phase::L2) {
                        sample.value = Conversions::double_to_string((double)power_meter_value["frequency_Hz"]["L2"]);
                    }
                    if (phase == Phase::L3) {
                        sample.value = Conversions::double_to_string((double)power_meter_value["frequency_Hz"]["L3"]);
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
                    EVLOG(error) << "No max current offered for connector " << connector
                                 << " yet, skipping meter value";
                    break;
                }

                // FIXME: this information should come from the (abstract) charger, not the power meter!
                sample.value = Conversions::double_to_string(this->max_current_offered[connector]);

                filtered_meter_value.sampledValue.push_back(sample);
                break;

            default:
                break;
            }
        }
    }
    lock.unlock();
    return filtered_meter_value;
}

void ChargePoint::send_meter_values(std::vector<MeasurandWithPhase> values_of_interest, ReadingContext context) {
    for (int32_t connector = 0; connector <= this->configuration->getNumberOfConnectors(); connector++) {
        std::unique_lock<std::mutex> running_transaction_lock(active_transactions_mutex);
        bool active_transaction = this->active_transactions.count(connector) != 0;
        running_transaction_lock.unlock();

        if (!active_transaction) {
            this->send_meter_values(connector, values_of_interest, context);
        }
    }
}

void ChargePoint::send_meter_values(int32_t connector, std::vector<MeasurandWithPhase> values_of_interest,
                                    ReadingContext context) {
    MeterValuesRequest req;
    // connector = 0 designates the main powermeter
    // connector > 0 designates a connector of the charge point
    req.connectorId = connector;
    std::ostringstream oss;
    oss << "Gathering measurands of connector: " << connector;
    if (connector > 0) {
        auto transaction_id = this->get_transaction_id(connector);
        if (transaction_id) {
            oss << " with active transaction: " << transaction_id.value();
            req.transactionId.emplace(transaction_id.value());
        }
    }

    EVLOG(debug) << oss.str();

    req.meterValue.push_back(this->get_latest_meter_value(connector, values_of_interest, context));

    Call<MeterValuesRequest> call(req, this->message_queue->createMessageId());
    this->send(call);
}

boost::optional<int32_t> ChargePoint::get_transaction_id(int32_t connector) {
    boost::optional<int32_t> transaction_id;
    std::unique_lock<std::mutex> running_transaction_lock(active_transactions_mutex);

    if (this->active_transactions.count(connector) != 0) {
        transaction_id.emplace(this->active_transactions[connector].transactionId);
    }
    running_transaction_lock.unlock();
    return transaction_id;
}

void ChargePoint::start() {
    this->websocket->connect();
}

void ChargePoint::stop_all_transactions() {
    this->stop_all_transactions(Reason::Other);
}

void ChargePoint::stop_all_transactions(Reason reason) {
    std::vector<CiString20Type> idTags;
    std::vector<int32_t> transactionIds;
    std::vector<int32_t> connectors;
    std::unique_lock<std::mutex> active_transaction_lock(active_transactions_mutex);
    for (auto active_transaction : this->active_transactions) {
        // TODO(kai): stop transaction if possible, this probably needs an overhaul of the active_transactions locking,
        // which is needed for handling resets as well
        if (active_transaction.second.meter_values_sample_timer != nullptr) {
            active_transaction.second.meter_values_sample_timer->stop();
        }
        idTags.push_back(active_transaction.second.idTag);

        transactionIds.push_back(active_transaction.second.transactionId);

        connectors.push_back(active_transaction.first);
    }
    active_transaction_lock.unlock();
    EVLOG(error) << "hacky stopping of transactions";
    for (size_t i = 0; i < idTags.size(); i++) {
        this->stop_transaction(idTags.at(i), this->get_meter_wh(connectors.at(i)), transactionIds.at(i), reason);
    }
}

void ChargePoint::stop() {
    EVLOG(info) << "Closing";
    this->initialized = false;
    if (this->boot_notification_timer != nullptr) {
        this->boot_notification_timer->stop();
    }
    if (this->heartbeat_timer != nullptr) {
        this->heartbeat_timer->stop();
    }
    if (this->meter_values_sample_timer != nullptr) {
        this->meter_values_sample_timer->stop();
    }
    if (this->clock_aligned_meter_values_timer != nullptr) {
        this->clock_aligned_meter_values_timer->stop();
    }
    // FIXME(kai): check if this can be done properly
    this->stop_all_transactions();

    this->message_queue->stop();

    this->websocket->disconnect();
    this->work.reset();
    this->io_service.stop();
    this->io_service_thread.join();

    this->configuration->stop();
    EVLOG(info) << "Terminating...";
}

void ChargePoint::connected_callback() {
    switch (this->connection.state) {
    case ChargePointConnectionState::Disconnected: {
        this->connection.state = ChargePointConnectionState::Connected;
        this->boot_notification();
        break;
    }
    case ChargePointConnectionState::Booted: {
        // on_open in a Booted state can happen after a successful reconnect.
        // according to spec, a charge point should not send a BootNotification after a reconnect
        break;
    }
    default:
        EVLOG(error) << "Connected but not in state 'Disconnected' or 'Booted', something is wrong: "
                     << this->connection.state;
        break;
    }
}

void ChargePoint::message_callback(const std::string& message) {
    EVLOG(debug) << "Received Message: " << message;

    // EVLOG(debug) << "json message: " << json_message;
    auto enhanced_message = this->message_queue->receive(message);
    auto json_message = enhanced_message.message;
    // reject unsupported messages
    if (this->configuration->getSupportedMessageTypesReceiving().count(enhanced_message.messageType) == 0) {
        EVLOG(warning) << "Received an unsupported message: " << enhanced_message.messageType;
        // FIXME(kai): however, only send a CALLERROR when it is a CALL message we just received
        if (enhanced_message.messageTypeId == MessageTypeId::CALL) {
            auto call_error = CallError(enhanced_message.uniqueId, "NotSupported", "", json({}));
            this->send(call_error);
        }

        // in any case stop message handling here:
        return;
    }

    switch (this->connection.state) {
    case ChargePointConnectionState::Disconnected: {
        EVLOG(error) << "Received a message in disconnected state, this cannot be correct";
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
            break;
        }
    }
    case ChargePointConnectionState::Booted: {
        // lots of messages are allowed here
        switch (enhanced_message.messageType) {

        case MessageType::AuthorizeResponse:
            // this->handleAuthorizeResponse(json_message, enhanced_message.call_message);
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
            // this->handleDataTransferResponse(json_message);
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

        default:
            // TODO(kai): not implemented error?
            break;
        }

        break;
    }

    default:
        EVLOG(error) << "Reached default statement in on_message, this should not be possible";
        break;
    }
}

void ChargePoint::handleAuthorizeResponse(CallResult<AuthorizeResponse> call_result,
                                          Call<AuthorizeRequest> call_request) {
    EVLOG(debug) << "Received AuthorizeResponse: " << call_result.msg << "\nwith messageId: " << call_result.uniqueId;

    auto idTag = call_request.msg.idTag;
    this->configuration->updateAuthorizationCacheEntry(idTag, call_result.msg.idTagInfo);

    // FIXME(kai): proper result
    if (this->authorize_handler) {
        this->authorize_handler(call_result.msg.idTagInfo.status == AuthorizationStatus::Accepted);
    }
}

void ChargePoint::handleBootNotificationResponse(CallResult<BootNotificationResponse> call_result) {
    EVLOG(debug) << "Received BootNotificationResponse: " << call_result.msg
                 << "\nwith messageId: " << call_result.uniqueId;

    this->registration_status = call_result.msg.status;
    this->initialized = true;
    this->boot_time = std::chrono::system_clock::now();
    if (call_result.msg.interval > 0) {
        this->configuration->setHeartbeatInterval(call_result.msg.interval);
    }
    switch (call_result.msg.status) {
    case RegistrationStatus::Accepted:
        this->connection.state = ChargePointConnectionState::Booted;
        // we are allowed to send messages to the central system
        // activate heartbeat
        this->update_heartbeat_interval();

        // activate sampling of meter values
        this->update_meter_values_sample_interval();

        // activate clock aligned sampling of meter values
        this->update_clock_aligned_meter_values_interval();

        for (auto connector_status : this->status) {
            this->status_notification(connector_status.first, ChargePointErrorCode::NoError,
                                      connector_status.second->get_current_state());
        }

        break;
    case RegistrationStatus::Pending:
        this->connection.state = ChargePointConnectionState::Booted;
        // TODO(kai):, theoretically we are in the Booted state because the central system can send us
        // any message it wants...
        break;
    default:
        this->connection.state = ChargePointConnectionState::Rejected;
        // In this state we are not allowed to send any messages to the central system, even when
        // requested. The first time we are allowed to send a message (a BootNotification) is
        // after boot_time + heartbeat_interval if the msg.interval is 0, or after boot_timer + msg.interval
        EVLOG(debug) << "BootNotification was rejected, trying again in " << this->configuration->getHeartbeatInterval()
                     << "s";

        this->boot_notification_timer->timeout(std::chrono::seconds(this->configuration->getHeartbeatInterval()));

        break;
    }
}
void ChargePoint::handleChangeAvailabilityRequest(Call<ChangeAvailabilityRequest> call) {
    EVLOG(debug) << "Received ChangeAvailabilityRequest: " << call.msg << "\nwith messageId: " << call.uniqueId;

    ChangeAvailabilityResponse response;
    // we can only change the connector availability if there is no active transaction on this
    // connector. is that case this change must be scheduled and we should report an availability status
    // of "Scheduled"

    std::vector<int32_t> connectors;
    bool transaction_running = false;
    std::unique_lock<std::mutex> running_transaction_lock(active_transactions_mutex);
    if (call.msg.connectorId == 0) {
        int32_t number_of_connectors = this->configuration->getNumberOfConnectors();
        for (int32_t connector = 1; connector <= number_of_connectors; connector++) {
            if (this->active_transactions.count(connector) != 0) {
                transaction_running = true;
                std::unique_lock<std::mutex> change_availability_lock(change_availability_mutex);
                this->change_availability_queue[connector] = call.msg.type;
                change_availability_lock.unlock();
            } else {
                connectors.push_back(connector);
            }
        }
    } else {
        connectors.push_back(call.msg.connectorId);
    }
    running_transaction_lock.unlock();

    if (transaction_running) {
        response.status = AvailabilityStatus::Scheduled;
    } else {
        for (auto connector : connectors) {
            bool availability_change_succeeded =
                this->configuration->setConnectorAvailability(connector, call.msg.type);
            if (availability_change_succeeded) {
                if (call.msg.type == AvailabilityType::Operative) {
                    this->status[connector]->change_state(
                        ChargePointStatusTransition::H1_ConnectorSetAvailableByChangeAvailability); // TODO(kai): rework
                                                                                                    // state transitions
                    this->status_notification(connector, ChargePointErrorCode::NoError, ChargePointStatus::Available);
                } else {
                    this->status[connector]->change_state(
                        ChargePointStatusTransition::A8_ChangeAvailabilityToUnavailable);
                    this->status_notification(connector, ChargePointErrorCode::NoError, ChargePointStatus::Unavailable);
                }
            }
        }

        response.status = AvailabilityStatus::Accepted;
    }

    CallResult<ChangeAvailabilityResponse> call_result(response, call.uniqueId);
    this->send(call_result);
}

void ChargePoint::handleChangeConfigurationRequest(Call<ChangeConfigurationRequest> call) {
    EVLOG(debug) << "Received ChangeConfigurationRequest: " << call.msg << "\nwith messageId: " << call.uniqueId;

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
    this->send(call_result);
}

void ChargePoint::handleClearCacheRequest(Call<ClearCacheRequest> call) {
    EVLOG(debug) << "Received ClearCacheRequest: " << call.msg << "\nwith messageId: " << call.uniqueId;

    ClearCacheResponse response;

    if (this->configuration->clearAuthorizationCache()) {
        response.status = ClearCacheStatus::Accepted;
    } else {
        response.status = ClearCacheStatus::Rejected;
    }

    CallResult<ClearCacheResponse> call_result(response, call.uniqueId);
    this->send(call_result);
}

void ChargePoint::handleDataTransferRequest(Call<DataTransferRequest> call) {
    EVLOG(debug) << "Received DataTransferRequest: " << call.msg << "\nwith messageId: " << call.uniqueId;

    // TODO(kai): here we can implement vendor specific extensions

    DataTransferResponse response;

    // TODO(kai): for now we reject any message sent this way with an unknown vendor id
    response.status = DataTransferStatus::UnknownVendorId;

    CallResult<DataTransferResponse> call_result(response, call.uniqueId);
    this->send(call_result);
}

void ChargePoint::handleGetConfigurationRequest(Call<GetConfigurationRequest> call) {
    EVLOG(debug) << "Received GetConfigurationRequest: " << call.msg << "\nwith messageId: " << call.uniqueId;

    GetConfigurationResponse response;
    std::vector<KeyValue> configurationKey;
    std::vector<CiString50Type> unknownKey;

    if (!call.msg.key) {
        EVLOG(debug) << "empty request, sending all configuration keys...";
        configurationKey = this->configuration->get_all_key_value();
    } else {
        auto keys = call.msg.key.value();
        if (keys.empty()) {
            EVLOG(debug) << "key field is empty, sending all configuration keys...";
            configurationKey = this->configuration->get_all_key_value();
        } else {
            EVLOG(debug) << "specific requests for some keys";
            for (const auto& key : call.msg.key.value()) {
                EVLOG(debug) << "retrieving key: " << key;
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
    this->send(call_result);
}

void ChargePoint::handleRemoteStartTransactionRequest(Call<RemoteStartTransactionRequest> call) {
    EVLOG(debug) << "Received RemoteStartTransactionRequest: " << call.msg << "\nwith messageId: " << call.uniqueId;

    // a charge point may reject a remote start transaction request without a connectorId
    // TODO(kai): what is our policy here? reject for now
    RemoteStartTransactionResponse response;
    if (!call.msg.connectorId) {
        response.status = RemoteStartStopStatus::Rejected;
        CallResult<RemoteStartTransactionResponse> call_result(response, call.uniqueId);
        this->send(call_result);
        return;
    }
    int32_t connector = call.msg.connectorId.value();
    if (this->configuration->getConnectorAvailability(connector) == AvailabilityType::Inoperative) {
        response.status = RemoteStartStopStatus::Rejected;
        CallResult<RemoteStartTransactionResponse> call_result(response, call.uniqueId);
        this->send(call_result);
        return;
    }
    if (call.msg.chargingProfile) {
        // TODO(kai): A charging profile was provided, forward to the charger
    }

    response.status = RemoteStartStopStatus::Accepted;
    CallResult<RemoteStartTransactionResponse> call_result(response, call.uniqueId);
    this->send(call_result);

    std::unique_lock<std::mutex> lock(remote_start_transaction_mutex);
    this->remote_start_transaction[call.uniqueId] = std::thread([this, call, connector]() {
        bool authorized = true;
        if (this->configuration->getAuthorizeRemoteTxRequests()) {
            // need to authorize first
            authorized = (this->authorize_id_tag(call.msg.idTag) == AuthorizationStatus::Accepted);
        }
        if (authorized) {
            this->start_transaction(connector, call.msg.idTag);
        }
    });
    lock.unlock();
}

void ChargePoint::handleRemoteStopTransactionRequest(Call<RemoteStopTransactionRequest> call) {
    EVLOG(debug) << "Received RemoteStopTransactionRequest: " << call.msg << "\nwith messageId: " << call.uniqueId;

    RemoteStopTransactionResponse response;
    response.status = RemoteStartStopStatus::Accepted;
    CallResult<RemoteStopTransactionResponse> call_result(response, call.uniqueId);
    this->send(call_result);

    std::unique_lock<std::mutex> running_transaction_lock(active_transactions_mutex);

    int32_t connector = this->transaction_at_connector[call.msg.transactionId];
    bool transaction_running = this->active_transactions.count(connector) != 0;
    CiString20Type idTag;
    if (transaction_running) {
        idTag = this->active_transactions[connector].idTag;
    }
    running_transaction_lock.unlock();

    if (transaction_running) {
        std::unique_lock<std::mutex> lock(remote_stop_transaction_mutex);

        this->remote_stop_transaction[call.uniqueId] = std::thread([this, idTag, connector, call]() {
            this->stop_transaction(idTag, this->get_meter_wh(connector), call.msg.transactionId, Reason::Remote);
        });
        lock.unlock();
    }
}

void ChargePoint::handleResetRequest(Call<ResetRequest> call) {
    EVLOG(debug) << "Received ResetRequest: " << call.msg << "\nwith messageId: " << call.uniqueId;

    ResetResponse response;
    response.status = ResetStatus::Accepted;

    CallResult<ResetResponse> call_result(response, call.uniqueId);
    this->send(call_result);

    if (call.msg.type == ResetType::Hard) {
        // TODO(kai): implement hard reset, if possible send StopTransaction for any running
        // transactions This type of reset should restart all hardware!
        this->reset_thread = std::thread([this]() {
            kill(getpid(), SIGINT); // FIXME(kai): this leads to a somewhat dirty reset
        });
    }
    if (call.msg.type == ResetType::Soft) {
        // TODO(kai): gracefully stop all transactions and send StopTransaction. Restart software
        // afterwards
        this->reset_thread = std::thread([this]() {
            kill(getpid(), SIGINT); // FIXME(kai): this leads to a somewhat dirty reset
        });
    }
}

void ChargePoint::handleStartTransactionResponse(CallResult<StartTransactionResponse> call_result) {
    EVLOG(debug) << "Received StartTransactionResponse: " << call_result.msg
                 << "\nwith messageId: " << call_result.uniqueId;

    std::unique_lock<std::mutex> lock(start_transaction_handler_mutex);
    this->start_transaction_handler[call_result.uniqueId](call_result.msg);
    lock.unlock();
}

void ChargePoint::handleStopTransactionResponse(CallResult<StopTransactionResponse> call_result) {
    EVLOG(debug) << "Received StopTransactionResponse: " << call_result.msg
                 << "\nwith messageId: " << call_result.uniqueId;

    std::unique_lock<std::mutex> lock(stop_transaction_handler_mutex);
    this->stop_transaction_handler[call_result.uniqueId](call_result.msg);
    lock.unlock();
}

void ChargePoint::handleUnlockConnectorRequest(Call<UnlockConnectorRequest> call) {
    EVLOG(debug) << "Received UnlockConnectorRequest: " << call.msg << "\nwith messageId: " << call.uniqueId;

    UnlockConnectorResponse response;
    if (call.msg.connectorId == 0 || call.msg.connectorId > this->configuration->getNumberOfConnectors()) {
        response.status = UnlockStatus::NotSupported;
    } else {
        // this message is not intended to remotely stop a transaction, but if a transaction is still ongoing it is
        // advised to stop it first
        std::unique_lock<std::mutex> running_transaction_lock(active_transactions_mutex);
        bool transaction_running = this->active_transactions.count(call.msg.connectorId) != 0;
        CiString20Type idTag;
        int32_t transactionId;
        if (transaction_running) {
            idTag = this->active_transactions[call.msg.connectorId].idTag;
            transactionId = this->active_transactions[call.msg.connectorId].transactionId;
        }
        running_transaction_lock.unlock();
        if (transaction_running) {
            this->stop_transaction(idTag, this->get_meter_wh(call.msg.connectorId), transactionId,
                                   Reason::UnlockCommand);
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
    this->send(call_result);
}

void ChargePoint::handleSetChargingProfileRequest(Call<SetChargingProfileRequest> call) {
    EVLOG(debug) << "Received SetChargingProfileRequest: " << call.msg << "\nwith messageId: " << call.uniqueId;

    SetChargingProfileResponse response;
    auto number_of_connectors = this->configuration->getNumberOfConnectors();
    if (call.msg.connectorId > number_of_connectors || call.msg.csChargingProfiles.stackLevel < 0 ||
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
                EVLOG(debug) << "Received ChargePointMaxProfile Charging Profile:\n    " << call.msg.csChargingProfiles;
                if (call.msg.csChargingProfiles.chargingProfileKind == ChargingProfileKindType::Relative) {
                    response.status = ChargingProfileStatus::Rejected;
                } else {
                    // FIXME(kai): only allow absolute or recurring profiles here since we do not really know what a
                    // relative profile in this situation could be relative to
                    if (call.msg.csChargingProfiles.chargingProfileKind == ChargingProfileKindType::Absolute) {
                        if (!call.msg.csChargingProfiles.chargingSchedule.startSchedule) {
                            response.status = ChargingProfileStatus::Rejected;
                        } else {
                            // we only accept absolute schedules with a start schedule
                            // TODO: make sure that the start schedule is between validFrom and validTo
                            std::unique_lock<std::mutex> charge_point_max_profiles_lock(
                                charge_point_max_profiles_mutex);
                            this->charge_point_max_profiles[call.msg.csChargingProfiles.stackLevel] =
                                call.msg.csChargingProfiles;
                            charge_point_max_profiles_mutex.unlock();
                            response.status = ChargingProfileStatus::Accepted;
                        }
                    } else if (call.msg.csChargingProfiles.chargingProfileKind == ChargingProfileKindType::Recurring) {
                        if (!call.msg.csChargingProfiles.recurrencyKind) {
                            response.status = ChargingProfileStatus::Rejected;
                        } else {
                            // we only accept recurring schedules with a recurrency kind
                            // here we don't neccesarily need a startSchedule, but if it is missing we just assume
                            // 0:00:00 UTC today
                            auto midnight = date::floor<date::days>(std::chrono::system_clock::now());
                            auto start_schedule = DateTime(midnight);
                            if (!call.msg.csChargingProfiles.chargingSchedule.startSchedule) {
                                EVLOG(debug)
                                    << "No startSchedule provided for a recurring charging profile, setting to "
                                    << start_schedule << " (midnight today)";
                                call.msg.csChargingProfiles.chargingSchedule.startSchedule.emplace(start_schedule);
                            }
                            std::unique_lock<std::mutex> charge_point_max_profiles_lock(
                                charge_point_max_profiles_mutex);
                            this->charge_point_max_profiles[call.msg.csChargingProfiles.stackLevel] =
                                call.msg.csChargingProfiles;
                            charge_point_max_profiles_mutex.unlock();
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
            std::unique_lock<std::mutex> tx_default_profiles_lock(tx_default_profiles_mutex);
            if (call.msg.connectorId == 0) {
                for (int32_t connector = 1; connector < number_of_connectors; connector++) {
                    this->tx_default_profiles[connector][call.msg.csChargingProfiles.stackLevel] =
                        call.msg.csChargingProfiles;
                }
            } else {
                this->tx_default_profiles[call.msg.connectorId][call.msg.csChargingProfiles.stackLevel] =
                    call.msg.csChargingProfiles;
            }
            tx_default_profiles_mutex.unlock();
            response.status = ChargingProfileStatus::Accepted;
        }
        if (call.msg.csChargingProfiles.chargingProfilePurpose == ChargingProfilePurposeType::TxProfile) {
            if (call.msg.connectorId == 0) {
                response.status = ChargingProfileStatus::Rejected;
            } else {
                // shall overrule a TxDefaultProfile for the duration of the running transaction
                // if there is no running transaction on the specified connector this change shall be discarded
                std::unique_lock<std::mutex> running_transaction_lock(active_transactions_mutex);
                bool transaction_running = this->active_transactions.count(call.msg.connectorId) != 0;
                if (!transaction_running) {
                    response.status = ChargingProfileStatus::Rejected;
                } else {
                    this->active_transactions[call.msg.connectorId].tx_charging_profile.emplace(
                        call.msg.csChargingProfiles);
                    response.status = ChargingProfileStatus::Accepted;
                }
                running_transaction_lock.unlock();
            }
        }
    }

    CallResult<SetChargingProfileResponse> call_result(response, call.uniqueId);
    this->send(call_result);
}

void ChargePoint::handleGetCompositeScheduleRequest(Call<GetCompositeScheduleRequest> call) {
    EVLOG(debug) << "Received GetCompositeScheduleRequest: " << call.msg << "\nwith messageId: " << call.uniqueId;

    GetCompositeScheduleResponse response;

    auto number_of_connectors = this->configuration->getNumberOfConnectors();
    if (call.msg.connectorId > number_of_connectors || call.msg.connectorId < 0) {
        response.status = GetCompositeScheduleStatus::Rejected;
    } else {
        auto start_time = std::chrono::system_clock::now();
        auto start_datetime = DateTime(start_time);
        auto midnight_of_start_time = date::floor<date::days>(start_time);
        auto duration = std::chrono::seconds(call.msg.duration);
        auto end_time = start_time + duration;
        auto end_datetime = DateTime(end_time);

        using date::operator<<;
        std::ostringstream oss;
        oss << "Calculating composite schedule from " << start_time << " to " << end_time;
        EVLOG(debug) << oss.str();
        if (call.msg.connectorId == 0) {
            EVLOG(debug) << "Calculate expected consumption for the grid connection";
            response.scheduleStart.emplace(start_datetime);
            ChargingSchedule composite_schedule; // = get_composite_schedule(start_time, duration,
                                                 // call.msg.chargingRateUnit); // TODO
            composite_schedule.duration.emplace(call.msg.duration);
            composite_schedule.startSchedule.emplace(start_datetime);
            if (call.msg.chargingRateUnit) {
                composite_schedule.chargingRateUnit = call.msg.chargingRateUnit.value();
            } else {
                composite_schedule.chargingRateUnit = ChargingRateUnit::W; // TODO default & conversion
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
                    if (start_time > profile.second.validTo.value()) {
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
                EVLOG(debug) << validity_oss.str();

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
                        // EVLOG(debug) << "valid: " << p;
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
                            EVLOG(error) << "ERROR we do not know when the schedule should start, this should not be "
                                            "possible...";
                            continue;
                        }

                        auto start_schedule = p.chargingSchedule.startSchedule.value().to_time_point();
                        if (p.chargingProfileKind == ChargingProfileKindType::Recurring) {
                            // TODO(kai): special handling of recurring charging profiles!
                            if (!p.recurrencyKind) {
                                EVLOG(warning)
                                    << "Recurring charging profile without a recurreny kind is not supported";
                                continue;
                            }
                            if (!p.chargingSchedule.startSchedule) {
                                EVLOG(warning)
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
                            EVLOG(error)
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
    this->send(call_result);
}

void ChargePoint::handleClearChargingProfileRequest(Call<ClearChargingProfileRequest> call) {
    EVLOG(debug) << "Received ClearChargingProfileRequest: " << call.msg << "\nwith messageId: " << call.uniqueId;

    ClearChargingProfileResponse response;
    response.status = ClearChargingProfileStatus::Accepted;

    CallResult<ClearChargingProfileResponse> call_result(response, call.uniqueId);
    this->send(call_result);
}

template <class T> bool ChargePoint::send(Call<T> call) {
    this->message_queue->push(call);
    return true;
}

template <class T> std::future<EnhancedMessage> ChargePoint::send_async(Call<T> call) {
    return this->message_queue->push_async(call);
}

template <class T> bool ChargePoint::send(CallResult<T> call_result) {
    return this->send(json(call_result));
}

bool ChargePoint::send(json message) {
    std::chrono::system_clock::time_point retry_time =
        this->boot_time +
        std::chrono::seconds(
            this->configuration->getHeartbeatInterval()); // TODO(kai): member variable, persistent or not?
    if (this->initialized && this->registration_status == RegistrationStatus::Rejected &&
        std::chrono::system_clock::now() < retry_time) {
        using date::operator<<;
        std::ostringstream oss;
        oss << "status is rejected and retry time not reached. Messages can be sent again at: " << retry_time;
        EVLOG(debug) << oss.str();
        return false;
    }
    if (this->initialized && this->registration_status == RegistrationStatus::Pending &&
        message.at(MESSAGE_TYPE_ID) == MessageTypeId::CALL) {
        std::string message_type = message.at(CALL_ACTION);

        if (this->allowed_message_types.find(message_type) == this->allowed_message_types.end()) {
            EVLOG(debug) << "registration_status is pending, but message: " << message_type
                         << " was not requested by central system";
            return false;
        }
    }

    return this->websocket->send(message.dump());
}

void ChargePoint::receive_power_meter(int32_t connector, json powermeter) {
    EVLOG(debug) << "updating power meter for connector: " << connector;
    std::lock_guard<std::mutex> lock(power_meter_mutex);
    this->power_meter[connector] = powermeter;
}

void ChargePoint::receive_max_current_offered(int32_t connector, double max_current) {
    std::lock_guard<std::mutex> lock(power_meter_mutex);
    // TODO(kai): uses power meter mutex because the reading context is similar, think about storing this information in
    // a unified struct
    this->max_current_offered[connector] = max_current;
}

ActiveTransaction ChargePoint::start_transaction(int32_t connector, CiString20Type idTag) {
    std::unique_lock<std::mutex> running_transaction_lock(active_transactions_mutex);
    bool transaction_running = this->active_transactions.count(connector) != 0;
    running_transaction_lock.unlock();
    if (transaction_running) {
        throw std::runtime_error("There is already a transaction running for connector " + std::to_string(connector));
    }
    AvailabilityType connector_availability = this->configuration->getConnectorAvailability(connector);
    if (connector_availability == AvailabilityType::Inoperative) {
        throw std::runtime_error("Connector " + std::to_string(connector) +
                                 " is unavailable, it is not possible to start a transaction");
    }
    // TODO(kai): queueing of transaction messages, retransmission on failure
    // TODO(kai): check if there is a transaction already running on that connector
    std::promise<StartTransactionResponse> start_transaction_promise;
    std::future<StartTransactionResponse> start_transaction_future = start_transaction_promise.get_future();

    StartTransactionRequest req;
    req.connectorId = connector;
    req.idTag = idTag;
    req.meterStart = this->get_meter_wh(connector);
    req.timestamp = DateTime();

    Call<StartTransactionRequest> call(req, this->message_queue->createMessageId());

    std::unique_lock<std::mutex> lock(start_transaction_handler_mutex);
    this->start_transaction_handler[call.uniqueId] = [this, &start_transaction_promise](StartTransactionResponse data) {
        start_transaction_promise.set_value(data);
    };
    lock.unlock();

    this->send(call);

    std::chrono::system_clock::time_point start_transaction_wait =
        std::chrono::system_clock::now() + future_wait_seconds;
    std::future_status start_transaction_future_status;
    do {
        start_transaction_future_status = start_transaction_future.wait_until(start_transaction_wait);
    } while (start_transaction_future_status == std::future_status::deferred);
    if (start_transaction_future_status == std::future_status::timeout) {
        EVLOG(error) << "start transaction future timeout";
    } else if (start_transaction_future_status == std::future_status::ready) {
        EVLOG(debug) << "start_transaction future ready";
    }

    std::unique_lock<std::mutex> erase_lock(start_transaction_handler_mutex);
    this->start_transaction_handler.erase(call.uniqueId);
    erase_lock.unlock();

    StartTransactionResponse response = start_transaction_future.get();

    // update authorization cache
    this->configuration->updateAuthorizationCacheEntry(idTag, response.idTagInfo);

    ActiveTransaction active_transaction;
    active_transaction.idTag = idTag;
    active_transaction.idTagInfo = response.idTagInfo;
    active_transaction.transactionId = response.transactionId;
    if (this->configuration->getStopTransactionOnInvalidId() &&
        active_transaction.idTagInfo.status != AuthorizationStatus::Accepted) {
        this->status_notification(connector, ChargePointErrorCode::NoError, this->status[connector]->suspended_evse());
        return active_transaction;
    }
    if (active_transaction.idTagInfo.status == AuthorizationStatus::ConcurrentTx) {
        return active_transaction;
    }
    int32_t interval = this->configuration->getMeterValueSampleInterval();
    if (interval != 0) {
        active_transaction.meter_values_sample_timer = new Everest::SteadyTimer(
            &this->io_service, [this, connector]() { this->meter_values_transaction_sample(connector); });

        active_transaction.meter_values_sample_timer->interval(std::chrono::seconds(interval));
    }

    std::unique_lock<std::mutex> active_transaction_lock(active_transactions_mutex);
    this->active_transactions[connector] = active_transaction;
    this->transaction_at_connector[response.transactionId] = connector;
    active_transaction_lock.unlock();

    this->status_notification(connector, ChargePointErrorCode::NoError, this->status[connector]->suspended_ev());

    return active_transaction;
}

StopTransactionResponse ChargePoint::stop_transaction(CiString20Type idTag, int32_t meterStop, int32_t transactionId,
                                                      Reason reason) {
    std::unique_lock<std::mutex> running_transaction_lock(active_transactions_mutex);
    int32_t connector = this->transaction_at_connector[transactionId];
    std::vector<TransactionData> transaction_data_vec;
    bool transaction_running = this->active_transactions.count(connector) != 0;
    if (transaction_running) {
        for (auto meter_value : this->active_transactions[connector].sampled_meter_values) {
            TransactionData transaction_data;
            transaction_data.timestamp = meter_value.timestamp;
            transaction_data.sampledValue = meter_value.sampledValue;
            transaction_data_vec.push_back(transaction_data);
        }
        for (auto meter_value : this->active_transactions[connector].clock_aligned_meter_values) {
            TransactionData transaction_data;
            transaction_data.timestamp = meter_value.timestamp;
            transaction_data.sampledValue = meter_value.sampledValue;
            transaction_data_vec.push_back(transaction_data);
        }
    }
    running_transaction_lock.unlock();
    if (!transaction_running) {
        throw std::runtime_error("There no transaction with id " + std::to_string(transactionId) + " running");
    }

    if (reason == Reason::EVDisconnected) {
        this->status_notification(connector, ChargePointErrorCode::NoError, std::string("EV side disconnected"),
                                  this->status[connector]->finishing(), DateTime());
    }

    std::promise<StopTransactionResponse> stop_transaction_promise;
    std::future<StopTransactionResponse> stop_transaction_future = stop_transaction_promise.get_future();

    StopTransactionRequest req;
    req.idTag.emplace(idTag);
    req.meterStop = meterStop;
    req.timestamp = DateTime();
    req.transactionId = transactionId;
    req.reason.emplace(reason);
    req.transactionData.emplace(transaction_data_vec);

    Call<StopTransactionRequest> call(req, this->message_queue->createMessageId());

    std::unique_lock<std::mutex> lock(stop_transaction_handler_mutex);
    this->stop_transaction_handler[call.uniqueId] = [this, &stop_transaction_promise](StopTransactionResponse data) {
        stop_transaction_promise.set_value(data);
    };
    lock.unlock();

    this->send(call);

    std::chrono::system_clock::time_point stop_transaction_wait =
        std::chrono::system_clock::now() + future_wait_seconds;
    std::future_status stop_transaction_future_status;
    do {
        stop_transaction_future_status = stop_transaction_future.wait_until(stop_transaction_wait);
    } while (stop_transaction_future_status == std::future_status::deferred);
    if (stop_transaction_future_status == std::future_status::timeout) {
        EVLOG(debug) << "stop transaction future timeout";
    } else if (stop_transaction_future_status == std::future_status::ready) {
        EVLOG(debug) << "stop transaction future ready";
    }

    std::unique_lock<std::mutex> erase_lock(stop_transaction_handler_mutex);
    this->stop_transaction_handler.erase(call.uniqueId);
    erase_lock.unlock();

    StopTransactionResponse response = stop_transaction_future.get();

    if (response.idTagInfo) {
        this->configuration->updateAuthorizationCacheEntry(idTag, response.idTagInfo.value());
    }

    std::unique_lock<std::mutex> active_transaction_lock(active_transactions_mutex);
    if (this->active_transactions[connector].meter_values_sample_timer != nullptr) {
        this->active_transactions[connector].meter_values_sample_timer->stop();
    }
    this->active_transactions.erase(connector);
    this->transaction_at_connector.erase(transactionId);
    active_transaction_lock.unlock();

    // TODO(kai): unlock connector!
    // this->unlock_connector(connector);

    // perform a queued connector availability change
    std::unique_lock<std::mutex> change_availability_lock(change_availability_mutex);
    bool change_queued = this->change_availability_queue.count(connector) != 0;
    AvailabilityType connector_availability;
    if (change_queued) {
        connector_availability = this->change_availability_queue[connector];
        this->change_availability_queue.erase(connector);
    }
    change_availability_lock.unlock();

    if (change_queued) {
        bool availability_change_succeeded =
            this->configuration->setConnectorAvailability(connector, connector_availability);
        std::ostringstream oss;
        oss << "Queued availability change of connector " << connector << " to "
            << conversions::availability_type_to_string(connector_availability);
        if (availability_change_succeeded) {
            oss << " successful." << std::endl;
            EVLOG(info) << oss.str();

            if (connector_availability == AvailabilityType::Operative) {
                this->status_notification(connector, ChargePointErrorCode::NoError, ChargePointStatus::Available);
            } else {
                this->status_notification(connector, ChargePointErrorCode::NoError, ChargePointStatus::Unavailable);
            }
        } else {
            oss << " failed" << std::endl;
            EVLOG(error) << oss.str();
        }
    }

    return response;
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
    this->send(call);
}

void ChargePoint::status_notification(int32_t connector, ChargePointErrorCode errorCode, ChargePointStatus status) {
    StatusNotificationRequest request;
    request.connectorId = connector;
    request.errorCode = errorCode;
    request.status = status;
    Call<StatusNotificationRequest> call(request, this->message_queue->createMessageId());
    this->send(call);
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
    }
    if (enhanced_message.offline) {
        // The charge point is offline or has a bad connection.
        // Check if local authorization via the authorization cache is allowed:
        if (this->configuration->getAuthorizationCacheEnabled() && this->configuration->getLocalAuthorizeOffline()) {
            EVLOG(info) << "Charge point appears to be offline, using authorization cache to check if IdTag is valid";
            auto idTagInfo = this->configuration->getAuthorizationCacheEntry(idTag);
            if (idTagInfo) {
                auth_status = idTagInfo.get().status;
            }
        }
    }

    return auth_status;
}

DataTransferResponse ChargePoint::data_transfer(const CiString255Type& vendorId, const CiString50Type& messageId,
                                                const std::string data) {
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
        response.status = DataTransferStatus::Rejected; // Rejected is not completely correct, but the best we have to
                                                        // indicate an error
    }

    return response;
}

// FIXME: start transaction if idTag has been presented already
bool ChargePoint::plug_connected(int32_t connector) {
    if (this->status[connector]->get_current_state() == ChargePointStatus::Available) {
        this->charging_session[connector].plug_connected = true;
        this->status[connector]->change_state(ChargePointStatusTransition::A2_UsageInitiated);
    }
    if (this->status[connector]->get_current_state() == ChargePointStatus::Preparing) {
        this->charging_session[connector].plug_connected = true;
        this->charging_session[connector].connection_timer->stop();
        return true;
    }
    return false;
}

bool ChargePoint::plug_disconnected(int32_t connector) {
    this->charging_session[connector].plug_connected = false;
    if (this->configuration->getStopTransactionOnEVSideDisconnect()) {
        std::unique_lock<std::mutex> active_transaction_lock(active_transactions_mutex);
        bool active_transaction = this->active_transactions.count(connector) != 0;
        CiString20Type idTag;
        int32_t transactionId;
        if (active_transaction) {
            idTag = this->active_transactions[connector].idTag;
            transactionId = this->active_transactions[connector].transactionId;
        }
        active_transaction_lock.unlock();

        if (active_transaction) {
            this->stop_transaction(idTag, this->get_meter_wh(connector), transactionId, Reason::EVDisconnected);
        }
    } else {
        this->status_notification(connector, ChargePointErrorCode::NoError, std::string("EV side disconnected"),
                                  this->status[connector]->suspended_ev(), DateTime());
    }
    return true;
}

bool ChargePoint::plug_connected_with_authorization(int32_t connector, CiString20Type idTag) {
    if (this->status[connector]->get_current_state() == ChargePointStatus::Available) {
        this->charging_session[connector].connection_timer->stop();
        this->charging_session[connector].plug_connected = true;

        this->status[connector]->change_state(ChargePointStatusTransition::A2_UsageInitiated);
    }
    if (this->status[connector]->get_current_state() == ChargePointStatus::Preparing) {
        if (this->authorize_id_tag(idTag) == AuthorizationStatus::Accepted) {
            this->start_transaction(connector, idTag);
            return true;
        }
    }
    return false;
}

// FIXME: start transaction if plug is connected already
bool ChargePoint::present_id_tag(int32_t connector, CiString20Type idTag) {
    if (this->status[connector]->get_current_state() == ChargePointStatus::Available) {
        auto timeout = this->configuration->getConnectionTimeOut();
        if (timeout != 0) {
            this->charging_session[connector].connection_timer->timeout(
                [this, connector]() { this->connection_timeout(connector); }, std::chrono::seconds(timeout));
        }
        this->status[connector]->change_state(ChargePointStatusTransition::A2_UsageInitiated);
        this->charging_session[connector].idTag = idTag;
    }
    if (this->status[connector]->get_current_state() == ChargePointStatus::Preparing &&
        this->charging_session[connector].plug_connected) {
        this->charging_session[connector].connection_timer->stop();
        this->charging_session[connector].plug_connected = true;
        return true;
    }
    return false;
}

bool ChargePoint::start_charging(int32_t connector) {
    if (this->status[connector]->get_current_state() == ChargePointStatus::SuspendedEV) {
        if (this->status[connector]->resume_ev() == ChargePointStatus::Charging) {
            return true;
        }
    }
    return false;
}

bool ChargePoint::suspend_charging_ev(int32_t connector) {
    // FIXME(kai): proper charging session handling - do we want, or even need to expose transaction ids?
    if (this->status[connector]->suspended_ev() == ChargePointStatus::SuspendedEV) {
        return true;
    }
    return false;
}

bool ChargePoint::suspend_charging_evse(int32_t connector) {
    // FIXME(kai): proper charging session handling - do we want, or even need to expose transaction ids?
    if (this->status[connector]->suspended_evse() == ChargePointStatus::SuspendedEVSE) {
        return true;
    }
    return false;
}

bool ChargePoint::stop_charging(int32_t connector) {
    std::unique_lock<std::mutex> running_transaction_lock(active_transactions_mutex);
    bool transaction_running = this->active_transactions.count(connector) != 0;
    CiString20Type idTag;
    int32_t transactionId;
    if (transaction_running) {
        idTag = this->active_transactions[connector].idTag;
        transactionId = this->active_transactions[connector].transactionId;
    }
    running_transaction_lock.unlock();
    if (transaction_running) {
        this->stop_transaction(idTag, this->get_meter_wh(connector), transactionId, Reason::Local);
        return true;
    }
    return false;
}

void ChargePoint::register_start_charging_callback(const std::function<void(int32_t connector)>& callback) {
    this->start_charging_callback = callback;
}

void ChargePoint::register_stop_charging_callback(const std::function<void(int32_t connector)>& callback) {
    this->stop_charging_callback = callback;
}

void ChargePoint::register_unlock_connector_callback(const std::function<bool(int32_t connector)>& callback) {
    this->unlock_connector_callback = callback;
}

void ChargePoint::register_get_meter_values_callback(
    const std::function<std::vector<MeterValue>(int32_t connector)>& callback) {
    this->get_meter_values_callback = callback;
}

void ChargePoint::register_get_meter_values_signed_callback(
    const std::function<std::vector<MeterValue>(int32_t connector)>& callback) {
    this->get_meter_values_signed_callback = callback;
}

void ChargePoint::register_get_meter_wh_callback(const std::function<int32_t(int32_t connector)>& callback) {
    this->get_meter_wh_callback = callback;
}

void ChargePoint::register_set_max_current_callback(
    const std::function<void(int32_t connector, double max_current)>& callback) {
    this->set_max_current_callback = callback;
}

} // namespace ocpp1_6
