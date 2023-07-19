// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include "ISO15118_chargerImpl.hpp"

namespace module {
namespace main {

void ISO15118_chargerImpl::init() {
}

void ISO15118_chargerImpl::ready() {
}

void ISO15118_chargerImpl::handle_set_EVSEID(std::string& EVSEID, std::string& EVSEID_DIN){
    // your code for cmd set_EVSEID goes here
};

void ISO15118_chargerImpl::handle_set_PaymentOptions(Array& PaymentOptions){
    // your code for cmd set_PaymentOptions goes here
};

void ISO15118_chargerImpl::handle_set_SupportedEnergyTransferMode(Array& SupportedEnergyTransferMode){
    // your code for cmd set_SupportedEnergyTransferMode goes here
};

void ISO15118_chargerImpl::handle_set_AC_EVSENominalVoltage(double& EVSENominalVoltage){
    // your code for cmd set_AC_EVSENominalVoltage goes here
};

void ISO15118_chargerImpl::handle_set_DC_EVSECurrentRegulationTolerance(double& EVSECurrentRegulationTolerance){
    // your code for cmd set_DC_EVSECurrentRegulationTolerance goes here
};

void ISO15118_chargerImpl::handle_set_DC_EVSEPeakCurrentRipple(double& EVSEPeakCurrentRipple){
    // your code for cmd set_DC_EVSEPeakCurrentRipple goes here
};

void ISO15118_chargerImpl::handle_set_ReceiptRequired(bool& ReceiptRequired){
    // your code for cmd set_ReceiptRequired goes here
};

void ISO15118_chargerImpl::handle_set_FreeService(bool& FreeService){
    // your code for cmd set_FreeService goes here
};

void ISO15118_chargerImpl::handle_set_EVSEEnergyToBeDelivered(double& EVSEEnergyToBeDelivered){
    // your code for cmd set_EVSEEnergyToBeDelivered goes here
};

void ISO15118_chargerImpl::handle_enable_debug_mode(types::iso15118_charger::DebugMode& debug_mode){
    // your code for cmd enable_debug_mode goes here
};

void ISO15118_chargerImpl::handle_set_Auth_Okay_EIM(bool& auth_okay_eim){
    // your code for cmd set_Auth_Okay_EIM goes here
};

void ISO15118_chargerImpl::handle_set_Auth_Okay_PnC(types::authorization::AuthorizationStatus& status,
                                                    types::authorization::CertificateStatus& certificateStatus){
    // your code for cmd set_Auth_Okay_PnC goes here
};

void ISO15118_chargerImpl::handle_set_FAILED_ContactorError(bool& ContactorError){
    // your code for cmd set_FAILED_ContactorError goes here
};

void ISO15118_chargerImpl::handle_set_RCD_Error(bool& RCD){
    // your code for cmd set_RCD_Error goes here
};

void ISO15118_chargerImpl::handle_stop_charging(bool& stop_charging){
    // your code for cmd stop_charging goes here
};

void ISO15118_chargerImpl::handle_set_DC_EVSEPresentVoltageCurrent(
    types::iso15118_charger::DC_EVSEPresentVoltage_Current& EVSEPresentVoltage_Current){
    // your code for cmd set_DC_EVSEPresentVoltageCurrent goes here
};

void ISO15118_chargerImpl::handle_set_AC_EVSEMaxCurrent(double& EVSEMaxCurrent){
    // your code for cmd set_AC_EVSEMaxCurrent goes here
};

void ISO15118_chargerImpl::handle_set_DC_EVSEMaximumLimits(
    types::iso15118_charger::DC_EVSEMaximumLimits& EVSEMaximumLimits){
    // your code for cmd set_DC_EVSEMaximumLimits goes here
};

void ISO15118_chargerImpl::handle_set_DC_EVSEMinimumLimits(
    types::iso15118_charger::DC_EVSEMinimumLimits& EVSEMinimumLimits){
    // your code for cmd set_DC_EVSEMinimumLimits goes here
};

void ISO15118_chargerImpl::handle_set_EVSEIsolationStatus(
    types::iso15118_charger::IsolationStatus& EVSEIsolationStatus){
    // your code for cmd set_EVSEIsolationStatus goes here
};

void ISO15118_chargerImpl::handle_set_EVSE_UtilityInterruptEvent(bool& EVSE_UtilityInterruptEvent){
    // your code for cmd set_EVSE_UtilityInterruptEvent goes here
};

void ISO15118_chargerImpl::handle_set_EVSE_Malfunction(bool& EVSE_Malfunction){
    // your code for cmd set_EVSE_Malfunction goes here
};

void ISO15118_chargerImpl::handle_set_EVSE_EmergencyShutdown(bool& EVSE_EmergencyShutdown){
    // your code for cmd set_EVSE_EmergencyShutdown goes here
};

void ISO15118_chargerImpl::handle_set_MeterInfo(types::powermeter::Powermeter& powermeter){
    // your code for cmd set_MeterInfo goes here
};

void ISO15118_chargerImpl::handle_contactor_closed(bool& status){
    // your code for cmd contactor_closed goes here
};

void ISO15118_chargerImpl::handle_contactor_open(bool& status){
    // your code for cmd contactor_open goes here
};

void ISO15118_chargerImpl::handle_cableCheck_Finished(bool& status){
    // your code for cmd cableCheck_Finished goes here
};

void ISO15118_chargerImpl::handle_set_Certificate_Service_Supported(bool& status){
    // your code for cmd set_Certificate_Service_Supported goes here
};

void ISO15118_chargerImpl::handle_set_Get_Certificate_Response(
    types::iso15118_charger::Response_Exi_Stream_Status& Existream_Status){
    // your code for cmd set_Get_Certificate_Response goes here
};

void ISO15118_chargerImpl::handle_dlink_ready(bool& value) {
}

} // namespace main
} // namespace module
