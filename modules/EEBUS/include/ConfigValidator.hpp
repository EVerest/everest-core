
#ifndef EVEREST_CORE_MODULES_EEBUS_INCLUDE_CONFIGVALIDATOR_HPP
#define EVEREST_CORE_MODULES_EEBUS_INCLUDE_CONFIGVALIDATOR_HPP

#include <filesystem>
#include <string>

namespace module {

class Conf;

class ConfigValidator {
    public:
        ConfigValidator(
            const Conf& config
        );

        bool validate() const;

        std::filesystem::path get_certificate_path() const;
        std::filesystem::path get_private_key_path() const;
        std::filesystem::path get_eebus_grpc_api_binary_path() const;
        std::string get_control_service_rpc_endpoint() const;
    private:
        bool validate_control_service_rpc_port() const;
        bool validate_eebus_ems_ski() const;
        bool validate_certificate_path() const;
        bool validate_private_key_path() const;
        bool validate_eebus_grpc_api_binary_path() const;
        bool validate_manage_eebus_grpc_api_binary() const;

        const Conf& config;

        std::filesystem::path certificate_path;
        std::filesystem::path private_key_path;
        std::filesystem::path eebus_grpc_api_binary_path;
        std::string control_service_rpc_endpoint;
        bool manage_eebus_grpc_api_binary;
};

} // namespace module


#endif // EVEREST_CORE_MODULES_EEBUS_INCLUDE_CONFIGVALIDATOR_HPP
