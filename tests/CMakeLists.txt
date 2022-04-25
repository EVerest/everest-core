list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/contrib)
include(Catch)

add_executable(test_config test_config.cpp)

target_link_libraries(test_config
    PRIVATE
        everest ${CMAKE_DL_LIBS}
        everest::log
        Catch2::Catch2
)

catch_discover_tests(test_config)

configure_file(test_configs/valid_config.json ${CMAKE_CURRENT_BINARY_DIR}/valid/config.json COPYONLY)
configure_file(${PROJECT_SOURCE_DIR}/schemas/config.json ${CMAKE_CURRENT_BINARY_DIR}/valid/schemes/config.json COPYONLY)
configure_file(${PROJECT_SOURCE_DIR}/schemas/manifest.json ${CMAKE_CURRENT_BINARY_DIR}/valid/schemes/manifest.json COPYONLY)
configure_file(${PROJECT_SOURCE_DIR}/schemas/interface.json ${CMAKE_CURRENT_BINARY_DIR}/valid/schemes/interface.json COPYONLY)

# missing config.json
configure_file(${PROJECT_SOURCE_DIR}/schemas/config.json ${CMAKE_CURRENT_BINARY_DIR}/missing_config/schemes/config.json COPYONLY)
configure_file(${PROJECT_SOURCE_DIR}/schemas/manifest.json ${CMAKE_CURRENT_BINARY_DIR}/missing_config/schemes/manifest.json COPYONLY)
configure_file(${PROJECT_SOURCE_DIR}/schemas/interface.json ${CMAKE_CURRENT_BINARY_DIR}/missing_config/schemes/interface.json COPYONLY)

# broken config.json
configure_file(test_configs/broken_config.json ${CMAKE_CURRENT_BINARY_DIR}/broken_config/config.json COPYONLY)
configure_file(${PROJECT_SOURCE_DIR}/schemas/config.json ${CMAKE_CURRENT_BINARY_DIR}/broken_config/schemes/config.json COPYONLY)
configure_file(${PROJECT_SOURCE_DIR}/schemas/manifest.json ${CMAKE_CURRENT_BINARY_DIR}/broken_config/schemes/manifest.json COPYONLY)
configure_file(${PROJECT_SOURCE_DIR}/schemas/interface.json ${CMAKE_CURRENT_BINARY_DIR}/broken_config/schemes/interface.json COPYONLY)

# missing module config.json
configure_file(test_configs/missing_module_config.json ${CMAKE_CURRENT_BINARY_DIR}/missing_module_config/config.json COPYONLY)
configure_file(${PROJECT_SOURCE_DIR}/schemas/config.json ${CMAKE_CURRENT_BINARY_DIR}/missing_module_config/schemes/config.json COPYONLY)
configure_file(${PROJECT_SOURCE_DIR}/schemas/manifest.json ${CMAKE_CURRENT_BINARY_DIR}/missing_module_config/schemes/manifest.json COPYONLY)
configure_file(${PROJECT_SOURCE_DIR}/schemas/interface.json ${CMAKE_CURRENT_BINARY_DIR}/missing_module_config/schemes/interface.json COPYONLY)

# broken manifest.json
configure_file(test_configs/broken_manifest_config.json ${CMAKE_CURRENT_BINARY_DIR}/broken_manifest/config.json COPYONLY)
configure_file(${PROJECT_SOURCE_DIR}/schemas/config.json ${CMAKE_CURRENT_BINARY_DIR}/broken_manifest/schemes/config.json COPYONLY)
configure_file(${PROJECT_SOURCE_DIR}/schemas/manifest.json ${CMAKE_CURRENT_BINARY_DIR}/broken_manifest/schemes/manifest.json COPYONLY)
configure_file(${PROJECT_SOURCE_DIR}/schemas/interface.json ${CMAKE_CURRENT_BINARY_DIR}/broken_manifest/schemes/interface.json COPYONLY)
file(COPY test_modules/TESTBrokenManifest DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/broken_manifest/modules)

# broken manifest.json
configure_file(test_configs/broken_manifest2_config.json ${CMAKE_CURRENT_BINARY_DIR}/broken_manifest2/config.json COPYONLY)
configure_file(${PROJECT_SOURCE_DIR}/schemas/config.json ${CMAKE_CURRENT_BINARY_DIR}/broken_manifest2/schemes/config.json COPYONLY)
configure_file(${PROJECT_SOURCE_DIR}/schemas/manifest.json ${CMAKE_CURRENT_BINARY_DIR}/broken_manifest2/schemes/manifest.json COPYONLY)
configure_file(${PROJECT_SOURCE_DIR}/schemas/interface.json ${CMAKE_CURRENT_BINARY_DIR}/broken_manifest2/schemes/interface.json COPYONLY)
file(COPY test_modules/TESTBrokenManifest2 DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/broken_manifest2/modules)

# valid manifest.json, missing interface
configure_file(test_configs/valid_manifest_missing_interface_config.json ${CMAKE_CURRENT_BINARY_DIR}/valid_manifest_missing_interface/config.json COPYONLY)
configure_file(${PROJECT_SOURCE_DIR}/schemas/config.json ${CMAKE_CURRENT_BINARY_DIR}/valid_manifest_missing_interface/schemes/config.json COPYONLY)
configure_file(${PROJECT_SOURCE_DIR}/schemas/manifest.json ${CMAKE_CURRENT_BINARY_DIR}/valid_manifest_missing_interface/schemes/manifest.json COPYONLY)
configure_file(${PROJECT_SOURCE_DIR}/schemas/interface.json ${CMAKE_CURRENT_BINARY_DIR}/valid_manifest_missing_interface/schemes/interface.json COPYONLY)
file(COPY test_modules/TESTValidManifestMissingInterface DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/valid_manifest_missing_interface/modules)

# valid manifest.json, valid interface
configure_file(test_interfaces/TESTmissinginterface.json ${CMAKE_CURRENT_BINARY_DIR}/valid_manifest_valid_interface/interfaces/TESTmissinginterface.json COPYONLY)
configure_file(test_configs/valid_manifest_missing_interface_config.json ${CMAKE_CURRENT_BINARY_DIR}/valid_manifest_valid_interface/config.json COPYONLY)
configure_file(${PROJECT_SOURCE_DIR}/schemas/config.json ${CMAKE_CURRENT_BINARY_DIR}/valid_manifest_valid_interface/schemes/config.json COPYONLY)
configure_file(${PROJECT_SOURCE_DIR}/schemas/manifest.json ${CMAKE_CURRENT_BINARY_DIR}/valid_manifest_valid_interface/schemes/manifest.json COPYONLY)
configure_file(${PROJECT_SOURCE_DIR}/schemas/interface.json ${CMAKE_CURRENT_BINARY_DIR}/valid_manifest_valid_interface/schemes/interface.json COPYONLY)
file(COPY test_modules/TESTValidManifestMissingInterface DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/valid_manifest_valid_interface/modules)
