
#ifndef EVEREST_CORE_MODULES_EEBUS_INCLUDE_CONFIGVALIDATOR_HPP
#define EVEREST_CORE_MODULES_EEBUS_INCLUDE_CONFIGVALIDATOR_HPP

#include <filesystem>
#include <string>

namespace module {

class Conf;

class ConfigValidator {
public:
    ConfigValidator(const Conf& config, std::filesystem::path etc_prefix, std::filesystem::path libexec_prefix);

    bool validate();

    std::filesystem::path get_certificate_path() const;
    std::filesystem::path get_private_key_path() const;
    std::filesystem::path get_eebus_grpc_api_binary_path() const;
    int get_eebus_service_port() const;
    int get_grpc_port() const;
    std::string get_vendor_code() const;
    std::string get_device_brand() const;
    std::string get_device_model() const;
    std::string get_serial_number() const;
    int get_failsafe_control_limit() const;
    int get_max_nominal_power() const;
    std::string get_eebus_ems_ski() const;
    int get_restart_delay_s() const;
    int get_reconnect_delay_s() const;

private:
    bool validate_eebus_service_port() const;
    bool validate_grpc_port() const;
    bool validate_eebus_ems_ski() const;
    bool validate_certificate_path() const;
    bool validate_private_key_path() const;
    bool validate_eebus_grpc_api_binary_path() const;
    bool validate_manage_eebus_grpc_api_binary() const;
    bool validate_vendor_code() const;
    bool validate_device_brand() const;
    bool validate_device_model() const;
    bool validate_serial_number() const;
    bool validate_failsafe_control_limit() const;
    bool validate_max_nominal_power() const;

    const Conf& config;

    std::filesystem::path etc_prefix;
    std::filesystem::path libexec_prefix;

    std::filesystem::path certificate_path;
    std::filesystem::path private_key_path;
    std::filesystem::path eebus_grpc_api_binary_path;
    int eebus_service_port;
    int grpc_port;
    bool manage_eebus_grpc_api_binary;
};

} // namespace module

#endif // EVEREST_CORE_MODULES_EEBUS_INCLUDE_CONFIGVALIDATOR_HPP
