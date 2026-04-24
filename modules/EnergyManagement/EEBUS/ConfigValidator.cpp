#include <ConfigValidator.hpp>

#include <algorithm>
#include <cctype>
#include <sstream>
#include <vector>

#include "EEBUS.hpp"

namespace {
std::string normalize_ski(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) { return std::tolower(c); });
    s.erase(std::remove(s.begin(), s.end(), ' '), s.end());
    return s;
}
bool is_valid_ski_format(const std::string& s) {
    if (s.size() != 40) {
        return false;
    }
    return std::all_of(s.begin(), s.end(), [](unsigned char c) { return std::isxdigit(c); });
}

// Mirrors ocpp::split_string from lib/everest/ocpp/lib/ocpp/common/utils.cpp.
// Kept local to avoid a runtime dep from EEBUS onto libocpp.
std::string trim_string(const std::string& s) {
    const auto first = s.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) {
        return {};
    }
    const auto last = s.find_last_not_of(" \t\r\n");
    return s.substr(first, last - first + 1);
}
std::vector<std::string> split_string(const std::string& string_to_split, char sep, bool trim) {
    std::stringstream input(string_to_split);
    std::string token;
    std::vector<std::string> result;
    while (std::getline(input, token, sep)) {
        if (trim) {
            token = trim_string(token);
        }
        if (!token.empty()) {
            result.push_back(token);
        }
    }
    return result;
}
} // namespace

namespace module {

ConfigValidator::ConfigValidator(const Conf& config, std::filesystem::path etc_prefix,
                                 std::filesystem::path libexec_prefix) :
    config(config),
    eebus_service_port(config.eebus_service_port),
    grpc_port(config.grpc_port),
    manage_eebus_grpc_api_binary(config.manage_eebus_grpc_api_binary),
    etc_prefix(std::move(etc_prefix)),
    libexec_prefix(std::move(libexec_prefix)),
    certificate_path(std::filesystem::path(config.certificate_path)),
    private_key_path(std::filesystem::path(config.private_key_path)),
    eebus_grpc_api_binary_path(std::filesystem::path(config.eebus_grpc_api_binary_path)) {

    if (this->manage_eebus_grpc_api_binary) {
        if (this->certificate_path.is_relative()) {
            this->certificate_path = this->etc_prefix / "certs" / this->certificate_path;
            EVLOG_info << "Certificate path is relative, using etc prefix: " << this->certificate_path;
        }
        if (this->private_key_path.is_relative()) {
            this->private_key_path = this->etc_prefix / "certs" / this->private_key_path;
            EVLOG_info << "Key path is relative, using etc prefix: " << this->private_key_path;
        }
        if (this->eebus_grpc_api_binary_path.is_relative()) {
            this->eebus_grpc_api_binary_path = this->libexec_prefix / this->eebus_grpc_api_binary_path;
            EVLOG_info << "EEBUS GRPC API binary path is relative, using libexec prefix: "
                       << this->eebus_grpc_api_binary_path;
        }
    }

    // Build the effective allowlist as a deduplicated set of normalized SKIs.
    for (const auto& ski : split_string(this->config.eebus_ems_ski_allowlist, ',', /*trim=*/true)) {
        this->effective_allowlist.insert(normalize_ski(ski));
    }
}

bool ConfigValidator::validate() {
    bool valid = true;
    valid &= this->validate_eebus_service_port();
    valid &= this->validate_grpc_port();
    valid &= this->validate_eebus_ems_ski_allowlist();
    valid &= this->validate_accept_unknown_ems();
    valid &= this->validate_certificate_path();
    valid &= this->validate_private_key_path();
    valid &= this->validate_eebus_grpc_api_binary_path();
    valid &= this->validate_manage_eebus_grpc_api_binary();
    valid &= this->validate_vendor_code();
    valid &= this->validate_device_brand();
    valid &= this->validate_device_model();
    valid &= this->validate_serial_number();
    valid &= this->validate_failsafe_control_limit();
    valid &= this->validate_max_nominal_power();
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

int ConfigValidator::get_eebus_service_port() const {
    return this->eebus_service_port;
}

int ConfigValidator::get_grpc_port() const {
    return this->grpc_port;
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

int ConfigValidator::get_failsafe_control_limit() const {
    return this->config.failsafe_control_limit_W;
}

int ConfigValidator::get_max_nominal_power() const {
    return this->config.max_nominal_power_W;
}

int ConfigValidator::get_restart_delay_s() const {
    return this->config.restart_delay_s;
}

int ConfigValidator::get_reconnect_delay_s() const {
    return this->config.reconnect_delay_s;
}

bool ConfigValidator::validate_eebus_service_port() const {
    if (this->config.eebus_service_port < 0) {
        EVLOG_error << "eebus service port is negative";
        return false;
    }
    return true;
}

bool ConfigValidator::validate_grpc_port() const {
    if (this->config.grpc_port < 0) {
        EVLOG_error << "grpc port is negative";
        return false;
    }
    return true;
}

bool ConfigValidator::validate_certificate_path() const {
    if (!this->manage_eebus_grpc_api_binary) {
        return true;
    }
    if (std::filesystem::exists(this->certificate_path)) {
        return true;
    }
    // Sidecar will generate the cert; ensure parent directory exists.
    auto parent = this->certificate_path.parent_path();
    std::error_code ec;
    std::filesystem::create_directories(parent, ec);
    if (ec) {
        EVLOG_error << "Failed to create certificate directory " << parent << ": " << ec.message();
        return false;
    }
    EVLOG_info << "Certificate not found at " << this->certificate_path
               << "; eebus_grpc_api will generate it on first run.";
    return true;
}

bool ConfigValidator::validate_private_key_path() const {
    if (!this->manage_eebus_grpc_api_binary) {
        return true;
    }
    if (std::filesystem::exists(this->private_key_path)) {
        return true;
    }
    // Sidecar will generate the key; ensure parent directory exists.
    auto parent = this->private_key_path.parent_path();
    std::error_code ec;
    std::filesystem::create_directories(parent, ec);
    if (ec) {
        EVLOG_error << "Failed to create private key directory " << parent << ": " << ec.message();
        return false;
    }
    EVLOG_info << "Private key not found at " << this->private_key_path
               << "; eebus_grpc_api will generate it on first run.";
    return true;
}

bool ConfigValidator::validate_eebus_grpc_api_binary_path() const {
    if (!this->manage_eebus_grpc_api_binary) {
        return true;
    }
    if (!std::filesystem::exists(this->eebus_grpc_api_binary_path)) {
        EVLOG_error << "EEBUS GRPC API binary does not exist: " << this->eebus_grpc_api_binary_path;
        return false;
    }
    const auto perms = std::filesystem::status(this->eebus_grpc_api_binary_path).permissions();
    const auto owner_exec_perms = std::filesystem::perms::owner_exec;
    if ((perms & owner_exec_perms) == std::filesystem::perms::none) {
        EVLOG_error << "EEBUS GRPC API binary is not executable: " << this->eebus_grpc_api_binary_path;
        return false;
    }
    return true;
}

bool ConfigValidator::validate_manage_eebus_grpc_api_binary() const {
    return true;
}

bool ConfigValidator::validate_vendor_code() const {
    if (this->config.vendor_code.empty()) {
        EVLOG_error << "vendor_code is empty";
        return false;
    }
    return true;
}

bool ConfigValidator::validate_device_brand() const {
    if (this->config.device_brand.empty()) {
        EVLOG_error << "device_brand is empty";
        return false;
    }
    return true;
}

bool ConfigValidator::validate_device_model() const {
    if (this->config.device_model.empty()) {
        EVLOG_error << "device_model is empty";
        return false;
    }
    return true;
}

bool ConfigValidator::validate_serial_number() const {
    if (this->config.serial_number.empty()) {
        EVLOG_error << "serial_number is empty";
        return false;
    }
    return true;
}

bool ConfigValidator::validate_failsafe_control_limit() const {
    if (this->config.failsafe_control_limit_W < 0) {
        EVLOG_error << "failsafe_control_limit is negative";
        return false;
    }
    return true;
}

bool ConfigValidator::validate_max_nominal_power() const {
    if (this->config.max_nominal_power_W < 0) {
        EVLOG_error << "max_nominal_power is negative";
        return false;
    }
    return true;
}

bool ConfigValidator::validate_eebus_ems_ski_allowlist() const {
    for (const auto& ski : split_string(this->config.eebus_ems_ski_allowlist, ',', /*trim=*/true)) {
        const auto normalized = normalize_ski(ski);
        if (!is_valid_ski_format(normalized)) {
            EVLOG_error << "eebus_ems_ski_allowlist entry is not 40 hex chars: " << ski;
            return false;
        }
        EVLOG_info << "EEBUS: trusting allowlisted EMS SKI=" << normalized;
    }
    if (this->effective_allowlist.empty() && !this->config.accept_unknown_ems) {
        EVLOG_warning << "EEBUS: no EMS SKI configured and accept_unknown_ems=false; "
                      << "no EG will ever be trusted.";
    }
    return true;
}

bool ConfigValidator::validate_accept_unknown_ems() const {
    if (this->config.accept_unknown_ems) {
        EVLOG_info << "EEBUS: accept_unknown_ems=true — any discovered EG on the LAN "
                   << "will be auto-trusted. Only enable on isolated networks.";
    }
    return true;
}

const std::set<std::string>& ConfigValidator::get_effective_ems_ski_allowlist() const {
    return this->effective_allowlist;
}

bool ConfigValidator::get_accept_unknown_ems() const {
    return this->config.accept_unknown_ems;
}

} // namespace module
