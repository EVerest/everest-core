set(INCLUDE_DIR "${PROJECT_SOURCE_DIR}/include")

# EVerest framework
# by default build as static
set(DEFAULT_LINKAGE SHARED)
# FIXME (aw): the following does not play well with CPM, because it get set by other dependencies ...
#if (DEFINED BUILD_SHARED_LIBS AND NOT BUILD_SHARED_LIBS)
#    message(FATAL_ERROR "everest-framework library does not support static build yet")
#endif()
add_library(framework ${DEFAULT_LINKAGE})
add_library(everest::framework ALIAS framework)

set_target_properties(framework PROPERTIES SOVERSION ${PROJECT_VERSION})

target_sources(framework
    PRIVATE
        config.cpp
        everest.cpp
        message_queue.cpp
        mqtt_abstraction.cpp
        mqtt_abstraction_impl.cpp
        conversions.cpp
        thread.cpp
        serial.cpp
        date.cpp
        runtime.cpp
        yaml_loader.cpp
)

target_include_directories(framework
    PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/everest>
)

target_link_libraries(framework
    PUBLIC
        date::date-tz
        nlohmann_json::nlohmann_json
        nlohmann_json_schema_validator
        fmt::fmt

        everest::log
    PRIVATE
        Boost::filesystem
        Boost::system
        Boost::thread
        Boost::program_options

        mqttc
        ryml::ryml
)
