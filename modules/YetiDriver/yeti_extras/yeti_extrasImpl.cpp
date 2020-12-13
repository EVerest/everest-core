#include "yeti_extrasImpl.hpp"

namespace module {
namespace yeti_extras {

void yeti_extrasImpl::init() {
    mod->serial.signalKeepAliveLo.connect([this](const KeepAliveLo& k) {
        publish_time_stamp(k.time_stamp);
        publish_hw_type(k.hw_type);
        publish_hw_revision(k.hw_revision);
        publish_protocol_version_major(k.protocol_version_major);
        publish_protocol_version_minor(k.protocol_version_minor);
        publish_sw_version_string(k.sw_version_string);
    });
}

void yeti_extrasImpl::ready() {
}

void yeti_extrasImpl::handle_firmware_update(std::string& firmware_binary){
    // your code for cmd firmware_update goes here
};

} // namespace yeti_extras
} // namespace module
