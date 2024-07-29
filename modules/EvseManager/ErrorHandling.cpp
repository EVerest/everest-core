// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest

#include "ErrorHandling.hpp"

namespace module {

ErrorHandling::ErrorHandling(const std::unique_ptr<evse_board_supportIntf>& _r_bsp,
                             const std::vector<std::unique_ptr<ISO15118_chargerIntf>>& _r_hlc,
                             const std::vector<std::unique_ptr<connector_lockIntf>>& _r_connector_lock,
                             const std::vector<std::unique_ptr<ac_rcdIntf>>& _r_ac_rcd,
                             const std::unique_ptr<evse_managerImplBase>& _p_evse,
                             const std::vector<std::unique_ptr<isolation_monitorIntf>>& _r_imd) :
    r_bsp(_r_bsp),
    r_hlc(_r_hlc),
    r_connector_lock(_r_connector_lock),
    r_ac_rcd(_r_ac_rcd),
    p_evse(_p_evse),
    r_imd(_r_imd) {

    if (r_hlc.size() > 0) {
        hlc = true;
    }

    // Subscribe to bsp driver to receive Errors from the bsp hardware
    r_bsp->subscribe_all_errors(
        [this](const Everest::error::Error& error) {
            if (modify_error_bsp(error, true)) {
                // signal to charger a new error has been set that prevents charging
                raise_permanent_fault_error(error.description);
            } else {
                // signal an error that does not prevent charging
                signal_error(false);
            }
        },
        [this](const Everest::error::Error& error) {
            if (modify_error_bsp(error, false)) {
                // signal to charger an error has been cleared that prevents charging
                signal_error_cleared(true);
            } else {
                // signal an error cleared that does not prevent charging
                signal_error_cleared(false);
            }

            if (active_errors.all_cleared()) {
                // signal to charger that all errors are cleared now
                clear_permanent_fault_error();
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
                if (modify_error_connector_lock(error, true)) {
                    // signal to charger a new error has been set that prevents charging
                    raise_permanent_fault_error(error.description);
                } else {
                    // signal an error that does not prevent charging
                    signal_error(false);
                }
            },
            [this](const Everest::error::Error& error) {
                if (modify_error_connector_lock(error, false)) {
                    // signal to charger an error has been cleared that prevents charging
                    signal_error_cleared(true);
                } else {
                    // signal an error cleared that does not prevent charging
                    signal_error_cleared(false);
                }

                if (active_errors.all_cleared()) {
                    // signal to charger that all errors are cleared now
                    clear_permanent_fault_error();
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
                if (modify_error_ac_rcd(error, true)) {
                    // signal to charger a new error has been set that prevents charging
                    raise_permanent_fault_error(error.description);
                } else {
                    // signal an error that does not prevent charging
                    signal_error(false);
                }
            },
            [this](const Everest::error::Error& error) {
                if (modify_error_ac_rcd(error, false)) {
                    // signal to charger an error has been cleared that prevents charging
                    signal_error_cleared(true);
                } else {
                    // signal an error cleared that does not prevent charging
                    signal_error_cleared(false);
                }

                if (active_errors.all_cleared()) {
                    // signal to charger that all errors are cleared now
                    clear_permanent_fault_error();
                    signal_all_errors_cleared();
                    // clear errors with HLC stack
                    if (hlc) {
                        r_hlc[0]->call_reset_error();
                    }
                }
            });
    }

    // Subscribe to ac_rcd to receive errors from IMD hardware
    if (r_imd.size() > 0) {
        r_imd[0]->subscribe_all_errors(
            [this](const Everest::error::Error& error) {
                if (modify_error_imd(error, true)) {
                    // signal to charger a new error has been set that prevents charging
                    raise_permanent_fault_error(error.description);
                } else {
                    // signal an error that does not prevent charging
                    signal_error(false);
                }
            },
            [this](const Everest::error::Error& error) {
                if (modify_error_imd(error, false)) {
                    // signal to charger an error has been cleared that prevents charging
                    signal_error_cleared(true);
                } else {
                    signal_error_cleared(false);
                }

                if (active_errors.all_cleared()) {
                    // signal to charger that all errors are cleared now
                    clear_permanent_fault_error();
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
    Everest::error::Error error_object = p_evse->error_factory->create_error(
        "evse_manager/MREC4OverCurrentFailure", "", description, Everest::error::Severity::High);
    p_evse->raise_error(error_object);

    if (modify_error_evse_manager("evse_manager/MREC4OverCurrentFailure", true)) {
        // signal to charger a new error has been set
        signal_error(true);
    };
}

void ErrorHandling::clear_overcurrent_error() {
    // clear externally
    if (active_errors.bsp.is_set(BspErrors::MREC4OverCurrentFailure)) {
        p_evse->clear_error("evse_manager/MREC4OverCurrentFailure");

        if (modify_error_evse_manager("evse_manager/MREC4OverCurrentFailure", false)) {
            // signal to charger an error has been cleared that prevents charging
            signal_error_cleared(true);
        } else {
            // signal an error cleared that does not prevent charging
            signal_error_cleared(false);
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

void ErrorHandling::raise_permanent_fault_error(const std::string& description) {
    // raise externally
    Everest::error::Error error_object = p_evse->error_factory->create_error(
        "evse_manager/PermanentFault", "", description, Everest::error::Severity::High);
    p_evse->raise_error(error_object);

    if (modify_error_evse_manager("evse_manager/PermanentFault", true)) {
        // signal to charger a new error has been set
        signal_error(true);
    };
}

void ErrorHandling::clear_permanent_fault_error() {
    // clear externally
    if (active_errors.evse_manager.is_set(EvseManagerErrors::PermanentFault)) {
        p_evse->clear_error("evse_manager/PermanentFault");

        if (modify_error_evse_manager("evse_manager/PermanentFault", false)) {
            // signal to charger an error has been cleared that prevents charging
            signal_error_cleared(true);
        } else {
            // signal an error cleared that does not prevent charging
            signal_error_cleared(false);
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
    Everest::error::Error error_object =
        p_evse->error_factory->create_error("evse_manager/Internal", "", description, Everest::error::Severity::High);
    p_evse->raise_error(error_object);

    if (modify_error_evse_manager("evse_manager/Internal", true)) {
        // signal to charger a new error has been set
        signal_error(true);
    };
}

void ErrorHandling::clear_internal_error() {
    // clear externally
    if (active_errors.evse_manager.is_set(EvseManagerErrors::Internal)) {
        p_evse->clear_error("evse_manager/Internal");

        if (modify_error_evse_manager("evse_manager/Internal", false)) {
            // signal to charger an error has been cleared that prevents charging
            signal_error_cleared(true);
        } else {
            // signal an error cleared that does not prevent charging
            signal_error_cleared(false);
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
    Everest::error::Error error_object = p_evse->error_factory->create_error(
        "evse_manager/PowermeterTransactionStartFailed", "", description, Everest::error::Severity::High);
    p_evse->raise_error(error_object);

    if (modify_error_evse_manager("evse_manager/PowermeterTransactionStartFailed", true)) {
        // signal to charger a new error has been set
        signal_error(true);
    };
}

void ErrorHandling::clear_powermeter_transaction_start_failed_error() {
    // clear externally
    if (active_errors.evse_manager.is_set(EvseManagerErrors::PowermeterTransactionStartFailed)) {
        p_evse->clear_error("evse_manager/PowermeterTransactionStartFailed");

        if (modify_error_evse_manager("evse_manager/PowermeterTransactionStartFailed", false)) {
            // signal to charger an error has been cleared that prevents charging
            signal_error_cleared(true);
        } else {
            // signal an error cleared that does not prevent charging
            signal_error_cleared(false);
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

bool ErrorHandling::modify_error_bsp(const Everest::error::Error& error, bool active) {
    const std::string& error_type = error.type;

    if (active) {
        EVLOG_error << "Raised error " << error_type << ": " << error.description << " (" << error.message << ")";
    } else {
        EVLOG_info << "Cleared error " << error_type << ": " << error.description << " (" << error.message << ")";
    }

    if (error_type == "evse_board_support/DiodeFault") {
        active_errors.bsp.set(BspErrors::DiodeFault, active);
    } else if (error_type == "evse_board_support/VentilationNotAvailable") {
        active_errors.bsp.set(BspErrors::VentilationNotAvailable, active);
    } else if (error_type == "evse_board_support/BrownOut") {
        active_errors.bsp.set(BspErrors::BrownOut, active);
        if (hlc && active) {
            r_hlc[0]->call_send_error(types::iso15118_charger::EvseError::Error_Malfunction);
        }
    } else if (error_type == "evse_board_support/EnergyManagement") {
        active_errors.bsp.set(BspErrors::EnergyManagement, active);
        if (hlc && active) {
            r_hlc[0]->call_send_error(types::iso15118_charger::EvseError::Error_Malfunction);
        }
    } else if (error_type == "evse_board_support/PermanentFault") {
        active_errors.bsp.set(BspErrors::PermanentFault, active);
        if (hlc && active) {
            r_hlc[0]->call_send_error(types::iso15118_charger::EvseError::Error_Malfunction);
        }
    } else if (error_type == "evse_board_support/MREC2GroundFailure") {
        active_errors.bsp.set(BspErrors::MREC2GroundFailure, active);
        if (hlc && active) {
            r_hlc[0]->call_send_error(types::iso15118_charger::EvseError::Error_Malfunction);
        }
    } else if (error_type == "evse_board_support/MREC4OverCurrentFailure") {
        active_errors.bsp.set(BspErrors::MREC4OverCurrentFailure, active);
        if (hlc && active) {
            r_hlc[0]->call_send_error(types::iso15118_charger::EvseError::Error_Malfunction);
        }
    } else if (error_type == "evse_board_support/MREC5OverVoltage") {
        active_errors.bsp.set(BspErrors::MREC5OverVoltage, active);
        if (hlc && active) {
            r_hlc[0]->call_send_error(types::iso15118_charger::EvseError::Error_Malfunction);
        }
    } else if (error_type == "evse_board_support/MREC6UnderVoltage") {
        active_errors.bsp.set(BspErrors::MREC6UnderVoltage, active);
        if (hlc && active) {
            r_hlc[0]->call_send_error(types::iso15118_charger::EvseError::Error_Malfunction);
        }
    } else if (error_type == "evse_board_support/MREC8EmergencyStop") {
        active_errors.bsp.set(BspErrors::MREC8EmergencyStop, active);
        if (hlc && active) {
            r_hlc[0]->call_send_error(types::iso15118_charger::EvseError::Error_EmergencyShutdown);
        }
    } else if (error_type == "evse_board_support/MREC10InvalidVehicleMode") {
        active_errors.bsp.set(BspErrors::MREC10InvalidVehicleMode, active);
        if (hlc && active) {
            r_hlc[0]->call_send_error(types::iso15118_charger::EvseError::Error_Malfunction);
        }
    } else if (error_type == "evse_board_support/MREC14PilotFault") {
        active_errors.bsp.set(BspErrors::MREC14PilotFault, active);
        if (hlc && active) {
            r_hlc[0]->call_send_error(types::iso15118_charger::EvseError::Error_Malfunction);
        }
    } else if (error_type == "evse_board_support/MREC15PowerLoss") {
        active_errors.bsp.set(BspErrors::MREC15PowerLoss, active);
        if (hlc && active) {
            r_hlc[0]->call_send_error(types::iso15118_charger::EvseError::Error_Malfunction);
        }
    } else if (error_type == "evse_board_support/MREC17EVSEContactorFault") {
        active_errors.bsp.set(BspErrors::MREC17EVSEContactorFault, active);
        if (hlc && active) {
            r_hlc[0]->call_send_error(types::iso15118_charger::EvseError::Error_Contactor);
        }
    } else if (error_type == "evse_board_support/MREC19CableOverTempStop") {
        active_errors.bsp.set(BspErrors::MREC19CableOverTempStop, active);
        if (hlc && active) {
            r_hlc[0]->call_send_error(types::iso15118_charger::EvseError::Error_Malfunction);
        }
    } else if (error_type == "evse_board_support/MREC20PartialInsertion") {
        active_errors.bsp.set(BspErrors::MREC20PartialInsertion, active);
        if (hlc && active) {
            r_hlc[0]->call_send_error(types::iso15118_charger::EvseError::Error_Malfunction);
        }
    } else if (error_type == "evse_board_support/MREC23ProximityFault") {
        active_errors.bsp.set(BspErrors::MREC23ProximityFault, active);
        if (hlc && active) {
            r_hlc[0]->call_send_error(types::iso15118_charger::EvseError::Error_Malfunction);
        }
    } else if (error_type == "evse_board_support/MREC24ConnectorVoltageHigh") {
        active_errors.bsp.set(BspErrors::MREC24ConnectorVoltageHigh, active);
        if (hlc && active) {
            r_hlc[0]->call_send_error(types::iso15118_charger::EvseError::Error_Malfunction);
        }
    } else if (error_type == "evse_board_support/MREC25BrokenLatch") {
        active_errors.bsp.set(BspErrors::MREC25BrokenLatch, active);
        if (hlc && active) {
            r_hlc[0]->call_send_error(types::iso15118_charger::EvseError::Error_Malfunction);
        }
    } else if (error_type == "evse_board_support/MREC26CutCable") {
        active_errors.bsp.set(BspErrors::MREC26CutCable, active);
        if (hlc && active) {
            r_hlc[0]->call_send_error(types::iso15118_charger::EvseError::Error_Malfunction);
        }
    } else if (error_type == "evse_board_support/VendorError") {
        active_errors.bsp.set(BspErrors::VendorError, active);
        if (hlc && active) {
            r_hlc[0]->call_send_error(types::iso15118_charger::EvseError::Error_Malfunction);
        }
    } else if (error_type == "evse_board_support/CommunicationFault") {
        active_errors.bsp.set(BspErrors::CommunicationFault, active);
        if (hlc && active) {
            r_hlc[0]->call_send_error(types::iso15118_charger::EvseError::Error_Malfunction);
        }
    } else {
        return false;
    }
    // Error stops charging
    return true;
};

bool ErrorHandling::modify_error_connector_lock(const Everest::error::Error& error, bool active) {
    const std::string& error_type = error.type;

    if (active) {
        EVLOG_error << "Raised error " << error_type << ": " << error.description << " (" << error.message << ")";
    } else {
        EVLOG_info << "Cleared error " << error_type << ": " << error.description << " (" << error.message << ")";
    }

    if (error_type == "connector_lock/ConnectorLockCapNotCharged") {
        active_errors.connector_lock.set(ConnectorLockErrors::ConnectorLockCapNotCharged, active);
    } else if (error_type == "connector_lock/ConnectorLockUnexpectedClose") {
        active_errors.connector_lock.set(ConnectorLockErrors::ConnectorLockUnexpectedClose, active);
    } else if (error_type == "connector_lock/ConnectorLockUnexpectedOpen") {
        active_errors.connector_lock.set(ConnectorLockErrors::ConnectorLockUnexpectedOpen, active);
    } else if (error_type == "connector_lock/ConnectorLockFailedLock") {
        active_errors.connector_lock.set(ConnectorLockErrors::ConnectorLockFailedLock, active);
    } else if (error_type == "connector_lock/ConnectorLockFailedUnlock") {
        active_errors.connector_lock.set(ConnectorLockErrors::ConnectorLockFailedUnlock, active);
    } else if (error_type == "connector_lock/MREC1ConnectorLockFailure") {
        active_errors.connector_lock.set(ConnectorLockErrors::MREC1ConnectorLockFailure, active);
    } else if (error_type == "connector_lock/VendorError") {
        active_errors.connector_lock.set(ConnectorLockErrors::VendorError, active);
        if (hlc && active) {
            r_hlc[0]->call_send_error(types::iso15118_charger::EvseError::Error_Malfunction);
        }
    } else {
        // Errors that do not stop charging
        return false;
    }
    // Error stops charging
    return true;
};

bool ErrorHandling::modify_error_ac_rcd(const Everest::error::Error& error, bool active) {
    const std::string& error_type = error.type;

    if (active) {
        EVLOG_error << "Raised error " << error_type << ": " << error.description << " (" << error.message << ")";
    } else {
        EVLOG_info << "Cleared error " << error_type << ": " << error.description << " (" << error.message << ")";
    }

    if (error_type == "ac_rcd/MREC2GroundFailure") {
        active_errors.ac_rcd.set(AcRcdErrors::MREC2GroundFailure, active);
        if (hlc && active) {
            r_hlc[0]->call_send_error(types::iso15118_charger::EvseError::Error_RCD);
        }
    } else if (error_type == "ac_rcd/VendorError") {
        active_errors.ac_rcd.set(AcRcdErrors::VendorError, active);
        if (hlc && active) {
            r_hlc[0]->call_send_error(types::iso15118_charger::EvseError::Error_Malfunction);
        }
    } else if (error_type == "ac_rcd/Selftest") {
        active_errors.ac_rcd.set(AcRcdErrors::Selftest, active);
        if (hlc && active) {
            r_hlc[0]->call_send_error(types::iso15118_charger::EvseError::Error_Malfunction);
        }
    } else if (error_type == "ac_rcd/AC") {
        active_errors.ac_rcd.set(AcRcdErrors::AC, active);
        if (hlc && active) {
            r_hlc[0]->call_send_error(types::iso15118_charger::EvseError::Error_RCD);
        }
    } else if (error_type == "ac_rcd/DC") {
        active_errors.ac_rcd.set(AcRcdErrors::DC, active);
        if (hlc && active) {
            r_hlc[0]->call_send_error(types::iso15118_charger::EvseError::Error_RCD);
        }
    } else {
        // Errors that do not stop charging
        return false;
    }
    // Error stops charging
    return true;
};

bool ErrorHandling::modify_error_evse_manager(const std::string& error_type, bool active) {
    if (error_type == "evse_manager/MREC4OverCurrentFailure") {
        active_errors.bsp.set(BspErrors::MREC4OverCurrentFailure, active);
        if (hlc && active) {
            r_hlc[0]->call_send_error(types::iso15118_charger::EvseError::Error_Malfunction);
        }

    } else if (error_type == "evse_manager/PowermeterTransactionStartFailed") {
        active_errors.evse_manager.set(EvseManagerErrors::PowermeterTransactionStartFailed, active);
        if (hlc && active) {
            r_hlc[0]->call_send_error(types::iso15118_charger::EvseError::Error_Malfunction);
        }
    } else if (error_type == "evse_manager/PermanentFault") {
        active_errors.evse_manager.set(EvseManagerErrors::PermanentFault, active);
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

bool ErrorHandling::modify_error_imd(const Everest::error::Error& error, bool active) {
    const std::string& error_type = error.type;

    if (active) {
        EVLOG_error << "Raised error " << error_type << ": " << error.description << " (" << error.message << ")";
    } else {
        EVLOG_info << "Cleared error " << error_type << ": " << error.description << " (" << error.message << ")";
    }

    if (error_type == "isolation_monitor/DeviceFault") {
        active_errors.imd.set(IMDErrors::DeviceFault, active);
        if (hlc && active) {
            r_hlc[0]->call_send_error(types::iso15118_charger::EvseError::Error_Malfunction);
        }
    } else if (error_type == "isolation_monitor/CommunicationFault") {
        active_errors.imd.set(IMDErrors::CommunicationFault, active);
        if (hlc && active) {
            r_hlc[0]->call_send_error(types::iso15118_charger::EvseError::Error_Malfunction);
        }
    } else if (error_type == "isolation_monitor/VendorError") {
        active_errors.connector_lock.set(ConnectorLockErrors::VendorError, active);
        if (hlc && active) {
            r_hlc[0]->call_send_error(types::iso15118_charger::EvseError::Error_Malfunction);
        }
    } else {
        // Errors that do not stop charging
        return false;
    }
    // Error stops charging
    return true;
};

} // namespace module
