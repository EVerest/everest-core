target_include_directories(libocpp_unit_tests PUBLIC
    ../ mocks
    ${CMAKE_CURRENT_SOURCE_DIR})

target_sources(libocpp_unit_tests PRIVATE
    test_smart_charging.cpp)
