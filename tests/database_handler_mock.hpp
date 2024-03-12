// SPDX-License-Identifier: Apache-2.0

#ifndef OCPP_DATABASE_HANDLE_MOCK_H
#define OCPP_DATABASE_HANDLE_MOCK_H

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <ocpp/v16/database_handler.hpp>

namespace ocpp {

class DatabaseHandlerMock : public v16::DatabaseHandler {
public:
    DatabaseHandlerMock(const std::string& chargepoint_id, const fs::path& database_path,
                        const fs::path& init_script_path) :
        DatabaseHandler(chargepoint_id, database_path, init_script_path){};
    MOCK_METHOD(void, insert_or_update_charging_profile, (const int, const v16::ChargingProfile&), (override));
    MOCK_METHOD(void, delete_charging_profile, (const int profile_id), (override));
};

} // namespace ocpp

#endif // DATABASE_HANDLE_MOCK_H