// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#ifndef EVSE_EVSE_MANAGER_IMPL_HPP
#define EVSE_EVSE_MANAGER_IMPL_HPP

//
// AUTO GENERATED - MARKED REGIONS WILL BE KEPT
// template version 3
//

#include <generated/interfaces/evse_manager/Implementation.hpp>

#include "../EvseManager.hpp"

// ev@75ac1216-19eb-4182-a85c-820f1fc2c091:v1
// insert your custom include headers here
// ev@75ac1216-19eb-4182-a85c-820f1fc2c091:v1

namespace module {
namespace evse {

struct Conf {};

class evse_managerImpl : public evse_managerImplBase {
public:
    evse_managerImpl() = delete;
    evse_managerImpl(Everest::ModuleAdapter* ev, const Everest::PtrContainer<EvseManager>& mod, Conf& config) :
        evse_managerImplBase(ev, "evse"), mod(mod), config(config){};

    // ev@8ea32d28-373f-4c90-ae5e-b4fcc74e2a61:v1
    // insert your public definitions here
    void set_nr_of_phases_available(int n);
    // ev@8ea32d28-373f-4c90-ae5e-b4fcc74e2a61:v1

protected:
    // command handler functions (virtual)
    virtual types::evse_manager::Evse handle_get_evse() override;
    virtual bool handle_enable_disable(int& connector_id,
                                       types::evse_manager::EnableDisableSource& cmd_source) override;
    virtual void handle_authorize_response(types::authorization::ProvidedIdToken& provided_token,
                                           types::authorization::ValidationResult& validation_result) override;
    virtual void handle_withdraw_authorization() override;
    virtual bool handle_reserve(int& reservation_id) override;
    virtual void handle_cancel_reservation() override;
    virtual void handle_set_faulted() override;
    virtual bool handle_pause_charging() override;
    virtual bool handle_resume_charging() override;
    virtual bool handle_stop_transaction(types::evse_manager::StopTransactionRequest& request) override;
    virtual bool handle_force_unlock(int& connector_id) override;
    virtual void handle_set_external_limits(types::energy::ExternalLimits& value) override;
    virtual types::evse_manager::SwitchThreePhasesWhileChargingResult
    handle_switch_three_phases_while_charging(bool& three_phases) override;
    virtual void handle_set_get_certificate_response(
        types::iso15118_charger::Response_Exi_Stream_Status& certificate_response) override;
    virtual bool handle_external_ready_to_start_charging() override;

    // ev@d2d1847a-7b88-41dd-ad07-92785f06f5c4:v1
    // insert your protected definitions here
    // ev@d2d1847a-7b88-41dd-ad07-92785f06f5c4:v1

private:
    const Everest::PtrContainer<EvseManager>& mod;
    const Conf& config;

    virtual void init() override;
    virtual void ready() override;

    // ev@3370e4dd-95f4-47a9-aaec-ea76f34a66c9:v1
    // insert your private definitions here
    std::atomic_bool connector_status_changed{false};
    types::evse_manager::Limits limits;

    std::string generate_session_uuid();
    void set_session_uuid();

    std::string session_uuid;
    // ev@3370e4dd-95f4-47a9-aaec-ea76f34a66c9:v1
};

// ev@3d7da0ad-02c2-493d-9920-0bbbd56b9876:v1
// insert other definitions here
// ev@3d7da0ad-02c2-493d-9920-0bbbd56b9876:v1

} // namespace evse
} // namespace module

#endif // EVSE_EVSE_MANAGER_IMPL_HPP
