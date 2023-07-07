#include "sunspec_scannerImpl.hpp"

using namespace everest;

namespace module {
namespace main {

json get_sunspec_device_mapping_information(const sunspec::SunspecDeviceMapping& sdm, const std::string& ip_address, const int& port, const int& unit) {

        json result;
        result["data"] = json::array();
        std::stringstream result_message;
        for (int i = 0; i < sdm.get_devices().size(); i++) {
            // Looping through each device and getting general information
            const auto& device = sdm.get_devices()[i];
            json device_information = device->get_device_information();
            json el_device;
            el_device["ip-address"] = ip_address;
            el_device["port"] = port;
            el_device["unit"] = unit;
            el_device["index"] = i;
            el_device["manufacturer"] = device_information["Mn"];
            el_device["device_model"] = device_information["Md"];
            el_device["version"] = device_information["Vn"];
            el_device["serial_number"] = device_information["SN"];
            el_device["models"] = json::array();
            for (const auto& model : device->get_models()) {
                // Getting information from each model within the logical device
                json el_model;
                el_model["id"] = model->model_id;
                el_model["name"] = model->get_name();
                el_device["models"].push_back(el_model);
            }
            result["data"].push_back(el_device);
        }

        if (result["data"].size() > 0) {
            result_message << "SUNSPEC device found at given address and unit ID. Scanning process finished successfully.";
            result["success"] = true;
        }

        result["message"] = result_message.str();
        EVLOG_debug << "Returning result from SunspecScanner callback: " << result;

        return result;
}

json scan_unit(const std::string& ip_address, const int& port, const int& unit) {

    json result;

    // Attempting scan
    std::stringstream result_message;
    try {
        connection::TCPConnection connection_(ip_address, port);
        modbus::ModbusIPClient modbus_client(connection_);
        sunspec::SunspecDeviceMapping sdm(modbus_client, unit);
        EVLOG_debug << "Trying to scan SunspecDeviceMapping...";
        sdm.scan();
        EVLOG_debug << "Trying to obtain scanned device mapping information...";
        return get_sunspec_device_mapping_information(sdm, ip_address, port, unit);
    }

    catch (connection::exceptions::connection_error& e) {
        result_message << "No device found in the specified given IP address and port. " << e.what();
        result["message"] = result_message.str();
        EVLOG_error << result["message"];
        result["success"] = false;
        return result;
    }
    catch (std::exception& e) {
        result_message << e.what();
        result["message"] = result_message.str();
        EVLOG_error << result["message"];
        result["success"] = false;
        return result;
    }

}

void sunspec_scannerImpl::init() {
    // Subscribing to nodered interface topics
    mod->mqtt.subscribe("/external/cmd/scan_unit", [this](std::string request_str) {
        json request = json::parse(request_str);
        std::string ip_address = request["ip_address"];
        int unit = request["unit"];
        int port = request["port"];
        const auto& scan_result = scan_unit(ip_address, port, unit);
        this->mod->mqtt.publish("/external/cmd/scan_unit/res", scan_result.dump());
    });
}

void sunspec_scannerImpl::ready() {

}

Object sunspec_scannerImpl::handle_scan_unit(std::string& ip_address, int& port, int& unit){
    return scan_unit(ip_address, port, unit);
};

Object sunspec_scannerImpl::handle_scan_port(std::string& ip_address, int& port){
    // your code for cmd scan_port goes here
    return {};
};

Object sunspec_scannerImpl::handle_scan_device(std::string& ip_address){
    // your code for cmd scan_device goes here
    return {};
};

Object sunspec_scannerImpl::handle_scan_network(){
    // your code for cmd scan_network goes here
    return {};
};

} // namespace main
} // namespace module
