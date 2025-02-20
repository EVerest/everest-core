// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

/**
 * @file device_model_test_helper.hpp
 * @brief @copybrief ocpp::v2::DeviceModelTestHelper
 *
 * @class ocpp::v2::DeviceModelTestHelper
 * @brief Helper for tests where the device model is needed.
 *
 * If the device model is stored in memory, a database connection must be kept open at all times to prevent the
 * device model to be thrown away.
 */

#pragma once

#include <memory>

#include <ocpp/v2/init_device_model_db.hpp>

const static std::string MIGRATION_FILES_PATH = "./resources/v2/device_model_migration_files";
const static std::string CONFIG_PATH = "./resources/example_config/v2/component_config";
const static std::string DEVICE_MODEL_DB_IN_MEMORY_PATH = "file::memory:?cache=shared";

namespace ocpp {
namespace common {
class DatabaseConnection;
}

namespace v2 {
class DeviceModel;

class DeviceModelTestHelper {
public:
    explicit DeviceModelTestHelper(const std::string& database_path = DEVICE_MODEL_DB_IN_MEMORY_PATH,
                                   const std::string& migration_files_path = MIGRATION_FILES_PATH,
                                   const std::string& config_path = CONFIG_PATH);
    DeviceModel* get_device_model();

    ///
    /// \brief Remove a variable from the database.
    /// \param component_name       The component name.
    /// \param component_instance   Component instance (optional).
    /// \param evse_id              Evse id (optional).
    /// \param connector_id         Connector id (optional).
    /// \param variable_name        Variable name to remove.
    /// \param variable_instance    Variable instance (optional).
    /// \return True on success.
    ///
    /// \note After using this function, request a new device model with DeviceModelTestHelper::get_device_model(),
    ///       because the device model has been changed in the database and the device model map has been read again.
    ///
    bool remove_variable_from_db(const std::string& component_name,
                                 const std::optional<std::string>& component_instance,
                                 const std::optional<uint32_t>& evse_id, const std::optional<uint32_t>& connector_id,
                                 const std::string& variable_name, const std::optional<std::string>& variable_instance);

    ///
    /// \brief Update characteristics of a variable.
    /// \param characteristics      The updated characteristics.
    /// \param component_name       Component name.
    /// \param component_instance   Component instance.
    /// \param evse_id              The evse id.
    /// \param connector_id         The connector id.
    /// \param variable_name        The variable name.
    /// \param variable_instance    The variable instance.
    /// \return     True on success.
    ///
    /// \note After using this function, request a new device model with DeviceModelTestHelper::get_device_model(),
    ///       because the device model has been changed in the database and the device model map has been read again.
    /// \note We assume with this function that there each variable has only one characteristics entry.
    ///
    bool update_variable_characteristics(const VariableCharacteristics& characteristics,
                                         const std::string& component_name,
                                         const std::optional<std::string>& component_instance,
                                         const std::optional<uint32_t>& evse_id,
                                         const std::optional<uint32_t>& connector_id, const std::string& variable_name,
                                         const std::optional<std::string>& variable_instance);

    ///
    /// \brief Set variable attribute to 'NULL'
    /// \param component_name       Component name.
    /// \param component_instance   Component instance.
    /// \param evse_id              The evse id.
    /// \param connector_id         The connector id.
    /// \param variable_name        The variable name.
    /// \param variable_instance    The variable instance.
    /// \param attribute_enum       The variable attribute.
    /// \return True on success.
    ///
    bool set_variable_attribute_value_null(const std::string& component_name,
                                           const std::optional<std::string>& component_instance,
                                           const std::optional<uint32_t>& evse_id,
                                           const std::optional<uint32_t>& connector_id,
                                           const std::string& variable_name,
                                           const std::optional<std::string>& variable_instance,
                                           const AttributeEnum& attribute_enum);

private:
    const std::string& database_path;
    const std::string& migration_files_path;
    const std::string& config_path;

    // DatabaseConnection as member so the database keeps open and is not destroyed (because this is an in memory
    // database).
    std::unique_ptr<ocpp::common::DatabaseConnection> database_connection;
    // Device model is a unique ptr here because of the database: it is stored in memory so as soon as the handle to
    // the database closes, the database is removed. So the handle should be opened before creating the devide model.
    // So the device model is initialized on nullptr, then the handle is opened, the devide model is created and the
    // handle stays open until the whole test is destructed.
    std::unique_ptr<DeviceModel> device_model;

    ///
    /// \brief Create the database for the device model and apply migrations.
    /// \param path Database path.
    ///
    void create_device_model_db();

    ///
    /// \brief Create device model.
    /// \return The created device model.
    ///
    std::unique_ptr<DeviceModel> create_device_model(const bool init = true);
};
} // namespace v2
} // namespace ocpp
