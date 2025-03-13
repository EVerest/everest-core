// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#pragma once

#include "everest/logging.hpp"
#include "ocpp/v2/functional_blocks/smart_charging.hpp"
#include "ocpp/v2/ocpp_types.hpp"
#include "ocpp/v2/profile.hpp"
#include "ocpp/v2/utils.hpp"
#include <ocpp/v2/messages/ClearChargingProfile.hpp>
#include <ocpp/v2/messages/GetChargingProfiles.hpp>
#include <ocpp/v2/messages/GetCompositeSchedule.hpp>
#include <ocpp/v2/messages/SetChargingProfile.hpp>
#include <ocpp/v2/ocpp_enums.hpp>

#include "connectivity_manager_mock.hpp"
#include "database_handler_mock.hpp"
#include "device_model_test_helper.hpp"
#include "lib/ocpp/common/database_testing_utils.hpp"
#include "message_dispatcher_mock.hpp"
#include "mocks/database_handler_fake.hpp"
#include <component_state_manager_mock.hpp>
#include <evse_manager_fake.hpp>
#include <evse_mock.hpp>
#include <evse_security_mock.hpp>
#include <gmock/gmock.h>
#include <ocpp/v2/functional_blocks/functional_block_context.hpp>

#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <chrono>
#include <filesystem>
#include <iomanip>
#include <iostream>
#include <openssl/evp.h>
#include <openssl/md5.h>
#include <openssl/sha.h>
#include <sstream>
#include <vector>

using ::testing::MockFunction;
namespace ocpp::v2 {

static const std::string BASE_JSON_PATH = std::string(TEST_PROFILES_LOCATION_V2) + "/json";

constexpr int NR_OF_EVSES = 1;
constexpr int NR_OF_TWO_EVSES = 2;
constexpr int STATION_WIDE_ID = 0;
constexpr int DEFAULT_EVSE_ID = 1;
constexpr int DEFAULT_PROFILE_ID = 1;
constexpr int DEFAULT_STACK_LEVEL = 1;
constexpr int DEFAULT_REQUEST_ID = 1;
constexpr int32_t DEFAULT_LIMIT_AMPERE = 57;
constexpr int32_t DEFAULT_LIMIT_WATT = 55612;
constexpr int32_t DEFAULT_NR_PHASES = 3;
static const std::string DEFAULT_TX_ID = "10c75ff7-74f5-44f5-9d01-f649f3ac7b78";

// Operator overloads
bool operator==(const V2XFreqWattPoint& a, const V2XFreqWattPoint& b);
bool operator==(const V2XSignalWattPoint& a, const V2XSignalWattPoint& b);
bool operator==(const ChargingSchedulePeriod& a, const ChargingSchedulePeriod& b);
bool operator!=(const ChargingSchedulePeriod& a, const ChargingSchedulePeriod& b);
bool operator==(const CompositeSchedule& a, const CompositeSchedule& b);
bool operator!=(const CompositeSchedule& a, const CompositeSchedule& b);
bool operator==(const LimitAtSoC& a, const LimitAtSoC& b);
bool operator==(const ChargingSchedule& a, const ChargingSchedule& b);
bool operator!=(const ChargingSchedule& a, const ChargingSchedule& b);
bool operator==(const period_entry_t& a, const period_entry_t& b);
bool operator!=(const period_entry_t& a, const period_entry_t& b);
bool operator==(const std::vector<period_entry_t>& a, const std::vector<period_entry_t>& b);

std::string to_string(const period_entry_t& entry);
std::ostream& operator<<(std::ostream& os, const period_entry_t& entry);
ocpp::DateTime dt(const std::string& dt_string);

ChargingProfile
create_charging_profile(int32_t charging_profile_id, ChargingProfilePurposeEnum charging_profile_purpose,
                        const std::vector<ChargingSchedule>& charging_schedules,
                        std::optional<std::string> transaction_id = {},
                        ChargingProfileKindEnum charging_profile_kind = ChargingProfileKindEnum::Absolute,
                        int stack_level = DEFAULT_STACK_LEVEL, std::optional<ocpp::DateTime> validFrom = {},
                        std::optional<ocpp::DateTime> validTo = {});
ChargingProfile
create_charging_profile(int32_t charging_profile_id, ChargingProfilePurposeEnum charging_profile_purpose,
                        ChargingSchedule charging_schedule, std::optional<std::string> transaction_id = {},
                        ChargingProfileKindEnum charging_profile_kind = ChargingProfileKindEnum::Absolute,
                        int stack_level = DEFAULT_STACK_LEVEL, std::optional<ocpp::DateTime> validFrom = {},
                        std::optional<ocpp::DateTime> validTo = {});
ChargingSchedule create_charge_schedule(ChargingRateUnitEnum charging_rate_unit);
ChargingSchedule create_charge_schedule(ChargingRateUnitEnum charging_rate_unit,
                                        const std::vector<ChargingSchedulePeriod>& charging_schedule_period,
                                        std::optional<ocpp::DateTime> start_schedule = std::nullopt);
std::vector<ChargingSchedulePeriod>
create_charging_schedule_periods(int32_t start_period, std::optional<int32_t> number_phases = std::nullopt,
                                 std::optional<int32_t> phase_to_use = std::nullopt);
std::vector<ChargingSchedulePeriod> create_charging_schedule_periods(const std::vector<int32_t>& start_periods);
std::vector<ChargingSchedulePeriod>
create_charging_schedule_periods_with_phases(int32_t start_period, int32_t numberPhases, int32_t phaseToUse);
ChargingProfileCriterion
create_charging_profile_criteria(std::optional<std::vector<ocpp::CiString<20>>> sources = std::nullopt,
                                 std::optional<std::vector<int32_t>> ids = std::nullopt,
                                 std::optional<ChargingProfilePurposeEnum> purpose = std::nullopt,
                                 std::optional<int32_t> stack_level = std::nullopt);
GetChargingProfilesRequest create_get_charging_profile_request(int32_t request_id, ChargingProfileCriterion criteria,
                                                               std::optional<int32_t> evse_id = std::nullopt);
ClearChargingProfileRequest
create_clear_charging_profile_request(std::optional<int32_t> id = std::nullopt,
                                      std::optional<ClearChargingProfile> criteria = std::nullopt);
ClearChargingProfile create_clear_charging_profile(std::optional<int32_t> evse_id = std::nullopt,
                                                   std::optional<ChargingProfilePurposeEnum> purpose = std::nullopt,
                                                   std::optional<int32_t> stack_level = std::nullopt);
namespace SmartChargingTestUtils {
std::vector<ChargingProfile> get_charging_profiles_from_directory(const std::string& path);
ChargingProfile get_charging_profile_from_path(const std::string& path);
ChargingProfile get_charging_profile_from_file(const std::string& filename);
std::vector<ChargingProfile> get_charging_profiles_from_file(const std::string& filename);
std::vector<ChargingProfile> get_baseline_profile_vector();
std::string to_string(std::vector<ChargingProfile>& profiles);
bool validate_profile_result(const std::vector<period_entry_t>& result);
} // namespace SmartChargingTestUtils

class TestSmartCharging : public SmartCharging {
public:
    using SmartCharging::add_profile;
    using SmartCharging::calculate_composite_schedule;
    using SmartCharging::clear_profiles;
    using SmartCharging::get_reported_profiles;
    using SmartCharging::get_valid_profiles;
    using SmartCharging::SmartCharging;
    using SmartCharging::validate_charging_station_max_profile;
    using SmartCharging::validate_evse_exists;
    using SmartCharging::validate_profile_schedules;
    using SmartCharging::validate_tx_default_profile;
    using SmartCharging::validate_tx_profile;
    using SmartCharging::verify_no_conflicting_external_constraints_id;
};

class CompositeScheduleTestFixtureV2 : public DatabaseTestingUtils {
protected:
    void SetUp() override;
    void TearDown() override;
    void load_charging_profiles_for_evse(const std::filesystem::path& path, int32_t evse_id);
    std::unique_ptr<TestSmartCharging> create_smart_charging_handler();

public:
    CompositeScheduleTestFixtureV2();
    void reconfigure_for_nr_of_evses(int32_t nr_of_evses);

    std::unique_ptr<EvseManagerFake> evse_manager;
    DeviceModelTestHelper device_model_test_helper;
    MockMessageDispatcher mock_dispatcher;
    DeviceModel* device_model;
    ::testing::NiceMock<ConnectivityManagerMock> connectivity_manager;
    ocpp::EvseSecurityMock evse_security;
    ComponentStateManagerMock component_state_manager;
    std::unique_ptr<FunctionalBlockContext> functional_block_context;
    std::unique_ptr<DatabaseHandlerFake> database_handler;
    MockFunction<void()> set_charging_profiles_callback_mock;
    std::unique_ptr<TestSmartCharging> handler;
    boost::uuids::random_generator uuid_generator;
};

} // namespace ocpp::v2
