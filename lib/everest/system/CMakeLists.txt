if(NOT DEFINED sdbus-c++_VERSION_MAJOR AND NOT DISABLE_EDM)
    string(REPLACE "." ";" SDBUS_CPP_VERSION_SPLIT ${CPM_PACKAGE_sdbus-cpp_VERSION})
    list(GET SDBUS_CPP_VERSION_SPLIT 0 sdbus-c++_VERSION_MAJOR)
endif()
message("Found sdbus-c++ major version: " ${sdbus-c++_VERSION_MAJOR})

add_library(everest_system STATIC)
add_library(everest::system ALIAS everest_system)
ev_register_library_target(everest_system)

target_sources(everest_system
    PRIVATE
        src/dbus_base.cpp
        src/rauc_dbus_base.cpp
        src/safe_system.cpp
)

target_include_directories(everest_system
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_link_libraries(everest_system
    PRIVATE
        everest::log
    PUBLIC
        SDBusCpp::sdbus-c++
)

target_compile_definitions(everest_system
    PRIVATE
        SDBUSCPP_MAJOR_VERSION=${sdbus-c++_VERSION_MAJOR}
)

if (BUILD_TESTING)
    target_compile_definitions(everest_system PUBLIC EVEREST_COVERAGE_ENABLED)
    add_subdirectory(tests)
endif()
