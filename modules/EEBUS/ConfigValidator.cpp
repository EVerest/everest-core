
#include <ConfigValidator.hpp>

#include "EEBUS.hpp"

namespace module {

ConfigValidator::ConfigValidator(const Conf& config, std::filesystem::path etc_prefix, std::filesystem::path libexec_prefix) : config(config) {
    this->etc_prefix = etc_prefix;
    this->libexec_prefix = libexec_prefix;

    this->certificate_path = std::filesystem::path(this->config.certificate_path);
    this->private_key_path = std::filesystem::path(this->config.private_key_path);
    this->eebus_grpc_api_binary_path = std::filesystem::path(this->config.eebus_grpc_api_binary_path);

    this->control_service_rpc_endpoint = "localhost:" + std::to_string(this->config.control_service_rpc_port);
    this->manage_eebus_grpc_api_binary = this->config.manage_eebus_grpc_api_binary;
}

bool ConfigValidator::validate() {
    bool valid = true;
    valid &= this->validate_control_service_rpc_port();
    valid &= this->validate_eebus_ems_ski();
    valid &= this->validate_certificate_path();
    valid &= this->validate_private_key_path();
    valid &= this->validate_eebus_grpc_api_binary_path();
    valid &= this->validate_manage_eebus_grpc_api_binary();
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

std::string ConfigValidator::get_control_service_rpc_endpoint() const {
    return this->control_service_rpc_endpoint;
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
        EVLOG_info << "EEBUS GRPC API binary path is relative, using libexec prefix: " << this->eebus_grpc_api_binary_path;
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

} // namespace module
