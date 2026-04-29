// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#include "utils/config/storage.hpp"
#include "utils/config/types.hpp"
#include <everest/compile_time_settings.hpp>
#include <everest/database/exceptions.hpp>
#include <everest/database/sqlite/schema_updater.hpp>
#include <everest/logging.hpp>

#include <utils/config/settings.hpp>
#include <utils/config/storage_sqlite.hpp>
#include <utils/date.hpp>

using json = nlohmann::json;
using namespace everest::db;
using namespace everest::db::sqlite;

namespace everest::config {
namespace {
const std::string& default_module_implementation_id() {
    static const std::string DEFAULT_MODULE_IMPLEMENTATION_ID = "!module";
    return DEFAULT_MODULE_IMPLEMENTATION_ID;
}
} // namespace

/// \brief Helper for accessing the column indices of the CONFIGURATION table
enum class ConfigurationColumnIndex {
    COL_CONFIG_ID = 1,
    COL_MODULE_ID,
    COL_PARAMETER_NAME,
    COL_VALUE,
    COL_MUTABILITY_ID,
    COL_DATATYPE_ID,
    COL_UNIT,
    COL_MODULE_IMPLEMENTATION_ID,
};

/// \brief Helper for accessing the column indices of the CONFIGURATION table of a specific MODULE_ID
enum class ConfigurationColumnModuleIdIndex {
    COL_PARAMETER_NAME = 0,
    COL_VALUE,
    COL_MODULE_IMPLEMENTATION_ID,
    COL_MUTABILITY_ID,
    COL_DATATYPE_ID,
    COL_UNIT,
};

namespace {
int to_int(ConfigurationColumnIndex configuration_column_index) {
    return static_cast<int>(configuration_column_index);
}

int to_int(ConfigurationColumnModuleIdIndex configuration_column_module_id_index) {
    return static_cast<int>(configuration_column_module_id_index);
}
} // namespace

std::shared_ptr<ConnectionInterface> open_config_database(const std::filesystem::path& db_path,
                                                          const std::filesystem::path& migrations_dir) {
    auto db = std::make_shared<Connection>(db_path);
    SchemaUpdater updater{db.get()};
    if (!updater.apply_migration_files(migrations_dir, TARGET_MIGRATION_FILE_VERSION)) {
        if (db_path.parent_path().empty()) {
            EVLOG_error << "Could not apply migrations for database at provided path: \"" << db_path.string()
                        << "\" likely because the database path is just a filename. You MUST provide a full path to "
                           "the database.";
        }
        throw MigrationException("SQL migration failed");
    }
    return db;
}

SqliteStorage::SqliteStorage(const fs::path& db_path, const std::filesystem::path& migration_files_path,
                             int config_id) :
    config_id_(config_id) {
    db = std::make_shared<Connection>(db_path);

    SchemaUpdater updater{db.get()};

    if (!updater.apply_migration_files(migration_files_path, TARGET_MIGRATION_FILE_VERSION)) {
        if (db_path.parent_path().empty()) {
            EVLOG_error
                << "Could not apply migrations for database at provided path: \"" << db_path.string()
                << "\" likely because the database path is just a filename. You MUST provide a full path to the "
                   "database.";
        }
        throw MigrationException("SQL migration failed");
    }

    if (!db->open_connection()) {
        throw std::runtime_error("Could not open database at provided path: " + db_path.string());
    } else {
        EVLOG_debug << "Established connection to database successfully: " << db_path;
    }
}

SqliteStorage::SqliteStorage(std::shared_ptr<ConnectionInterface> connection, int config_id) :
    db(std::move(connection)), config_id_(config_id) {
    if (!db->open_connection()) {
        throw std::runtime_error("Could not open shared database connection");
    }
}

SqliteStorage::~SqliteStorage() {
    db->close_connection();
}

GenericResponseStatus SqliteStorage::write_module_configs(const ModuleConfigurations& module_configs) {
    try {
        auto transaction = this->db->begin_transaction();

        for (const auto& [module_id, module] : module_configs) {
            ModuleData module_data;
            module_data.module_id = module_id;
            module_data.module_name = module.module_name;
            module_data.standalone = module.standalone;
            module_data.capabilities = module.capabilities;

            if (this->write_module_data(module_data) != GenericResponseStatus::OK) {
                EVLOG_error << "Failed to write module info for module: " << module_id;
                return GenericResponseStatus::Failed;
            }

            for (const auto& [requirement_id, connections] : module.connections) {
                for (const auto& connection : connections) {
                    Fulfillment fulfillment;
                    fulfillment.module_id = connection.module_id;
                    fulfillment.implementation_id = connection.implementation_id;
                    fulfillment.requirement = {requirement_id};

                    if (this->write_module_fulfillment(module_id, fulfillment) != GenericResponseStatus::OK) {
                        EVLOG_error << "Failed to write module fulfillment for module: " << module_id
                                    << " and requirement: " << requirement_id;
                        return GenericResponseStatus::Failed;
                    }
                }
            }

            if (module.mapping.module.has_value()) {
                const auto& map = module.mapping.module.value();
                if (this->write_module_tier_mapping(module_id, default_module_implementation_id(), map.evse,
                                                    map.connector) != GenericResponseStatus::OK) {
                    EVLOG_error << "Failed to write module tier mapping for module: " << module_id;
                    return GenericResponseStatus::Failed;
                }
            }

            for (const auto& [impl_id, mapping] : module.mapping.implementations) {
                if (mapping.has_value()) {
                    const auto& map = mapping.value();
                    if (this->write_module_tier_mapping(module_id, impl_id, map.evse, map.connector) !=
                        GenericResponseStatus::OK) {
                        EVLOG_error << "Failed to write module tier mapping for module: " << module_id
                                    << " and implementation id: " << impl_id;
                    }
                }
            }

            for (const auto& [impl_id, params] : module.configuration_parameters) {
                for (const auto& param : params) {
                    ConfigurationParameterIdentifier identifier;
                    identifier.module_id = module_id;
                    identifier.module_implementation_id = impl_id;
                    identifier.configuration_parameter_name = param.name;

                    std::string value;
                    if (std::holds_alternative<std::string>(param.value)) {
                        value = std::get<std::string>(param.value);
                    } else {
                        const nlohmann::json temp = param.value;
                        value = temp.dump();
                    }
                    if (this->write_configuration_parameter(identifier, param.characteristics, value) !=
                        GetSetResponseStatus::OK) {
                        EVLOG_error << "Failed to write configuration parameter for module: " << module_id
                                    << ", param: " << identifier.configuration_parameter_name;
                    }
                }
            }

            if (this->write_access(module_id, module.access) != GenericResponseStatus::OK) {
                EVLOG_error << "Failed to write module access for module: " << module_id;
            }
        }

        transaction->commit();
        return GenericResponseStatus::OK;
    } catch (const std::exception& e) {
        EVLOG_error << "Failed writing config to database: " << e.what();
        return GenericResponseStatus::Failed;
    }
}

GetModuleConfigsResponse SqliteStorage::get_module_configs() {
    GetModuleConfigsResponse response;

    try {
        const std::string sql = "SELECT ID FROM MODULE WHERE CONFIG_ID = @config_id";

        auto stmt = this->db->new_statement(sql);
        stmt->bind_int("@config_id", config_id_);

        while (stmt->step() == SQLITE_ROW) {
            auto module_config_response = this->get_module_config(stmt->column_text(0));
            if (module_config_response.status == GenericResponseStatus::OK and
                module_config_response.config.has_value()) {
                response.module_configs[module_config_response.config.value().module_id] =
                    module_config_response.config.value();
            } else {
                response.status = GenericResponseStatus::Failed;
                return response;
            }
        }

        response.status = GenericResponseStatus::OK;
        return response;
    } catch (const std::exception& e) {
        EVLOG_error << "Failed to get EVerest config: " << e.what();
        response.status = GenericResponseStatus::Failed;
        return response;
    }
}

GetModuleConfigurationResponse SqliteStorage::get_module_config(const std::string& module_id) {

    GetModuleConfigurationResponse response;

    const auto module_data_response = this->get_module_data(module_id);
    if (module_data_response.status == GenericResponseStatus::Failed or !module_data_response.module_data.has_value()) {
        response.status = GenericResponseStatus::Failed;
        return response;
    }

    const auto module_data = module_data_response.module_data.value();

    const auto module_fulfillments_response = this->get_module_fulfillments(module_id);
    if (module_fulfillments_response.status == GenericResponseStatus::Failed) {
        response.status = GenericResponseStatus::Failed;
        return response;
    }
    const auto module_fulfillments = module_fulfillments_response.module_fulfillments;

    const auto module_tier_mappings_response = this->get_module_tier_mappings(module_id);
    if (module_tier_mappings_response.status == GenericResponseStatus::Failed) {
        response.status = GenericResponseStatus::Failed;
        return response;
    }
    const auto module_tier_mappings = module_tier_mappings_response.module_tier_mappings;

    const auto config_access_response = this->get_config_access(module_id);
    if (config_access_response.status == GenericResponseStatus::Failed) {
        response.status = GenericResponseStatus::Failed;
        return response;
    }
    const auto config_access = config_access_response.config_access;

    try {

        const std::string sql =
            "SELECT PARAMETER_NAME, VALUE, MODULE_IMPLEMENTATION_ID, MUTABILITY_ID, DATATYPE_ID, UNIT "
            "FROM CONFIGURATION WHERE CONFIG_ID = @config_id AND MODULE_ID = @module_id";
        auto stmt = this->db->new_statement(sql);

        stmt->bind_int("@config_id", config_id_);
        stmt->bind_text("@module_id", module_id);

        ModuleConfig module_config;
        module_config.capabilities = module_data.capabilities;
        module_config.module_id = module_data.module_id;
        module_config.module_name = module_data.module_name;
        module_config.standalone = module_data.standalone;
        for (const auto& fulfillment : module_fulfillments) {
            module_config.connections[fulfillment.requirement.id].push_back(fulfillment);
        }
        module_config.mapping = module_tier_mappings;
        module_config.access.config = config_access;

        while (stmt->step() == SQLITE_ROW) {
            ConfigurationParameter configuration_parameter;
            configuration_parameter.name =
                stmt->column_text(to_int(ConfigurationColumnModuleIdIndex::COL_PARAMETER_NAME));
            const auto value_str = stmt->column_text(to_int(ConfigurationColumnModuleIdIndex::COL_VALUE));
            auto implementation_id =
                stmt->column_text(to_int(ConfigurationColumnModuleIdIndex::COL_MODULE_IMPLEMENTATION_ID));
            ConfigurationParameterCharacteristics characteristics;
            characteristics.mutability =
                static_cast<Mutability>(stmt->column_int(to_int(ConfigurationColumnModuleIdIndex::COL_MUTABILITY_ID)));
            characteristics.datatype =
                static_cast<Datatype>(stmt->column_int(to_int(ConfigurationColumnModuleIdIndex::COL_DATATYPE_ID)));
            characteristics.unit = stmt->column_text_nullable(to_int(ConfigurationColumnModuleIdIndex::COL_UNIT));
            configuration_parameter.characteristics = characteristics;
            configuration_parameter.value = parse_config_value(characteristics.datatype, value_str);

            module_config.configuration_parameters[implementation_id].push_back(configuration_parameter);
        }
        response.status = GenericResponseStatus::OK;
        response.config = module_config;
        return response;
    } catch (const std::exception& e) {
        EVLOG_error << "Failed to get module config with module_id: " << module_id;
        response.status = GenericResponseStatus::Failed;
        return response;
    }
}

GetConfigurationParameterResponse
SqliteStorage::get_configuration_parameter(const ConfigurationParameterIdentifier& identifier) {

    GetConfigurationParameterResponse response;

    try {
        const std::string sql = "SELECT VALUE, MUTABILITY_ID, DATATYPE_ID, UNIT FROM "
                                "CONFIGURATION WHERE CONFIG_ID = @config_id AND MODULE_ID = "
                                "@module_id AND PARAMETER_NAME = @config_param_name AND MODULE_IMPLEMENTATION_ID = "
                                "@module_implementation_id";
        auto stmt = this->db->new_statement(sql);

        stmt->bind_int("@config_id", config_id_);
        stmt->bind_text("@module_id", identifier.module_id);
        stmt->bind_text("@config_param_name", identifier.configuration_parameter_name);
        stmt->bind_text("@module_implementation_id", identifier.module_implementation_id.has_value()
                                                         ? identifier.module_implementation_id.value()
                                                         : default_module_implementation_id());

        const auto status = stmt->step();

        if (status == SQLITE_DONE) {
            response.status = GetSetResponseStatus::NotFound;
            return response;
        }

        if (status == SQLITE_ROW) {
            response.status = GetSetResponseStatus::OK;

            ConfigurationParameter configuration_parameter;
            configuration_parameter.name = identifier.configuration_parameter_name;
            const auto value_str = stmt->column_text(0);
            ConfigurationParameterCharacteristics characteristics;
            characteristics.mutability = static_cast<Mutability>(stmt->column_int(1));
            characteristics.datatype = static_cast<Datatype>(stmt->column_int(2));
            characteristics.unit = stmt->column_text_nullable(3);
            configuration_parameter.characteristics = characteristics;
            configuration_parameter.value = parse_config_value(characteristics.datatype, value_str);
            response.configuration_parameter = configuration_parameter;
            return response;
        }
    } catch (const std::exception& e) {
        EVLOG_error << "Failed to get config value with module_id: " << identifier.module_id
                    << " and config_parameter_name: " << identifier.configuration_parameter_name;
        response.status = GetSetResponseStatus::Failed;
        return response;
    }

    return response;
}

GetSetResponseStatus
SqliteStorage::write_configuration_parameter(const ConfigurationParameterIdentifier& identifier,
                                             const ConfigurationParameterCharacteristics characteristics,
                                             const std::string& value) {
    try {
        const std::string insert_query = "INSERT OR REPLACE INTO CONFIGURATION (CONFIG_ID, MODULE_ID, PARAMETER_NAME, "
                                         "VALUE, "
                                         "MUTABILITY_ID, DATATYPE_ID, UNIT, MODULE_IMPLEMENTATION_ID) VALUES "
                                         "(?, ?, ?, ?, ?, ?, ?, ?);";

        auto stmt = this->db->new_statement(insert_query);
        stmt->bind_int(to_int(ConfigurationColumnIndex::COL_CONFIG_ID), config_id_);
        stmt->bind_text(to_int(ConfigurationColumnIndex::COL_MODULE_ID), identifier.module_id);
        stmt->bind_text(to_int(ConfigurationColumnIndex::COL_PARAMETER_NAME), identifier.configuration_parameter_name);
        stmt->bind_text(to_int(ConfigurationColumnIndex::COL_VALUE), value);
        stmt->bind_int(to_int(ConfigurationColumnIndex::COL_MUTABILITY_ID),
                       static_cast<int>(characteristics.mutability));
        stmt->bind_int(to_int(ConfigurationColumnIndex::COL_DATATYPE_ID), static_cast<int>(characteristics.datatype));
        if (characteristics.unit.has_value()) {
            stmt->bind_text(to_int(ConfigurationColumnIndex::COL_UNIT), characteristics.unit.value());
        } else {
            stmt->bind_null(to_int(ConfigurationColumnIndex::COL_UNIT));
        }

        stmt->bind_text(to_int(ConfigurationColumnIndex::COL_MODULE_IMPLEMENTATION_ID),
                        identifier.module_implementation_id.value_or(default_module_implementation_id()));

        if (stmt->step() != SQLITE_DONE) {
            return GetSetResponseStatus::NotFound;
        }

        return GetSetResponseStatus::OK;
    } catch (const std::exception& e) {
        EVLOG_error << "Failed to set config value with module_id: " << identifier.module_id
                    << " and config_parameter_name: " << identifier.configuration_parameter_name;
        return GetSetResponseStatus::Failed;
    }
}

void SqliteStorage::mark_valid(const bool is_valid, const std::string& config_dump,
                               const std::optional<fs::path>& config_file_path,
                               const std::optional<std::string>& description) {
    const std::string sql =
        "INSERT INTO CONFIG_META (ID, LAST_UPDATED, VALID, CONFIG_DUMP, CONFIG_FILE_PATH, DESCRIPTION) VALUES "
        "(@config_id, @last_updated, @is_valid, @config_dump, @config_file_path, @description) "
        "ON CONFLICT(ID) DO UPDATE SET "
        "LAST_UPDATED=excluded.LAST_UPDATED, VALID=excluded.VALID, "
        "CONFIG_DUMP=excluded.CONFIG_DUMP, CONFIG_FILE_PATH=excluded.CONFIG_FILE_PATH, "
        "DESCRIPTION=excluded.DESCRIPTION;";
    auto stmt = this->db->new_statement(sql);

    stmt->bind_int("@config_id", config_id_);
    const auto last_updated = Everest::Date::to_rfc3339(date::utc_clock::now());
    stmt->bind_text("@last_updated", last_updated);
    stmt->bind_int("@is_valid", is_valid ? 1 : 0);
    stmt->bind_text("@config_dump", config_dump, SQLiteString::Transient);
    if (config_file_path.has_value()) {
        stmt->bind_text("@config_file_path", config_file_path.value().string(), SQLiteString::Transient);
    } else {
        stmt->bind_null("@config_file_path");
    }
    if (description.has_value()) {
        stmt->bind_text("@description", description.value(), SQLiteString::Transient);
    } else {
        stmt->bind_null("@description");
    }
    if (stmt->step() != SQLITE_DONE) {
        EVLOG_error << "Failed to mark config as valid";
    } else {
        EVLOG_debug << "Marked config as valid";
    }
}

GetSetResponseStatus SqliteStorage::update_configuration_parameter(const ConfigurationParameterIdentifier& identifier,
                                                                   const std::string& value) {

    try {
        const std::string update_query = "UPDATE CONFIGURATION SET VALUE = @value "
                                         "WHERE CONFIG_ID = @config_id AND MODULE_ID = @module_id AND "
                                         "PARAMETER_NAME = @parameter_name AND "
                                         "MODULE_IMPLEMENTATION_ID = @module_implementation_id;";

        auto stmt = this->db->new_statement(update_query);
        stmt->bind_text("@value", value);
        stmt->bind_int("@config_id", config_id_);
        stmt->bind_text("@module_id", identifier.module_id);
        stmt->bind_text("@parameter_name", identifier.configuration_parameter_name);
        const std::string impl_id = identifier.module_implementation_id.value_or(default_module_implementation_id());
        stmt->bind_text("@module_implementation_id", impl_id);

        if (stmt->step() != SQLITE_DONE) {
            return GetSetResponseStatus::Failed;
        }

        if (stmt->changes() == 0) {
            return GetSetResponseStatus::NotFound;
        }

        return GetSetResponseStatus::OK;
    } catch (const std::exception& e) {
        EVLOG_error << "Failed to set config value with module_id: " << identifier.module_id
                    << " and config_parameter_name: " << identifier.configuration_parameter_name;
        return GetSetResponseStatus::Failed;
    }
}

GenericResponseStatus SqliteStorage::write_module_data(const ModuleData& module_data) {

    const std::string insert_query =
        "INSERT OR REPLACE INTO MODULE (CONFIG_ID, ID, NAME, STANDALONE, CAPABILITIES) VALUES (?, ?, ?, ?, ?);";

    auto stmt = this->db->new_statement(insert_query);

    stmt->bind_int(1, config_id_);
    stmt->bind_text(2, module_data.module_id);
    stmt->bind_text(3, module_data.module_name);
    stmt->bind_int(4, module_data.standalone);

    if (module_data.capabilities.has_value()) {
        stmt->bind_text(5, json(module_data.capabilities.value()).dump(), SQLiteString::Transient);
    } else {
        stmt->bind_null(5);
    }

    if (stmt->step() != SQLITE_DONE) {
        return GenericResponseStatus::Failed;
    }

    return GenericResponseStatus::OK;
}

GenericResponseStatus SqliteStorage::write_module_fulfillment(const std::string& module_id,
                                                              const Fulfillment& fulfillment) {
    const std::string sql =
        "INSERT OR REPLACE INTO MODULE_FULFILLMENT (CONFIG_ID, MODULE_ID, REQUIREMENT_NAME, IMPLEMENTATION_ID, "
        "IMPLEMENTATION_MODULE_ID) VALUES (?,?,?,?,?)";

    auto stmt = this->db->new_statement(sql);

    stmt->bind_int(1, config_id_);
    stmt->bind_text(2, module_id);
    stmt->bind_text(3, fulfillment.requirement.id);
    stmt->bind_text(4, fulfillment.implementation_id);
    stmt->bind_text(5, fulfillment.module_id);

    if (stmt->step() != SQLITE_DONE) {
        return GenericResponseStatus::Failed;
    }

    return GenericResponseStatus::OK;
}

GenericResponseStatus SqliteStorage::write_module_tier_mapping(const std::string& module_id,
                                                               const std::string& implementation_id,
                                                               const int32_t evse_id,
                                                               const std::optional<int32_t> connector_id) {
    const std::string sql = "INSERT OR REPLACE INTO MODULE_TIER_MAPPING (CONFIG_ID, MODULE_ID, IMPLEMENTATION_ID, "
                            "EVSE_ID, CONNECTOR_ID) VALUES (?,?,?,?,?)";

    auto stmt = this->db->new_statement(sql);

    stmt->bind_int(1, config_id_);
    stmt->bind_text(2, module_id);
    stmt->bind_text(3, implementation_id);
    stmt->bind_int(4, evse_id);

    if (connector_id.has_value()) {
        stmt->bind_int(5, connector_id.value());
    } else {
        stmt->bind_null(5);
    }

    if (stmt->step() != SQLITE_DONE) {
        return GenericResponseStatus::Failed;
    }

    return GenericResponseStatus::OK;
}

GenericResponseStatus SqliteStorage::write_access(const std::string& module_id, const Access& access) {
    if (access.config.has_value()) {
        return write_config_access(module_id, access.config.value());
    }
    return GenericResponseStatus::OK;
}
GenericResponseStatus SqliteStorage::write_config_access(const std::string& module_id,
                                                         const ConfigAccess& config_access) {
    // write global config access to db
    const std::string sql =
        "INSERT OR REPLACE INTO CONFIG_ACCESS (CONFIG_ID, MODULE_ID, ALLOW_GLOBAL_READ, ALLOW_GLOBAL_WRITE, "
        "ALLOW_SET_READ_ONLY) VALUES (?,?,?,?,?)";

    auto stmt = this->db->new_statement(sql);

    stmt->bind_int(1, config_id_);
    stmt->bind_text(2, module_id);
    stmt->bind_int(3, config_access.allow_global_read ? 1 : 0);
    stmt->bind_int(4, config_access.allow_global_write ? 1 : 0);
    stmt->bind_int(5, config_access.allow_set_read_only ? 1 : 0);

    if (stmt->step() != SQLITE_DONE) {
        return GenericResponseStatus::Failed;
    }

    // write individual module config access to db
    for (const auto& [other_module_id, module_config_access] : config_access.modules) {
        const auto result = write_module_config_access(module_id, other_module_id, module_config_access);
        if (result != GenericResponseStatus::OK) {
            return GenericResponseStatus::Failed;
        }
    }

    return GenericResponseStatus::OK;
}
GenericResponseStatus SqliteStorage::write_module_config_access(const std::string& module_id,
                                                                const std::string& other_module_id,
                                                                const ModuleConfigAccess& module_config_access) {
    const std::string sql = "INSERT OR REPLACE INTO MODULE_CONFIG_ACCESS (CONFIG_ID, MODULE_ID, OTHER_MODULE_ID, "
                            "ALLOW_READ, ALLOW_WRITE, ALLOW_SET_READ_ONLY) VALUES (?,?,?,?,?,?)";

    auto stmt = this->db->new_statement(sql);

    stmt->bind_int(1, config_id_);
    stmt->bind_text(2, module_id);
    stmt->bind_text(3, other_module_id);
    stmt->bind_int(4, module_config_access.allow_read ? 1 : 0);
    stmt->bind_int(5, module_config_access.allow_write ? 1 : 0);
    stmt->bind_int(6, module_config_access.allow_set_read_only ? 1 : 0);

    if (stmt->step() != SQLITE_DONE) {
        return GenericResponseStatus::Failed;
    }
    return GenericResponseStatus::OK;
}

GetModuleDataResponse SqliteStorage::get_module_data(const std::string& module_id) {

    GetModuleDataResponse response;

    const std::string sql =
        "SELECT NAME, STANDALONE, CAPABILITIES FROM MODULE WHERE CONFIG_ID = @config_id AND ID = @module_id";

    auto stmt = this->db->new_statement(sql);
    stmt->bind_int("@config_id", config_id_);
    stmt->bind_text("@module_id", module_id);
    const auto status = stmt->step();

    if (status == SQLITE_DONE) {
        response.status = GenericResponseStatus::Failed;
        return response;
    }

    if (status == SQLITE_ROW) {
        ModuleData module_data;
        module_data.module_id = module_id;
        module_data.module_name = stmt->column_text(0);
        module_data.standalone = stmt->column_int(1);
        const auto capabilities_str = stmt->column_text_nullable(2);
        if (capabilities_str.has_value()) {
            module_data.capabilities = json::parse(capabilities_str.value()).get<std::vector<std::string>>();
        }
        response.module_data = module_data;
        response.status = GenericResponseStatus::OK;
    }

    return response;
}

GetModuleFulfillmentsResponse SqliteStorage::get_module_fulfillments(const std::string& module_id) {
    GetModuleFulfillmentsResponse response;

    const std::string sql =
        "SELECT REQUIREMENT_NAME, IMPLEMENTATION_ID, IMPLEMENTATION_MODULE_ID FROM MODULE_FULFILLMENT "
        "WHERE CONFIG_ID = @config_id AND MODULE_ID = @module_id";

    auto stmt = this->db->new_statement(sql);
    stmt->bind_int("@config_id", config_id_);
    stmt->bind_text("@module_id", module_id);

    size_t index = 0;
    while (stmt->step() == SQLITE_ROW) {
        Fulfillment fulfillment;
        fulfillment.requirement = {stmt->column_text(0), index};
        fulfillment.implementation_id = stmt->column_text(1);
        fulfillment.module_id = stmt->column_text(2);
        response.module_fulfillments.push_back(fulfillment);
        index++;
    }

    response.status = GenericResponseStatus::OK; // FIXME: when to return failed?
    return response;
}

GetModuleTierMappingsResponse SqliteStorage::get_module_tier_mappings(const std::string& module_id) {
    GetModuleTierMappingsResponse response;

    const std::string sql = "SELECT IMPLEMENTATION_ID, EVSE_ID, CONNECTOR_ID FROM MODULE_TIER_MAPPING "
                            "WHERE CONFIG_ID = @config_id AND MODULE_ID = @module_id";

    auto stmt = this->db->new_statement(sql);
    stmt->bind_int("@config_id", config_id_);
    stmt->bind_text("@module_id", module_id);

    ModuleTierMappings module_tier_mappings;

    while (stmt->step() == SQLITE_ROW) {
        auto implementation_id = stmt->column_text(0);
        Mapping mapping = {stmt->column_int(1)};
        if (stmt->column_type(2) != SQLITE_NULL) {
            mapping.connector = stmt->column_int(2);
        }
        if (implementation_id == default_module_implementation_id()) {
            module_tier_mappings.module = mapping;
        } else {
            module_tier_mappings.implementations[implementation_id] = mapping;
        }
    }

    response.module_tier_mappings = module_tier_mappings;
    response.status = GenericResponseStatus::OK;
    return response;
}

GetModuleConfigAccessResponse SqliteStorage::get_module_config_access(const std::string& module_id) {
    GetModuleConfigAccessResponse response;
    const std::string sql = "SELECT OTHER_MODULE_ID, ALLOW_SET_READ_ONLY FROM MODULE_CONFIG_ACCESS "
                            "WHERE CONFIG_ID = @config_id AND MODULE_ID = @module_id";

    auto stmt = this->db->new_statement(sql);
    stmt->bind_int("@config_id", config_id_);
    stmt->bind_text("@module_id", module_id);

    std::map<std::string, everest::config::ModuleConfigAccess> module_config_access;
    while (stmt->step() == SQLITE_ROW) {
        const auto other_module_id = stmt->column_text(0);
        const auto allow_set_read_only = stmt->column_int(1) != 0;
        module_config_access[other_module_id].allow_set_read_only = allow_set_read_only;
    }

    response.module_config_access = module_config_access;
    response.status = GenericResponseStatus::OK;
    return response;
}

GetConfigAccessResponse SqliteStorage::get_config_access(const std::string& module_id) {
    GetConfigAccessResponse response;

    const std::string sql = "SELECT ALLOW_GLOBAL_READ, ALLOW_SET_READ_ONLY FROM CONFIG_ACCESS "
                            "WHERE CONFIG_ID = @config_id AND MODULE_ID = @module_id";

    auto stmt = this->db->new_statement(sql);
    stmt->bind_int("@config_id", config_id_);
    stmt->bind_text("@module_id", module_id);

    const auto status = stmt->step();

    if (status == SQLITE_DONE) {
        response.status = GenericResponseStatus::OK;
        return response;
    }

    if (status == SQLITE_ROW) {
        ConfigAccess config_access;
        config_access.allow_global_read = stmt->column_int(0) != 0;
        config_access.allow_set_read_only = stmt->column_int(1) != 0;
        const auto module_config_access = get_module_config_access(module_id);
        if (module_config_access.status == GenericResponseStatus::OK) {
            config_access.modules = module_config_access.module_config_access;
        }
        response.config_access = config_access;
        response.status = GenericResponseStatus::OK;
    }

    return response;
}

} // namespace everest::config
