// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

#include <everest/logging.hpp>
#include <ocpp/v201/device_model_storage_sqlite.hpp>

namespace ocpp {

namespace v201 {

DeviceModelStorageSqlite::DeviceModelStorageSqlite(const std::filesystem::path& db_path) {
    if (sqlite3_open(db_path.c_str(), &this->db) != SQLITE_OK) {
        EVLOG_error << "Could not open database at provided path: " << db_path;
        EVLOG_AND_THROW(std::runtime_error("Could not open device model database at provided path."));
    } else {
        EVLOG_info << "Established connection to device model database successfully: " << db_path;
    }
}

int DeviceModelStorageSqlite::get_component_id(const Component& component_id) {
    std::string select_query = "SELECT ID FROM COMPONENT WHERE NAME = ? AND INSTANCE IS ?";
    SQLiteStatement select_stmt(this->db, select_query);

    select_stmt.bind_text(component_id.name.get(), 1);
    if (component_id.instance.has_value()) {
        select_stmt.bind_text(component_id.instance.value().get(), 2);
    } else {
        select_stmt.bind_null(2);
    }

    if (select_stmt.step() == SQLITE_ROW) {
        return select_stmt.column_int(0);
    } else {
        return -1;
    }
}

int DeviceModelStorageSqlite::get_variable_id(const Component& component_id, const Variable& variable_id) {
    const auto _component_id = this->get_component_id(component_id);
    if (_component_id == -1) {
        return -1;
    }

    std::string select_query = "SELECT ID FROM VARIABLE WHERE COMPONENT_ID = ? AND NAME = ? AND INSTANCE IS ?";
    SQLiteStatement select_stmt(this->db, select_query);

    select_stmt.bind_int(_component_id, 1);
    select_stmt.bind_text(variable_id.name.get(), 2);
    if (variable_id.instance.has_value()) {
        select_stmt.bind_text(variable_id.instance.value().get(), 3);
    } else {
        select_stmt.bind_null(3);
    }
    if (select_stmt.step() == SQLITE_ROW) {
        return select_stmt.column_int(0);
    } else {
        return -1;
    }
}

DeviceModelMap DeviceModelStorageSqlite::get_device_model() {
    std::map<Component, std::map<Variable, VariableMetaData>> device_model;

    std::string select_query = "SELECT c.NAME, c.EVSE_ID, c.INSTANCE, v.NAME, v.INSTANCE, vc.DATATYPE_ID, "
                               "vc.SUPPORTS_MONITORING, vc.UNIT, vc.MIN_LIMIT, vc.MAX_LIMIT, vc.VALUES_LIST "
                               "FROM COMPONENT c "
                               "JOIN VARIABLE v ON c.ID = v.COMPONENT_ID "
                               "JOIN VARIABLE_CHARACTERISTICS vc ON v.VARIABLE_CHARACTERISTICS_ID = vc.ID";

    SQLiteStatement select_stmt(this->db, select_query);

    while (select_stmt.step() == SQLITE_ROW) {
        Component component;
        component.name = select_stmt.column_text(0);

        if (select_stmt.column_type(1) != SQLITE_NULL) {
            auto evse_id = select_stmt.column_int(1);
            EVSE evse;
            evse.id = evse_id;
            component.evse = evse;
        }

        if (select_stmt.column_type(2) != SQLITE_NULL) {
            component.instance = select_stmt.column_text(2);
        }

        Variable variable;
        variable.name = select_stmt.column_text(3);

        if (select_stmt.column_type(4) != SQLITE_NULL) {
            variable.instance = select_stmt.column_text(4);
        }

        VariableCharacteristics characteristics;
        characteristics.dataType = static_cast<DataEnum>(select_stmt.column_int(5));
        characteristics.supportsMonitoring = select_stmt.column_int(6) != 0;

        if (select_stmt.column_type(7) != SQLITE_NULL) {
            characteristics.unit = select_stmt.column_text(7);
        }

        if (select_stmt.column_type(8) != SQLITE_NULL) {
            characteristics.minLimit = select_stmt.column_double(8);
        }

        if (select_stmt.column_type(9) != SQLITE_NULL) {
            characteristics.maxLimit = select_stmt.column_double(9);
        }

        if (select_stmt.column_type(10) != SQLITE_NULL) {
            characteristics.valuesList = select_stmt.column_text(10);
        }

        VariableMetaData meta_data;
        meta_data.characteristics = characteristics;

        device_model[component][variable] = meta_data;
    }

    EVLOG_info << "Successfully retrieved Device Model from DeviceModelStorage";
    return device_model;
}

std::optional<VariableAttribute> DeviceModelStorageSqlite::get_variable_attribute(const Component& component_id,
                                                                                  const Variable& variable_id,
                                                                                  const AttributeEnum& attribute_enum) {
    const auto attributes = this->get_variable_attributes(component_id, variable_id, attribute_enum);
    if (!attributes.empty()) {
        return attributes.at(0);
    } else {
        return std::nullopt;
    }
}

std::vector<VariableAttribute>
DeviceModelStorageSqlite::get_variable_attributes(const Component& component_id, const Variable& variable_id,
                                                  const std::optional<AttributeEnum>& attribute_enum) {
    std::vector<VariableAttribute> attributes;
    const auto _variable_id = this->get_variable_id(component_id, variable_id);
    if (_variable_id == -1) {
        return attributes;
    }

    std::string select_query = "SELECT va.VALUE, va.MUTABILITY_ID, va.PERSISTENT, va.CONSTANT, va.TYPE_ID "
                               "FROM VARIABLE_ATTRIBUTE va "
                               "WHERE va.VARIABLE_ID = @variable_id";

    if (attribute_enum.has_value()) {
        std::stringstream ss;
        ss << select_query << "  AND va.TYPE_ID = " << static_cast<int>(attribute_enum.value());
        select_query = ss.str();
    }

    SQLiteStatement select_stmt(this->db, select_query);

    select_stmt.bind_int(_variable_id, 1);

    while (select_stmt.step() == SQLITE_ROW) {
        VariableAttribute attribute;

        if (select_stmt.column_type(0) != SQLITE_NULL) {
            attribute.value = select_stmt.column_text(0);
        }
        attribute.mutability = static_cast<MutabilityEnum>(select_stmt.column_int(1));
        attribute.persistent = static_cast<bool>(select_stmt.column_int(2));
        attribute.constant = static_cast<bool>(select_stmt.column_int(3));
        attribute.type = static_cast<AttributeEnum>(select_stmt.column_int(4));
        attributes.push_back(attribute);
    }

    return attributes;
}

bool DeviceModelStorageSqlite::set_variable_attribute_value(const Component& component_id, const Variable& variable_id,
                                                            const AttributeEnum& attribute_enum,
                                                            const std::string& value) {
    std::string insert_query = "UPDATE VARIABLE_ATTRIBUTE SET VALUE = ? WHERE VARIABLE_ID = ? AND TYPE_ID = ?";
    SQLiteStatement insert_stmt(this->db, insert_query);

    const auto _variable_id = this->get_variable_id(component_id, variable_id);

    if (_variable_id == -1) {
        return false;
    }

    insert_stmt.bind_text(value, 1);
    insert_stmt.bind_int(_variable_id, 2);
    insert_stmt.bind_int(static_cast<int>(attribute_enum), 3);
    if (insert_stmt.step() != SQLITE_DONE) {
        EVLOG_error << sqlite3_errmsg(this->db);
        return false;
    }
    return true;
}

} // namespace v201
} // namespace ocpp
