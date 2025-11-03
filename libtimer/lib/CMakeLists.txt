add_library(timer INTERFACE)
add_library(everest::timer ALIAS timer)

target_include_directories(timer
    INTERFACE
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_link_libraries(timer
    INTERFACE
        Boost::system
        date::date-tz
)
