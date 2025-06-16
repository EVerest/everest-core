// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include "generated/types/evse_manager.hpp"
#include <gtest/gtest.h>

#include <Charger.hpp>
#include <memory>

namespace {
using namespace module;
using namespace types::evse_manager;

// ----------------------------------------------------------------------------
// test classes

// class that provides access to internal state from the Charger class
struct ChargerDerived : public Charger {
    using Charger::Charger;
    using Charger::get_enable_disable_source_table;
    using Charger::get_shared_context;

    // updated when a non-zero connector is used to enable_disable()
    constexpr const auto& connector_enabled() {
        return get_shared_context().connector_enabled;
    }

    constexpr const auto& current_state() {
        return get_shared_context().current_state;
    }

    constexpr void current_state(EvseState state) {
        get_shared_context().current_state = state;
    }
};

// class that creates a consistent starting state for tests
struct ChargerTest : public testing::Test {
    // charger requirements
    std::unique_ptr<IECStateMachine> charger_bsp;
    std::unique_ptr<ErrorHandling> charger_error_handling;
    std::vector<std::unique_ptr<powermeterIntf>> charger_powermeter_billing;
    std::unique_ptr<PersistentStore> charger_store;

    // error handling requirements
    std::unique_ptr<evse_board_supportIntf> error_handler_bsp;
    std::vector<std::unique_ptr<ISO15118_chargerIntf>> error_handler_hlc;
    std::vector<std::unique_ptr<connector_lockIntf>> error_handler_connector_lock;
    std::vector<std::unique_ptr<ac_rcdIntf>> error_handler_ac_rcd;
    std::unique_ptr<evse_managerImplBase> error_handler_evse;
    std::vector<std::unique_ptr<isolation_monitorIntf>> error_handler_imd;
    std::vector<std::unique_ptr<power_supply_DCIntf>> error_handler_powersupply;
    std::vector<std::unique_ptr<powermeterIntf>> error_handler_powermeter;
    std::vector<std::unique_ptr<over_voltage_monitorIntf>> error_handler_over_voltage_monitor;

    std::unique_ptr<ChargerDerived> charger;

    ChargerTest() :
        charger_error_handling(std::make_unique<ErrorHandling>(
            error_handler_bsp, error_handler_hlc, error_handler_connector_lock, error_handler_ac_rcd,
            error_handler_evse, error_handler_imd, error_handler_powersupply, error_handler_powermeter,
            error_handler_over_voltage_monitor)) {
    }

    void SetUp() override {
        reset_last_event();
        charger = std::make_unique<ChargerDerived>(
            charger_bsp, charger_error_handling, charger_powermeter_billing, charger_store,
            types::evse_board_support::Connector_type::IEC62196Type2Socket, "EVSETEST");
        charger->signal_simple_event.connect(&ChargerTest::session_event, this);
    }

    void TearDown() override {
        charger.reset(nullptr);
    }

    void session_event(SessionEventEnum event) {
        last_event = event;
    }

    static constexpr SessionEventEnum default_event{SessionEventEnum::SessionFinished};
    static constexpr EnableDisableSource default_source{Enable_source::Unspecified, Enable_state::Unassigned, 10000};

    SessionEventEnum last_event{default_event};

    constexpr void reset_last_event() {
        last_event = default_event;
    }
};

// ----------------------------------------------------------------------------
// tests for enable_disable()
// interesting variables:
// - enable_disable_source_table        (not directly available)
// - shared_context.connector_enabled - charger->connector_enabled()
// - shared_context.current_state     - charger->current_state()
// - signal_simple_event              - last_event

TEST_F(ChargerTest, EnableDisableSourceInit) {
    // check the default values on startup
    // this is the starting point for all tests
    const auto last_source = charger->get_last_enable_disable_source();
    EXPECT_EQ(last_source, default_source);
    EXPECT_EQ(last_event, default_event);
    EXPECT_TRUE(charger->connector_enabled());
    EXPECT_EQ(charger->current_state(), Charger::EvseState::Idle);
}

TEST_F(ChargerTest, EnableDisableSourceInitPlusStateEnabled) {
    charger->current_state(Charger::EvseState::Idle);

    // check the default values on startup
    // this is the starting point for all tests
    auto last_source = charger->get_last_enable_disable_source();
    EXPECT_EQ(last_source, default_source);
    EXPECT_EQ(last_event, default_event);
    EXPECT_TRUE(charger->connector_enabled());
    EXPECT_EQ(charger->current_state(), Charger::EvseState::Idle);

    reset_last_event();
    constexpr EnableDisableSource enable_default{Enable_source::Unspecified, Enable_state::Enable, 10000};
    charger->enable_disable_initial_state_publish();
    last_source = charger->get_last_enable_disable_source();
    EXPECT_EQ(last_source, enable_default);
    EXPECT_EQ(last_event, SessionEventEnum::Enabled);
    EXPECT_TRUE(charger->connector_enabled());
    EXPECT_EQ(charger->current_state(), Charger::EvseState::Idle);

    // already enabled so no event
    reset_last_event();
    constexpr EnableDisableSource enable_source{Enable_source::CSMS, Enable_state::Enable, 100};
    EXPECT_TRUE(charger->enable_disable(0, enable_source));
    last_source = charger->get_last_enable_disable_source();
    EXPECT_EQ(last_source, enable_source);
    EXPECT_EQ(last_event, default_event);
}

TEST_F(ChargerTest, EnableDisableSourceInitPlusStateDisabled) {
    charger->current_state(Charger::EvseState::Disabled);

    // check the default values on startup
    // this is the starting point for all tests
    auto last_source = charger->get_last_enable_disable_source();
    EXPECT_EQ(last_source, default_source);
    EXPECT_EQ(last_event, default_event);
    EXPECT_TRUE(charger->connector_enabled());
    EXPECT_EQ(charger->current_state(), Charger::EvseState::Disabled);

    reset_last_event();
    constexpr EnableDisableSource disable_default{Enable_source::Unspecified, Enable_state::Disable, 10000};
    charger->enable_disable_initial_state_publish();
    last_source = charger->get_last_enable_disable_source();
    EXPECT_EQ(last_source, disable_default);
    EXPECT_EQ(last_event, SessionEventEnum::Disabled);
    EXPECT_TRUE(charger->connector_enabled());
    EXPECT_EQ(charger->current_state(), Charger::EvseState::Disabled);

    // already disabled so no event
    reset_last_event();
    constexpr EnableDisableSource disable_source{Enable_source::CSMS, Enable_state::Disable, 100};
    EXPECT_FALSE(charger->enable_disable(0, disable_source));
    last_source = charger->get_last_enable_disable_source();
    EXPECT_EQ(last_source, disable_source);
    EXPECT_EQ(last_event, default_event);
}

TEST_F(ChargerTest, EnableDisableSourceConnectorEnabled0) {
    // connector_enabled must only change when a non-zero connector ID is used
    ASSERT_TRUE(charger->connector_enabled());

    constexpr EnableDisableSource enable_source{Enable_source::CSMS, Enable_state::Enable, 100};
    constexpr EnableDisableSource disable_source{Enable_source::CSMS, Enable_state::Disable, 100};

    // test with connector ID 0: connector_enabled must not change
    EXPECT_FALSE(charger->enable_disable(0, disable_source));
    EXPECT_TRUE(charger->connector_enabled());
    EXPECT_EQ(charger->current_state(), Charger::EvseState::Disabled);
    EXPECT_TRUE(charger->enable_disable(0, enable_source));
    EXPECT_TRUE(charger->connector_enabled());
    // tricky case, connector_enabled is true but the change was to connector 0
    // this is what the original code does ...
    EXPECT_EQ(charger->current_state(), Charger::EvseState::Idle);

    // force connector_enabled false
    EXPECT_FALSE(charger->enable_disable(1, disable_source));
    EXPECT_FALSE(charger->connector_enabled());

    // test with connector ID 0: connector_enabled must not change
    EXPECT_FALSE(charger->enable_disable(0, disable_source));
    EXPECT_FALSE(charger->connector_enabled());
    EXPECT_EQ(charger->current_state(), Charger::EvseState::Disabled);
    EXPECT_TRUE(charger->enable_disable(0, enable_source));
    EXPECT_FALSE(charger->connector_enabled());
    // enable on connector 0 does not change state to Idle
    EXPECT_EQ(charger->current_state(), Charger::EvseState::Disabled);
}

TEST_F(ChargerTest, EnableDisableSourceConnectorEnabled1) {
    // connector_enabled must only change when a non-zero connector ID is used
    ASSERT_TRUE(charger->connector_enabled());

    constexpr EnableDisableSource enable_source{Enable_source::CSMS, Enable_state::Enable, 100};
    constexpr EnableDisableSource disable_source{Enable_source::CSMS, Enable_state::Disable, 100};

    // test with connector ID 1: connector_enabled must change
    EXPECT_FALSE(charger->enable_disable(1, disable_source));
    EXPECT_FALSE(charger->connector_enabled());
    EXPECT_EQ(charger->current_state(), Charger::EvseState::Disabled);
    EXPECT_TRUE(charger->enable_disable(1, enable_source));
    EXPECT_TRUE(charger->connector_enabled());
    EXPECT_EQ(charger->current_state(), Charger::EvseState::Idle);
    EXPECT_FALSE(charger->enable_disable(1, disable_source));
    EXPECT_FALSE(charger->connector_enabled());
    EXPECT_EQ(charger->current_state(), Charger::EvseState::Disabled);
}

constexpr EnableDisableSource enable_source_10{Enable_source::CSMS, Enable_state::Enable, 10};
constexpr EnableDisableSource enable_source_100{Enable_source::CSMS, Enable_state::Enable, 100};
constexpr EnableDisableSource enable_source_1000{Enable_source::CSMS, Enable_state::Enable, 1000};
constexpr EnableDisableSource disable_source_10{Enable_source::CSMS, Enable_state::Disable, 10};
constexpr EnableDisableSource disable_source_100{Enable_source::CSMS, Enable_state::Disable, 100};
constexpr EnableDisableSource disable_source_1000{Enable_source::CSMS, Enable_state::Disable, 1000};
constexpr EnableDisableSource unassigned_source_10{Enable_source::CSMS, Enable_state::Unassigned, 10};
constexpr EnableDisableSource unassigned_source_100{Enable_source::CSMS, Enable_state::Unassigned, 100};
constexpr EnableDisableSource unassigned_source_1000{Enable_source::CSMS, Enable_state::Unassigned, 1000};

TEST_F(ChargerTest, EnableDisableTableSingleSource) {
    // enable_disable settings are added to a table
    // parse_enable_disable_source_table() processes the table and updates
    // active_enable_disable_source which is available via get_last_enable_disable_source()
    // enable_disable() updates the table and calls parse_enable_disable_source_table()

    // EnableDisableTable
    // Evaluation will be done based on priorities. 0 is the highest priority, 10000 the lowest
    // If all sources are unassigned, the connector is enabled
    // If two sources have the same priority, "disabled" has priority over "enabled"

    const int connector_id = 1; // use a consistent value

    const auto& enable_disable_source_table = charger->get_enable_disable_source_table();

    EXPECT_TRUE(enable_disable_source_table.empty());
    EXPECT_EQ(enable_disable_source_table.size(), 0);
    auto last_source = charger->get_last_enable_disable_source();
    EXPECT_EQ(last_source, default_source);

    // check source change of state

    EXPECT_TRUE(charger->enable_disable(connector_id, enable_source_100));
    EXPECT_EQ(enable_disable_source_table.size(), 1);
    last_source = charger->get_last_enable_disable_source();
    EXPECT_EQ(last_source, enable_source_100);

    EXPECT_FALSE(charger->enable_disable(connector_id, disable_source_100));
    EXPECT_EQ(enable_disable_source_table.size(), 1);
    last_source = charger->get_last_enable_disable_source();
    EXPECT_EQ(last_source, disable_source_100);

    EXPECT_TRUE(charger->enable_disable(connector_id, unassigned_source_100));
    EXPECT_EQ(enable_disable_source_table.size(), 1);
    last_source = charger->get_last_enable_disable_source();
    // unexpected
    EXPECT_EQ(last_source, default_source);

    // check source change of priority

    EXPECT_TRUE(charger->enable_disable(connector_id, enable_source_1000));
    EXPECT_EQ(enable_disable_source_table.size(), 1);
    last_source = charger->get_last_enable_disable_source();
    EXPECT_EQ(last_source, enable_source_1000);

    EXPECT_TRUE(charger->enable_disable(connector_id, enable_source_100));
    EXPECT_EQ(enable_disable_source_table.size(), 1);
    last_source = charger->get_last_enable_disable_source();
    EXPECT_EQ(last_source, enable_source_100);

    EXPECT_TRUE(charger->enable_disable(connector_id, enable_source_10));
    EXPECT_EQ(enable_disable_source_table.size(), 1);
    last_source = charger->get_last_enable_disable_source();
    EXPECT_EQ(last_source, enable_source_10);

    EXPECT_FALSE(charger->enable_disable(connector_id, disable_source_1000));
    EXPECT_EQ(enable_disable_source_table.size(), 1);
    last_source = charger->get_last_enable_disable_source();
    EXPECT_EQ(last_source, disable_source_1000);

    EXPECT_FALSE(charger->enable_disable(connector_id, disable_source_100));
    EXPECT_EQ(enable_disable_source_table.size(), 1);
    last_source = charger->get_last_enable_disable_source();
    EXPECT_EQ(last_source, disable_source_100);

    EXPECT_FALSE(charger->enable_disable(connector_id, disable_source_10));
    EXPECT_EQ(enable_disable_source_table.size(), 1);
    last_source = charger->get_last_enable_disable_source();
    EXPECT_EQ(last_source, disable_source_10);

    // last_source is always the default since Unassigned
    // entries are ignored

    EXPECT_TRUE(charger->enable_disable(connector_id, unassigned_source_1000));
    EXPECT_EQ(enable_disable_source_table.size(), 1);
    last_source = charger->get_last_enable_disable_source();
    EXPECT_EQ(last_source, default_source);

    EXPECT_TRUE(charger->enable_disable(connector_id, unassigned_source_100));
    EXPECT_EQ(enable_disable_source_table.size(), 1);
    last_source = charger->get_last_enable_disable_source();
    EXPECT_EQ(last_source, default_source);

    EXPECT_TRUE(charger->enable_disable(connector_id, unassigned_source_10));
    EXPECT_EQ(enable_disable_source_table.size(), 1);
    last_source = charger->get_last_enable_disable_source();
    EXPECT_EQ(last_source, default_source);
}

TEST_F(ChargerTest, EnableDisableTableSourceMulti) {
    // enable_disable settings are added to a table
    // parse_enable_disable_source_table() processes the table and updates
    // active_enable_disable_source which is available via get_last_enable_disable_source()
    // enable_disable() updates the table and calls parse_enable_disable_source_table()

    // EnableDisableTable
    // Evaluation will be done based on priorities. 0 is the highest priority, 10000 the lowest
    // If all sources are unassigned, the connector is enabled
    // If two sources have the same priority, "disabled" has priority over "enabled"

    const int connector_id = 1; // use a consistent value

    const auto& enable_disable_source_table = charger->get_enable_disable_source_table();

    EXPECT_TRUE(enable_disable_source_table.empty());
    EXPECT_EQ(enable_disable_source_table.size(), 0);
    auto last_source = charger->get_last_enable_disable_source();
    EXPECT_EQ(last_source, default_source);

    // check multiple sources are added to the table

    EXPECT_TRUE(charger->enable_disable(connector_id, enable_source_10));
    EXPECT_EQ(enable_disable_source_table.size(), 1);
    last_source = charger->get_last_enable_disable_source();
    EXPECT_EQ(last_source, enable_source_10);

    EXPECT_TRUE(charger->enable_disable(connector_id, {Enable_source::FirmwareUpdate, Enable_state::Enable, 11}));
    EXPECT_EQ(enable_disable_source_table.size(), 2);
    last_source = charger->get_last_enable_disable_source();
    EXPECT_EQ(last_source, enable_source_10);

    EXPECT_TRUE(charger->enable_disable(connector_id, {Enable_source::LocalAPI, Enable_state::Enable, 11}));
    EXPECT_EQ(enable_disable_source_table.size(), 3);
    last_source = charger->get_last_enable_disable_source();
    EXPECT_EQ(last_source, enable_source_10);

    EXPECT_TRUE(charger->enable_disable(connector_id, {Enable_source::LocalKeyLock, Enable_state::Enable, 11}));
    EXPECT_EQ(enable_disable_source_table.size(), 4);
    last_source = charger->get_last_enable_disable_source();
    EXPECT_EQ(last_source, enable_source_10);

    EXPECT_TRUE(charger->enable_disable(connector_id, {Enable_source::MobileApp, Enable_state::Enable, 11}));
    EXPECT_EQ(enable_disable_source_table.size(), 5);
    last_source = charger->get_last_enable_disable_source();
    EXPECT_EQ(last_source, enable_source_10);

    EXPECT_TRUE(charger->enable_disable(connector_id, {Enable_source::RemoteKeyLock, Enable_state::Enable, 11}));
    EXPECT_EQ(enable_disable_source_table.size(), 6);
    last_source = charger->get_last_enable_disable_source();
    EXPECT_EQ(last_source, enable_source_10);

    const EnableDisableSource disable_source{Enable_source::ServiceTechnician, Enable_state::Disable, 11};
    EXPECT_TRUE(charger->enable_disable(connector_id, disable_source));
    EXPECT_EQ(enable_disable_source_table.size(), 7);
    last_source = charger->get_last_enable_disable_source();
    EXPECT_EQ(last_source, enable_source_10);

    EXPECT_TRUE(charger->enable_disable(connector_id, {Enable_source::Unspecified, Enable_state::Enable, 11}));
    EXPECT_EQ(enable_disable_source_table.size(), 8);
    last_source = charger->get_last_enable_disable_source();
    EXPECT_EQ(last_source, enable_source_10);

    // update CSMS - next highest expected - disable_source
    EXPECT_FALSE(charger->enable_disable(connector_id, enable_source_100));
    EXPECT_EQ(enable_disable_source_table.size(), 8);
    last_source = charger->get_last_enable_disable_source();
    EXPECT_EQ(last_source, disable_source);
}

TEST_F(ChargerTest, EnableDisableTablePriority) {
    // enable_disable settings are added to a table
    // parse_enable_disable_source_table() processes the table and updates
    // active_enable_disable_source which is available via get_last_enable_disable_source()
    // enable_disable() updates the table and calls parse_enable_disable_source_table()

    // EnableDisableTable
    // Evaluation will be done based on priorities. 0 is the highest priority, 10000 the lowest
    // If all sources are unassigned, the connector is enabled
    // If two sources have the same priority, "disabled" has priority over "enabled"

    const int connector_id = 1; // use a consistent value

    const auto& enable_disable_source_table = charger->get_enable_disable_source_table();

    EXPECT_TRUE(enable_disable_source_table.empty());
    EXPECT_EQ(enable_disable_source_table.size(), 0);
    auto last_source = charger->get_last_enable_disable_source();
    EXPECT_EQ(last_source, default_source);

    // check priority is being respected (via different sources)
    // base priority is 10 - higher values must be ignored

    EXPECT_FALSE(charger->enable_disable(connector_id, disable_source_10));
    EXPECT_EQ(enable_disable_source_table.size(), 1);
    last_source = charger->get_last_enable_disable_source();
    EXPECT_EQ(last_source, disable_source_10);

    // ignored because disable has higher priority over enabled
    const EnableDisableSource next_expected{Enable_source::FirmwareUpdate, Enable_state::Enable, 10};
    EXPECT_FALSE(charger->enable_disable(connector_id, next_expected));
    EXPECT_EQ(enable_disable_source_table.size(), 2);
    last_source = charger->get_last_enable_disable_source();
    EXPECT_EQ(last_source, disable_source_10);

    // ignored because unassigned is ignored
    EXPECT_FALSE(charger->enable_disable(connector_id, {Enable_source::LocalAPI, Enable_state::Unassigned, 9}));
    EXPECT_EQ(enable_disable_source_table.size(), 3);
    last_source = charger->get_last_enable_disable_source();
    EXPECT_EQ(last_source, disable_source_10);

    // ignored because priority is lower
    EXPECT_FALSE(charger->enable_disable(connector_id, {Enable_source::LocalAPI, Enable_state::Disable, 12}));
    EXPECT_EQ(enable_disable_source_table.size(), 3);
    last_source = charger->get_last_enable_disable_source();
    EXPECT_EQ(last_source, disable_source_10);

    // ignored because priority is even lower
    EXPECT_FALSE(charger->enable_disable(connector_id, {Enable_source::LocalAPI, Enable_state::Enable, 200}));
    EXPECT_EQ(enable_disable_source_table.size(), 3);
    last_source = charger->get_last_enable_disable_source();
    EXPECT_EQ(last_source, disable_source_10);

    // overrides CSMS so it is ignored - expected is
    EXPECT_TRUE(charger->enable_disable(connector_id, unassigned_source_10));
    EXPECT_EQ(enable_disable_source_table.size(), 3);
    last_source = charger->get_last_enable_disable_source();
    EXPECT_EQ(last_source, next_expected);

    // disabled takes priority
    const EnableDisableSource last_expected{Enable_source::LocalAPI, Enable_state::Disable, 10};
    EXPECT_FALSE(charger->enable_disable(connector_id, last_expected));
    EXPECT_EQ(enable_disable_source_table.size(), 3);
    last_source = charger->get_last_enable_disable_source();
    EXPECT_EQ(last_source, last_expected);

    // higher priority
    const EnableDisableSource higher_expected{Enable_source::FirmwareUpdate, Enable_state::Enable, 5};
    EXPECT_TRUE(charger->enable_disable(connector_id, higher_expected));
    EXPECT_EQ(enable_disable_source_table.size(), 3);
    last_source = charger->get_last_enable_disable_source();
    EXPECT_EQ(last_source, higher_expected);
}

TEST_F(ChargerTest, EnableDisableSourceEnable0) {
    constexpr EnableDisableSource sourceA{Enable_source::CSMS, Enable_state::Unassigned, 100};
    constexpr EnableDisableSource sourceB{Enable_source::FirmwareUpdate, Enable_state::Enable, 90};

    // enable from default state

    EXPECT_TRUE(charger->enable_disable(0, sourceA));
    auto last_source = charger->get_last_enable_disable_source();
    // Unassigned updates do not change the result from get_last_enable_disable_source()
    EXPECT_EQ(last_source, default_source);
    // There should not be an update event - default state is enabled
    EXPECT_EQ(last_event, default_event);
    EXPECT_EQ(charger->current_state(), Charger::EvseState::Idle);
    EXPECT_TRUE(charger->connector_enabled()); // default state

    reset_last_event();
    EXPECT_TRUE(charger->enable_disable(0, sourceB));
    last_source = charger->get_last_enable_disable_source();
    // Unassigned updates do not change the result from get_last_enable_disable_source()
    EXPECT_EQ(last_source, sourceB);
    // There should not be an update event - enabled -> enabled
    EXPECT_EQ(last_event, default_event);
    EXPECT_EQ(charger->current_state(), Charger::EvseState::Idle);
    EXPECT_TRUE(charger->connector_enabled()); // default state
}

TEST_F(ChargerTest, EnableDisableSourceEnable1A) {
    constexpr EnableDisableSource sourceA{Enable_source::CSMS, Enable_state::Disable, 100};
    constexpr EnableDisableSource sourceB{Enable_source::FirmwareUpdate, Enable_state::Enable, 90};

    // force a complete disable state
    EXPECT_FALSE(charger->enable_disable(1, sourceA));
    auto last_source = charger->get_last_enable_disable_source();
    // Unassigned updates do not change the result from get_last_enable_disable_source()
    EXPECT_EQ(last_source, sourceA);
    EXPECT_EQ(last_event, SessionEventEnum::Disabled);
    EXPECT_EQ(charger->current_state(), Charger::EvseState::Disabled);
    EXPECT_FALSE(charger->connector_enabled());

    // enable on connector 1
    reset_last_event();
    EXPECT_TRUE(charger->enable_disable(1, sourceB));
    last_source = charger->get_last_enable_disable_source();
    EXPECT_EQ(last_source, sourceB);
    EXPECT_EQ(last_event, SessionEventEnum::Enabled);
    EXPECT_EQ(charger->current_state(), Charger::EvseState::Idle);
    EXPECT_TRUE(charger->connector_enabled());
}

TEST_F(ChargerTest, EnableDisableSourceEnable1B) {
    constexpr EnableDisableSource sourceA{Enable_source::CSMS, Enable_state::Disable, 100};
    constexpr EnableDisableSource sourceB{Enable_source::FirmwareUpdate, Enable_state::Enable, 90};

    // force a complete disable state
    EXPECT_FALSE(charger->enable_disable(1, sourceA));
    auto last_source = charger->get_last_enable_disable_source();
    // Unassigned updates do not change the result from get_last_enable_disable_source()
    EXPECT_EQ(last_source, sourceA);
    EXPECT_EQ(last_event, SessionEventEnum::Disabled);
    EXPECT_EQ(charger->current_state(), Charger::EvseState::Disabled);
    EXPECT_FALSE(charger->connector_enabled());

    // enable on connector 0
    reset_last_event();
    EXPECT_TRUE(charger->enable_disable(0, sourceB));
    last_source = charger->get_last_enable_disable_source();
    EXPECT_EQ(last_source, sourceB);
    EXPECT_EQ(last_event, SessionEventEnum::Enabled);
    EXPECT_EQ(charger->current_state(), Charger::EvseState::Disabled);
    EXPECT_FALSE(charger->connector_enabled());

    // enable on connector 1
    reset_last_event();
    EXPECT_TRUE(charger->enable_disable(1, sourceB));
    last_source = charger->get_last_enable_disable_source();
    EXPECT_EQ(last_source, sourceB);
    // enable -> enable so no event
    EXPECT_EQ(last_event, default_event);
    EXPECT_EQ(charger->current_state(), Charger::EvseState::Idle);
    EXPECT_TRUE(charger->connector_enabled());
}

TEST_F(ChargerTest, EnableDisableSourceDisable0) {
    constexpr EnableDisableSource sourceA{Enable_source::CSMS, Enable_state::Unassigned, 100};
    constexpr EnableDisableSource sourceB{Enable_source::FirmwareUpdate, Enable_state::Disable, 100};

    // disable from default state

    EXPECT_TRUE(charger->enable_disable(0, sourceA));
    auto last_source = charger->get_last_enable_disable_source();
    // Unassigned updates do not change the result from get_last_enable_disable_source()
    EXPECT_EQ(last_source, default_source);
    // This is possibly an error
    EXPECT_EQ(last_event, default_event);
    EXPECT_EQ(charger->current_state(), Charger::EvseState::Idle);

    reset_last_event();
    EXPECT_FALSE(charger->enable_disable(0, sourceB));
    last_source = charger->get_last_enable_disable_source();
    EXPECT_EQ(last_source, sourceB);
    EXPECT_EQ(last_event, SessionEventEnum::Disabled);
    EXPECT_EQ(charger->current_state(), Charger::EvseState::Disabled);
}

TEST_F(ChargerTest, EnableDisableSourceDisable1) {
    constexpr EnableDisableSource sourceA{Enable_source::CSMS, Enable_state::Unassigned, 100};
    constexpr EnableDisableSource sourceB{Enable_source::FirmwareUpdate, Enable_state::Disable, 100};

    // disable from default state

    EXPECT_TRUE(charger->enable_disable(0, sourceA));
    auto last_source = charger->get_last_enable_disable_source();
    // Unassigned updates do not change the result from get_last_enable_disable_source()
    EXPECT_EQ(last_source, default_source);
    // This is possibly an error
    EXPECT_EQ(last_event, default_event);
    EXPECT_EQ(charger->current_state(), Charger::EvseState::Idle);

    reset_last_event();
    EXPECT_FALSE(charger->enable_disable(0, sourceB));
    last_source = charger->get_last_enable_disable_source();
    EXPECT_EQ(last_source, sourceB);
    EXPECT_EQ(last_event, SessionEventEnum::Disabled);
    EXPECT_EQ(charger->current_state(), Charger::EvseState::Disabled);

    reset_last_event();
    EXPECT_FALSE(charger->enable_disable(1, sourceB));
    last_source = charger->get_last_enable_disable_source();
    EXPECT_EQ(last_source, sourceB);
    // disable -> disable so no event
    EXPECT_EQ(last_event, default_event);
    EXPECT_EQ(charger->current_state(), Charger::EvseState::Disabled);
}

TEST_F(ChargerTest, EnableDisableSourceDisableEnable) {
    constexpr EnableDisableSource sourceA{Enable_source::CSMS, Enable_state::Unassigned, 100};
    constexpr EnableDisableSource sourceB{Enable_source::FirmwareUpdate, Enable_state::Disable, 100};
    constexpr EnableDisableSource sourceC{Enable_source::LocalAPI, Enable_state::Enable, 90};

    // default state
    EXPECT_TRUE(charger->enable_disable(0, sourceA));
    auto last_source = charger->get_last_enable_disable_source();
    // Unassigned updates do not change the result from get_last_enable_disable_source()
    EXPECT_EQ(last_source, default_source);
    // This is possibly an error
    EXPECT_EQ(last_event, default_event);
    EXPECT_EQ(charger->current_state(), Charger::EvseState::Idle);

    // disable 0
    reset_last_event();
    EXPECT_FALSE(charger->enable_disable(0, sourceB));
    last_source = charger->get_last_enable_disable_source();
    EXPECT_EQ(last_source, sourceB);
    EXPECT_EQ(last_event, SessionEventEnum::Disabled);
    EXPECT_EQ(charger->current_state(), Charger::EvseState::Disabled);

    // disable 1
    reset_last_event();
    EXPECT_FALSE(charger->enable_disable(1, sourceB));
    last_source = charger->get_last_enable_disable_source();
    EXPECT_EQ(last_source, sourceB);
    // no event: disable -> disable
    EXPECT_EQ(last_event, default_event);
    EXPECT_EQ(charger->current_state(), Charger::EvseState::Disabled);

    // enable 0
    reset_last_event();
    EXPECT_TRUE(charger->enable_disable(0, sourceC));
    last_source = charger->get_last_enable_disable_source();
    EXPECT_EQ(last_source, sourceC);
    EXPECT_EQ(last_event, SessionEventEnum::Enabled);
    // remains disabled because the enable was on connector 0
    EXPECT_EQ(charger->current_state(), Charger::EvseState::Disabled);

    // enable 1
    reset_last_event();
    EXPECT_TRUE(charger->enable_disable(1, sourceC));
    last_source = charger->get_last_enable_disable_source();
    EXPECT_EQ(last_source, sourceC);
    // enable->enable hence no event
    EXPECT_EQ(last_event, default_event);
    // updated because not connector 0
    EXPECT_EQ(charger->current_state(), Charger::EvseState::Idle);
}

} // namespace

// ----------------------------------------------------------------------------
// the following code is stubs to enable testing Charger in isolation.
// If these stubs are needed elsewhere then they could go into separate files

// ----------------------------------------------------------------------------
// backtrace stub
namespace Everest {
void signal_handler(int signo) {
}
void install_backtrace_handler() {
}
void request_backtrace(pthread_t id) {
}
} // namespace Everest

namespace module {

// ----------------------------------------------------------------------------
// IECStateMachine stub
IECStateMachine::IECStateMachine(const std::unique_ptr<evse_managerImplBase>& p_evse_,
                                 const std::unique_ptr<evse_board_supportIntf>& r_bsp_,
                                 bool lock_connector_in_state_b_) :
    p_evse(p_evse_), r_bsp(r_bsp_) {
}
void IECStateMachine::process_bsp_event(const types::board_support_common::BspEvent bsp_event) {
}
void IECStateMachine::allow_power_on(bool value, types::evse_board_support::Reason reason) {
}

double IECStateMachine::read_pp_ampacity() {
    return 0.0;
}
void IECStateMachine::evse_replug(int ms) {
}
void IECStateMachine::switch_three_phases_while_charging(bool n) {
}
void IECStateMachine::setup(bool has_ventilation) {
}

void IECStateMachine::set_overcurrent_limit(double amps) {
}

void IECStateMachine::set_pwm(double value) {
}
void IECStateMachine::set_pwm_off() {
}
void IECStateMachine::set_pwm_F() {
}

void IECStateMachine::enable(bool en) {
}

void IECStateMachine::connector_force_unlock() {
}

const std::string cpevent_to_string(CPEvent e) {
    switch (e) {
    case CPEvent::CarPluggedIn:
        return "CarPluggedIn";
    case CPEvent::CarRequestedPower:
        return "CarRequestedPower";
    case CPEvent::PowerOn:
        return "PowerOn";
    case CPEvent::PowerOff:
        return "PowerOff";
    case CPEvent::CarRequestedStopPower:
        return "CarRequestedStopPower";
    case CPEvent::CarUnplugged:
        return "CarUnplugged";
    case CPEvent::EFtoBCD:
        return "EFtoBCD";
    case CPEvent::BCDtoEF:
        return "BCDtoEF";
    case CPEvent::BCDtoE:
        return "BCDtoE";
    case CPEvent::EvseReplugStarted:
        return "EvseReplugStarted";
    case CPEvent::EvseReplugFinished:
        return "EvseReplugFinished";
    }
    throw std::out_of_range("No known string conversion for provided enum of type CPEvent");
}

// ----------------------------------------------------------------------------
//  ErrorHandling stub
ErrorHandling::ErrorHandling(const std::unique_ptr<evse_board_supportIntf>& r_bsp,
                             const std::vector<std::unique_ptr<ISO15118_chargerIntf>>& r_hlc,
                             const std::vector<std::unique_ptr<connector_lockIntf>>& r_connector_lock,
                             const std::vector<std::unique_ptr<ac_rcdIntf>>& r_ac_rcd,
                             const std::unique_ptr<evse_managerImplBase>& _p_evse,
                             const std::vector<std::unique_ptr<isolation_monitorIntf>>& _r_imd,
                             const std::vector<std::unique_ptr<power_supply_DCIntf>>& _r_powersupply,
                             const std::vector<std::unique_ptr<powermeterIntf>>& _r_powermeter,
                             const std::vector<std::unique_ptr<over_voltage_monitorIntf>>& _r_over_voltage_monitor) :
    r_bsp(r_bsp),
    r_hlc(r_hlc),
    r_connector_lock(r_connector_lock),
    r_ac_rcd(r_ac_rcd),
    p_evse(p_evse),
    r_imd(_r_imd),
    r_powersupply(r_powersupply),
    r_powermeter(_r_powermeter),
    r_over_voltage_monitor(_r_over_voltage_monitor) {
}

void ErrorHandling::raise_overcurrent_error(const std::string& description) {
}
void ErrorHandling::clear_overcurrent_error() {
}

void ErrorHandling::raise_internal_error(const std::string& description) {
}
void ErrorHandling::clear_internal_error() {
}

void ErrorHandling::raise_authorization_timeout_error(const std::string& description) {
}
void ErrorHandling::clear_authorization_timeout_error() {
}

void ErrorHandling::raise_powermeter_transaction_start_failed_error(const std::string& description) {
}
void ErrorHandling::clear_powermeter_transaction_start_failed_error() {
}

// ----------------------------------------------------------------------------
// SessionLog stub
SessionLog::SessionLog() {
}
SessionLog::~SessionLog() {
}

void SessionLog::setPath(const std::string& path) {
}
void SessionLog::setMqtt(const std::function<void(nlohmann::json data)>& mqtt_provider) {
}
void SessionLog::enable() {
}
std::optional<std::string> SessionLog::startSession(const std::string& suffix_string) {
    return {};
}
void SessionLog::stopSession() {
}

void SessionLog::car(bool iso15118, const std::string& msg) {
}
void SessionLog::car(bool iso15118, const std::string& msg, const std::string& xml, const std::string& xml_hex,
                     const std::string& xml_base64, const std::string& json_str) {
}

void SessionLog::evse(bool iso15118, const std::string& msg) {
}
void SessionLog::evse(bool iso15118, const std::string& msg, const std::string& xml, const std::string& xml_hex,
                      const std::string& xml_base64, const std::string& json_str) {
}

void SessionLog::xmlOutput(bool e) {
}

void SessionLog::sys(const std::string& msg) {
}

SessionLog session_log;

// ----------------------------------------------------------------------------
// PersistentStore stub
PersistentStore::PersistentStore(const std::vector<std::unique_ptr<kvsIntf>>& r_store, const std::string module_id) :
    r_store(r_store) {
}

void PersistentStore::store_session(const std::string& session_uuid) {
}
void PersistentStore::clear_session() {
}
std::string PersistentStore::get_session() {
    return {};
}

} // namespace module
