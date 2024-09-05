// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

#include <comparators.hpp>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <ocpp/v201/component_state_manager.hpp>

namespace ocpp::v201 {

class DatabaseHandlerMock : public DatabaseHandler {
private:
    std::map<std::pair<int32_t, int32_t>, OperationalStatusEnum> data;

    void insert(int32_t evse_id, int32_t connector_id, OperationalStatusEnum status, bool replace) {
        if (replace || this->data.count(std::make_pair(evse_id, connector_id)) == 0) {
            this->data.insert_or_assign(std::make_pair(evse_id, connector_id), status);
        }
    }

    OperationalStatusEnum get(int32_t evse_id, int32_t connector_id) {
        if (this->data.count(std::make_pair(evse_id, connector_id)) == 0) {
            throw std::logic_error("Get: no data available");
        } else {
            return this->data.at(std::make_pair(evse_id, connector_id));
        }
    }

public:
    DatabaseHandlerMock() : DatabaseHandler(std::unique_ptr<common::DatabaseConnectionInterface>(), "/dev/null") {
    }

    virtual void insert_cs_availability(OperationalStatusEnum operational_status, bool replace) override {
        this->insert(0, 0, operational_status, replace);
    }
    virtual OperationalStatusEnum get_cs_availability() override {
        return this->get(0, 0);
    }

    virtual void insert_evse_availability(int32_t evse_id, OperationalStatusEnum operational_status,
                                          bool replace) override {

        this->insert(evse_id, 0, operational_status, replace);
    }
    virtual OperationalStatusEnum get_evse_availability(int32_t evse_id) override {
        return this->get(evse_id, 0);
    }

    virtual void insert_connector_availability(int32_t evse_id, int32_t connector_id,
                                               OperationalStatusEnum operational_status, bool replace) override {
        this->insert(evse_id, connector_id, operational_status, replace);
    }
    virtual OperationalStatusEnum get_connector_availability(int32_t evse_id, int32_t connector_id) override {
        return this->get(evse_id, connector_id);
    }
};

class MockCallbacks {
public:
    MOCK_METHOD(bool, connector_status_update, (int32_t, int32_t, std::string), ());
    MOCK_METHOD(void, cs_op_state_update, (std::string), ());
    MOCK_METHOD(void, evse_op_state_update, (int32_t, std::string), ());
    MOCK_METHOD(void, connector_op_state_update, (int32_t, int32_t, std::string), ());
};

class ComponentStateManagerTest : public ::testing::Test {
protected:
    void SetUp() override {
        testing::FLAGS_gmock_verbose = "error";
    }

    ComponentStateManager component_state_manager(std::shared_ptr<DatabaseHandler> database,
                                                  std::vector<uint32_t> connector_structure) {
        std::map<int32_t, int32_t> evse_connector_structure;
        for (int i = 0; i < connector_structure.size(); i++) {
            evse_connector_structure.insert_or_assign(i + 1, connector_structure[i]);
        }

        ComponentStateManager mgr(evse_connector_structure, database,
                                  [this](int32_t evse_id, int32_t connector_id, ConnectorStatusEnum status,
                                         bool initiated_by_trigger_message) {
                                      return this->callbacks.connector_status_update(
                                          evse_id, connector_id, conversions::connector_status_enum_to_string(status));
                                      return true;
                                  });
        mgr.set_cs_effective_availability_changed_callback([this](OperationalStatusEnum status) {
            this->callbacks.cs_op_state_update(conversions::operational_status_enum_to_string(status));
        });
        mgr.set_evse_effective_availability_changed_callback([this](int32_t evse_id, OperationalStatusEnum status) {
            this->callbacks.evse_op_state_update(evse_id, conversions::operational_status_enum_to_string(status));
        });
        mgr.set_connector_effective_availability_changed_callback(
            [this](int32_t evse_id, int32_t connector_id, OperationalStatusEnum status) {
                this->callbacks.connector_op_state_update(evse_id, connector_id,
                                                          conversions::operational_status_enum_to_string(status));
            });

        return mgr;
    }

    void TearDown() override {
    }

    MockCallbacks callbacks;
};

/// \brief Test that the ComponentStateManager can be constructed on an empty database
TEST_F(ComponentStateManagerTest, test_boot_empty_db) {
    // Prepare
    std::shared_ptr<DatabaseHandler> mock_database_123 = std::make_shared<DatabaseHandlerMock>();
    std::shared_ptr<DatabaseHandler> mock_database_1 = std::make_shared<DatabaseHandlerMock>();
    std::shared_ptr<DatabaseHandler> mock_database_1111 = std::make_shared<DatabaseHandlerMock>();

    // Act & Verify: No crash
    auto state_mgr_123 = this->component_state_manager(mock_database_123, {1, 2, 3});
    auto state_mgr_1 = this->component_state_manager(mock_database_1, {1});
    auto state_mgr_1111 = this->component_state_manager(mock_database_1111, {1, 1, 1, 1});
}

/// \brief Test that the ComponentStateManager correctly recovers persisted states on boot
TEST_F(ComponentStateManagerTest, test_boot_recover_persisted_states) {
    // Prepare
    std::shared_ptr<DatabaseHandler> mock_database = std::make_shared<DatabaseHandlerMock>();
    mock_database->insert_cs_availability(OperationalStatusEnum::Operative, false);
    mock_database->insert_evse_availability(1, OperationalStatusEnum::Inoperative, false);
    mock_database->insert_evse_availability(2, OperationalStatusEnum::Operative, false);
    mock_database->insert_connector_availability(1, 1, OperationalStatusEnum::Operative, false);
    mock_database->insert_connector_availability(2, 1, OperationalStatusEnum::Operative, false);
    mock_database->insert_connector_availability(2, 2, OperationalStatusEnum::Inoperative, false);

    // Act
    auto state_mgr = this->component_state_manager(mock_database, {1, 2});

    // Verify
    ASSERT_EQ(state_mgr.get_cs_individual_operational_status(), OperationalStatusEnum::Operative);
    ASSERT_EQ(state_mgr.get_evse_individual_operational_status(1), OperationalStatusEnum::Inoperative);
    ASSERT_EQ(state_mgr.get_evse_individual_operational_status(2), OperationalStatusEnum::Operative);
    ASSERT_EQ(state_mgr.get_connector_individual_operational_status(1, 1), OperationalStatusEnum::Operative);
    ASSERT_EQ(state_mgr.get_connector_individual_operational_status(2, 1), OperationalStatusEnum::Operative);
    ASSERT_EQ(state_mgr.get_connector_individual_operational_status(2, 2), OperationalStatusEnum::Inoperative);
}

/// \brief Test that the ComponentStateManager sanity-checks input EVSE and connector IDs
TEST_F(ComponentStateManagerTest, test_check_evse_and_connector_ids) {
    // Prepare
    std::shared_ptr<DatabaseHandler> mock_database = std::make_shared<DatabaseHandlerMock>();
    auto state_mgr = this->component_state_manager(mock_database, {1, 2});

    // Act & Verify
    ASSERT_THROW(state_mgr.get_evse_effective_operational_status(0), std::out_of_range);
    ASSERT_THROW(state_mgr.get_evse_effective_operational_status(-1), std::out_of_range);
    ASSERT_THROW(state_mgr.get_evse_effective_operational_status(3), std::out_of_range);
    ASSERT_THROW(state_mgr.get_connector_effective_operational_status(0, 1), std::out_of_range);
    ASSERT_THROW(state_mgr.get_connector_effective_operational_status(-1, 1), std::out_of_range);
    ASSERT_THROW(state_mgr.get_connector_effective_operational_status(3, 1), std::out_of_range);
    ASSERT_THROW(state_mgr.get_connector_effective_operational_status(1, -1), std::out_of_range);
    ASSERT_THROW(state_mgr.get_connector_effective_operational_status(1, 0), std::out_of_range);
    ASSERT_THROW(state_mgr.get_connector_effective_operational_status(1, 2), std::out_of_range);
    ASSERT_THROW(state_mgr.get_connector_effective_operational_status(2, -1), std::out_of_range);
    ASSERT_THROW(state_mgr.get_connector_effective_operational_status(2, 0), std::out_of_range);
    ASSERT_THROW(state_mgr.get_connector_effective_operational_status(2, 3), std::out_of_range);
}

/// \brief Test that the ComponentStateManager assumes missing states are Operative
TEST_F(ComponentStateManagerTest, test_boot_missing_states_are_operative) {
    // Prepare
    std::shared_ptr<DatabaseHandler> mock_database = std::make_shared<DatabaseHandlerMock>();
    mock_database->insert_evse_availability(1, OperationalStatusEnum::Inoperative, false);
    mock_database->insert_connector_availability(2, 2, OperationalStatusEnum::Inoperative, false);

    // Act
    auto state_mgr = this->component_state_manager(mock_database, {1, 2});

    // Verify
    // The missing states should be filled into the DB
    ASSERT_EQ(mock_database->get_cs_availability(), OperationalStatusEnum::Operative);
    ASSERT_EQ(mock_database->get_evse_availability(2), OperationalStatusEnum::Operative);
    ASSERT_EQ(mock_database->get_connector_availability(1, 1), OperationalStatusEnum::Operative);
    ASSERT_EQ(mock_database->get_connector_availability(2, 1), OperationalStatusEnum::Operative);
    // Individual state getters
    ASSERT_EQ(state_mgr.get_cs_individual_operational_status(), OperationalStatusEnum::Operative);
    ASSERT_EQ(state_mgr.get_evse_individual_operational_status(1), OperationalStatusEnum::Inoperative);
    ASSERT_EQ(state_mgr.get_evse_individual_operational_status(2), OperationalStatusEnum::Operative);
    ASSERT_EQ(state_mgr.get_connector_individual_operational_status(1, 1), OperationalStatusEnum::Operative);
    ASSERT_EQ(state_mgr.get_connector_individual_operational_status(2, 1), OperationalStatusEnum::Operative);
    ASSERT_EQ(state_mgr.get_connector_individual_operational_status(2, 2), OperationalStatusEnum::Inoperative);
}

/// \brief Test that the ComponentStateManager persists changes in operative state
TEST_F(ComponentStateManagerTest, test_persist_operative_states) {
    // Prepare
    std::shared_ptr<DatabaseHandler> mock_database = std::make_shared<DatabaseHandlerMock>();
    auto state_mgr = this->component_state_manager(mock_database, {1, 2});

    // Act
    state_mgr.set_cs_individual_operational_status(OperationalStatusEnum::Inoperative, true);
    state_mgr.set_evse_individual_operational_status(1, OperationalStatusEnum::Inoperative, true);
    state_mgr.set_connector_individual_operational_status(2, 1, OperationalStatusEnum::Inoperative, true);
    // Saves the same status as the one the EVSE is already in
    state_mgr.set_evse_individual_operational_status(2, OperationalStatusEnum::Operative, true);
    // This one is not persisted - should not be saved
    state_mgr.set_connector_individual_operational_status(1, 1, OperationalStatusEnum::Inoperative, false);

    // Verify
    ASSERT_EQ(mock_database->get_cs_availability(), OperationalStatusEnum::Inoperative);
    ASSERT_EQ(mock_database->get_evse_availability(1), OperationalStatusEnum::Inoperative);
    ASSERT_EQ(mock_database->get_evse_availability(2), OperationalStatusEnum::Operative);
    ASSERT_EQ(mock_database->get_connector_availability(1, 1), OperationalStatusEnum::Operative);
    ASSERT_EQ(mock_database->get_connector_availability(2, 1), OperationalStatusEnum::Inoperative);
    ASSERT_EQ(mock_database->get_connector_availability(2, 2), OperationalStatusEnum::Operative);
    // Also check the persisted state getters - they should return the same value
    ASSERT_EQ(state_mgr.get_cs_persisted_operational_status(), OperationalStatusEnum::Inoperative);
    ASSERT_EQ(state_mgr.get_evse_persisted_operational_status(1), OperationalStatusEnum::Inoperative);
    ASSERT_EQ(state_mgr.get_evse_persisted_operational_status(2), OperationalStatusEnum::Operative);
    ASSERT_EQ(state_mgr.get_connector_persisted_operational_status(1, 1), OperationalStatusEnum::Operative);
    ASSERT_EQ(state_mgr.get_connector_persisted_operational_status(2, 1), OperationalStatusEnum::Inoperative);
    ASSERT_EQ(state_mgr.get_connector_persisted_operational_status(2, 2), OperationalStatusEnum::Operative);
}

/// \brief Test the ComponentStateManager's effective state getters when the CS is inoperative
TEST_F(ComponentStateManagerTest, test_effective_state_getters_cs_inoperative) {
    // Prepare
    std::shared_ptr<DatabaseHandler> mock_database = std::make_shared<DatabaseHandlerMock>();
    auto state_mgr = this->component_state_manager(mock_database, {1, 2});

    // Act
    state_mgr.set_cs_individual_operational_status(OperationalStatusEnum::Inoperative, true);
    // These state changes should be hidden
    state_mgr.set_connector_occupied(1, 1, true);
    state_mgr.set_connector_reserved(2, 1, true);
    state_mgr.set_connector_faulted(2, 2, true);

    // Verify
    ASSERT_EQ(state_mgr.get_evse_effective_operational_status(1), OperationalStatusEnum::Inoperative);
    ASSERT_EQ(state_mgr.get_evse_effective_operational_status(2), OperationalStatusEnum::Inoperative);
    ASSERT_EQ(state_mgr.get_connector_effective_operational_status(1, 1), OperationalStatusEnum::Inoperative);
    ASSERT_EQ(state_mgr.get_connector_effective_operational_status(2, 1), OperationalStatusEnum::Inoperative);
    ASSERT_EQ(state_mgr.get_connector_effective_operational_status(2, 2), OperationalStatusEnum::Inoperative);
    ASSERT_EQ(state_mgr.get_connector_effective_status(1, 1), ConnectorStatusEnum::Unavailable);
    ASSERT_EQ(state_mgr.get_connector_effective_status(2, 1), ConnectorStatusEnum::Unavailable);
    ASSERT_EQ(state_mgr.get_connector_effective_status(2, 2), ConnectorStatusEnum::Unavailable);
}

/// \brief Test the ComponentStateManager's effective state getters when an EVSE is inoperative
TEST_F(ComponentStateManagerTest, test_effective_state_getters_evse_inoperative) {
    // Prepare
    std::shared_ptr<DatabaseHandler> mock_database = std::make_shared<DatabaseHandlerMock>();
    auto state_mgr = this->component_state_manager(mock_database, {1, 2});

    // Act
    state_mgr.set_evse_individual_operational_status(1, OperationalStatusEnum::Inoperative, true);
    // These state changes should be hidden
    state_mgr.set_connector_occupied(1, 1, true);
    // These should be visible
    state_mgr.set_connector_reserved(2, 1, true);
    state_mgr.set_connector_faulted(2, 2, true);

    // Verify
    ASSERT_EQ(state_mgr.get_evse_effective_operational_status(1), OperationalStatusEnum::Inoperative);
    ASSERT_EQ(state_mgr.get_evse_effective_operational_status(2), OperationalStatusEnum::Operative);
    ASSERT_EQ(state_mgr.get_connector_effective_operational_status(1, 1), OperationalStatusEnum::Inoperative);
    ASSERT_EQ(state_mgr.get_connector_effective_operational_status(2, 1), OperationalStatusEnum::Operative);
    ASSERT_EQ(state_mgr.get_connector_effective_operational_status(2, 2), OperationalStatusEnum::Operative);
    ASSERT_EQ(state_mgr.get_connector_effective_status(1, 1), ConnectorStatusEnum::Unavailable);
    ASSERT_EQ(state_mgr.get_connector_effective_status(2, 1), ConnectorStatusEnum::Reserved);
    ASSERT_EQ(state_mgr.get_connector_effective_status(2, 2), ConnectorStatusEnum::Faulted);
}

/// \brief Test the ComponentStateManager's state transitions for a connector
TEST_F(ComponentStateManagerTest, test_connector_state_machine) {
    // Prepare
    std::shared_ptr<DatabaseHandler> mock_database = std::make_shared<DatabaseHandlerMock>();
    auto state_mgr = this->component_state_manager(mock_database, {1});

    // Act & Verify multiple times
    state_mgr.set_connector_reserved(1, 1, true);
    ASSERT_EQ(state_mgr.get_connector_effective_operational_status(1, 1), OperationalStatusEnum::Operative);
    ASSERT_EQ(state_mgr.get_connector_effective_status(1, 1), ConnectorStatusEnum::Reserved);

    state_mgr.set_connector_occupied(1, 1, true);
    ASSERT_EQ(state_mgr.get_connector_effective_operational_status(1, 1), OperationalStatusEnum::Operative);
    ASSERT_EQ(state_mgr.get_connector_effective_status(1, 1), ConnectorStatusEnum::Occupied);

    state_mgr.set_connector_faulted(1, 1, true);
    ASSERT_EQ(state_mgr.get_connector_effective_operational_status(1, 1), OperationalStatusEnum::Operative);
    ASSERT_EQ(state_mgr.get_connector_effective_status(1, 1), ConnectorStatusEnum::Faulted);

    state_mgr.set_connector_individual_operational_status(1, 1, OperationalStatusEnum::Inoperative, false);
    ASSERT_EQ(state_mgr.get_connector_effective_operational_status(1, 1), OperationalStatusEnum::Inoperative);
    state_mgr.set_connector_unavailable(1, 1, true);
    // faulted has precedence over inoperative (G03.FR.06)
    ASSERT_EQ(state_mgr.get_connector_effective_status(1, 1), ConnectorStatusEnum::Faulted);

    state_mgr.set_connector_faulted(1, 1, false);
    ASSERT_EQ(state_mgr.get_connector_effective_operational_status(1, 1), OperationalStatusEnum::Inoperative);
    ASSERT_EQ(state_mgr.get_connector_effective_status(1, 1), ConnectorStatusEnum::Unavailable);

    state_mgr.set_connector_individual_operational_status(1, 1, OperationalStatusEnum::Operative, false);
    state_mgr.set_connector_unavailable(1, 1, false);
    ASSERT_EQ(state_mgr.get_connector_effective_operational_status(1, 1), OperationalStatusEnum::Operative);
    ASSERT_EQ(state_mgr.get_connector_effective_status(1, 1), ConnectorStatusEnum::Occupied);

    state_mgr.set_connector_occupied(1, 1, false);
    ASSERT_EQ(state_mgr.get_connector_effective_operational_status(1, 1), OperationalStatusEnum::Operative);
    ASSERT_EQ(state_mgr.get_connector_effective_status(1, 1), ConnectorStatusEnum::Reserved);

    state_mgr.set_connector_reserved(1, 1, false);
    ASSERT_EQ(state_mgr.get_connector_effective_operational_status(1, 1), OperationalStatusEnum::Operative);
    ASSERT_EQ(state_mgr.get_connector_effective_status(1, 1), ConnectorStatusEnum::Available);
}

/// \brief Test the ComponentStateManager calls "effective state changed" callbacks correctly at run-time
TEST_F(ComponentStateManagerTest, test_effective_state_changed_callbacks) {
    // Prepare
    std::shared_ptr<DatabaseHandler> mock_database = std::make_shared<DatabaseHandlerMock>();
    auto state_mgr = this->component_state_manager(mock_database, {1, 2});

    // Set up mock expectations
    testing::Sequence seq;
    // CS disabled
    EXPECT_CALL(this->callbacks, cs_op_state_update("Inoperative"))
        .Times(1)
        .InSequence(seq)
        .WillOnce(testing::Return());
    EXPECT_CALL(this->callbacks, evse_op_state_update(1, "Inoperative"))
        .Times(1)
        .InSequence(seq)
        .WillOnce(testing::Return());
    EXPECT_CALL(this->callbacks, connector_op_state_update(1, 1, "Inoperative"))
        .Times(1)
        .InSequence(seq)
        .WillOnce(testing::Return());
    EXPECT_CALL(this->callbacks, evse_op_state_update(2, "Inoperative"))
        .Times(1)
        .InSequence(seq)
        .WillOnce(testing::Return());
    EXPECT_CALL(this->callbacks, connector_op_state_update(2, 1, "Inoperative"))
        .Times(1)
        .InSequence(seq)
        .WillOnce(testing::Return());
    EXPECT_CALL(this->callbacks, connector_op_state_update(2, 2, "Inoperative"))
        .Times(1)
        .InSequence(seq)
        .WillOnce(testing::Return());
    // EVSE 1 disabled (no change, no callbacks)
    // Connector 2 in EVSE 1 disabled (no change, no callbacks)
    // CS re-enabled
    EXPECT_CALL(this->callbacks, cs_op_state_update("Operative")).Times(1).InSequence(seq).WillOnce(testing::Return());
    EXPECT_CALL(this->callbacks, evse_op_state_update(2, "Operative"))
        .Times(1)
        .InSequence(seq)
        .WillOnce(testing::Return());
    EXPECT_CALL(this->callbacks, connector_op_state_update(2, 1, "Operative"))
        .Times(1)
        .InSequence(seq)
        .WillOnce(testing::Return());

    // Act & Verify
    state_mgr.set_cs_individual_operational_status(OperationalStatusEnum::Inoperative, false);
    state_mgr.set_evse_individual_operational_status(1, OperationalStatusEnum::Inoperative, false);
    state_mgr.set_connector_individual_operational_status(2, 2, OperationalStatusEnum::Inoperative, false);
    state_mgr.set_cs_individual_operational_status(OperationalStatusEnum::Operative, false);
}

/// \brief Test the ComponentStateManager::trigger_all_effective_availability_changed_callbacks()
TEST_F(ComponentStateManagerTest, test_trigger_boot_callbacks) {
    // Prepare
    std::shared_ptr<DatabaseHandler> mock_database = std::make_shared<DatabaseHandlerMock>();
    // EVSE 1 disabled on boot
    mock_database->insert_evse_availability(1, OperationalStatusEnum::Inoperative, true);
    auto state_mgr = this->component_state_manager(mock_database, {1, 2});

    // Set up mock expectations
    testing::Sequence seq;
    EXPECT_CALL(this->callbacks, cs_op_state_update("Operative")).Times(1).InSequence(seq).WillOnce(testing::Return());
    EXPECT_CALL(this->callbacks, evse_op_state_update(1, "Inoperative"))
        .Times(1)
        .InSequence(seq)
        .WillOnce(testing::Return());
    EXPECT_CALL(this->callbacks, connector_op_state_update(1, 1, "Inoperative"))
        .Times(1)
        .InSequence(seq)
        .WillOnce(testing::Return());
    EXPECT_CALL(this->callbacks, evse_op_state_update(2, "Operative"))
        .Times(1)
        .InSequence(seq)
        .WillOnce(testing::Return());
    EXPECT_CALL(this->callbacks, connector_op_state_update(2, 1, "Operative"))
        .Times(1)
        .InSequence(seq)
        .WillOnce(testing::Return());
    EXPECT_CALL(this->callbacks, connector_op_state_update(2, 2, "Operative"))
        .Times(1)
        .InSequence(seq)
        .WillOnce(testing::Return());

    // Act & Verify
    state_mgr.trigger_all_effective_availability_changed_callbacks();
}

/// \brief Test the ComponentStateManager::send_status_notification_changed_connectors()
TEST_F(ComponentStateManagerTest, test_send_status_notification_changed_connectors) {
    // Prepare
    std::shared_ptr<DatabaseHandler> mock_database = std::make_shared<DatabaseHandlerMock>();
    auto state_mgr = this->component_state_manager(mock_database, {1, 2});

    // Set up mock expectations
    testing::Sequence seq;
    // EVSE 1 connector 1 faulted - success
    EXPECT_CALL(this->callbacks, connector_status_update(1, 1, "Faulted"))
        .Times(1)
        .InSequence(seq)
        .WillOnce(testing::Return(true));
    // EVSE 2 connector 2 occupied - failed
    EXPECT_CALL(this->callbacks, connector_status_update(2, 2, "Occupied"))
        .Times(1)
        .InSequence(seq)
        .WillOnce(testing::Return(false));
    // test_send_status_notification_changed_connectors() -> retry sending EVSE 2 connector 2 occupied, success
    EXPECT_CALL(this->callbacks, connector_status_update(2, 2, "Occupied"))
        .Times(1)
        .InSequence(seq)
        .WillOnce(testing::Return(true));
    // test_send_status_notification_changed_connectors() again -> no change, nothing sent

    // Act & Verify
    state_mgr.set_connector_faulted(1, 1, true);
    state_mgr.set_connector_occupied(2, 2, true);
    state_mgr.send_status_notification_changed_connectors();
    state_mgr.send_status_notification_changed_connectors();
}

/// \brief Test the ComponentStateManager::send_status_notification_single_connector()
TEST_F(ComponentStateManagerTest, test_send_status_notification_single_connector) {
    // Prepare
    std::shared_ptr<DatabaseHandler> mock_database = std::make_shared<DatabaseHandlerMock>();
    auto state_mgr = this->component_state_manager(mock_database, {1, 2});

    // Set up mock expectations
    testing::Sequence seq;
    // send_status_notification_single_connector() twice - report twice
    EXPECT_CALL(this->callbacks, connector_status_update(1, 1, "Available"))
        .Times(1)
        .InSequence(seq)
        .WillOnce(testing::Return(true));
    EXPECT_CALL(this->callbacks, connector_status_update(1, 1, "Available"))
        .Times(1)
        .InSequence(seq)
        .WillOnce(testing::Return(true));
    // set_connector_occupied() -> fail
    EXPECT_CALL(this->callbacks, connector_status_update(1, 1, "Occupied"))
        .Times(1)
        .InSequence(seq)
        .WillOnce(testing::Return(false));
    // send_status_notification_single_connector() -> success
    EXPECT_CALL(this->callbacks, connector_status_update(1, 1, "Occupied"))
        .Times(1)
        .InSequence(seq)
        .WillOnce(testing::Return(true));
    // send_status_notification_changed_connectors() -> nothing more sent

    // Act & Verify
    state_mgr.send_status_notification_single_connector(1, 1);
    state_mgr.send_status_notification_single_connector(1, 1);
    state_mgr.set_connector_occupied(1, 1, true);
    state_mgr.send_status_notification_single_connector(1, 1);
    state_mgr.send_status_notification_changed_connectors();
}

} // namespace ocpp::v201
