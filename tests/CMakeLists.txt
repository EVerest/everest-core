include(Catch)

add_executable(tests main.cpp test_config.cpp)

target_link_libraries(tests
    PRIVATE
        everest::framework
        everest::log
        Catch2::Catch2
)

catch_discover_tests(tests)

# NOTE: using configure_file() here, because file(COPY) can't rename

# valid config.json
configure_file(test_configs/valid_config.json valid/config.json COPYONLY)

# broken config.json
configure_file(test_configs/broken_config.json broken_config/config.json COPYONLY)

# missing module config.json
configure_file(test_configs/missing_module_config.json missing_module_config/config.json COPYONLY)

# broken manifest.json
configure_file(test_configs/broken_manifest_config.json broken_manifest/config.json COPYONLY)
file(COPY test_modules/TESTBrokenManifest DESTINATION broken_manifest/modules)

# broken manifest.json
configure_file(test_configs/broken_manifest2_config.json broken_manifest2/config.json COPYONLY)
file(COPY test_modules/TESTBrokenManifest2 DESTINATION broken_manifest2/modules)

# valid manifest.json, missing interface
configure_file(test_configs/valid_manifest_missing_interface_config.json valid_manifest_missing_interface/config.json COPYONLY)
file(COPY test_modules/TESTValidManifestMissingInterface DESTINATION valid_manifest_missing_interface/modules)

# valid manifest.json, valid interface
configure_file(test_interfaces/TESTmissinginterface.json valid_manifest_valid_interface/interfaces/TESTmissinginterface.json COPYONLY)
configure_file(test_configs/valid_manifest_missing_interface_config.json valid_manifest_valid_interface/config.json COPYONLY)
file(COPY test_modules/TESTValidManifestMissingInterface DESTINATION valid_manifest_valid_interface/modules)

# create dummy directory for types
file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/dummy_types")
