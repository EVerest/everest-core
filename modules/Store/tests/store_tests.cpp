// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include <gtest/gtest.h>

#define protected public
#define private   public
#include "kvsImpl.hpp"
#undef private
#undef protected

namespace module {

TEST(StoreTest, kvs_impl_test) {
    Everest::PtrContainer<Store> mod_ptr{};
    main::Conf main_config;

    auto p_main = std::make_unique<main::kvsImpl>(nullptr, mod_ptr, main_config);
    p_main->init();
    p_main->ready();

    std::string key = "key";
    std::variant<std::nullptr_t, Array, Object, bool, double, int, std::string> value = 42;
    EXPECT_EQ(p_main->handle_exists(key), false);
    p_main->handle_store(key, value);
    EXPECT_EQ(p_main->handle_exists(key), true);
    EXPECT_EQ(std::get<int>(p_main->handle_load(key)), 42);
    p_main->handle_delete(key);
    EXPECT_EQ(p_main->handle_exists(key), false);
}

TEST(StoreTest, kvs_impl_test_reset) {
    Everest::PtrContainer<Store> mod_ptr{};
    main::Conf main_config;

    auto p_main = std::make_unique<main::kvsImpl>(nullptr, mod_ptr, main_config);

    std::string key = "key";
    std::variant<std::nullptr_t, Array, Object, bool, double, int, std::string> value = 42;
    EXPECT_EQ(p_main->handle_exists(key), false);
    p_main->handle_store(key, value);
    EXPECT_EQ(p_main->handle_exists(key), true);

    // The Store module should not persist anything after it has been re-initialized
    p_main = std::make_unique<main::kvsImpl>(nullptr, mod_ptr, main_config);
    EXPECT_EQ(p_main->handle_exists(key), false);
}

TEST(StoreTest, module_test) {
    Everest::PtrContainer<Store> mod_ptr{};
    main::Conf main_config;

    auto p_main = std::make_unique<main::kvsImpl>(nullptr, mod_ptr, main_config);

    ModuleInfo module_info{};
    Conf module_conf;
    Store module(module_info, std::move(p_main), module_conf);

    mod_ptr.set(&module);
    mod_ptr->init();
    mod_ptr->ready();
}

} // namespace module
