// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2024 Pionix GmbH and Contributors to EVerest

#include <ocpp/common/database/database_schema_updater.hpp>

#include <fstream>
#include <regex>

#include <everest/logging.hpp>

namespace ocpp::common {

// Helper functions

enum class Direction {
    Up,
    Down
};

struct MigrationFile {
    fs::path path;
    uint32_t version;
    Direction direction;
};

std::ostream& operator<<(std::ostream& os, const MigrationFile& info) {
    os << "Migration file [" << (info.direction == Direction::Up ? "up" : "down") << "] version " << info.version
       << ", path: " << info.path.c_str();
    return os;
}

std::vector<MigrationFile> get_migration_file_list(const fs::path& migration_file_directory) {
    std::regex filename_pattern{R"(^(\d+)_(up|down)(-[ \S]+|)\.sql$)"};

    std::vector<MigrationFile> result;

    for (auto& entry : fs::directory_iterator(migration_file_directory)) {
        if (entry.is_regular_file() and entry.file_size() > 0) {
            const fs::path& path = entry.path();
            std::cmatch match;
            std::string filename =
                path.filename(); // Store in a variable otherwise after the match the string temporary is gone
            if (std::regex_match(filename.c_str(), match, filename_pattern) and match.size() == 4) {
                // [0] = whole match
                // [1] = version id
                // [2] = up or down
                // [3] = description or empty
                result.push_back(MigrationFile{path, static_cast<uint32_t>(std::stoul(match[1].str())),
                                               match[2] == "up" ? Direction::Up : Direction::Down});
            }
        }
    }

    return result;
}

void filter_and_sort_migration_file_list(std::vector<MigrationFile>& list, Direction direction, uint32_t min_version,
                                         uint32_t max_version) {
    auto filter = [direction, min_version, max_version](const MigrationFile& item) {
        return item.direction != direction or item.version < min_version or item.version > max_version;
    };

    list.erase(std::remove_if(list.begin(), list.end(), filter), list.end());

    std::sort(list.begin(), list.end(), [direction](const auto& a, const auto& b) {
        if (direction == Direction::Up) {
            return a.version < b.version;
        } else {
            return b.version < a.version;
        }
    });
}

bool is_migration_file_list_valid(std::vector<MigrationFile>& list, uint32_t max_version) {
    auto expected_files = (max_version * 2) - 1;
    if (list.size() < expected_files) {
        EVLOG_error << "Expected " << expected_files << " files but only found: " << list.size();
        return false;
    }
    if (list.size() % 2 == 0) {
        EVLOG_error << "Nr of migration files should always be uneven: 1 initial file + n pairs";
        return false;
    }

    std::sort(list.begin(), list.end(), [](const auto& a, const auto& b) {
        return std::tie(a.version, a.direction) < std::tie(b.version, b.direction);
    });

    if (list.at(0).version != 1 or list.at(0).direction != Direction::Up) {
        EVLOG_error << "Invalid initial migration file";
        return false;
    }

    for (size_t i = 1; i < list.size(); i += 2) {
        uint32_t expected_version = (i / 2) + 2;
        const auto& up = list.at(i);
        const auto& down = list.at(i + 1);
        if (up.version != expected_version || up.direction != Direction::Up) {
            EVLOG_error << "Expected migration file " << expected_version << "_up.sql but got: " << up.path.filename();
            return false;
        }
        if (down.version != expected_version || down.direction != Direction::Down) {
            EVLOG_error << "Expected migration file " << expected_version
                        << "_down.sql but got: " << down.path.filename();
            return false;
        }
    }
    return true;
}

std::optional<std::vector<MigrationFile>> get_migration_file_sequence(const fs::path& migration_file_directory,
                                                                      Direction direction, uint32_t current_version,
                                                                      uint32_t target_version) {
    auto list = get_migration_file_list(migration_file_directory);

    EVLOG_debug << "Migration list:";

    for (auto& item : list) {
        EVLOG_debug << item;
    }

    if (!is_migration_file_list_valid(list, std::max(current_version, target_version))) {
        return std::nullopt;
    }

    const auto lowest = std::min(current_version, target_version) + 1;
    const auto highest = std::max(current_version, target_version);

    filter_and_sort_migration_file_list(list, direction, lowest, highest);

    EVLOG_info << "Migration files to apply:";

    for (auto& item : list) {
        EVLOG_info << item;
    }
    return list;
}

DatabaseSchemaUpdater::DatabaseSchemaUpdater(DatabaseConnectionInterface* database) noexcept : database(database) {
}

uint32_t DatabaseSchemaUpdater::get_user_version() {
    auto statement = this->database->new_statement("PRAGMA user_version");

    if (statement->step() != SQLITE_ROW) {
        throw std::runtime_error("Could not get user_version from database");
    }
    return statement->column_int(0);
}

void DatabaseSchemaUpdater::set_user_version(uint32_t version) {
    using namespace std::string_literals;

    if (!this->database->execute_statement("PRAGMA user_version = "s + std::to_string(version))) {
        throw std::runtime_error("Could not set user_version in database");
    }
}

bool DatabaseSchemaUpdater::apply_migration_files(const fs::path& migration_file_directory,
                                                  uint32_t target_schema_version) {
    if (!fs::is_directory(migration_file_directory)) {
        EVLOG_error << "Migration files must be in a directory: " << migration_file_directory.c_str();
        return false;
    }

    if (target_schema_version == 0) {
        EVLOG_error << "Migration target_version 0 is invalid";
        return false;
    }

    uint32_t current_version = 0;

    try {
        this->database->open_connection();
        current_version = this->get_user_version();
        EVLOG_info << "Target version: " << target_schema_version << ", current version: " << current_version;
    } catch (std::runtime_error& e) {
        EVLOG_error << "Failure during migration file apply: " << e.what();
        return false;
    }

    if (current_version == target_schema_version) {
        EVLOG_info << "No migrations to apply since versions match";
        this->database->close_connection();
        return true;
    }

    Direction direction = Direction::Up;

    if (current_version > target_schema_version) {
        direction = Direction::Down;
    }

    auto list =
        get_migration_file_sequence(migration_file_directory, direction, current_version, target_schema_version);

    if (!list.has_value()) {
        EVLOG_error << "Missing migration files in sequence, no actions performed";
        this->database->close_connection();
        return false;
    }

    bool retval = true;
    try {
        auto transaction = this->database->begin_transaction();

        for (const auto& item : list.value()) {
            std::ifstream stream{item.path};
            std::stringstream init_sql;

            init_sql << stream.rdbuf();

            if (!this->database->execute_statement(init_sql.str())) {
                EVLOG_error << "Could not apply migration file " << item.path;
                throw std::runtime_error("Database access error");
            }
        }

        this->set_user_version(target_schema_version);
        transaction->commit();
    } catch (std::exception& e) {
        EVLOG_error << "Failure during migration file apply: " << e.what();
        retval = false;
    }

    this->database->close_connection();
    return retval;
}

} // namespace ocpp::common