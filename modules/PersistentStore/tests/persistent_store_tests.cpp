// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include <filesystem>

#include <gtest/gtest.h>

#define protected public
#define private   public
#include "kvsImpl.hpp"
#undef private
#undef protected

namespace module {

TEST(PersistentStoreTest, module_test) {
    Everest::PtrContainer<PersistentStore> mod_ptr{};
    main::Conf main_config;

    auto p_main = std::make_unique<main::kvsImpl>(nullptr, mod_ptr, main_config);

    ModuleInfo module_info{};
    Conf module_conf;
    auto temp_dir = std::filesystem::temp_directory_path();
    auto temp_file = temp_dir / "persistent_store_tests.db";
    if (std::filesystem::exists(temp_file)) {
        std::filesystem::remove(temp_file);
    }
    module_conf.sqlite_db_file_path = temp_file.string();
    PersistentStore module(module_info, std::move(p_main), module_conf);

    mod_ptr.set(&module);
    mod_ptr->init();
    mod_ptr->ready();

    std::string key = "key";
    std::variant<std::nullptr_t, Array, Object, bool, double, int, std::string> value = 42;
    EXPECT_EQ(mod_ptr->p_main->handle_exists(key), false);
    mod_ptr->p_main->handle_store(key, value);
    EXPECT_EQ(mod_ptr->p_main->handle_exists(key), true);

    // The PersistentStore module should persist anything after it has been re-initialized
    auto p_main2 = std::make_unique<main::kvsImpl>(nullptr, mod_ptr, main_config);
    PersistentStore module2(module_info, std::move(p_main2), module_conf);
    mod_ptr.set(&module2);
    mod_ptr->init();
    mod_ptr->ready();
    EXPECT_EQ(mod_ptr->p_main->handle_exists(key), true);
}

} // namespace module
