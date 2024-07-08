// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

///
/// @file init_device_model_db.hpp
/// @brief @copybrief ocpp::v201::InitDeviceModelDb///
/// @details @copydetails ocpp::v201::InitDeviceModelDb
///
/// @class ocpp::v201::InitDeviceModelDb
/// @brief Class to initialize the device model db using the schema's and config file
///
/// This class will read the device model schemas and config file and put them in the device model database.
///
/// If the database already exists and there are some changes on the Connector and / or EVSE components, this class
/// will make the changes in the database accordingly. For other components, no changes will be made.
///
/// It will also re-apply the config file. Config items will only be replaced if they are changed and the value in the
/// database is not set by an external source, like the CSMS.
///
/// The data from the schema json files or database are read into some structs. Some structs could be reused from
/// the DeviceModelStorage class, but some members are missing there and and to prevent too many database reads, some
/// structs are 'redefined' in this class with the proper members.
///
/// Since the DeviceModel class creates a map based on the device model database in the constructor, this class should
/// first be finished with the initialization before creating the DeviceModel class.
///
/// The config values are updated every startup as well, as long as the initial / default values are set in the
/// database. If the value is set by the user or csms or some other process, the value will not be overwritten.
///
/// Almost every function throws exceptions, because this class should be used only when initializing the chargepoint
/// and the database must be correct before starting the application.
///

#pragma once

#include <filesystem>

#include <ocpp/common/database/database_handler_common.hpp>
#include <ocpp/v201/device_model_storage.hpp>

namespace ocpp::v201 {
///
/// \brief Class that holds a component.
///
/// When the component is read from the database, the component id will be set.
/// When the component is read from the schema file, the 'required' vector will be filled.
///
struct ComponentKey {
    std::optional<uint64_t> db_id;       ///< \brief Component id in the database.
    std::string name;                    ///< \brief Component name.
    std::optional<std::string> instance; ///< \brief Component instance.
    std::optional<int32_t> evse_id;      ///< \brief Component evse id.
    std::optional<int32_t> connector_id; ///< \brief Component connector id.
    std::vector<std::string> required;   ///< \brief List of required variables.

    ///
    /// \brief operator <, needed to add this class as key in a map.
    /// \param l ComponentKey to compare with another ComponentKey
    /// \param r Second componentKey to compare with the other ComponentKey
    /// \return True if 'l' is 'smaller' than r (should be placed before 'r').
    ///
    friend bool operator<(const ComponentKey& l, const ComponentKey& r);
};

///
/// \brief Struct that holds a VariableAttribute struct and an database id.
///
struct DbVariableAttribute {
    std::optional<uint64_t> db_id;           ///< \brief The id in the database, if the record is read from the db.
    std::optional<std::string> value_source; ///< \brief Source of the attribute value (who set the value).
    VariableAttribute variable_attribute;    ///< \brief The variable attribute
};

///
/// \brief Struct holding the Variable of a device model
///
struct DeviceModelVariable {
    /// \brief The id in the database, if the record is read from the db.
    std::optional<uint64_t> db_id;
    /// \brief Id from the characteristics in the database, if the records is read from db.
    std::optional<uint64_t> variable_characteristics_db_id;
    /// \brief Variable name
    std::string name;
    /// \brief Variable characteristics
    VariableCharacteristics characteristics;
    /// \brief Variable attributes
    std::vector<DbVariableAttribute> attributes;
    /// \brief True if variable is required
    bool required;
    /// \brief Variable instance
    std::optional<std::string> instance;
    /// \brief Default value, if this is set in the schemas json
    std::optional<std::string> default_actual_value;
};

///
/// \brief Struct holding a config item value.
///
struct VariableAttributeKey {
    std::string name;                    ///< \brief Variable name
    std::optional<std::string> instance; ///< \brief Variable instance
    AttributeEnum attribute_type;        ///< \brief Attribute type
    std::string value;                   ///< \brief Attribute value
};

/// \brief Convert from json to a ComponentKey struct.
/// The to_json is not implemented as we don't need to write the schema to a json file.
void from_json(const json& j, ComponentKey& c);

/// \brief Convert from json to a DeviceModelVariable struct.
/// The to_json is not implemented for this struct as we don't need to write the schema to a json file.
void from_json(const json& j, DeviceModelVariable& c);

///
/// \brief Error class to be able to throw a custom error within the class.
///
class InitDeviceModelDbError : public std::exception {
public:
    [[nodiscard]] const char* what() const noexcept override {
        return this->reason.c_str();
    }
    explicit InitDeviceModelDbError(std::string msg) {
        this->reason = std::move(msg);
    }
    explicit InitDeviceModelDbError(const char* msg) {
        this->reason = std::string(msg);
    }

private:
    std::string reason;
};

class InitDeviceModelDb : public common::DatabaseHandlerCommon {
private: // Members
    /// \brief Database path of the device model database.
    const std::filesystem::path database_path;
    /// \brief True if the database exists on the filesystem.
    bool database_exists;

public:
    ///
    /// \brief Constructor.
    /// \param database_path        Path to the database.
    /// \param migration_files_path Path to the migration files.
    ///
    InitDeviceModelDb(const std::filesystem::path& database_path, const std::filesystem::path& migration_files_path);

    ///
    /// \brief Destructor
    ///
    virtual ~InitDeviceModelDb();

    ///
    /// \brief Initialize the database schema.
    /// \param schemas_path         Path to the database schemas.
    /// \param delete_db_if_exists  Set to true to delete the database if it already exists.
    ///
    /// \throws InitDeviceModelDbError  - When database could not be initialized or
    ///                                 - Foreign keys could not be turned on or
    ///                                 - Something could not be added to, retrieved or removed from the database
    /// \throws std::runtime_error      If something went wrong during migration
    /// \throws DatabaseMigrationException  If something went wrong during migration
    /// \throws DatabaseConnectionException If the database could not be opened
    /// \throws std::filesystem::filesystem_error   If the schemas path does not exist
    ///
    ///
    void initialize_database(const std::filesystem::path& schemas_path, const bool delete_db_if_exists);

    ///
    /// \brief Insert database configuration and default values.
    /// \param schemas_path Path to the database schemas.
    /// \param config_path  Path to the database config.
    /// \return True on success. False if at least one of the items could not be set.
    ///
    /// \throws InitDeviceModelDbError  When config and default values could not be set
    ///
    bool insert_config_and_default_values(const std::filesystem::path& schemas_path,
                                          const std::filesystem::path& config_path);

private: // Functions
    ///
    /// \brief Initialize the database.
    ///
    /// Create the database, run the migration script to create tables, open database connection.
    ///
    /// \param delete_db_if_exists  True if the database should be removed if it already exists (to start with a clean
    ///                             database).
    ///
    /// \throws InitDeviceModelDbError when the database could not be removed.
    ///
    void execute_init_sql(const bool delete_db_if_exists);

    ///
    /// \brief Get all paths to the component schemas (*.json) in the given directory.
    /// \param directory    Parent directory holding the standardized and component schema's.
    /// \return All path to the component schema's json files in the given directory.
    ///
    std::vector<std::filesystem::path> get_component_schemas_from_directory(const std::filesystem::path& directory);

    ///
    /// \brief Read all component schema's from the given directory and create a map holding the structure.
    /// \param directory    The parent directory containing the standardized and custom schema's.
    /// \return A map with the device model components, variables, characteristics and attributes.
    ///
    std::map<ComponentKey, std::vector<DeviceModelVariable>>
    get_all_component_schemas(const std::filesystem::path& directory);

    ///
    /// \brief Insert components, including variables, characteristics and attributes, to the database.
    /// \param components               The map with all components, variables, characteristics and attributes.
    /// \param existing_components      Vector with components that already exist in the database (Connector and EVSE).
    ///
    /// \throw InitDeviceModelDbError   When component could not be inserted
    ///
    void insert_components(const std::map<ComponentKey, std::vector<DeviceModelVariable>>& components,
                           const std::vector<ComponentKey>& existing_components);

    ///
    /// \brief Insert a single component with its variables, characteristics and attributes.
    /// \param component_key        The component.
    /// \param component_variables  The variables with its characteristics and attributes.
    ///
    void insert_component(const ComponentKey& component_key,
                          const std::vector<DeviceModelVariable>& component_variables);

    ///
    /// \brief Read component schemas from given files.
    /// \param components_schema_path   The paths to the component schema files.
    /// \return A map holding the components with its variables, characteristics and attributes.
    ///
    std::map<ComponentKey, std::vector<DeviceModelVariable>>
    read_component_schemas(const std::vector<std::filesystem::path>& components_schema_path);

    ///
    /// \brief Get all component properties (variables) from the given (component) json.
    /// \param component_properties The json component properties
    /// \param required_properties  The vector of required properties of this component.
    /// \return A vector with all Variables belonging to this component.
    ///
    std::vector<DeviceModelVariable> get_all_component_properties(const json& component_properties,
                                                                  std::vector<std::string> required_properties);

    ///
    /// \brief Insert variable characteristics
    /// \param characteristics  The characteristics.
    /// \param variable_id      The variable id in the database.
    ///
    /// \throws InitDeviceModelDbError if row could not be added to db.
    ///
    void insert_variable_characteristics(const VariableCharacteristics& characteristics, const int64_t& variable_id);

    ///
    /// \brief Update characteristics of a variable.
    /// \param characteristics      The characteristics to update.
    /// \param characteristics_id   The characteristics id in the database.
    /// \param variable_id          The variable id in the database.
    ///
    /// \throw InitDeviceModelDbError if variable characteristics could not be updated.
    ///
    void update_variable_characteristics(const VariableCharacteristics& characteristics,
                                         const int64_t& characteristics_id, const int64_t& variable_id);

    ///
    /// \brief Insert a variable in the database.
    /// \param variable         The variable to insert
    /// \param component_id     The component id the variable belongs to.
    ///
    /// \throws InitDeviceModelDbError if variable could not be inserted
    ///
    void insert_variable(const DeviceModelVariable& variable, const uint64_t& component_id);

    ///
    /// \brief Update a variable in the database.
    /// \param variable         The new variable to update.
    /// \param db_variable      The variable currently in the database (that needs updating).
    /// \param component_id     The component id the variable belongs to.
    ///
    /// \throws InitDeviceModelDbError If variable could not be updated
    ///
    void update_variable(const DeviceModelVariable& variable, const DeviceModelVariable& db_variable,
                         const uint64_t component_id);

    ///
    /// \brief Delete a variable from the database.
    /// \param variable The variable to delete.
    ///
    /// \throws InitDeviceModelDbError If variable could not be deleted
    ///
    void delete_variable(const DeviceModelVariable& variable);

    ///
    /// \brief Insert a variable attribute into the database.
    /// \param attribute    The attribute to insert.
    /// \param variable_id  The variable id the attribute belongs to.
    ///
    /// \throws InitDeviceModelDbError If attribute could not be inserted
    ///
    void insert_attribute(const VariableAttribute& attribute, const uint64_t& variable_id);

    ///
    /// \brief Insert variable attributes into the database.
    /// \param attributes   The attributes to insert.
    /// \param variable_id  The variable id the attributes belong to.
    ///
    /// \throws InitDeviceModelDbError If one of the attributes could not be inserted or updated
    ///
    void insert_attributes(const std::vector<DbVariableAttribute>& attributes, const uint64_t& variable_id);

    ///
    /// \brief Update variable attributes in the database.
    ///
    /// This will also remove attributes that are currently in the database, but not in the 'new_attributes' list, and
    /// add new attributes that are not in the database, but exist in the 'new_attributes' list.
    ///
    /// \param new_attributes   The attributes information with the new values.
    /// \param db_attributes    The attributes currently in the database that needs updating.
    /// \param variable_id      The variable id the attributes belong to.
    ///
    /// \throws InitDeviceModelDbError If one of the attributes could not be updated
    ///
    void update_attributes(const std::vector<DbVariableAttribute>& new_attributes,
                           const std::vector<DbVariableAttribute>& db_attributes, const uint64_t& variable_id);

    ///
    /// \brief Update a single attribute
    /// \param attribute        The attribute with the new values
    /// \param db_attribute     The attribute currently in the database, that needs updating.
    ///
    /// \throws InitDeviceModelDbError If the attribute could not be updated
    ///
    void update_attribute(const VariableAttribute& attribute, const DbVariableAttribute& db_attribute);

    ///
    /// \brief Delete an attribute from the database.
    /// \param attribute    The attribute to remove.
    ///
    /// \throws InitDeviceModelDbError If attribute could not be removed from the database
    ///
    void delete_attribute(const DbVariableAttribute& attribute);

    ///
    /// \brief Get default values of all variable attribute of a component.
    /// \param schemas_path The path of the schema holding the default values.
    /// \return A map with default variable attribute values per component.
    ///
    std::map<ComponentKey, std::vector<VariableAttributeKey>>
    get_component_default_values(const std::filesystem::path& schemas_path);

    ///
    /// \brief Get config values.
    /// \param config_file_path The path to the config file.
    /// \return A map with variable attribute values per component.
    ///
    std::map<ComponentKey, std::vector<VariableAttributeKey>>
    get_config_values(const std::filesystem::path& config_file_path);

    ///
    /// \brief Insert variable attribute value
    /// \param component_key                Component the variable attribute belongs to.
    /// \param variable_attribute_key       Variable attribute including value to insert.
    /// \param warn_source_not_default      Put a warning in the log if the variable could not be added because the
    ///                                     value source is not 'default'.
    /// \return true on success
    ///
    /// \throws InitDeviceModelDbError  When inserting failed.
    ///
    bool insert_variable_attribute_value(const ComponentKey& component_key,
                                         const VariableAttributeKey& variable_attribute_key,
                                         const bool warn_source_not_default);

    ///
    /// \brief Get all components from the db that are either EVSE or Connector.
    /// \return EVSE and Connector components.
    ///
    /// \throws InitDeviceModelDbError When getting components from db failed.
    ///
    std::vector<ComponentKey> get_all_connector_and_evse_components_from_db();

    ///
    /// \brief Get all components with its variables (and characteristics / attributes) from the database.
    /// \return A map of Components with it Variables.
    ///
    std::map<ComponentKey, std::vector<DeviceModelVariable>> get_all_components_from_db();

    ///
    /// \brief Check if a specific component exists in the databsae.
    /// \param db_components    The current components in the database.
    /// \param component        The component to check against.
    /// \return The component from the database if it exists.
    ///
    std::optional<ComponentKey> component_exists_in_db(const std::vector<ComponentKey>& db_components,
                                                       const ComponentKey& component);

    ///
    /// \brief Check if a component exist in the component schema.
    /// \param component_schema The map of component / variables read from the json component schema.
    /// \param component    The component to check.
    /// \return True when the component exists in the schema.
    ///
    bool component_exists_in_schemas(const std::map<ComponentKey, std::vector<DeviceModelVariable>>& component_schema,
                                     const ComponentKey& component);

    ///
    /// \brief Remove components from db that do not exist in the component schemas.
    /// \param component_schemas The component schemas.
    /// \param db_components     The components in the database.
    ///
    /// \throws InitDeviceModelDbError When one of the components could not be removed from the db.
    ///
    void remove_not_existing_components_from_db(
        const std::map<ComponentKey, std::vector<DeviceModelVariable>>& component_schemas,
        const std::vector<ComponentKey>& db_components);

    ///
    /// \brief Remove a component from the database.
    /// \param component    The component to remove.
    /// \return True on success.
    ///
    /// \throws InitDeviceModelDbError When component could not be removed from the db.
    ///
    bool remove_component_from_db(const ComponentKey& component);

    ///
    /// \brief Update variables of a component in the database.
    ///
    /// \param db_component         The component that currently exists in the database and needs updating.
    /// \param variables            The variables of the component.
    ///
    void update_component_variables(const ComponentKey& db_component,
                                    const std::vector<DeviceModelVariable>& variables);

    ///
    /// \brief Get variables belonging to a specific component from the database.
    /// \param db_component The component to get the variables from.
    /// \return The variables that belong to the given component.
    ///
    /// \throw InitDeviceModelDbError When variables could not be retrieved from the database.
    ///
    std::vector<DeviceModelVariable> get_variables_from_component_from_db(const ComponentKey& db_component);

    ///
    /// \brief Get variable attributes belonging to a specific variable from the database.
    /// \param variable_id  The id of the variable to get the attributes from.
    /// \return The attributes belonging to the given variable.
    ///
    /// \throw InitDeviceModelDbError   When variable attributes could not be retrieved from the database.
    ///
    std::vector<DbVariableAttribute> get_variable_attributes_from_db(const uint64_t& variable_id);

protected: // Functions
    // DatabaseHandlerCommon interface
    ///
    /// \brief Init database: set foreign keys on (so when a component is removed or updated, all variables,
    ///        characteristics and attributes belonging to that component are also removed or updated, for example).
    ///
    /// \throw InitDeviceModelDbError When foreign key pragma could not be set to 'ON'.
    ///
    virtual void init_sql() override;
};
} // namespace ocpp::v201
