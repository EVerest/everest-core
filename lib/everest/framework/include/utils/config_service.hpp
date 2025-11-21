// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#pragma once

#include <utils/config.hpp>
#include <utils/mqtt_abstraction.hpp>
namespace Everest {
namespace config {

constexpr auto MODULE_IMPLEMENTATION_ID = "!module";

/// \brief The type of request or response
enum class Type {
    Get,    ///< Identifies a get request or response
    Set,    ///< Identifies a set request or response
    Unknown ///< Used for unknown requests that could not be parsed
};

/// \brief Possible get request and response sub-types
enum class GetType {
    All,         ///< All module configurations that the requesting module has access to
    Module,      ///< The module configuration for the requesting module
    Value,       ///< A specific configuration value identified by a ConfigurationParameterIdentifier
    AllMappings, ///< All module mappings that the requesting module has access to
    Unknown      ///< Used for unknown requests that could not be parsed

    // TODO: Potential additions in the future:
    // Delta, // This would need tracking of when the last Request was made
};

/// \brief Represents a get request
struct GetRequest {
    GetType type = GetType::Unknown;                                             ///< The type of get request
    std::optional<everest::config::ConfigurationParameterIdentifier> identifier; ///< Used for GetType::Value

    // TODO: Potential additions in the future:
    // optional timestamp for GetType::Delta?
    // a list of requested modules?
};

/// \brief Represents a response to a get request
struct GetResponse {
    GetType type = GetType::Unknown; ///< The type of get response, the same as in the get request
    nlohmann::json data;             ///< Data associated with this reponse.
    // FIXME: use proper type(s) for data?
};

/// \brief Represents a set request
struct SetRequest {
    everest::config::ConfigurationParameterIdentifier
        identifier;    ///< An identifier for the configuration parameter to be set
    std::string value; ///< The string representation of the configuration value to be set
    // TODO: should value be a ConfigEntry variant type?
};

/// \brief Possible response status values
enum class ResponseStatus {
    Ok,          ///< Everything worked
    Error,       ///< There was an error during handling of the request
    AccessDenied ///< There was an access error during handling of the request
};

/// \brief Possible set response status values
enum class SetResponseStatus {
    Accepted,      ///< Configuration value was set successfully
    Rejected,      ///< Configuration value could not be set
    RebootRequired ///< Configuration value was set successfully but a reboot is required for modules to actually use
                   ///< this value
};

/// \brief Represents a response to a set request
struct SetResponse {
    SetResponseStatus status = SetResponseStatus::Rejected; ///< Status of the set response
    std::string status_info;                                ///< Can contain additional status information
};

/// \brief Represents a container for various requests that can be made to the ConfigService
struct Request {
    Type type = Type::Unknown;                                    ///< The type of request
    std::variant<std::monostate, GetRequest, SetRequest> request; ///< The request itself
    std::string origin; ///< The origin of the request, the module id of the requesting module
};

/// \brief Represents a container for various responses to requests made to the ConfigService
struct Response {
    ResponseStatus status = ResponseStatus::Error; ///< Status of the response
    std::string status_info;                       ///< Can contain additional status information
    std::optional<Type> type; ///< The type of the response, identical to the request, missing when status is Error
    std::variant<std::monostate, GetResponse, SetResponse> response; ///< The response itself
};

/// \brief Represents a container for getting a configuration parameter
struct GetConfigResult {
    ResponseStatus status = ResponseStatus::Error;                   ///< Status of the result
    std::string status_info;                                         ///< Can contain additional status information
    everest::config::ConfigurationParameter configuration_parameter; ///< The requested configuration parameter
};

/// \brief Represents a container for the result of setting a configuration parameter
struct SetConfigResult {
    ResponseStatus status = ResponseStatus::Error; ///< Status of the result
    std::string status_info;                       ///< Can contain additional status information
    everest::config::SetConfigStatus set_status =
        everest::config::SetConfigStatus::Rejected; ///< Specific status for the resut of setting the config parameter
};

/// \brief Represents a compound type to identify a specific module instance and its type
struct ModuleIdType {
    std::string module_id;   ///< The module id
    std::string module_type; ///< The associated module type

    bool operator<(const ModuleIdType& rhs) const;
};

class ConfigServiceClient {
public:
    /// \brief ConfigService client using the provided \p mqtt_abstraction for the module identified by \p module_id
    /// \p module_names is a mapping of all module ids to module names/types for usage in get_module_configs()
    ConfigServiceClient(std::shared_ptr<MQTTAbstraction> mqtt_abstraction, const std::string& module_id,
                        const std::unordered_map<std::string, std::string>& module_names);

    /// \brief Compiles and \returns all module configs that this module has access to
    std::map<ModuleIdType, everest::config::ModuleConfigurationParameters> get_module_configs();

    /// \brief Compiles and \returns all mappings of modules that this module has access to
    std::map<std::string, ModuleTierMappings> get_mappings();

    /// \brief Sets the config \p value associated with the \p identifier
    /// \returns a result containing status and potential error information
    SetConfigResult set_config_value(const everest::config::ConfigurationParameterIdentifier& identifier,
                                     const std::string& value);

    /// \brief Gets the config value associated with the \p identifier
    /// \returns a result containing the configuration item or an error
    GetConfigResult get_config_value(const everest::config::ConfigurationParameterIdentifier& identifier);

private:
    std::shared_ptr<MQTTAbstraction> mqtt_abstraction;
    std::string origin;
    std::unordered_map<std::string, std::string> module_names;
};

class ConfigService {
public:
    /// \brief ConfigService using the provided \p mqtt_abstraction to distribute relevant parts of the given \p config
    /// when another module requests them and has appropriate access rights to them
    ConfigService(MQTTAbstraction& mqtt_abstraction, std::shared_ptr<ManagerConfig> config);

private:
    MQTTAbstraction& mqtt_abstraction;
    std::shared_ptr<TypedHandler> get_config_token;
    std::shared_ptr<ManagerConfig> config;
};

namespace conversions {
std::string type_to_string(Type type);

Type string_to_type(const std::string& type);

std::string get_type_to_string(GetType type);

GetType string_to_get_type(const std::string& type);

std::string response_status_to_string(ResponseStatus status);

ResponseStatus string_to_response_status(const std::string& status);

std::string set_response_status_to_string(SetResponseStatus status);

SetResponseStatus string_to_set_response_status(const std::string& status);

everest::config::SetConfigStatus set_response_status_to_set_config_status(SetResponseStatus status);

SetResponseStatus set_config_status_to_set_response_status(everest::config::SetConfigStatus status);
} // namespace conversions

std::ostream& operator<<(std::ostream& os, const GetType& t);

void to_json(nlohmann::json& j, const GetRequest& r);

void from_json(const nlohmann::json& j, GetRequest& r);

void to_json(nlohmann::json& j, const SetRequest& r);

void from_json(const nlohmann::json& j, SetRequest& r);

void to_json(nlohmann::json& j, const GetResponse& r);

void from_json(const nlohmann::json& j, GetResponse& r);

void to_json(nlohmann::json& j, const SetResponse& r);

void from_json(const nlohmann::json& j, SetResponse& r);

void to_json(nlohmann::json& j, const Request& r);

void from_json(const nlohmann::json& j, Request& r);

void to_json(nlohmann::json& j, const Response& r);

void from_json(const nlohmann::json& j, Response& r);
} // namespace config
} // namespace Everest

NLOHMANN_JSON_NAMESPACE_BEGIN

template <> struct adl_serializer<everest::config::ConfigurationParameterIdentifier> {
    static void to_json(nlohmann::json& j, const everest::config::ConfigurationParameterIdentifier& c);
    static void from_json(const nlohmann::json& j, everest::config::ConfigurationParameterIdentifier& c);
};

NLOHMANN_JSON_NAMESPACE_END
