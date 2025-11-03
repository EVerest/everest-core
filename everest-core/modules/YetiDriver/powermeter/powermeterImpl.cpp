#include "powermeterImpl.hpp"

namespace module {
namespace powermeter {


static Everest::json yeti_to_everest (const PowerMeter& p) {
    Everest::json j;

    j["timestamp"] = p.time_stamp;
    j["meter_id"] = "YETI_POWERMETER";
    j["phase_seq_error"] = p.phaseSeqError;

    j["energy_Wh_import"]["total"] = p.totalWattHr;
    j["energy_Wh_import"]["L1"] = p.wattHrL1;
    j["energy_Wh_import"]["L2"] = p.wattHrL2;
    j["energy_Wh_import"]["L3"] = p.wattHrL3;

    j["power_W"]["total"] = p.wattL1+p.wattL2+p.wattL3;
    j["power_W"]["L1"] = p.wattL1;
    j["power_W"]["L2"] = p.wattL2;
    j["power_W"]["L3"] = p.wattL3;

    j["voltage_V"]["L1"] = p.vrmsL1;
    j["voltage_V"]["L2"] = p.vrmsL2;
    j["voltage_V"]["L3"] = p.vrmsL3;

    j["current_A"]["L1"] = p.irmsL1;
    j["current_A"]["L2"] = p.irmsL2;
    j["current_A"]["L3"] = p.irmsL3;
    j["current_A"]["N"] = p.irmsN;

    j["frequency_Hz"]["L1"] = p.freqL1;
    j["frequency_Hz"]["L2"] = p.freqL2;
    j["frequency_Hz"]["L3"] = p.freqL3;

    return j;
}

void powermeterImpl::init() {
    mod->serial.signalPowerMeter.connect([this](const PowerMeter& p) {
        publish_powermeter(yeti_to_everest(p));

        mod->mqtt.publish("/external/powermeter/phaseSeqError", p.phaseSeqError);
        mod->mqtt.publish("/external/powermeter/time_stamp", (int)p.time_stamp);
        mod->mqtt.publish("/external/powermeter/tempL1", p.tempL1);
        mod->mqtt.publish("/external/powermeter/totalKw", (p.wattL1 + p.wattL2 + p.wattL3) / 1000., 1);
        mod->mqtt.publish("/external/powermeter/totalKWattHr", p.totalWattHr / 1000., 2);
        mod->mqtt.publish("/external/powermeter_json", power_meter_data_to_json(p).dump());
    });
}

void powermeterImpl::ready() {
}

std::string powermeterImpl::handle_get_signed_meter_value(std::string& auth_token) {
    return "NOT_AVAILABLE";
}



} // namespace powermeter
} // namespace module
