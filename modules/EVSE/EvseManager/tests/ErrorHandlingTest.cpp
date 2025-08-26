// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include "ErrorHandling.hpp"
#include "EvseManagerStub.hpp"
#include "evse_board_supportIntfStub.hpp"

#include <gtest/gtest.h>

#include <memory>
#include <set>

namespace Everest::error {
bool operator<(const Error& lhs, const Error& rhs) {
    return lhs.type < rhs.type;
}
bool operator==(const Error& lhs, const Error& rhs) {
    return (lhs.type == rhs.type) && (lhs.sub_type == rhs.sub_type);
}
} // namespace Everest::error

namespace {

struct ErrorHandlingTest : public module::ErrorHandling {
    using module::ErrorHandling::ErrorHandling;
    using module::ErrorHandling::raise_inoperative_error;
};

struct ErrorDatabaseStub : public Everest::error::ErrorDatabase {
    using Error = Everest::error::Error;
    using ErrorPtr = Everest::error::ErrorPtr;
    using ErrorFilter = Everest::error::ErrorFilter;

    std::list<Error>& active_errors;

    ErrorDatabaseStub(std::list<Error>& active) : Everest::error::ErrorDatabase(), active_errors(active) {
    }

    virtual void add_error(ErrorPtr error) {
    }
    virtual std::list<ErrorPtr> get_errors(const std::list<ErrorFilter>& filters) const {
        std::list<ErrorPtr> result;
        for (const auto& error : active_errors) {
            result.push_back(std::make_shared<Error>(error));
        }
        return result;
    }
    virtual std::list<ErrorPtr> edit_errors(const std::list<ErrorFilter>& filters, EditErrorFunc edit_func) {
        return {};
    }
    virtual std::list<ErrorPtr> remove_errors(const std::list<ErrorFilter>& filters) {
        return {};
    }
};

struct EvseManagerModuleAdapterStub : public module::stub::EvseManagerModuleAdapter {
    std::shared_ptr<Everest::error::ErrorTypeMap> error_type_map;
    std::shared_ptr<ErrorDatabaseStub> error_database;
    std::list<Everest::error::ErrorType> error_list;
    std::list<Everest::error::Error> active_errors;

    EvseManagerModuleAdapterStub() :
        module::stub::EvseManagerModuleAdapter(),
        error_type_map(std::make_shared<Everest::error::ErrorTypeMap>()),
        error_database(std::make_shared<ErrorDatabaseStub>(active_errors)),
        error_list{} {
    }

    virtual std::shared_ptr<Everest::error::ErrorManagerImpl> get_error_manager_impl_fn(const std::string& str) {
        return std::make_shared<Everest::error::ErrorManagerImpl>(
            error_type_map, std::make_shared<Everest::error::ErrorDatabaseMap>(), error_list,
            [this](const Everest::error::Error& error) {
                if (error_raise.find(error.type) == error_raise.end()) {
                    throw std::runtime_error("Error type " + error.type + " not found");
                }
                error_raise[error.type](error);
                active_errors.push_back(error);
            },
            [this](const Everest::error::Error& error) {
                if (error_raise.find(error.type) == error_raise.end()) {
                    throw std::runtime_error("Error type " + error.type + " not found");
                }
                error_clear[error.type](error);
                active_errors.remove(error);
            },
            false);
    }

    virtual std::shared_ptr<Everest::error::ErrorManagerReq> get_error_manager_req_fn(const Requirement& req) {
        return std::make_shared<Everest::error::ErrorManagerReq>(
            error_type_map, std::make_shared<Everest::error::ErrorDatabaseMap>(), error_list,
            [this](const Everest::error::ErrorType& error_type, const Everest::error::ErrorCallback& callback,
                   const Everest::error::ErrorCallback& clear_callback) {
                error_raise[error_type] = callback;
                error_clear[error_type] = clear_callback;
            });
    }

    virtual std::shared_ptr<Everest::error::ErrorFactory> get_error_factory_fn(const std::string&) {
        return std::make_shared<Everest::error::ErrorFactory>(error_type_map);
    }

    virtual std::shared_ptr<Everest::error::ErrorStateMonitor> get_error_state_monitor_impl_fn(const std::string&) {
        return std::make_shared<Everest::error::ErrorStateMonitor>(error_database);
    }
};

struct ErrorHandlingTesting : public testing::Test {
    EvseManagerModuleAdapterStub adapter;
    std::map<Everest::error::ErrorType, std::string> error_types_map;

    std::unique_ptr<evse_board_supportIntf> r_bsp{};
    const std::vector<std::unique_ptr<ISO15118_chargerIntf>> r_hlc{};
    const std::vector<std::unique_ptr<connector_lockIntf>> r_connector_lock{};
    const std::vector<std::unique_ptr<ac_rcdIntf>> r_ac_rcd{};
    std::unique_ptr<evse_managerImplBase> p_evse{};
    const std::vector<std::unique_ptr<isolation_monitorIntf>> _r_imd{};
    const std::vector<std::unique_ptr<power_supply_DCIntf>> _r_powersupply{};
    const std::vector<std::unique_ptr<powermeterIntf>> _r_powermeter{};
    const std::vector<std::unique_ptr<over_voltage_monitorIntf>> _r_over_voltage_monitor{};

    std::unique_ptr<ErrorHandlingTest> error_handler;

    void construct(bool _inoperative_error_use_vendor_id) {
        error_types_map = {{"evse_board_support/VendorWarning", "warning"},
                           {"evse_manager/Inoperative", "inoperative"}};
        adapter.error_type_map->load_error_types_map(error_types_map);
        adapter.active_errors.clear();
        for (const auto& [error, description] : error_types_map) {
            adapter.error_list.push_back(error);
        }
        r_bsp = std::make_unique<module::stub::evse_board_supportIntfStub>(adapter);
        p_evse = std::make_unique<module::stub::evse_managerImplStub>(&adapter, "manager");
        error_handler = std::make_unique<ErrorHandlingTest>(r_bsp, r_hlc, r_connector_lock, r_ac_rcd, p_evse, _r_imd,
                                                            _r_powersupply, _r_powermeter, _r_over_voltage_monitor,
                                                            _inoperative_error_use_vendor_id);
    }

    static constexpr std::string_view default_description{"description"};
    static constexpr std::string_view default_no_vendor_id{"EVerest"};
    static constexpr std::string_view default_vendor_id{"vendor_id"};

    Everest::error::Error test_description(const std::string& type, const std::string& subtype) {
        Everest::error::Error error{};
        error.type = type;
        error.sub_type = subtype;
        error.message = "message";
        error.description = default_description;
        error.vendor_id = default_vendor_id;
        error_handler->raise_inoperative_error(error);

        EXPECT_EQ(adapter.active_errors.size(), 1);
        auto& active = adapter.active_errors.front();
        EXPECT_EQ(active.type, "evse_manager/Inoperative");
        EXPECT_EQ(active.sub_type, "");
        EXPECT_EQ(active.message, error.type);
        return active;
    }
};

TEST_F(ErrorHandlingTesting, NoType) {
    construct(true);
    auto error = test_description("", "sub-type");
    EXPECT_EQ(error.description, default_description);
    EXPECT_EQ(error.vendor_id, default_vendor_id);
    construct(false);
    error = test_description("", "sub-type");
    EXPECT_EQ(error.description, default_description);
    EXPECT_EQ(error.vendor_id, default_no_vendor_id);
}

TEST_F(ErrorHandlingTesting, TypeNoSlashA) {
    construct(true);
    auto error = test_description("type", "sub-type");
    EXPECT_EQ(error.description, default_description);
    EXPECT_EQ(error.vendor_id, default_vendor_id);
    construct(false);
    error = test_description("type", "sub-type");
    EXPECT_EQ(error.description, default_description);
    EXPECT_EQ(error.vendor_id, default_no_vendor_id);
}

TEST_F(ErrorHandlingTesting, TypeNoSlashB) {
    construct(true);
    auto error = test_description("type", "");
    EXPECT_EQ(error.description, default_description);
    EXPECT_EQ(error.vendor_id, default_vendor_id);
    construct(false);
    error = test_description("type", "");
    EXPECT_EQ(error.description, default_description);
    EXPECT_EQ(error.vendor_id, default_no_vendor_id);
}

TEST_F(ErrorHandlingTesting, TypeSlashEndA) {
    construct(true);
    auto error = test_description("type/", "sub-type");
    EXPECT_EQ(error.description, "type/sub-type");
    EXPECT_EQ(error.vendor_id, default_vendor_id);
    construct(false);
    error = test_description("type/", "sub-type");
    EXPECT_EQ(error.description, "type/sub-type");
    EXPECT_EQ(error.vendor_id, default_no_vendor_id);
}

TEST_F(ErrorHandlingTesting, TypeSlashEndB) {
    construct(true);
    auto error = test_description("type/", "");
    EXPECT_EQ(error.description, "type");
    EXPECT_EQ(error.vendor_id, default_vendor_id);
    construct(false);
    error = test_description("type/", "");
    EXPECT_EQ(error.description, "type");
    EXPECT_EQ(error.vendor_id, default_no_vendor_id);
}

TEST_F(ErrorHandlingTesting, TypeSlashBeginA) {
    construct(true);
    auto error = test_description("/type", "sub-type");
    EXPECT_EQ(error.description, "type/sub-type");
    EXPECT_EQ(error.vendor_id, default_vendor_id);
    construct(false);
    error = test_description("/type", "sub-type");
    EXPECT_EQ(error.description, "type/sub-type");
    EXPECT_EQ(error.vendor_id, default_no_vendor_id);
}

TEST_F(ErrorHandlingTesting, TypeSlashBeginB) {
    construct(true);
    auto error = test_description("/type", "");
    EXPECT_EQ(error.description, "type");
    EXPECT_EQ(error.vendor_id, default_vendor_id);
    construct(false);
    error = test_description("/type", "");
    EXPECT_EQ(error.description, "type");
    EXPECT_EQ(error.vendor_id, default_no_vendor_id);
}

TEST_F(ErrorHandlingTesting, TypeFullA) {
    construct(true);
    auto error = test_description("module/type", "sub-type");
    EXPECT_EQ(error.description, "type/sub-type");
    EXPECT_EQ(error.vendor_id, default_vendor_id);
    construct(false);
    error = test_description("module/type", "sub-type");
    EXPECT_EQ(error.description, "type/sub-type");
    EXPECT_EQ(error.vendor_id, default_no_vendor_id);
}

TEST_F(ErrorHandlingTesting, TypeFullB) {
    construct(true);
    auto error = test_description("module/type", "");
    EXPECT_EQ(error.description, "type");
    EXPECT_EQ(error.vendor_id, default_vendor_id);
    construct(false);
    error = test_description("module/type", "");
    EXPECT_EQ(error.description, "type");
    EXPECT_EQ(error.vendor_id, default_no_vendor_id);
}

TEST_F(ErrorHandlingTesting, TypeFullC) {
    construct(true);
    auto error = test_description("module/type/", "sub-type");
    EXPECT_EQ(error.description, "type/sub-type");
    EXPECT_EQ(error.vendor_id, default_vendor_id);
    construct(false);
    error = test_description("module/type/", "sub-type");
    EXPECT_EQ(error.description, "type/sub-type");
    EXPECT_EQ(error.vendor_id, default_no_vendor_id);
}

TEST_F(ErrorHandlingTesting, TypeFullD) {
    construct(true);
    auto error = test_description("module/type/", "");
    EXPECT_EQ(error.description, "type");
    EXPECT_EQ(error.vendor_id, default_vendor_id);
    construct(false);
    error = test_description("module/type/", "");
    EXPECT_EQ(error.description, "type");
    EXPECT_EQ(error.vendor_id, default_no_vendor_id);
}

TEST_F(ErrorHandlingTesting, NoVendorId) {
    Everest::error::Error raised_error{};
    raised_error.type = "module/type";
    raised_error.sub_type = "sub-type";
    raised_error.message = "message";
    raised_error.description = default_description;
    raised_error.vendor_id = "";

    construct(true);
    error_handler->raise_inoperative_error(raised_error);
    EXPECT_EQ(adapter.active_errors.size(), 1);
    auto& error = adapter.active_errors.front();
    EXPECT_EQ(error.type, "evse_manager/Inoperative");
    EXPECT_EQ(error.sub_type, "");
    EXPECT_EQ(error.message, raised_error.type);
    EXPECT_EQ(error.description, "type/sub-type");
    EXPECT_EQ(error.vendor_id, default_no_vendor_id);

    construct(false);
    error_handler->raise_inoperative_error(raised_error);
    EXPECT_EQ(adapter.active_errors.size(), 1);
    error = adapter.active_errors.front();
    EXPECT_EQ(error.type, "evse_manager/Inoperative");
    EXPECT_EQ(error.sub_type, "");
    EXPECT_EQ(error.message, raised_error.type);
    EXPECT_EQ(error.description, "type/sub-type");
    EXPECT_EQ(error.vendor_id, default_no_vendor_id);
}

TEST_F(ErrorHandlingTesting, IsActive) {
    Everest::error::Error first_error{};
    first_error.type = "module/type";
    first_error.sub_type = "sub-type";
    first_error.message = "message";
    first_error.description = "description";
    first_error.vendor_id = "vendor_id";

    construct(true);
    EXPECT_FALSE(p_evse->error_state_monitor->is_error_active("evse_manager/Inoperative", ""));

    error_handler->raise_inoperative_error(first_error);
    EXPECT_EQ(adapter.active_errors.size(), 1);
    auto error = adapter.active_errors.front();
    EXPECT_EQ(error.type, "evse_manager/Inoperative");
    EXPECT_EQ(error.sub_type, "");
    EXPECT_EQ(error.message, first_error.type);
    EXPECT_EQ(error.description, "type/sub-type");
    EXPECT_EQ(error.vendor_id, first_error.vendor_id);

    EXPECT_TRUE(p_evse->error_state_monitor->is_error_active("evse_manager/Inoperative", ""));

    Everest::error::Error raised_error{};
    raised_error.type = "module/new-type";
    raised_error.sub_type = "new-sub-type";
    raised_error.message = "new-message";
    raised_error.description = "new-description";
    raised_error.vendor_id = "new-vendor_id";

    // should not be a new error
    error_handler->raise_inoperative_error(raised_error);
    EXPECT_EQ(adapter.active_errors.size(), 1);
    error = adapter.active_errors.front();
    // should be first error
    EXPECT_EQ(error.type, "evse_manager/Inoperative");
    EXPECT_EQ(error.sub_type, "");
    EXPECT_EQ(error.message, first_error.type);
    EXPECT_EQ(error.description, "type/sub-type");
    EXPECT_EQ(error.vendor_id, first_error.vendor_id);
}

} // namespace
