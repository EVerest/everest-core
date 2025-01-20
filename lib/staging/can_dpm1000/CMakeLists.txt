add_library(can_dpm1000)
add_library(can_protocols::dpm1000 ALIAS can_dpm1000)
ev_register_library_target(can_dpm1000)

target_include_directories(can_dpm1000
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_sources(can_dpm1000
    PRIVATE
        src/dpm1000.cpp
)

if(BUILD_DEV_TESTS)
    add_subdirectory(tests)
endif()
