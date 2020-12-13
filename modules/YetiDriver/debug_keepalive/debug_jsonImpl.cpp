#include "debug_jsonImpl.hpp"

namespace module {
namespace debug_keepalive {

void debug_jsonImpl::init() {
    mod->serial.signalKeepAliveLo.connect([this](const KeepAliveLo& k) {
        auto k_json = keep_alive_lo_to_json(k);
        mod->mqtt.publish("/external/keepalive_json", k_json.dump());
        publish_debug_json(k_json);
    });
}

void debug_jsonImpl::ready() {
}

} // namespace debug_keepalive
} // namespace module
