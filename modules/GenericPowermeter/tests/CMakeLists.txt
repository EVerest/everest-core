add_executable(generic_powermeter_tests serialization_test.cpp)

set(INCLUDE_DIR 
    "${PROJECT_SOURCE_DIR}/modules/GenericPowermeter"
    "${PROJECT_SOURCE_DIR}/modules/GenericPowermeter/tests")

get_target_property(GENERATED_INCLUDE_DIR generate_cpp_files EVEREST_GENERATED_INCLUDE_DIR)

target_include_directories(generic_powermeter_tests PUBLIC 
    ${GTEST_INCLUDE_DIRS}
    ${INCLUDE_DIR}
    ${GENERATED_INCLUDE_DIR}
)

find_package(GTest REQUIRED)

target_link_libraries(generic_powermeter_tests PRIVATE
    ${GTEST_LIBRARIES} 
    ${GTEST_MAIN_LIBRARIES} 
    everest::framework
    everest::log
    nlohmann_json::nlohmann_json
)

add_dependencies(generic_powermeter_tests generate_cpp_files)

add_test(generic_powermeter_tests generic_powermeter_tests)
