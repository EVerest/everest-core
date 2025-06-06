# EVerest run application helper functions

add_library(everest_staging_run_application STATIC)
add_library(everest::staging::run_application ALIAS everest_staging_run_application)
ev_register_library_target(everest_staging_run_application)

target_sources(everest_staging_run_application
    PRIVATE
        lib/run_application.cpp
)

target_include_directories(everest_staging_run_application
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_link_libraries(everest_staging_run_application
    PRIVATE
        Boost::headers
        fmt::fmt
        everest::log
)
