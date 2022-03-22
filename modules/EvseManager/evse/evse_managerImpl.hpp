// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#ifndef EVSE_EVSE_MANAGER_IMPL_HPP
#define EVSE_EVSE_MANAGER_IMPL_HPP

//
// AUTO GENERATED - MARKED REGIONS WILL BE KEPT
// template version 1
//

#include <generated/evse_manager/Implementation.hpp>

#include "../EvseManager.hpp"
#include <algorithm>

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

    void set_session_uuid();


protected:
    // command handler functions (virtual)
    virtual bool handle_enable() override;
    virtual bool handle_disable() override;
    virtual bool handle_set_faulted() override;
    virtual bool handle_pause_charging() override;
    virtual bool handle_resume_charging() override;
    virtual bool handle_cancel_charging() override;
    virtual bool handle_accept_new_session() override;
    virtual std::string handle_reserve_now(int& reservation_id, std::string& auth_token, std::string& expiry_date,
                                           std::string& parent_id) override;
    virtual bool handle_cancel_reservation() override;
    virtual bool handle_force_unlock() override;
    virtual std::string handle_set_local_max_current(double& max_current) override;
    virtual std::string handle_switch_three_phases_while_charging(bool& three_phases) override;
    virtual std::string handle_get_signed_meter_value() override;

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
    json limits;

    std::string generate_session_uuid();

    std::string session_uuid;
    // ev@3370e4dd-95f4-47a9-aaec-ea76f34a66c9:v1
};

// ev@3d7da0ad-02c2-493d-9920-0bbbd56b9876:v1
// insert other definitions here
// ev@3d7da0ad-02c2-493d-9920-0bbbd56b9876:v1

} // namespace evse
} // namespace module

#endif // EVSE_EVSE_MANAGER_IMPL_HPP
