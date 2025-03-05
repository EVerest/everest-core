add_library(everest_staging_run_application STATIC)
add_library(everest::staging::run_application ALIAS everest_staging_run_application)
ev_register_library_target(everest_staging_run_application)

target_sources(everest_staging_run_application
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/RunApplication.cpp
)

target_include_directories(everest_staging_run_application
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries(everest_staging_run_application
    PRIVATE
        fmt::fmt
        everest::framework
)