// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#include "TestPowermeter.hpp"

namespace module {

void TestPowermeter::init() {
    invoke_init(*p_if_impl_id_empty);
    r_powermeter->subscribe_powermeter([this](types::powermeter::Powermeter pm){
        EVLOG_info << "Powermeter values received:";
        EVLOG_info << "Import Device Energy: " << pm.energy_Wh_import.total << " Wh";
        if(pm.power_W.has_value()){
            EVLOG_info << "Import Device Power: " << pm.power_W.value().total << " W";
        }
        if(pm.voltage_V.has_value()){
            if(pm.voltage_V.value().DC.has_value()){
                EVLOG_info << "Voltage: " << pm.voltage_V.value().DC.value() << " V";
            }
        }
        if(pm.current_A.has_value()){
            if(pm.current_A.value().DC.has_value()){
                EVLOG_info << "Current: " << pm.current_A.value().DC.value() << " A";
            }
        }
    });
}

void TestPowermeter::ready() {
    invoke_ready(*p_if_impl_id_empty);
    //thread to start and stop transactions
    types::powermeter::TransactionReq reqData;
    reqData.evse_id = "MyTestCar";
    reqData.transaction_id = "OCPP transaction UUID";
    reqData.client_id = "TestUser";
    reqData.tariff_id = 123;
    reqData.cable_id = 1;
    reqData.user_data = "BlaBla";
    bool transaction_assigned_to_user = true;
    reqData.transaction_assigned_to_user = transaction_assigned_to_user;
    types::powermeter::UserIdType user_id_type;
    user_id_type = (types::powermeter::UserIdType)gsh01_app_layer::UserIdType::ISO14443;
    reqData.user_identification_type = user_id_type;

    std::thread([this, reqData]{
            r_powermeter->call_start_transaction(reqData);
            sleep(60);
            r_powermeter->call_stop_transaction(reqData.transaction_id);
            sleep(1800);
    }).detach();
}

} // namespace module
