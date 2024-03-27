// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest

#include "ErrorHandling.hpp"

namespace module {

static types::evse_manager::Error_severity to_evse_manager_severity(Everest::error::Severity s) {
    switch (s) {
    case Everest::error::Severity::High:
        return types::evse_manager::Error_severity::High;
    case Everest::error::Severity::Medium:
        return types::evse_manager::Error_severity::Medium;
    case Everest::error::Severity::Low:
        return types::evse_manager::Error_severity::Low;
    }
    return types::evse_manager::Error_severity::Low;
}

ErrorHandling::ErrorHandling(const std::unique_ptr<evse_board_supportIntf>& _r_bsp,
                             const std::vector<std::unique_ptr<ISO15118_chargerIntf>>& _r_hlc,
                             const std::vector<std::unique_ptr<connector_lockIntf>>& _r_connector_lock,
                             const std::vector<std::unique_ptr<ac_rcdIntf>>& _r_ac_rcd,
                             const std::unique_ptr<evse_managerImplBase>& _p_evse) :
    r_bsp(_r_bsp), r_hlc(_r_hlc), r_connector_lock(_r_connector_lock), r_ac_rcd(_r_ac_rcd), p_evse(_p_evse) {

    if (r_hlc.size() > 0) {
        hlc = true;
    }

    // Subscribe to bsp driver to receive Errors from the bsp hardware
    r_bsp->subscribe_all_errors(
        [this](const Everest::error::Error& error) {
            types::evse_manager::ErrorEnum evse_error{types::evse_manager::ErrorEnum::VendorWarning};
            types::evse_manager::Error output_error;
            output_error.error_description = error.description;
            output_error.error_severity = to_evse_manager_severity(error.severity);

            if (modify_error_bsp(error, true, evse_error)) {
                // signal to charger a new error has been set that prevents charging
                output_error.error_code = evse_error;
                signal_error(output_error, true);
            } else {
                // signal an error that does not prevent charging
                output_error.error_code = evse_error;
                signal_error(output_error, false);
            }
        },
        [this](const Everest::error::Error& error) {
            types::evse_manager::ErrorEnum evse_error{types::evse_manager::ErrorEnum::VendorWarning};
            types::evse_manager::Error output_error;
            output_error.error_description = error.description;
            output_error.error_severity = to_evse_manager_severity(error.severity);

            if (modify_error_bsp(error, false, evse_error)) {
                // signal to charger an error has been cleared that prevents charging
                output_error.error_code = evse_error;
                signal_error_cleared(output_error, true);
            } else {
                // signal an error cleared that does not prevent charging
                output_error.error_code = evse_error;
                signal_error_cleared(output_error, false);
            }

            if (active_errors.all_cleared()) {
                // signal to charger that all errors are cleared now
                signal_all_errors_cleared();
                // clear errors with HLC stack
                if (hlc) {
                    r_hlc[0]->call_reset_error();
                }
            }
        });

    // Subscribe to connector lock to receive errors from connector lock hardware
    if (r_connector_lock.size() > 0) {
        r_connector_lock[0]->subscribe_all_errors(
            [this](const Everest::error::Error& error) {
                types::evse_manager::ErrorEnum evse_error{types::evse_manager::ErrorEnum::VendorWarning};
                types::evse_manager::Error output_error;
                output_error.error_description = error.description;
                output_error.error_severity = to_evse_manager_severity(error.severity);

                if (modify_error_connector_lock(error, true, evse_error)) {
                    // signal to charger a new error has been set that prevents charging
                    output_error.error_code = evse_error;
                    signal_error(output_error, true);
                } else {
                    // signal an error that does not prevent charging
                    output_error.error_code = evse_error;
                    signal_error(output_error, false);
                }
            },
            [this](const Everest::error::Error& error) {
                types::evse_manager::ErrorEnum evse_error{types::evse_manager::ErrorEnum::VendorWarning};
                types::evse_manager::Error output_error;
                output_error.error_description = error.description;
                output_error.error_severity = to_evse_manager_severity(error.severity);

                if (modify_error_connector_lock(error, false, evse_error)) {
                    // signal to charger an error has been cleared that prevents charging
                    output_error.error_code = evse_error;
                    signal_error_cleared(output_error, true);
                } else {
                    // signal an error cleared that does not prevent charging
                    output_error.error_code = evse_error;
                    signal_error_cleared(output_error, false);
                }

                if (active_errors.all_cleared()) {
                    // signal to charger that all errors are cleared now
                    signal_all_errors_cleared();
                    // clear errors with HLC stack
                    if (hlc) {
                        r_hlc[0]->call_reset_error();
                    }
                }
            });
    }

    // Subscribe to ac_rcd to receive errors from AC RCD hardware
    if (r_ac_rcd.size() > 0) {
        r_ac_rcd[0]->subscribe_all_errors(
            [this](const Everest::error::Error& error) {
                types::evse_manager::ErrorEnum evse_error{types::evse_manager::ErrorEnum::VendorWarning};
                types::evse_manager::Error output_error;
                output_error.error_description = error.description;
                output_error.error_severity = to_evse_manager_severity(error.severity);

                if (modify_error_ac_rcd(error, true, evse_error)) {
                    // signal to charger a new error has been set that prevents charging
                    output_error.error_code = evse_error;
                    signal_error(output_error, true);
                } else {
                    // signal an error that does not prevent charging
                    output_error.error_code = evse_error;
                    signal_error(output_error, false);
                }
            },
            [this](const Everest::error::Error& error) {
                types::evse_manager::ErrorEnum evse_error{types::evse_manager::ErrorEnum::VendorWarning};
                types::evse_manager::Error output_error;
                output_error.error_description = error.description;
                output_error.error_severity = to_evse_manager_severity(error.severity);

                if (modify_error_ac_rcd(error, false, evse_error)) {
                    // signal to charger an error has been cleared that prevents charging
                    output_error.error_code = evse_error;
                    signal_error_cleared(output_error, true);
                } else {
                    // signal an error cleared that does not prevent charging
                    output_error.error_code = evse_error;
                    signal_error_cleared(output_error, false);
                }

                if (active_errors.all_cleared()) {
                    // signal to charger that all errors are cleared now
                    signal_all_errors_cleared();
                    // clear errors with HLC stack
                    if (hlc) {
                        r_hlc[0]->call_reset_error();
                    }
                }
            });
    }
}

void ErrorHandling::raise_overcurrent_error(const std::string& description) {
    // raise externally
    p_evse->raise_evse_manager_MREC4OverCurrentFailure(description, Everest::error::Severity::High);
    types::evse_manager::ErrorEnum evse_error{types::evse_manager::ErrorEnum::VendorWarning};

    if (modify_error_evse_manager("evse_manager/MREC4OverCurrentFailure", true, evse_error)) {
        // signal to charger a new error has been set
        types::evse_manager::Error output_error;
        output_error.error_description = description;
        output_error.error_severity = types::evse_manager::Error_severity::High;
        output_error.error_code = types::evse_manager::ErrorEnum::MREC4OverCurrentFailure;
        signal_error(output_error, true);
    };
}

void ErrorHandling::clear_overcurrent_error() {
    // clear externally
    if (active_errors.bsp.is_set(BspErrors::MREC4OverCurrentFailure)) {
        p_evse->request_clear_all_evse_manager_MREC4OverCurrentFailure();

        types::evse_manager::ErrorEnum evse_error{types::evse_manager::ErrorEnum::VendorWarning};
        types::evse_manager::Error output_error;
        output_error.error_description = "";
        output_error.error_severity = types::evse_manager::Error_severity::High;
        output_error.error_code = types::evse_manager::ErrorEnum::MREC4OverCurrentFailure;

        if (modify_error_evse_manager("evse_manager/MREC4OverCurrentFailure", false, evse_error)) {
            // signal to charger an error has been cleared that prevents charging
            output_error.error_code = evse_error;
            signal_error_cleared(output_error, true);
        } else {
            // signal an error cleared that does not prevent charging
            output_error.error_code = evse_error;
            signal_error_cleared(output_error, false);
        }

        if (active_errors.all_cleared()) {
            // signal to charger that all errors are cleared now
            signal_all_errors_cleared();
            // clear errors with HLC stack
            if (hlc) {
                r_hlc[0]->call_reset_error();
            }
        }
    }
}

void ErrorHandling::raise_internal_error(const std::string& description) {
    // raise externally
    p_evse->raise_evse_manager_Internal(description, Everest::error::Severity::High);
    types::evse_manager::ErrorEnum evse_error{types::evse_manager::ErrorEnum::VendorWarning};

    if (modify_error_evse_manager("evse_manager/Internal", true, evse_error)) {
        // signal to charger a new error has been set
        types::evse_manager::Error output_error;
        output_error.error_description = description;
        output_error.error_severity = types::evse_manager::Error_severity::High;
        output_error.error_code = types::evse_manager::ErrorEnum::VendorError;
        signal_error(output_error, true);
    };
}

void ErrorHandling::clear_internal_error() {
    // clear externally
    if (active_errors.evse_manager.is_set(EvseManagerErrors::Internal)) {
        p_evse->request_clear_all_evse_manager_Internal();

        types::evse_manager::ErrorEnum evse_error{types::evse_manager::ErrorEnum::VendorWarning};
        types::evse_manager::Error output_error;
        output_error.error_description = "";
        output_error.error_severity = types::evse_manager::Error_severity::High;
        output_error.error_code = types::evse_manager::ErrorEnum::VendorError;

        if (modify_error_evse_manager("evse_manager/Internal", false, evse_error)) {
            // signal to charger an error has been cleared that prevents charging
            output_error.error_code = evse_error;
            signal_error_cleared(output_error, true);
        } else {
            // signal an error cleared that does not prevent charging
            output_error.error_code = evse_error;
            signal_error_cleared(output_error, false);
        }

        if (active_errors.all_cleared()) {
            // signal to charger that all errors are cleared now
            signal_all_errors_cleared();
            // clear errors with HLC stack
            if (hlc) {
                r_hlc[0]->call_reset_error();
            }
        }
    }
}

void ErrorHandling::raise_powermeter_transaction_start_failed_error(const std::string& description) {
    // raise externally
    p_evse->raise_evse_manager_PowermeterTransactionStartFailed(description, Everest::error::Severity::High);
    types::evse_manager::ErrorEnum evse_error{types::evse_manager::ErrorEnum::VendorWarning};

    if (modify_error_evse_manager("evse_manager/PowermeterTransactionStartFailed", true, evse_error)) {
        // signal to charger a new error has been set
        types::evse_manager::Error output_error;
        output_error.error_description = description;
        output_error.error_severity = types::evse_manager::Error_severity::High;
        output_error.error_code = types::evse_manager::ErrorEnum::PowermeterTransactionStartFailed;
        signal_error(output_error, true);
    };
}

void ErrorHandling::clear_powermeter_transaction_start_failed_error() {
    // clear externally
    if (active_errors.evse_manager.is_set(EvseManagerErrors::PowermeterTransactionStartFailed)) {
        p_evse->request_clear_all_evse_manager_PowermeterTransactionStartFailed();

        types::evse_manager::ErrorEnum evse_error{types::evse_manager::ErrorEnum::VendorWarning};
        types::evse_manager::Error output_error;
        output_error.error_description = "";
        output_error.error_severity = types::evse_manager::Error_severity::High;
        output_error.error_code = types::evse_manager::ErrorEnum::VendorError;

        if (modify_error_evse_manager("evse_manager/PowermeterTransactionStartFailed", false, evse_error)) {
            // signal to charger an error has been cleared that prevents charging
            output_error.error_code = evse_error;
            signal_error_cleared(output_error, true);
        } else {
            // signal an error cleared that does not prevent charging
            output_error.error_code = evse_error;
            signal_error_cleared(output_error, false);
        }

        if (active_errors.all_cleared()) {
            // signal to charger that all errors are cleared now
            signal_all_errors_cleared();
            // clear errors with HLC stack
            if (hlc) {
                r_hlc[0]->call_reset_error();
            }
        }
    }
}

bool ErrorHandling::modify_error_bsp(const Everest::error::Error& error, bool active,
                                     types::evse_manager::ErrorEnum& evse_error) {
    const std::string& error_type = error.type;

    if (active) {
        EVLOG_error << "Raised error " << error_type << ": " << error.description << " (" << error.message << ")";
    } else {
        EVLOG_info << "Cleared error " << error_type << ": " << error.description << " (" << error.message << ")";
    }

    if (error_type == "evse_board_support/DiodeFault") {
        active_errors.bsp.set(BspErrors::DiodeFault, active);
        evse_error = types::evse_manager::ErrorEnum::DiodeFault;
    } else if (error_type == "evse_board_support/VentilationNotAvailable") {
        active_errors.bsp.set(BspErrors::VentilationNotAvailable, active);
        evse_error = types::evse_manager::ErrorEnum::VentilationNotAvailable;
    } else if (error_type == "evse_board_support/BrownOut") {
        active_errors.bsp.set(BspErrors::BrownOut, active);
        evse_error = types::evse_manager::ErrorEnum::BrownOut;
        if (hlc && active) {
            r_hlc[0]->call_send_error(types::iso15118_charger::EvseError::Error_Malfunction);
        }
    } else if (error_type == "evse_board_support/EnergyManagement") {
        active_errors.bsp.set(BspErrors::EnergyManagement, active);
        evse_error = types::evse_manager::ErrorEnum::EnergyManagement;
        if (hlc && active) {
            r_hlc[0]->call_send_error(types::iso15118_charger::EvseError::Error_Malfunction);
        }
    } else if (error_type == "evse_board_support/PermanentFault") {
        active_errors.bsp.set(BspErrors::PermanentFault, active);
        evse_error = types::evse_manager::ErrorEnum::PermanentFault;
        if (hlc && active) {
            r_hlc[0]->call_send_error(types::iso15118_charger::EvseError::Error_Malfunction);
        }
    } else if (error_type == "evse_board_support/MREC2GroundFailure") {
        active_errors.bsp.set(BspErrors::MREC2GroundFailure, active);
        evse_error = types::evse_manager::ErrorEnum::MREC2GroundFailure;
        if (hlc && active) {
            r_hlc[0]->call_send_error(types::iso15118_charger::EvseError::Error_Malfunction);
        }
    } else if (error_type == "evse_board_support/MREC4OverCurrentFailure") {
        active_errors.bsp.set(BspErrors::MREC4OverCurrentFailure, active);
        evse_error = types::evse_manager::ErrorEnum::MREC4OverCurrentFailure;
        if (hlc && active) {
            r_hlc[0]->call_send_error(types::iso15118_charger::EvseError::Error_Malfunction);
        }
    } else if (error_type == "evse_board_support/MREC5OverVoltage") {
        active_errors.bsp.set(BspErrors::MREC5OverVoltage, active);
        evse_error = types::evse_manager::ErrorEnum::MREC5OverVoltage;
        if (hlc && active) {
            r_hlc[0]->call_send_error(types::iso15118_charger::EvseError::Error_Malfunction);
        }
    } else if (error_type == "evse_board_support/MREC6UnderVoltage") {
        active_errors.bsp.set(BspErrors::MREC6UnderVoltage, active);
        evse_error = types::evse_manager::ErrorEnum::MREC6UnderVoltage;
        if (hlc && active) {
            r_hlc[0]->call_send_error(types::iso15118_charger::EvseError::Error_Malfunction);
        }
    } else if (error_type == "evse_board_support/MREC8EmergencyStop") {
        active_errors.bsp.set(BspErrors::MREC8EmergencyStop, active);
        evse_error = types::evse_manager::ErrorEnum::MREC8EmergencyStop;
        if (hlc && active) {
            r_hlc[0]->call_send_error(types::iso15118_charger::EvseError::Error_EmergencyShutdown);
        }
    } else if (error_type == "evse_board_support/MREC10InvalidVehicleMode") {
        active_errors.bsp.set(BspErrors::MREC10InvalidVehicleMode, active);
        evse_error = types::evse_manager::ErrorEnum::MREC10InvalidVehicleMode;
        if (hlc && active) {
            r_hlc[0]->call_send_error(types::iso15118_charger::EvseError::Error_Malfunction);
        }
    } else if (error_type == "evse_board_support/MREC14PilotFault") {
        active_errors.bsp.set(BspErrors::MREC14PilotFault, active);
        evse_error = types::evse_manager::ErrorEnum::MREC14PilotFault;
        if (hlc && active) {
            r_hlc[0]->call_send_error(types::iso15118_charger::EvseError::Error_Malfunction);
        }
    } else if (error_type == "evse_board_support/MREC15PowerLoss") {
        active_errors.bsp.set(BspErrors::MREC15PowerLoss, active);
        evse_error = types::evse_manager::ErrorEnum::MREC15PowerLoss;
        if (hlc && active) {
            r_hlc[0]->call_send_error(types::iso15118_charger::EvseError::Error_Malfunction);
        }
    } else if (error_type == "evse_board_support/MREC17EVSEContactorFault") {
        active_errors.bsp.set(BspErrors::MREC17EVSEContactorFault, active);
        evse_error = types::evse_manager::ErrorEnum::MREC17EVSEContactorFault;
        if (hlc && active) {
            r_hlc[0]->call_send_error(types::iso15118_charger::EvseError::Error_Contactor);
        }
    } else if (error_type == "evse_board_support/MREC19CableOverTempStop") {
        active_errors.bsp.set(BspErrors::MREC19CableOverTempStop, active);
        evse_error = types::evse_manager::ErrorEnum::MREC19CableOverTempStop;
        if (hlc && active) {
            r_hlc[0]->call_send_error(types::iso15118_charger::EvseError::Error_Malfunction);
        }
    } else if (error_type == "evse_board_support/MREC20PartialInsertion") {
        active_errors.bsp.set(BspErrors::MREC20PartialInsertion, active);
        evse_error = types::evse_manager::ErrorEnum::MREC20PartialInsertion;
        if (hlc && active) {
            r_hlc[0]->call_send_error(types::iso15118_charger::EvseError::Error_Malfunction);
        }
    } else if (error_type == "evse_board_support/MREC23ProximityFault") {
        active_errors.bsp.set(BspErrors::MREC23ProximityFault, active);
        evse_error = types::evse_manager::ErrorEnum::MREC23ProximityFault;
        if (hlc && active) {
            r_hlc[0]->call_send_error(types::iso15118_charger::EvseError::Error_Malfunction);
        }
    } else if (error_type == "evse_board_support/MREC24ConnectorVoltageHigh") {
        active_errors.bsp.set(BspErrors::MREC24ConnectorVoltageHigh, active);
        evse_error = types::evse_manager::ErrorEnum::MREC24ConnectorVoltageHigh;
        if (hlc && active) {
            r_hlc[0]->call_send_error(types::iso15118_charger::EvseError::Error_Malfunction);
        }
    } else if (error_type == "evse_board_support/MREC25BrokenLatch") {
        active_errors.bsp.set(BspErrors::MREC25BrokenLatch, active);
        evse_error = types::evse_manager::ErrorEnum::MREC25BrokenLatch;
        if (hlc && active) {
            r_hlc[0]->call_send_error(types::iso15118_charger::EvseError::Error_Malfunction);
        }
    } else if (error_type == "evse_board_support/MREC26CutCable") {
        active_errors.bsp.set(BspErrors::MREC26CutCable, active);
        evse_error = types::evse_manager::ErrorEnum::MREC26CutCable;
        if (hlc && active) {
            r_hlc[0]->call_send_error(types::iso15118_charger::EvseError::Error_Malfunction);
        }
    } else if (error_type == "evse_board_support/VendorError") {
        active_errors.bsp.set(BspErrors::VendorError, active);
        evse_error = types::evse_manager::ErrorEnum::VendorError;
        if (hlc && active) {
            r_hlc[0]->call_send_error(types::iso15118_charger::EvseError::Error_Malfunction);
        }
    } else {
        // Errors that do not stop charging
        if (error_type == "evse_board_support/MREC3HighTemperature") {
            evse_error = types::evse_manager::ErrorEnum::MREC3HighTemperature;
        } else if (error_type == "evse_board_support/MREC18CableOverTempDerate") {
            evse_error = types::evse_manager::ErrorEnum::MREC18CableOverTempDerate;
        } else if (error_type == "evse_board_support/VendorWarning") {
            evse_error = types::evse_manager::ErrorEnum::VendorWarning;
        }
        return false;
    }
    // Error stops charging
    return true;
};

bool ErrorHandling::modify_error_connector_lock(const Everest::error::Error& error, bool active,
                                                types::evse_manager::ErrorEnum& evse_error) {
    const std::string& error_type = error.type;

    if (active) {
        EVLOG_error << "Raised error " << error_type << ": " << error.description << " (" << error.message << ")";
    } else {
        EVLOG_info << "Cleared error " << error_type << ": " << error.description << " (" << error.message << ")";
    }

    if (error_type == "connector_lock/ConnectorLockCapNotCharged") {
        active_errors.connector_lock.set(ConnectorLockErrors::ConnectorLockCapNotCharged, active);
        evse_error = types::evse_manager::ErrorEnum::ConnectorLockCapNotCharged;
    } else if (error_type == "connector_lock/ConnectorLockUnexpectedClose") {
        active_errors.connector_lock.set(ConnectorLockErrors::ConnectorLockUnexpectedClose, active);
        evse_error = types::evse_manager::ErrorEnum::ConnectorLockUnexpectedClose;
    } else if (error_type == "connector_lock/ConnectorLockUnexpectedOpen") {
        active_errors.connector_lock.set(ConnectorLockErrors::ConnectorLockUnexpectedOpen, active);
        evse_error = types::evse_manager::ErrorEnum::ConnectorLockUnexpectedOpen;
    } else if (error_type == "connector_lock/ConnectorLockFailedLock") {
        active_errors.connector_lock.set(ConnectorLockErrors::ConnectorLockFailedLock, active);
        evse_error = types::evse_manager::ErrorEnum::ConnectorLockFailedLock;
    } else if (error_type == "connector_lock/ConnectorLockFailedUnlock") {
        active_errors.connector_lock.set(ConnectorLockErrors::ConnectorLockFailedUnlock, active);
        evse_error = types::evse_manager::ErrorEnum::ConnectorLockFailedUnlock;
    } else if (error_type == "connector_lock/MREC1ConnectorLockFailure") {
        active_errors.connector_lock.set(ConnectorLockErrors::MREC1ConnectorLockFailure, active);
        evse_error = types::evse_manager::ErrorEnum::MREC1ConnectorLockFailure;
    } else if (error_type == "connector_lock/VendorError") {
        active_errors.connector_lock.set(ConnectorLockErrors::VendorError, active);
        evse_error = types::evse_manager::ErrorEnum::VendorError;
        if (hlc && active) {
            r_hlc[0]->call_send_error(types::iso15118_charger::EvseError::Error_Malfunction);
        }
    } else {
        // Errors that do not stop charging
        if (error_type == "connector_lock/VendorWarning") {
            evse_error = types::evse_manager::ErrorEnum::VendorWarning;
        }
        return false;
    }
    // Error stops charging
    return true;
};

bool ErrorHandling::modify_error_ac_rcd(const Everest::error::Error& error, bool active,
                                        types::evse_manager::ErrorEnum& evse_error) {
    const std::string& error_type = error.type;

    if (active) {
        EVLOG_error << "Raised error " << error_type << ": " << error.description << " (" << error.message << ")";
    } else {
        EVLOG_info << "Cleared error " << error_type << ": " << error.description << " (" << error.message << ")";
    }

    if (error_type == "ac_rcd/MREC2GroundFailure") {
        active_errors.ac_rcd.set(AcRcdErrors::MREC2GroundFailure, active);
        evse_error = types::evse_manager::ErrorEnum::MREC2GroundFailure;
        if (hlc && active) {
            r_hlc[0]->call_send_error(types::iso15118_charger::EvseError::Error_RCD);
        }
    } else if (error_type == "ac_rcd/VendorError") {
        active_errors.ac_rcd.set(AcRcdErrors::VendorError, active);
        evse_error = types::evse_manager::ErrorEnum::VendorError;
        if (hlc && active) {
            r_hlc[0]->call_send_error(types::iso15118_charger::EvseError::Error_Malfunction);
        }
    } else if (error_type == "ac_rcd/Selftest") {
        active_errors.ac_rcd.set(AcRcdErrors::Selftest, active);
        evse_error = types::evse_manager::ErrorEnum::RCD_Selftest;
        if (hlc && active) {
            r_hlc[0]->call_send_error(types::iso15118_charger::EvseError::Error_Malfunction);
        }
    } else if (error_type == "ac_rcd/AC") {
        active_errors.ac_rcd.set(AcRcdErrors::AC, active);
        evse_error = types::evse_manager::ErrorEnum::RCD_AC;
        if (hlc && active) {
            r_hlc[0]->call_send_error(types::iso15118_charger::EvseError::Error_RCD);
        }
    } else if (error_type == "ac_rcd/DC") {
        active_errors.ac_rcd.set(AcRcdErrors::DC, active);
        evse_error = types::evse_manager::ErrorEnum::RCD_DC;
        if (hlc && active) {
            r_hlc[0]->call_send_error(types::iso15118_charger::EvseError::Error_RCD);
        }
    } else {
        // Errors that do not stop charging
        if (error_type == "ac_rcd/VendorWarning") {
            evse_error = types::evse_manager::ErrorEnum::VendorWarning;
        }
        return false;
    }
    // Error stops charging
    return true;
};

bool ErrorHandling::modify_error_evse_manager(const std::string& error_type, bool active,
                                              types::evse_manager::ErrorEnum& evse_error) {
    if (error_type == "evse_manager/MREC4OverCurrentFailure") {
        active_errors.bsp.set(BspErrors::MREC4OverCurrentFailure, active);
        evse_error = types::evse_manager::ErrorEnum::MREC4OverCurrentFailure;
        if (hlc && active) {
            r_hlc[0]->call_send_error(types::iso15118_charger::EvseError::Error_Malfunction);
        }

    } else if (error_type == "evse_manager/PowermeterTransactionStartFailed") {
        active_errors.evse_manager.set(EvseManagerErrors::PowermeterTransactionStartFailed, active);
        evse_error = types::evse_manager::ErrorEnum::PowermeterTransactionStartFailed;
        if (hlc && active) {
            r_hlc[0]->call_send_error(types::iso15118_charger::EvseError::Error_Malfunction);
        }

    } else {
        // Error does not stop charging, ignored here
        return false;
    }
    // Error stops charging
    return true;
};

} // namespace module
