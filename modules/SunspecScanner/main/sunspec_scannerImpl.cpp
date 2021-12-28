#include "sunspec_scannerImpl.hpp"

namespace module {
namespace main {

void sunspec_scannerImpl::init() {
    mod->mqtt.subscribe("/external/cmd/scan_unit", [](json data) {
        EVLOG(debug) << "Received data from external MQTT handler." << data.dump();
    });
}

void sunspec_scannerImpl::ready() {

}

Object sunspec_scannerImpl::handle_scan_unit(std::string& ip_address, int& port, int& unit){
    // your code for cmd scan_unit goes here
    return {};
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
