// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <ModuleAdapterStub.hpp>

#define protected public
#define private   public
#include "auth_token_providerImpl.hpp"
#undef private
#undef protected

using testing::_;
using testing::MockFunction;
using testing::Return;
using testing::SaveArg;

namespace module {

struct EvseManagerModuleAdapter : public stub::ModuleAdapterStub {
    EvseManagerModuleAdapter() : id("evse_manager", "main") {
    }

    ImplementationIdentifier id;
    ValueCallback session_event_cb;

    void publish_session_event_for_test() {
        std::printf("publish_session_event_for_test\n");
        types::evse_manager::SessionEvent se;
        se.event = types::evse_manager::SessionEventEnum::SessionStarted;
        types::evse_manager::SessionStarted session_started;

        se.timestamp = Everest::Date::to_rfc3339(date::utc_clock::now());

        types::authorization::ProvidedIdToken provided_token;
        provided_token.authorization_type = types::authorization::AuthorizationType::RFID;
        provided_token.id_token = {"FREESERVICE", types::authorization::IdTokenType::Local};
        provided_token.prevalidated = true;
        session_started.reason = types::evse_manager::StartSessionReason::EVConnected;
        session_started.id_tag = provided_token;

        se.session_started = session_started;
        se.uuid = "UUID";
        session_event_cb(se);
    }

    virtual void subscribe_fn(const Requirement&, const std::string& fn, ValueCallback cb) {
        std::printf("subscribe_fn(%s)\n", fn.c_str());
        if (fn == "session_event") {
            session_event_cb = cb;
        }
    }
};

struct DummyTokenProviderModuleAdapter : public stub::ModuleAdapterStub {
    DummyTokenProviderModuleAdapter() : id("auth_token_provider", "main") {
    }

    ImplementationIdentifier id;
    std::function<void()> provided_token_callback;

    virtual void publish_fn(const std::string& impl_id, const std::string& var_name, Value value) {
        std::printf("publish_fn(%s, %s)\n", impl_id.c_str(), var_name.c_str());
        if (var_name == "provided_token") {
            provided_token_callback();
        }
    }
};

TEST(DummyTokenProviderTest, auth_token_provider_impl_test) {
    Everest::PtrContainer<DummyTokenProvider> mod_ptr{};
    main::Conf main_config;
    main_config.token = "DEADBEEF";
    main_config.type = "RFID";
    auto evse_manager_adapter = EvseManagerModuleAdapter();
    auto dummy_token_provider_adapter = DummyTokenProviderModuleAdapter();

    // TODO add param with token
    MockFunction<void()> provided_token_callback_mock;

    dummy_token_provider_adapter.provided_token_callback = provided_token_callback_mock.AsStdFunction();
    EXPECT_CALL(provided_token_callback_mock, Call());

    auto p_main = std::make_unique<main::auth_token_providerImpl>(&dummy_token_provider_adapter, mod_ptr, main_config);
    std::string r_evse_requirement_module_id;
    Requirement r_evse_requirement;
    std::optional<Mapping> r_evse_mapping;
    auto r_evse = std::make_unique<evse_managerIntf>(&evse_manager_adapter, r_evse_requirement,
                                                     r_evse_requirement_module_id, r_evse_mapping);

    ModuleInfo module_info{};
    Conf module_conf;
    DummyTokenProvider module(module_info, std::move(p_main), std::move(r_evse), module_conf);

    mod_ptr.set(&module);
    mod_ptr->init();
    mod_ptr->ready();

    evse_manager_adapter.publish_session_event_for_test();
}

} // namespace module
