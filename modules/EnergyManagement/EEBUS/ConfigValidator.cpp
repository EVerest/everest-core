
#include <ConfigValidator.hpp>

#include "EEBUS.hpp"

namespace module {

ConfigValidator::ConfigValidator(const Conf& config, std::filesystem::path etc_prefix,
                                 std::filesystem::path libexec_prefix) :
    config(config),
    control_service_rpc_endpoint(config.control_service_rpc_port),
    manage_eebus_grpc_api_binary(config.manage_eebus_grpc_api_binary),
    etc_prefix(std::move(etc_prefix)),
    libexec_prefix(std::move(libexec_prefix)),
    certificate_path(std::filesystem::path(config.certificate_path)),
    private_key_path(std::filesystem::path(config.private_key_path)),
    eebus_grpc_api_binary_path(std::filesystem::path(config.eebus_grpc_api_binary_path)) {
}

bool ConfigValidator::validate() {
    bool valid = true;
    valid &= this->validate_control_service_rpc_port();
    valid &= this->validate_eebus_ems_ski();
    valid &= this->validate_certificate_path();
    valid &= this->validate_private_key_path();
    valid &= this->validate_eebus_grpc_api_binary_path();
    valid &= this->validate_manage_eebus_grpc_api_binary();
    valid &= this->validate_vendor_code();
    valid &= this->validate_device_brand();
    valid &= this->validate_device_model();
    valid &= this->validate_serial_number();
    return valid;
}

std::filesystem::path ConfigValidator::get_certificate_path() const {
    return this->certificate_path;
}

std::filesystem::path ConfigValidator::get_private_key_path() const {
    return this->private_key_path;
}

std::filesystem::path ConfigValidator::get_eebus_grpc_api_binary_path() const {
    return this->eebus_grpc_api_binary_path;
}

int ConfigValidator::get_control_service_port() const {
    return this->control_service_rpc_endpoint;
}

std::string ConfigValidator::get_vendor_code() const {
    return this->config.vendor_code;
}

std::string ConfigValidator::get_device_brand() const {
    return this->config.device_brand;
}

std::string ConfigValidator::get_device_model() const {
    return this->config.device_model;
}

std::string ConfigValidator::get_serial_number() const {
    return this->config.serial_number;
}

bool ConfigValidator::validate_control_service_rpc_port() const {
    if (this->config.control_service_rpc_port < 0) {
        EVLOG_error << "control service rpc port is negative";
        return false;
    }
    return true;
}

bool ConfigValidator::validate_eebus_ems_ski() const {
    if (this->config.eebus_ems_ski.empty()) {
        EVLOG_error << "EEBUS EMS SKI is empty";
        return false;
    }
    return true;
}

bool ConfigValidator::validate_certificate_path() {
    if (!this->manage_eebus_grpc_api_binary) {
        return true;
    }
    if (this->certificate_path.is_relative()) {
        this->certificate_path = this->etc_prefix / "certs" / this->certificate_path;
        EVLOG_info << "Certificate path is relative, using etc prefix: " << this->certificate_path;
    }
    if (!std::filesystem::exists(this->certificate_path)) {
        EVLOG_error << "Certificate file does not exist";
        return false;
    }
    return true;
}

bool ConfigValidator::validate_private_key_path() {
    if (!this->manage_eebus_grpc_api_binary) {
        return true;
    }
    if (this->private_key_path.is_relative()) {
        this->private_key_path = this->etc_prefix / "certs" / this->private_key_path;
        EVLOG_info << "Key path is relative, using etc prefix: " << this->private_key_path;
    }
    if (!std::filesystem::exists(this->private_key_path)) {
        EVLOG_error << "Key file does not exist";
        return false;
    }
    return true;
}

bool ConfigValidator::validate_eebus_grpc_api_binary_path() {
    if (!this->manage_eebus_grpc_api_binary) {
        return true;
    }
    if (this->eebus_grpc_api_binary_path.is_relative()) {
        this->eebus_grpc_api_binary_path = this->libexec_prefix / this->eebus_grpc_api_binary_path;
        EVLOG_info << "EEBUS GRPC API binary path is relative, using libexec prefix: "
                   << this->eebus_grpc_api_binary_path;
    }
    if (!std::filesystem::exists(this->eebus_grpc_api_binary_path)) {
        EVLOG_error << "EEBUS GRPC API binary does not exist";
        return false;
    }
    std::filesystem::perms perms = std::filesystem::status(this->eebus_grpc_api_binary_path).permissions();
    std::filesystem::perms owner_exec_perms = std::filesystem::perms::owner_exec;
    if ((perms & owner_exec_perms) == std::filesystem::perms::none) {
        EVLOG_error << "EEBUS GRPC API binary is not executable";
        return false;
    }
    return true;
}

bool ConfigValidator::validate_manage_eebus_grpc_api_binary() const {
    return true;
}

bool ConfigValidator::validate_vendor_code() const {
    return !this->config.vendor_code.empty();
}

bool ConfigValidator::validate_device_brand() const {
    return !this->config.device_brand.empty();
}

bool ConfigValidator::validate_device_model() const {
    return !this->config.device_model.empty();
}

bool ConfigValidator::validate_serial_number() const {
    return !this->config.serial_number.empty();
}

} // namespace module
