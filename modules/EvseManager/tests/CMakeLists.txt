set(TEST_TARGET_NAME ${PROJECT_NAME}_EvseManager_tests)
add_executable(${TEST_TARGET_NAME})

target_include_directories(${TEST_TARGET_NAME} PUBLIC
    ${GTEST_INCLUDE_DIRS} . ..
    ${GENERATED_INCLUDE_DIR}
    ${CMAKE_BINARY_DIR}/generated/modules/${MODULE_NAME}
    ${CMAKE_BINARY_DIR}/generated/include
)

target_sources(${TEST_TARGET_NAME} PRIVATE
    EnumFlagsTest.cpp
    ErrorHandlingTest.cpp
    EventQueueTest.cpp
    ../ErrorHandling.cpp
)

target_compile_definitions(${TEST_TARGET_NAME} PRIVATE
    BUILD_TESTING_MODULE_EVSE_MANAGER
)

add_dependencies(${TEST_TARGET_NAME} ${MODULE_NAME})

find_package(GTest REQUIRED)

target_link_libraries(${TEST_TARGET_NAME} PRIVATE
    ${GTEST_LIBRARIES}
    ${GTEST_MAIN_LIBRARIES}
    everest::log
    everest::framework
    sigslot
)

add_test(${TEST_TARGET_NAME} ${TEST_TARGET_NAME})
