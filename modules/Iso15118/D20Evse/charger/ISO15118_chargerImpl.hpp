// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#ifndef CHARGER_ISO15118_CHARGER_IMPL_HPP
#define CHARGER_ISO15118_CHARGER_IMPL_HPP

//
// AUTO GENERATED - MARKED REGIONS WILL BE KEPT
// template version 3
//

#include <generated/interfaces/ISO15118_charger/Implementation.hpp>

#include "../D20Evse.hpp"

// ev@75ac1216-19eb-4182-a85c-820f1fc2c091:v1
// insert your custom include headers here
// ev@75ac1216-19eb-4182-a85c-820f1fc2c091:v1

namespace module {
namespace charger {

struct Conf {};

class ISO15118_chargerImpl : public ISO15118_chargerImplBase {
public:
    ISO15118_chargerImpl() = delete;
    ISO15118_chargerImpl(Everest::ModuleAdapter* ev, const Everest::PtrContainer<D20Evse>& mod, Conf& config) :
        ISO15118_chargerImplBase(ev, "charger"), mod(mod), config(config){};

    // ev@8ea32d28-373f-4c90-ae5e-b4fcc74e2a61:v1
    // insert your public definitions here
    // ev@8ea32d28-373f-4c90-ae5e-b4fcc74e2a61:v1

protected:
    // command handler functions (virtual)
    virtual void handle_set_EVSEID(std::string& EVSEID, std::string& EVSEID_DIN) override;
    virtual void handle_set_PaymentOptions(Array& PaymentOptions) override;
    virtual void handle_set_SupportedEnergyTransferMode(Array& SupportedEnergyTransferMode) override;
    virtual void handle_set_AC_EVSENominalVoltage(double& EVSENominalVoltage) override;
    virtual void handle_set_DC_EVSECurrentRegulationTolerance(double& EVSECurrentRegulationTolerance) override;
    virtual void handle_set_DC_EVSEPeakCurrentRipple(double& EVSEPeakCurrentRipple) override;
    virtual void handle_set_ReceiptRequired(bool& ReceiptRequired) override;
    virtual void handle_set_FreeService(bool& FreeService) override;
    virtual void handle_set_EVSEEnergyToBeDelivered(double& EVSEEnergyToBeDelivered) override;
    virtual void handle_enable_debug_mode(types::iso15118_charger::DebugMode& debug_mode) override;
    virtual void handle_set_Auth_Okay_EIM(bool& auth_okay_eim) override;
    virtual void handle_set_Auth_Okay_PnC(types::authorization::AuthorizationStatus& status,
                                          types::authorization::CertificateStatus& certificateStatus) override;
    virtual void handle_set_FAILED_ContactorError(bool& ContactorError) override;
    virtual void handle_set_RCD_Error(bool& RCD) override;
    virtual void handle_stop_charging(bool& stop_charging) override;
    virtual void handle_set_DC_EVSEPresentVoltageCurrent(
        types::iso15118_charger::DC_EVSEPresentVoltage_Current& EVSEPresentVoltage_Current) override;
    virtual void handle_set_AC_EVSEMaxCurrent(double& EVSEMaxCurrent) override;
    virtual void
    handle_set_DC_EVSEMaximumLimits(types::iso15118_charger::DC_EVSEMaximumLimits& EVSEMaximumLimits) override;
    virtual void
    handle_set_DC_EVSEMinimumLimits(types::iso15118_charger::DC_EVSEMinimumLimits& EVSEMinimumLimits) override;
    virtual void handle_set_EVSEIsolationStatus(types::iso15118_charger::IsolationStatus& EVSEIsolationStatus) override;
    virtual void handle_set_EVSE_UtilityInterruptEvent(bool& EVSE_UtilityInterruptEvent) override;
    virtual void handle_set_EVSE_Malfunction(bool& EVSE_Malfunction) override;
    virtual void handle_set_EVSE_EmergencyShutdown(bool& EVSE_EmergencyShutdown) override;
    virtual void handle_set_MeterInfo(types::powermeter::Powermeter& powermeter) override;
    virtual void handle_contactor_closed(bool& status) override;
    virtual void handle_contactor_open(bool& status) override;
    virtual void handle_cableCheck_Finished(bool& status) override;
    virtual void handle_set_Certificate_Service_Supported(bool& status) override;
    virtual void
    handle_set_Get_Certificate_Response(types::iso15118_charger::Response_Exi_Stream_Status& Existream_Status) override;
    virtual void handle_dlink_ready(bool& value) override;
    virtual void handle_supporting_sae_j2847_bidi(types::iso15118_charger::SAE_J2847_Bidi_Mode& mode) override;

    // ev@d2d1847a-7b88-41dd-ad07-92785f06f5c4:v1
    // insert your protected definitions here
    // ev@d2d1847a-7b88-41dd-ad07-92785f06f5c4:v1

private:
    const Everest::PtrContainer<D20Evse>& mod;
    const Conf& config;

    virtual void init() override;
    virtual void ready() override;

    // ev@3370e4dd-95f4-47a9-aaec-ea76f34a66c9:v1
    // insert your private definitions here
    // ev@3370e4dd-95f4-47a9-aaec-ea76f34a66c9:v1
};

// ev@3d7da0ad-02c2-493d-9920-0bbbd56b9876:v1
// insert other definitions here
// ev@3d7da0ad-02c2-493d-9920-0bbbd56b9876:v1

} // namespace charger
} // namespace module

#endif // CHARGER_ISO15118_CHARGER_IMPL_HPP
