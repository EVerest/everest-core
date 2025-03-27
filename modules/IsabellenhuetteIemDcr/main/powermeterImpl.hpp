// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#ifndef MAIN_POWERMETER_IMPL_HPP
#define MAIN_POWERMETER_IMPL_HPP

//
// AUTO GENERATED - MARKED REGIONS WILL BE KEPT
// template version 3
//

#include <generated/interfaces/powermeter/Implementation.hpp>

#include "../IsabellenhuetteIemDcr.hpp"

// ev@75ac1216-19eb-4182-a85c-820f1fc2c091:v1
// insert your custom include headers here
#include "httpClientInterface.hpp"
#include "isabellenhuetteIemDcrController.hpp"
// ev@75ac1216-19eb-4182-a85c-820f1fc2c091:v1

namespace module {
namespace main {

struct Conf {};

class powermeterImpl : public powermeterImplBase {
public:
    powermeterImpl() = delete;
    powermeterImpl(Everest::ModuleAdapter* ev, const Everest::PtrContainer<IsabellenhuetteIemDcr>& mod, Conf& config) :
        powermeterImplBase(ev, "main"), mod(mod), config(config), dcr_status(){};

    // ev@8ea32d28-373f-4c90-ae5e-b4fcc74e2a61:v1
    // insert your public definitions here
    // ev@8ea32d28-373f-4c90-ae5e-b4fcc74e2a61:v1

protected:
    // command handler functions (virtual)
    virtual types::powermeter::TransactionStartResponse
    handle_start_transaction(types::powermeter::TransactionReq& value) override;
    virtual types::powermeter::TransactionStopResponse handle_stop_transaction(std::string& transaction_id) override;

    // ev@d2d1847a-7b88-41dd-ad07-92785f06f5c4:v1
    // insert your protected definitions here
    // ev@d2d1847a-7b88-41dd-ad07-92785f06f5c4:v1

private:
    const Everest::PtrContainer<IsabellenhuetteIemDcr>& mod;
    const Conf& config;

    virtual void init() override;
    virtual void ready() override;

    // ev@3370e4dd-95f4-47a9-aaec-ea76f34a66c9:v1
    // insert your private definitions here
    IsaIemDcrController::ThreadSafeString dcr_status;
    std::atomic<bool> is_initialized = false;
    std::atomic<bool> transaction_active = true;
    std::atomic<bool> start_transaction_running = false;
    std::atomic<bool> stop_transaction_running = false;
    // At construction time, there is no controller and no HTTP client, so these are null pointers.
    // When init() is called, the controller is initialized.
    std::unique_ptr<IsaIemDcrController> controller = nullptr;
    // The live_measure_publisher thread handles the periodic (1/s) publication of the live measurements
    // Initially it's a default-constructed thread (which is valid, but doesn't represent an actual running thread)
    // In ready(), the live_measure_publisher thread is started and placed in this field.
    std::thread live_measure_publisher_thread;
    // private functions
    void check_config();
    // ev@3370e4dd-95f4-47a9-aaec-ea76f34a66c9:v1
};

// ev@3d7da0ad-02c2-493d-9920-0bbbd56b9876:v1
// insert other definitions here
// ev@3d7da0ad-02c2-493d-9920-0bbbd56b9876:v1

} // namespace main
} // namespace module

#endif // MAIN_POWERMETER_IMPL_HPP
