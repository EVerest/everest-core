// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include "device_model_test_helper.hpp"

#include <ocpp/common/database/database_connection.hpp>
#include <ocpp/v201/device_model.hpp>
#include <ocpp/v201/device_model_storage_sqlite.hpp>

namespace ocpp::v201 {
DeviceModelTestHelper::DeviceModelTestHelper(const std::string& database_path, const std::string& migration_files_path,
                                             const std::string& config_path) :
    database_path(database_path),
    migration_files_path(migration_files_path),
    config_path(config_path),
    database_connection(std::make_unique<ocpp::common::DatabaseConnection>(database_path)) {
    this->database_connection->open_connection();
    this->device_model = create_device_model();
}

DeviceModel* DeviceModelTestHelper::get_device_model() {
    if (this->device_model == nullptr) {
        return nullptr;
    }

    return this->device_model.get();
}

bool DeviceModelTestHelper::remove_variable_from_db(const std::string& component_name,
                                                    const std::optional<std::string>& component_instance,
                                                    const std::optional<uint32_t>& evse_id,
                                                    const std::optional<uint32_t>& connector_id,
                                                    const std::string& variable_name,
                                                    const std::optional<std::string>& variable_instance) {
    const std::string delete_query = "DELETE FROM VARIABLE WHERE ID = "
                                     "(SELECT ID FROM VARIABLE WHERE COMPONENT_ID = "
                                     "(SELECT ID FROM COMPONENT WHERE NAME = ? AND INSTANCE IS ? AND "
                                     "EVSE_ID IS ? AND CONNECTOR_ID IS ?) "
                                     "AND NAME = ? AND INSTANCE IS ?)";

    auto delete_stmt = this->database_connection->new_statement(delete_query);
    delete_stmt->bind_text(1, component_name, common::SQLiteString::Transient);
    if (component_instance.has_value()) {
        delete_stmt->bind_text(2, component_instance.value(), common::SQLiteString::Transient);
    } else {
        delete_stmt->bind_null(2);
    }
    if (evse_id.has_value()) {
        delete_stmt->bind_int(3, evse_id.value());
        if (connector_id.has_value()) {
            delete_stmt->bind_int(4, connector_id.value());
        } else {
            delete_stmt->bind_null(4);
        }
    } else {
        delete_stmt->bind_null(3);
        delete_stmt->bind_null(4);
    }

    delete_stmt->bind_text(5, variable_name, common::SQLiteString::Transient);
    if (variable_instance.has_value()) {
        delete_stmt->bind_text(6, variable_instance.value(), common::SQLiteString::Transient);
    } else {
        delete_stmt->bind_null(6);
    }

    if (delete_stmt->step() == SQLITE_DONE) {
        EVLOG_error << this->database_connection->get_error_message();
        return false;
    }

    return true;
}

void DeviceModelTestHelper::create_device_model_db() {
    InitDeviceModelDb db(this->database_path, this->migration_files_path);
    db.initialize_database(this->config_path, true);
}

std::unique_ptr<DeviceModel> DeviceModelTestHelper::create_device_model() {
    create_device_model_db();
    auto device_model_storage = std::make_unique<DeviceModelStorageSqlite>(this->database_path);
    auto dm = std::make_unique<DeviceModel>(std::move(device_model_storage));

    return dm;
}
} // namespace ocpp::v201
