add_subdirectory(functional_blocks)

target_sources(libocpp_unit_tests PRIVATE
        test_composite_schedule.cpp
)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/json DESTINATION ${TEST_PROFILES_LOCATION_V21})