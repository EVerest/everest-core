#include "powermeterImpl.hpp"

namespace module {
namespace powermeter {

void powermeterImpl::init() {
    mod->r_powermeter->subscribe_powermeter([this](json p) {
        // Republish data on proxy powermeter interface
        publish_powermeter(p);
    });
}

void powermeterImpl::ready() {

}

std::string powermeterImpl::handle_get_signed_meter_value(std::string& auth_token){
    return mod->r_powermeter->call_get_signed_meter_value(auth_token);
};

} // namespace powermeter
} // namespace module
