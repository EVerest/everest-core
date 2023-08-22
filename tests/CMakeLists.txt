include(Catch)

add_executable(tests test_config.cpp)

target_link_libraries(tests
    PRIVATE
        everest::framework
        everest::log
        Catch2::Catch2WithMain
)

catch_discover_tests(tests)

# NOTE: using configure_file() here, because file(COPY) can't rename

# valid config.json
configure_file(test_configs/valid_config.yaml valid/config.yaml COPYONLY)

# broken config.json
configure_file(test_configs/broken_config.yaml broken_config/config.yaml COPYONLY)

# missing module config.json
configure_file(test_configs/missing_module_config.yaml missing_module_config/config.yaml COPYONLY)

# broken manifest.json
configure_file(test_configs/broken_manifest_config.yaml broken_manifest/config.yaml COPYONLY)
file(COPY test_modules/TESTBrokenManifest DESTINATION broken_manifest/modules)

# broken manifest.json
configure_file(test_configs/broken_manifest2_config.yaml broken_manifest2/config.yaml COPYONLY)
file(COPY test_modules/TESTBrokenManifest2 DESTINATION broken_manifest2/modules)

# valid manifest.json, missing interface
configure_file(test_configs/valid_manifest_missing_interface_config.yaml valid_manifest_missing_interface/config.yaml COPYONLY)
file(COPY test_modules/TESTValidManifestMissingInterface DESTINATION valid_manifest_missing_interface/modules)

# valid manifest.json, valid interface
configure_file(test_interfaces/TESTmissinginterface.yaml valid_manifest_valid_interface/interfaces/TESTmissinginterface.yaml COPYONLY)
configure_file(test_configs/valid_manifest_missing_interface_config.yaml valid_manifest_valid_interface/config.yaml COPYONLY)
file(COPY test_modules/TESTValidManifestMissingInterface DESTINATION valid_manifest_valid_interface/modules)

# create dummy directory for types
file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/dummy_types")
