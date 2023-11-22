include(Catch)

add_executable(tests)

target_include_directories(tests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_sources(tests PRIVATE
    test_config.cpp
    helpers.cpp
)

target_link_libraries(tests
    PRIVATE
        everest::framework
        everest::log
        Catch2::Catch2WithMain
)

catch_discover_tests(tests)

include(test_directory_setups/empty_config.cmake)
include(test_directory_setups/broken_yaml.cmake)
include(test_directory_setups/missing_module.cmake)
include(test_directory_setups/broken_manifest_1.cmake)
include(test_directory_setups/broken_manifest_2.cmake)
include(test_directory_setups/missing_interface.cmake)
include(test_directory_setups/valid_config.cmake)
include(test_directory_setups/empty_yaml_object.cmake)
include(test_directory_setups/empty_yaml.cmake)
include(test_directory_setups/null_yaml.cmake)
include(test_directory_setups/string_yaml.cmake)
