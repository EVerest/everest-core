set(TEST_TARGET_NAME ${PROJECT_NAME}_EnergyManager_tests)
add_executable(${TEST_TARGET_NAME})

add_dependencies(${TEST_TARGET_NAME} ${MODULE_NAME})

get_target_property(GENERATED_INCLUDE_DIR generate_cpp_files EVEREST_GENERATED_INCLUDE_DIR)

target_include_directories(${TEST_TARGET_NAME} PRIVATE
    . ..
    ${GENERATED_INCLUDE_DIR}
    ${CMAKE_BINARY_DIR}/generated/modules/${MODULE_NAME}
)

target_sources(${TEST_TARGET_NAME} PRIVATE
    EnergyManagerTest.cpp
    EnergyManagerSimpleTest.cpp
    ../Broker.cpp
    ../BrokerFastCharging.cpp
    ../EnergyManagerImpl.cpp
    ../Market.cpp
    ../Offer.cpp
)

set(ENERGY_FLOW_REQUESTS_LOCATION "${CMAKE_CURRENT_BINARY_DIR}/energy_flow_requests")

target_compile_definitions(${TEST_TARGET_NAME} PRIVATE
    BUILD_TESTING_MODULE_ENERGY_MANAGER
    ENERGY_FLOW_REQUESTS_LOCATION="${ENERGY_FLOW_REQUESTS_LOCATION}"   
)

target_link_libraries(${TEST_TARGET_NAME} PRIVATE
    GTest::gmock
    GTest::gtest_main
    everest::log
    everest::framework
)

add_test(${TEST_TARGET_NAME} ${TEST_TARGET_NAME})

# Copy the json files used for testing to the destination directory
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/energy_flow_requests DESTINATION ${CMAKE_CURRENT_BINARY_DIR})