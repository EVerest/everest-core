// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

#ifndef DEVICE_MODEL_STORAGE_SQLITE_HPP
#define DEVICE_MODEL_STORAGE_SQLITE_HPP

#include <filesystem>
#include <sqlite3.h>

#include <ocpp/v201/device_model_storage.hpp>

namespace ocpp {
namespace v201 {

class DeviceModelStorageSqlite : public DeviceModelStorage {

private:
    sqlite3* db;

    int get_component_id(const Component& component_id);

    int get_variable_id(const Component& component_id, const Variable& variable_id);

public:
    /// \brief Opens SQLite connection at given \p db_path
    /// \param db_path  path to database
    explicit DeviceModelStorageSqlite(const std::filesystem::path& db_path);

    std::map<Component, std::map<Variable, VariableMetaData>> get_device_model() final;

    std::optional<VariableAttribute> get_variable_attribute(const Component& component_id, const Variable& variable_id,
                                                            const AttributeEnum& attribute_enum) final;

    std::vector<VariableAttribute> get_variable_attributes(const Component& component_id, const Variable& variable_id,
                                                           const std::optional<AttributeEnum>& attribute_enum) final;

    bool set_variable_attribute_value(const Component& component_id, const Variable& variable_id,
                                      const AttributeEnum& attribute_enum, const std::string& value) final;
};

} // namespace v201
} // namespace ocpp

#endif // DEVICE_MODEL_STORAGE_SQLITE_HPP
